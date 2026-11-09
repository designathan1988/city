// Holds the agent meshes (built at boot, agent_looks.cpp) and refills the instance
// buffers every frame: transform from the interpolated pose (+Z forward, +X left), INSTANCE_CUSTOM = (paint index, wheel
// roll angle in radians + AGENT_BRAKE_FLAG when braking, passengers, 0) for vehicles, all small whole numbers or a few bits of an
// angle, since the Compatibility renderer keeps custom data as halfs; the instance colour's alpha a seed of the vehicle. Walkers
// and whoever rides go to PeopleRenderer.
#include "render/agents/agent_renderer.h"
#include "core/shader_contracts/agents.h"

#include "render/agents/agent_lod.h"

#include "render/agents/agent_meshes.h"
#include "render/surface_data.h"

#include "core/grid.h"
#include "core/random.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace godot;

namespace city {

namespace {

constexpr int WAGON_EVERY = 3; // one sedan in three is an estate, by its paint: a car keeps its shape from the road to its bay
int model_of(int type, int paint) { return type == int(VehicleType::SEDAN) && paint % WAGON_EVERY == 1 ? WAGON_MODEL : type; }
constexpr float NOBODY_ABOARD = -1.f; // passengers of a car standing in a bay: not even a driver (agents.gdshader)
// streams of a vehicle's id (any distinct words; the seats' is 2^32 over the golden ratio), and the byte a load's order is read past
constexpr uint32_t SEED_SALT = 0x5ea7u, LOAD_SALT = 0x10adu, ORDER_SHIFT = 8, SEAT_SALT = 0x9e3779b1u;
constexpr float ROLL_LEAST_M = 0.002f; // a frame's movement under this is standing
constexpr int STILL_FRAMES = 12; // so many in a row and a rider puts a foot down
constexpr uint64_t SEAT_KEYS = 128; // more than any vehicle's seats: a rider's key is the vehicle's and the seat's

} // namespace

bool AgentRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("agent", errors);
	heads_.init(scenario_, ctx.catalog->tune, RenderingServer::get_singleton()->material_get_param(material_, "head_glow"));
	glass_ = ctx.material("agent_glass", errors);
	shadow_ = ctx.material("shadow_caster", errors);
	const Dictionary table = ctx.table("agents", errors);
	if (!errors.is_empty()) return false;
	const DataValue t = DataReader{ errors, "agents.json" }.at(table); // render/data_reader.h
	if (!read_shapes(table, errors)) return false;
	if (!build_meshes(t, errors)) return false;
	if (!push_palettes(t, ctx.catalog->traffic.line_colours, errors) || !read_loads(t["occupants"]["load"], errors)) return false;
	vehicle_r_ = t["vehicle_radius_m"].f();
	if (vehicle_r_ <= 0) { // the cells of the bays' cars are this wide (index_parked)
		errors.push_back("agents.json: vehicle_radius_m must be positive");
		return false;
	}
	cabin_r_ = t["cabin_radius_m"].f();
	far_r_ = t["far_radius_m"].f();
	level_hold_ = t["lod"]["hold"].f();
	view_slack_ = t["view_slack"].f();
	cabin_close_m_ = t["lod"]["cabin_close_m"].f();
	people_r_ = t["people_radius_m"].f();
	jump_m_ = t["wheel_jump_m"].f();
	if (!errors.is_empty()) return false;
	people_.set_level_hold(level_hold_);
	return people_.init(ctx, t["people"], t["held"], shapes_, errors);
}

void AgentRenderer::commit_mesh(int model, int draw, const MeshBuilder &mb) {
	// with the cabin, the glass is see-through over it; past it, it stays in the body, opaque
	const bool cabin = draw == CABIN_LEVEL || draw == MID_DRAW;
	MeshBuilder shell, glass;
	if (cabin) split_tag(mb, AgentTag::GLASS, shell, glass);
	const MeshBuilder &body = cabin ? shell : mb;
	body_tris_[model][draw] = int(body.idx.size() / 3);
	glass_tris_[model][draw] = int(glass.idx.size() / 3);
	OwnedMesh &m = meshes_[model][draw];
	m.reset();
	TypedArray<Dictionary> surfaces;
	surfaces.push_back(body.surface(material_));
	if (!glass.empty()) surfaces.push_back(glass.surface(glass_));
	m.rid = create_mesh(surfaces);
	// the levels the shadows are cast from (draw_rows), again with the caster's material: a draw is given its materials by its mesh (surface_data.h)
	if (draw == BODY_LEVEL || draw == FAR_LEVEL) cast_meshes_[model][draw] = body.bake(shadow_);
}

