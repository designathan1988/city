// Builds the agent meshes once at both levels, pushes the palettes to the "agent" material and refills the instance
// buffers every frame: transform from the interpolated pose (+Z forward, +X left), INSTANCE_CUSTOM = (paint index, wheel
// roll angle in radians + BRAKE_FLAG when braking, passengers, 0) for vehicles, all small whole numbers or a few bits of an
// angle, since the Compatibility renderer keeps custom data as halfs; the instance colour's alpha a seed of the vehicle. Walkers
// and whoever rides go to PeopleRenderer.
#include "render/agents/agent_renderer.h"

#include "render/agents/agent_lod.h"

#include "render/agents/agent_meshes.h"

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
constexpr float NOBODY_ABOARD = -1.f; // passengers of a car standing in a bay: not even a driver (agents.gdshader)
constexpr float BRAKE_FLAG = 8.f; // added to the roll angle (0..TAU) when the brake light is on; agents.gdshader
constexpr uint32_t SEED_SALT = 0x5ea7u, LOAD_SALT = 0x10adu, ORDER_SHIFT = 8, SEAT_SALT = 0x9e3779b1u;
constexpr float STILL_M = 0.002f; // a frame's movement under this is standing
constexpr int STILL_FRAMES = 12; // so many in a row and a rider puts a foot down
constexpr uint64_t SEAT_KEYS = 128; // more than any vehicle's seats: a rider's key is the vehicle's and the seat's

} // namespace

bool AgentRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("agent", errors);
	glass_ = ctx.material("agent_glass", errors);
	shadow_ = ctx.material("shadow_caster", errors);
	Dictionary t = ctx.table("agents", errors);
	if (!errors.is_empty() || !build_meshes(t, errors)) return false;
	if (!push_palettes(t, ctx.catalog->traffic.line_colours, errors) || !read_loads(Dictionary(t["occupants"])["load"], errors)) return false;
	vehicle_r_ = float(t["vehicle_radius_m"]);
	cabin_r_ = float(t["cabin_radius_m"]);
	far_r_ = float(t["far_radius_m"]);
	view_slack_ = float(t["view_slack"]);
	cabin_close_m_ = float(Dictionary(t["lod"])["cabin_close_m"]);
	people_r_ = float(t["people_radius_m"]);
	jump_m_ = float(t["wheel_jump_m"]);
	return people_.init(ctx, t["people"], errors);
}

bool AgentRenderer::build_meshes(const Dictionary &t, PackedStringArray &errors) {
	const AgentTrim trim = read_trim(t["trim"]);
	const Array cars = t["cars"];
	const Dictionary body = t["car_body"], inside = t["cars_cabin"];
	if (cars.size() != CAR_MODELS + 1) errors.push_back("agents.json: esperado um carro por estilo");
	CabinLook look;
	if (!look.load(t["cabin"])) errors.push_back("agents.json: cabin incompleto");
	if (!errors.is_empty()) return false;
	const Array wheel_sides = body["wheel_sides"];
	for (const char *key : { "wheel_sides", "stations", "arch_steps", "end_steps", "end_roll_steps" }) // one entry per level (agent_meshes.h)
		if (Array(body[key]).size() != LEVELS + 1) errors.push_back(String("agents.json: car_body.") + key + " precisa de um valor por nível e um para a sombra da frota");
	if (!errors.is_empty()) return false;
	const Dictionary far_shadow = t["far_shadow"], lod = t["lod"];
	const float cabin_error = lod["cabin_error_m"], body_error = lod["body_error_m"], far_error = lod["far_error_m"];
	MeshBuilder hull;
	build_car(hull, cars[0], body, inside, nullptr, trim, AGENT_HULL_LEVEL);
	fleet_shadow_.init(scenario_, shadow_, simplify_parts(hull, far_error), far_shadow["sector_m"], far_shadow["inset"], AGENT_MODELS);
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < LEVELS; level++) {
			MeshBuilder mb;
			Dictionary d;
			const int sides = int(wheel_sides[level]);
			const CabinLook *l = level == CABIN_LEVEL ? &look : nullptr;
			look.spots = &seats_[i];
			int seats = 0;
			if (i < CAR_MODELS || i == WAGON_MODEL) seats = build_car(mb, d = cars[i == WAGON_MODEL ? CAR_MODELS : i], body, inside, l, trim, level);
			else if (i == int(VehicleType::MOTO)) seats = build_moto(mb, d = t["moto"], sides, trim);
			else if (i == int(VehicleType::BICYCLE)) seats = build_bicycle(mb, d = t["bicycle"], sides, trim);
			else if (i == int(VehicleType::BUS)) seats = build_bus(mb, d = t["bus"], sides, l, trim);
			else seats = build_truck(mb, d = t["truck"], sides, l, trim);
			wheel_r_[i] = agent_tyre(d["tyre"]).x; // the motorbike's: its rear wheel
			if (level == CABIN_LEVEL && (i == int(VehicleType::MOTO) || i == int(VehicleType::BICYCLE))) {
				astride_[i] = true;
				seats_[i].push_back({ agent_v3(Dictionary(d["rider"])["hip"]), i == int(VehicleType::MOTO) ? SeatPose::MOTO : SeatPose::BICYCLE, DRIVER_SEAT });
			}
			if (level == FAR_LEVEL) fleet_shadow_.set_model(i, mb);
			if (level == CABIN_LEVEL) {
				const AABB box = mesh_outline(mb);
				reach_m_[i] = box.position.abs().max(box.get_end().abs()).length();
			}
			if (seats < 0) errors.push_back(String("agents.json: o veículo ") + String::num_int64(i) + " tem a cabine invertida (fundo à frente da frente)");
			if (level == CABIN_LEVEL) {
				load_[i].seats = seats;
				commit_mesh(i, MID_DRAW, simplify_parts(mb, cabin_error));
				commit_mesh(i, level, mb);
			} else {
				commit_mesh(i, level, simplify_parts(mb, level == BODY_LEVEL ? body_error : far_error));
			}
		}
	return errors.is_empty();
}