void AgentRenderer::riders(int type, uint32_t id, float &passengers, float &order) const {
	const Load &l = load_[type];
	passengers = order = 0;
	if (l.seats == 0) return;
	const uint32_t h = hash_u32(id ^ LOAD_SALT);
	const float u = hash_float(h), v = hash_float(h + 1);
	size_t band = 0;
	while (band + 1 < l.bands.size() && u >= l.bands[band]) band++;
	// how full it runs: a share in its band, as whole passengers out of seats + 1 steps (none .. every seat)
	const float share = (float(band) + v) / float(l.bands.size());
	passengers = float(std::min(l.seats, int(share * float(l.seats + 1))));
	const int stride = l.strides[(h >> ORDER_SHIFT) % l.strides.size()], first = l.shuffle ? int((h >> (ORDER_SHIFT * 2)) % uint32_t(l.seats)) : 0;
	order = float(stride * l.seats + first);
}

Dictionary AgentRenderer::cost() const {
	static const char *const NAMES[AGENT_MODELS] = { "sedan", "hatch", "suv", "van", "pickup", "moto", "bus", "truck", "bicycle", "wagon" };
	static const char *const LEVEL_NAMES[DRAWS] = { "cabin", "body", "far", "mid" };
	Dictionary out;
	out["cabins"] = cabins_drawn();
	out["people_near"] = people_.shown(0);
	out["people_cast"] = people_.cast_census();
	out["people_plays"] = people_.plays_census();
	out["rows_lost"] = rows_lost_;
	int draws = 0, tris = 0;
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			Dictionary d;
			d["body_tris"] = body_tris_[i][level];
			d["glass_tris"] = glass_tris_[i][level];
			d["instances"] = shown_[i][level];
			out[String(NAMES[i]) + "_" + LEVEL_NAMES[level]] = d;
			if (shown_[i][level] > 0) draws += glass_tris_[i][level] > 0 ? 2 : 1;
			tris += shown_[i][level] * (body_tris_[i][level] + glass_tris_[i][level]);
		}
	out["draws"] = draws;
	out["triangles"] = tris; // drawn last frame, in the main pass (shadow passes again for the casters)
	return out;
}

void AgentRenderer::reset() {
	heads_.reset();
	fleet_shadow_.free();
	for (auto &levels : shadow_draws_)
		for (MultiMeshDraw &m : levels) m.free();
	for (auto &levels : draws_)
		for (MultiMeshDraw &m : levels) m.free();
	people_.reset();
	keeper_.clear();
	keeper_.draws(size_t(AGENT_MODELS) * DRAWS);
	for (auto &levels : seen_rows_)
		for (InstanceRows &rows : levels) rows.restart();
	odo_.clear();
	parked_indexed_.reset();
	parked_cells_.clear();
	parked_near_.clear();
	drawn_vehicles_ = drawn_people_ = 0;
}

float AgentRenderer::roll(int id, const Pose &p, float wheel_r) {
	if (size_t(id) >= odo_.size()) odo_.resize(size_t(id) + 1);
	Odometer &o = odo_[size_t(id)];
	float dx = p.x - o.x, dz = p.z - o.z, moved = dx * p.fx + dz * p.fz;
	if (o.seen && std::fabs(moved) < jump_m_ && wheel_r > 0) o.angle = std::fmod(o.angle + moved / wheel_r, TAU_F);
	o.still = std::fabs(moved) > ROLL_LEAST_M ? 0 : std::min(o.still + 1, STILL_FRAMES);
	o.x = p.x;
	o.z = p.z;
	o.seen = true;
	return o.angle;
}

void AgentRenderer::seat_people(uint32_t id, int type, int model, const Pose &p, float dist, float passengers, float order, float spin) {
	const int seats = load_[type].seats;
	const int stride = seats > 0 ? int(order) / seats : 0, first = seats > 0 ? int(order) - stride * seats : 0;
	const Odometer &o = odo_[size_t(id)];
	const float turn = spin / TAU_F;
	for (const SeatSpot &s : seats_[model]) {
		// passenger seat j is taken when (stride * j + first) mod seats < passengers, so exactly that many are
		if (s.order != DRIVER_SEAT && (seats == 0 || (stride * s.order + first) % seats >= int(passengers))) continue;
		const uint64_t seat = uint64_t(s.order - DRIVER_SEAT);
		people_.seat(uint64_t(id) * SEAT_KEYS + seat, p, s, hash_u32(id * SEAT_SALT + uint32_t(seat)), dist, turn - std::floor(turn), o.still >= STILL_FRAMES);
	}
}

int AgentRenderer::LevelRule::plain_level(float d2) const { return d2 < close2 ? CABIN_LEVEL : (d2 < near2 ? MID_DRAW : (d2 < far2 ? BODY_LEVEL : FAR_LEVEL)); }

int AgentRenderer::LevelRule::rank(int level) { return level == CABIN_LEVEL ? 0 : level == MID_DRAW ? 1 : level == BODY_LEVEL ? 2 : 3; }

AgentRenderer::LevelRule AgentRenderer::level_rule() const {
	const float reach = cabin_reach_ > 0 ? cabin_reach_ : cabin_r_;
	const float r2 = vehicle_r_ * vehicle_r_, near2 = cabin_limit2_ > 0 ? std::min(cabin_limit2_, reach * reach) : reach * reach;
	const float far2 = far_r_ * far_r_;
	const float people_r2 = people_r_ * people_r_;
	const float close2 = std::min(cabin_close_m_ * cabin_close_m_, near2);
	// A vehicle keeps its level until it is past the edge by the hold (agents.ts VEHICLE_HYSTERESIS), or it flickers there.
	const float hold2 = (1 + level_hold_) * (1 + level_hold_);
	return LevelRule{ reach, r2, near2, far2, people_r2, close2, hold2 };
}

int AgentRenderer::level_of(const LevelRule &rule, uint64_t key, float d2) const {
	const int now = rule.plain_level(d2), place = keeper_.draw_of(key), was = place < 0 ? now : place % DRAWS;
	if (was == now) return now;
	return LevelRule::rank(now) > LevelRule::rank(was) ? (LevelRule::rank(rule.plain_level(d2 / rule.hold2)) > LevelRule::rank(was) ? now : was) : (LevelRule::rank(rule.plain_level(d2 * rule.hold2)) < LevelRule::rank(was) ? now : was);
}

// In the race for the cabins a vehicle that has one stands as near as the hold makes it: the cap counts what the hold keeps, and no more than that stay.
void AgentRenderer::count_for_cap(const LevelRule &rule, int level, float d2) {
	if (cabin_cap_ > 0 && d2 <= rule.reach * rule.reach * rule.hold2) near_d2_.push_back(level == CABIN_LEVEL || level == MID_DRAW ? d2 / rule.hold2 : d2);
}

// Sent to the level's lists: the sun's every time, the camera's only in the view, with room for the frame the camera turned since.
bool AgentRenderer::in_view(const ViewCull &view, int model, const Pose &p, float d2) const { return view.sees(Vector3(p.x, p.y, p.z), reach_m_[model] + std::sqrt(d2) * view_slack_); }

void AgentRenderer::update(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const PoseSample &at) {
	if (!material_.is_valid() || !meshes_[0][CABIN_LEVEL].valid()) return;
	people_.begin(f);
	keeper_.begin();
	auto &bufs = seen_rows_; // what the camera sees
	auto &casts = cast_rows_; // what the sun sees, out of the view too: a car behind the camera shades what is before it
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			bufs[i][level].reopen();
			casts[i][level].restart();
		}
	const LevelRule rule = level_rule();
	near_d2_.clear();
	drawn_vehicles_ = 0;
	heads_.begin();
	int placed = 0; // what the camera sees has a row each
	place_vehicles(f, cam, view, at, rule, placed);
	place_parked(f, cam, view, rule, placed);
	keeper_.sweep([&](int d) -> InstanceRows & { return bufs[d / DRAWS][d % DRAWS]; });
	rows_lost_ = placed;
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) rows_lost_ -= bufs[i][level].count();
	limit_cabins();
	drawn_people_ = people_.update(f, cam, view, at, people_r_);
	draw_rows(cam);
}