void AgentRenderer::commit_mesh(int model, int draw, const MeshBuilder &mb) {
	RenderingServer *rs = RenderingServer::get_singleton();
	// with the cabin, the glass is see-through over it; past it, it stays in the body, opaque
	const bool cabin = draw == CABIN_LEVEL || draw == MID_DRAW;
	MeshBuilder shell, glass;
	if (cabin) split_tag(mb, AgentTag::GLASS, shell, glass);
	const MeshBuilder &body = cabin ? shell : mb;
	body_tris_[model][draw] = int(body.idx.size() / 3);
	glass_tris_[model][draw] = int(glass.idx.size() / 3);
	OwnedMesh &m = meshes_[model][draw];
	m.reset();
	m.rid = rs->mesh_create();
	body.commit_to(m.rid);
	rs->mesh_surface_set_material(m.rid, 0, material_);
	if (!glass.empty()) {
		glass.commit_to(m.rid);
		rs->mesh_surface_set_material(m.rid, 1, glass_);
	}
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
	drawn_vehicles_ = drawn_people_ = 0;
}

float AgentRenderer::roll(int id, const Pose &p, float wheel_r) {
	if (size_t(id) >= odo_.size()) odo_.resize(size_t(id) + 1);
	Odometer &o = odo_[size_t(id)];
	float dx = p.x - o.x, dz = p.z - o.z, moved = dx * p.fx + dz * p.fz;
	if (o.seen && std::fabs(moved) < jump_m_ && wheel_r > 0) o.angle = std::fmod(o.angle + moved / wheel_r, TAU_F);
	o.still = std::fabs(moved) > STILL_M ? 0 : std::min(o.still + 1, STILL_FRAMES);
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

void AgentRenderer::update(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const PoseSample &at) {
	if (!material_.is_valid() || !meshes_[0][CABIN_LEVEL].valid()) return;
	people_.begin();
	keeper_.begin();
	auto &bufs = seen_rows_; // what the camera sees
	auto &casts = cast_rows_; // what the sun sees, out of the view too: a car behind the camera shades what is before it
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			bufs[i][level].reopen();
			casts[i][level].restart();
		}
	const auto model_of = [](int type, int paint) { return type == int(VehicleType::SEDAN) && paint % WAGON_EVERY == 1 ? WAGON_MODEL : type; };
	const float reach = cabin_reach_ > 0 ? cabin_reach_ : cabin_r_;
	const float r2 = vehicle_r_ * vehicle_r_, near2 = cabin_limit2_ > 0 ? std::min(cabin_limit2_, reach * reach) : reach * reach;
	const float far2 = far_r_ * far_r_;
	const float people_r2 = people_r_ * people_r_;
	const float close2 = std::min(cabin_close_m_ * cabin_close_m_, near2);
	auto level_at = [&](float d2) { return d2 < close2 ? CABIN_LEVEL : (d2 < near2 ? MID_DRAW : (d2 < far2 ? BODY_LEVEL : FAR_LEVEL)); };
	near_d2_.clear();
	drawn_vehicles_ = 0;
	// Sent to the level's lists: the sun's every time, the camera's only in the view, with room for the frame the camera turned since.
	auto seen = [&](int model, const Pose &p, float d2) { return view.sees(Vector3(p.x, p.y, p.z), reach_m_[model] + std::sqrt(d2) * view_slack_); };
	for (const AgentRecord &rec : f.vehicles) {
		Pose p = record_pose(rec, at);
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const bool brake = (rec.look & (1u << LOOK_BRAKE_SHIFT)) != 0;
		if (type < 0 || type >= VEHICLE_TYPES) continue;
		float spin = roll(int(rec.id), p, wheel_r_[type]);
		const float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > r2) continue;
		float passengers, order;
		riders(type, rec.id, passengers, order);
		const uint32_t aboard = (rec.look >> LOOK_ABOARD_SHIFT) & LOOK_ABOARD_MASK;
		if (aboard == ABOARD_TO_SET_DOWN) passengers = std::max(passengers, 1.f);
		else if (aboard == ABOARD_SET_DOWN) passengers = std::max(passengers, 1.f) - 1;
		else if (aboard == ABOARD_PICKED_UP) passengers += 1;
		if ((rec.look & (1u << LOOK_EMPTY_SHIFT)) != 0) passengers = NOBODY_ABOARD;
		const float d2 = flat2 + dy * dy;
		const int model = model_of(type, paint);
		const bool shown = seen(model, p, d2);
		if (shown && cabin_cap_ > 0 && d2 <= reach * reach) near_d2_.push_back(d2);
		const int level = level_at(d2);
		if (level == FAR_LEVEL) fleet_shadow_.add(model, p);
		if (!shown && level == FAR_LEVEL) continue;
		agent_row(shown ? keeper_.place(rec.id, model * DRAWS + level, bufs[model][level]) : casts[model][level].add(), p, 1.f, float(paint >= LINE_PAINT ? paints_ + paint - LINE_PAINT : paint % paints_), spin + (brake ? BRAKE_FLAG : 0.f), passengers, 0.f, hash_float(rec.id * SEED_SALT));
		if (shown && d2 <= people_r2 && (astride_[model] || level == CABIN_LEVEL || level == MID_DRAW)) seat_people(rec.id, type, model, p, std::sqrt(d2), passengers, order, spin);
		drawn_vehicles_++;
	}
	if (f.parked) for (const ParkedRecord &rec : *f.parked) {
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const float dx = rec.at.x - cam.x, dy = rec.at.y - cam.y, dz = rec.at.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > r2 || type < 0 || type >= VEHICLE_TYPES) continue;
		const float d2 = flat2 + dy * dy;
		const int model = model_of(type, paint);
		const bool shown = seen(model, rec.at, d2);
		if (shown && cabin_cap_ > 0 && d2 <= reach * reach) near_d2_.push_back(d2);
		const int level = level_at(d2);
		if (level == FAR_LEVEL) fleet_shadow_.add(model, rec.at);
		if (!shown && level == FAR_LEVEL) continue;
		agent_row(shown ? keeper_.place(rec.id, model * DRAWS + level, bufs[model][level]) : casts[model][level].add(), rec.at, 1.f, float(paint % paints_), 0.f, NOBODY_ABOARD, 0.f, hash_float(rec.id * SEED_SALT));
		drawn_vehicles_++;
	}
	keeper_.sweep([&](int d) -> InstanceRows & { return bufs[d / DRAWS][d % DRAWS]; });
	if (cabin_cap_ > 0 && int(near_d2_.size()) > cabin_cap_) { // the next frame's limit: the nearest cabin_cap_ keep theirs
		std::nth_element(near_d2_.begin(), near_d2_.begin() + cabin_cap_, near_d2_.end());
		cabin_limit2_ = near_d2_[size_t(cabin_cap_)];
	} else {
		cabin_limit2_ = 0;
	}
	drawn_people_ = people_.update(f, cam, view, at, people_r_);
	AABB box(Vector3(cam.x - vehicle_r_, cam.y - AABB_HALF_H, cam.z - vehicle_r_), Vector3(vehicle_r_ * 2, AABB_HALF_H * 2, vehicle_r_ * 2));
	// no override: each surface carries its material (the body and the near level's glass); the room is MultiMeshDraw's
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			MultiMeshDraw &d = draws_[i][level];
			d.set(scenario_, meshes_[i][level].rid, RID(), bufs[i][level], box, false, LAYER_VEHICLES);
			shown_[i][level] = d.count;
			if (level == FAR_LEVEL) continue;
			casts[i][level].append(bufs[i][level]);
			shadow_draws_[i][level].shadow_only = true;
			shadow_draws_[i][level].set(scenario_, meshes_[i][level == BODY_LEVEL ? FAR_LEVEL : BODY_LEVEL].rid, shadow_, casts[i][level], box, true, LAYER_VEHICLES);
		}
	fleet_shadow_.draw();
}

int AgentRenderer::cabins_drawn() const {
	int n = 0;
	for (int i = 0; i < VEHICLE_TYPES; i++) n += shown_[i][CABIN_LEVEL] + shown_[i][MID_DRAW];
	return n;
}

} // namespace city