void AgentRenderer::place_vehicles(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const PoseSample &at, const LevelRule &rule, int &placed) {
	auto &bufs = seen_rows_;
	auto &casts = cast_rows_;
	for (const AgentRecord &rec : f.vehicles) {
		Pose p = record_pose(rec, at);
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const bool brake = (rec.look & (1u << LOOK_BRAKE_SHIFT)) != 0;
		if (type < 0 || type >= VEHICLE_TYPES) continue;
		const float door = float((rec.look >> LOOK_DOOR_SHIFT) & LOOK_DOOR_MASK) * AGENT_DOOR_STEP;
		float spin = roll(int(rec.id), p, wheel_r_[type]);
		const float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > rule.r2) continue;
		float passengers, order;
		riders(type, rec.id, passengers, order);
		const uint32_t aboard = (rec.look >> LOOK_ABOARD_SHIFT) & LOOK_ABOARD_MASK;
		if (aboard == ABOARD_TO_SET_DOWN) passengers = std::max(passengers, 1.f);
		else if (aboard == ABOARD_SET_DOWN) passengers = std::max(passengers, 1.f) - 1;
		else if (aboard == ABOARD_PICKED_UP) passengers += 1;
		const uint32_t counted = (rec.look >> LOOK_RIDERS_SHIFT) & LOOK_RIDERS_MASK; // a bus: who got on less who got off, in the seats it has
		if (counted > 0) passengers = std::min(float(load_[type].seats), float(counted - 1));
		if ((rec.look & (1u << LOOK_EMPTY_SHIFT)) != 0) passengers = NOBODY_ABOARD;
		const float d2 = flat2 + dy * dy;
		const int model = model_of(type, paint);
		heads_.add(rec.id, p, reach_m_[model], cam, view);
		const bool shown = in_view(view, model, p, d2);
		const uint64_t key = rec.id * ROW_KINDS;
		const int level = level_of(rule, key, d2);
		if (shown) count_for_cap(rule, level, d2);
		if (level == FAR_LEVEL) fleet_shadow_.add(model, p);
		if (!shown && level == FAR_LEVEL) continue;
		placed += shown ? 1 : 0;
		agent_row(shown ? keeper_.place(key, model * DRAWS + level, bufs[model][level]) : casts[model][level].add(), p, 1.f, float(paint >= LINE_PAINT ? paints_ + paint - LINE_PAINT : paint % paints_), spin + (brake ? AGENT_BRAKE_FLAG : 0.f) + door, passengers, 0.f, hash_float(rec.id * SEED_SALT));
		if (shown && d2 <= rule.people_r2 && (astride_[model] || level == CABIN_LEVEL || level == MID_DRAW)) seat_people(rec.id, type, model, p, std::sqrt(d2), passengers, order, spin);
		drawn_vehicles_++;
	}
}

void AgentRenderer::place_parked(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const LevelRule &rule, int &placed) {
	auto &bufs = seen_rows_;
	auto &casts = cast_rows_;
	if (f.parked != parked_indexed_) index_parked(f.parked);
	if (f.parked) for (uint32_t index : parked_near(cam)) {
		const ParkedRecord &rec = (*f.parked)[index];
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const float dx = rec.at.x - cam.x, dy = rec.at.y - cam.y, dz = rec.at.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > rule.r2 || type < 0 || type >= VEHICLE_TYPES) continue;
		const float d2 = flat2 + dy * dy;
		const int model = model_of(type, paint);
		const bool shown = in_view(view, model, rec.at, d2);
		const uint64_t key = uint64_t(rec.id) * ROW_KINDS + PARKED_ROW;
		const int level = level_of(rule, key, d2);
		if (shown) count_for_cap(rule, level, d2);
		if (level == FAR_LEVEL) fleet_shadow_.add(model, rec.at);
		if (!shown && level == FAR_LEVEL) continue;
		placed += shown ? 1 : 0;
		agent_row(shown ? keeper_.place(key, model * DRAWS + level, bufs[model][level]) : casts[model][level].add(), rec.at, 1.f, float(paint % paints_), 0.f, NOBODY_ABOARD, 0.f, hash_float(rec.id * SEED_SALT));
		drawn_vehicles_++;
	}
}

void AgentRenderer::limit_cabins() {
	if (cabin_cap_ > 0 && int(near_d2_.size()) > cabin_cap_) { // the next frame's limit: the nearest cabin_cap_ keep theirs
		std::nth_element(near_d2_.begin(), near_d2_.begin() + cabin_cap_, near_d2_.end());
		cabin_limit2_ = near_d2_[size_t(cabin_cap_)];
	} else {
		cabin_limit2_ = 0;
	}
}

void AgentRenderer::draw_rows(const Vector3 &cam) {
	auto &bufs = seen_rows_;
	auto &casts = cast_rows_;
	AABB box(Vector3(cam.x - vehicle_r_, cam.y - AABB_HALF_H, cam.z - vehicle_r_), Vector3(vehicle_r_ * 2, AABB_HALF_H * 2, vehicle_r_ * 2));
	// each surface carries its material (the body and the near level's glass); the room is MultiMeshDraw's
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			MultiMeshDraw &d = draws_[i][level];
			d.set(scenario_, meshes_[i][level].rid, bufs[i][level], box, false, LAYER_VEHICLES);
			shown_[i][level] = d.count;
			if (level == FAR_LEVEL) continue;
			casts[i][level].append(bufs[i][level]);
			shadow_draws_[i][level].shadow_only = true;
			shadow_draws_[i][level].set(scenario_, cast_meshes_[i][level == BODY_LEVEL ? FAR_LEVEL : BODY_LEVEL].rid, casts[i][level], box, true, LAYER_VEHICLES);
		}
	fleet_shadow_.draw();
}

int AgentRenderer::cabins_drawn() const {
	int n = 0;
	for (int i = 0; i < VEHICLE_TYPES; i++) n += shown_[i][CABIN_LEVEL] + shown_[i][MID_DRAW];
	return n;
}

} // namespace city
