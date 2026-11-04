// Builds the agent meshes once at both levels, pushes the palettes to the "agent" material and refills the instance
// buffers every frame: transform from the interpolated pose (+Z forward, +X left), INSTANCE_CUSTOM = (paint index, wheel
// roll angle in radians + BRAKE_FLAG when braking, passengers, the order the seats fill in: stride * seats + first) for
// vehicles, all small whole numbers or a few bits of an angle, since the Compatibility renderer keeps custom data as
// halfs; the instance colour's alpha a seed of the vehicle (how its people look). People go to PeopleRenderer.
#include "render/agents/agent_renderer.h"

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

constexpr float AABB_HALF_H = 500.f;
constexpr int CAR_MODELS = int(VehicleType::MOTO); // sedan .. pickup come from agents.json "cars", then the wagon
constexpr int WAGON_EVERY = 3; // one sedan in three is an estate, by its paint: a car keeps its shape from the road to its bay
constexpr float NOBODY_ABOARD = -1.f; // passengers of a car standing in a bay: not even a driver (agents.gdshader)
constexpr float BRAKE_FLAG = 8.f; // added to the roll angle (0..TAU) when the brake light is on; agents.gdshader
constexpr uint32_t SEED_SALT = 0x5ea7u, LOAD_SALT = 0x10adu, ORDER_SHIFT = 8;
// agents.gdshader palette array sizes
constexpr int HAIR_MAX = 8, SKIN_MAX = 8, PAINT_MAX = 16, LINE_PAINTS = 8, CUTS = 3, OUTFITS_MAX = 8, OUTFIT_COLOURS = 8;
constexpr int HALF_EXACT = 2048; // whole numbers a half float holds exactly: the order (stride * seats + first) below it
constexpr int MIN_STRIDE = 2;    // a shuffled order skips at least one seat (stride 1 fills them front to back)
// A draw's room in instances grows past what it holds, in these steps, and shrinks only two steps below: vehicles
// coming and going do not reallocate its buffers every frame (the rest of the room is not drawn).
constexpr int ROOM_STEP = 32;

PackedColorArray palette(const Variant &list, int max, const char *name, PackedStringArray &errors) {
	PackedColorArray out;
	for (const Variant &c : Array(list)) out.push_back(agent_hex(c));
	if (out.is_empty() || out.size() > max) errors.push_back(String("agents.json: paleta ") + name + " vazia ou longa demais");
	while (out.size() < max) out.push_back(out.is_empty() ? Color() : out[0]);
	return out;
}

// The cumulative shares of `weights`, `count` of them, padded to `max` with 1 (what the shader's draws walk).
PackedFloat32Array cumulative(const Variant &weights, int count, int max, const char *name, PackedStringArray &errors) {
	PackedFloat32Array out;
	float sum = 0;
	for (const Variant &w : Array(weights)) out.push_back(sum += float(w));
	if (out.size() != count || count > max || sum <= 0) errors.push_back(String("agents.json: occupants.") + name + " sem os pesos certos");
	for (int64_t i = 0; i < out.size(); i++) out.set(i, sum > 0 ? out[i] / sum : 1.f);
	while (out.size() < max) out.push_back(1.f);
	return out;
}

// agents.json occupants.load key of a vehicle type (the motorbike carries its rider alone).
const char *load_key(int type) {
	if (type < CAR_MODELS || type == WAGON_MODEL) return "car";
	if (type == int(VehicleType::BUS)) return "bus";
	return type == int(VehicleType::TRUCK) ? "truck" : nullptr;
}

} // namespace

bool AgentRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("agent", errors);
	glass_ = ctx.material("agent_glass", errors);
	Dictionary t = ctx.table("agents", errors);
	if (!errors.is_empty() || !build_meshes(t, errors)) return false;
	if (!push_palettes(t, ctx.catalog->traffic.line_colours, errors) || !read_loads(Dictionary(t["occupants"])["load"], errors)) return false;
	vehicle_r_ = float(t["vehicle_radius_m"]);
	cabin_r_ = float(t["cabin_radius_m"]);
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
	RenderingServer *rs = RenderingServer::get_singleton();
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level : { CABIN_LEVEL, BODY_LEVEL }) {
			MeshBuilder mb;
			Dictionary d;
			const int sides = int(wheel_sides[level]);
			const CabinLook *l = level == CABIN_LEVEL ? &look : nullptr;
			int seats = 0;
			if (i < CAR_MODELS || i == WAGON_MODEL) seats = build_car(mb, d = cars[i == WAGON_MODEL ? CAR_MODELS : i], body, inside, l, trim);
			else if (i == int(VehicleType::MOTO)) seats = build_moto(mb, d = t["moto"], sides, look.person, trim);
			else if (i == int(VehicleType::BICYCLE)) seats = build_bicycle(mb, d = t["bicycle"], sides, look.person, trim);
			else if (i == int(VehicleType::BUS)) seats = build_bus(mb, d = t["bus"], sides, l, trim);
			else seats = build_truck(mb, d = t["truck"], sides, l, trim);
			wheel_r_[i] = agent_tyre(d["tyre"]).x; // the motorbike's: its rear wheel
			if (seats < 0) errors.push_back(String("agents.json: o veículo ") + String::num_int64(i) + " tem a cabine invertida (fundo à frente da frente)");
			if (level == CABIN_LEVEL) load_[i].seats = seats;
			// near, the glass is see-through over the cabin; far, it stays in the body, opaque
			MeshBuilder shell, glass;
			if (level == CABIN_LEVEL) split_tag(mb, AgentTag::GLASS, shell, glass);
			body_tris_[i][level] = int((level == CABIN_LEVEL ? shell : mb).idx.size() / 3);
			glass_tris_[i][level] = int(glass.idx.size() / 3);
			OwnedMesh &m = meshes_[i][level];
			m.reset();
			m.rid = rs->mesh_create();
			(level == CABIN_LEVEL ? shell : mb).commit_to(m.rid);
			rs->mesh_surface_set_material(m.rid, 0, material_);
			if (!glass.empty()) {
				glass.commit_to(m.rid);
				rs->mesh_surface_set_material(m.rid, 1, glass_);
			}
		}
	return errors.is_empty();
}

bool AgentRenderer::push_palettes(const Dictionary &t, const std::vector<uint32_t> &lines, PackedStringArray &errors) {
	const Dictionary people = t["people"], occupants = t["occupants"];
	RenderingServer *rs = RenderingServer::get_singleton();
	const Dictionary styles = t["styles"], style = styles[REGIONAL_STYLE];
	const Array hair = people["hair"], skin = occupants["skin"], paints = style["paints"];
	rs->material_set_param(material_, "hair", palette(hair, HAIR_MAX, "hair", errors));
	rs->material_set_param(material_, "skin", palette(skin, SKIN_MAX, "skin", errors));
	// as numbers: a Color array would be turned linear on the way (the parts' vertex colours are not)
	PackedVector3Array raw;
	for (const Color &c : palette(paints, PAINT_MAX, "paints", errors)) raw.push_back(Vector3(c.r, c.g, c.b));
	for (size_t i = 0; i < lines.size() && i < size_t(LINE_PAINTS); i++) { // after them: a bus's line (traffic_state.h LINE_PAINT)
		const Color c = rgb8_to_color(lines[i]);
		raw.push_back(Vector3(c.r, c.g, c.b));
	}
	rs->material_set_param(material_, "paints", raw);
	rs->material_set_param(material_, "paint_n", int(paints.size())); // the riders' helmets come in the paints too
	const Dictionary roof = Dictionary(t["car_body"])["black_roof"];
	rs->material_set_param(material_, "black_roof", agent_hex(roof["colour"]));
	rs->material_set_param(material_, "black_roof_share", float(roof["share"]));
	rs->material_set_param(material_, "skin_cdf", cumulative(occupants["skin_weights"], int(skin.size()), SKIN_MAX, "skin_weights", errors));
	push_looks(occupants, people["clothes"], int(hair.size()), errors);
	paints_ = int(paints.size());
	return errors.is_empty();
}

void AgentRenderer::push_looks(const Dictionary &o, const Array &clothes, int hair_n, PackedStringArray &errors) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const Dictionary hw = o["hair_weights"];
	PackedFloat32Array hair = cumulative(hw["light_skinned"], hair_n, HAIR_MAX, "hair_weights.light_skinned", errors);
	hair.append_array(cumulative(hw["dark_skinned"], hair_n, HAIR_MAX, "hair_weights.dark_skinned", errors));
	rs->material_set_param(material_, "hair_cdf", hair);
	rs->material_set_param(material_, "light_skins", int(o["light_skins"]));
	rs->material_set_param(material_, "cut_cdf", cumulative(o["cuts"], CUTS, CUTS, "cuts", errors));
	// one outfit: the shirt and the trousers each drawn from the pedestrians' clothes (agents.json people.clothes)
	if (clothes.is_empty() || clothes.size() > OUTFITS_MAX * OUTFIT_COLOURS) errors.push_back("agents.json: people.clothes com cores demais ou nenhuma");
	PackedColorArray shirts;
	for (const Variant &c : clothes) shirts.push_back(agent_hex(c));
	while (shirts.size() < OUTFITS_MAX * OUTFIT_COLOURS) shirts.push_back(Color());
	const PackedColorArray &trousers = shirts;
	PackedInt32Array from, count;
	for (int i = 0; i < OUTFITS_MAX; i++) {
		from.push_back(0);
		count.push_back(std::max(int(clothes.size()), 1));
	}
	Array one;
	one.push_back(1);
	rs->material_set_param(material_, "outfit_cdf", cumulative(one, 1, OUTFITS_MAX, "outfits", errors));
	rs->material_set_param(material_, "shirts", shirts);
	rs->material_set_param(material_, "trousers", trousers);
	rs->material_set_param(material_, "shirt_from", from);
	rs->material_set_param(material_, "shirt_n", count);
	rs->material_set_param(material_, "trousers_from", from);
	rs->material_set_param(material_, "trousers_n", count);
}

bool AgentRenderer::read_loads(const Dictionary &loads, PackedStringArray &errors) {
	for (int i = 0; i < AGENT_MODELS; i++) {
		Load &l = load_[i];
		const char *key = load_key(i);
		if (!key || l.seats == 0) continue;
		const Dictionary d = loads[key];
		l.shuffle = bool(d["shuffle"]);
		float sum = 0;
		for (const Variant &w : Array(d["bands"])) l.bands.push_back(sum += float(w));
		for (float &b : l.bands) b /= sum;
		if (l.bands.empty() || sum <= 0) errors.push_back(String("agents.json: occupants.load.") + key + " sem faixas");
		const int widest = (HALF_EXACT - l.seats) / l.seats; // the largest stride whose order a half float still holds
		if (l.shuffle && widest < MIN_STRIDE) errors.push_back(String("agents.json: ") + key + " tem lugares demais para a ordem em meia precisão");
		for (int s = MIN_STRIDE; l.shuffle && s <= std::min(l.seats - MIN_STRIDE, widest); s++)
			if (std::gcd(s, l.seats) == 1) l.strides.push_back(s);
		if (l.strides.empty()) l.strides.push_back(1);
	}
	return errors.is_empty();
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
	static const char *const LEVEL_NAMES[LEVELS] = { "cabin", "body" };
	Dictionary out;
	int draws = 0, tris = 0;
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level : { CABIN_LEVEL, BODY_LEVEL }) {
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
	for (auto &levels : draws_)
		for (MultiMeshDraw &m : levels) m.free();
	people_.reset();
	odo_.clear();
	drawn_vehicles_ = drawn_people_ = 0;
}

float AgentRenderer::roll(int id, const Pose &p, float wheel_r) {
	if (size_t(id) >= odo_.size()) odo_.resize(size_t(id) + 1);
	Odometer &o = odo_[size_t(id)];
	float dx = p.x - o.x, dz = p.z - o.z, moved = dx * p.fx + dz * p.fz;
	if (o.seen && std::fabs(moved) < jump_m_ && wheel_r > 0) o.angle = std::fmod(o.angle + moved / wheel_r, TAU_F);
	o.x = p.x;
	o.z = p.z;
	o.seen = true;
	return o.angle;
}

void AgentRenderer::update(const PoseFrame &f, const Vector3 &cam, const PoseSample &at) {
	if (!material_.is_valid() || !meshes_[0][CABIN_LEVEL].valid()) return;
	PackedFloat32Array bufs[AGENT_MODELS][LEVELS];
	const auto model_of = [](int type, int paint) { return type == int(VehicleType::SEDAN) && paint % WAGON_EVERY == 1 ? WAGON_MODEL : type; };
	const float r2 = vehicle_r_ * vehicle_r_, near2 = cabin_r_ * cabin_r_;
	drawn_vehicles_ = 0;
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
		const int level = flat2 + dy * dy > near2 ? BODY_LEVEL : CABIN_LEVEL;
		push_agent(bufs[model_of(type, paint)][level], p, 1.f, float(paint >= LINE_PAINT ? paints_ + paint - LINE_PAINT : paint % paints_), spin + (brake ? BRAKE_FLAG : 0.f), passengers, order, hash_float(rec.id * SEED_SALT));
		drawn_vehicles_++;
	}
	if (f.parked) for (const ParkedRecord &rec : *f.parked) {
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const float dx = rec.at.x - cam.x, dy = rec.at.y - cam.y, dz = rec.at.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > r2 || type < 0 || type >= VEHICLE_TYPES) continue;
		const int level = flat2 + dy * dy > near2 ? BODY_LEVEL : CABIN_LEVEL;
		push_agent(bufs[model_of(type, paint)][level], rec.at, 1.f, float(paint % paints_), 0.f, NOBODY_ABOARD, 0.f, hash_float(rec.id * SEED_SALT));
		drawn_vehicles_++;
	}
	drawn_people_ = people_.update(f, cam, at, people_r_);
	AABB box(Vector3(cam.x - vehicle_r_, cam.y - AABB_HALF_H, cam.z - vehicle_r_), Vector3(vehicle_r_ * 2, AABB_HALF_H * 2, vehicle_r_ * 2));
	// no override: each surface carries its material (the body and the near level's glass)
	RenderingServer *rs = RenderingServer::get_singleton();
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level : { CABIN_LEVEL, BODY_LEVEL }) {
			MultiMeshDraw &d = draws_[i][level];
			PackedFloat32Array &buf = bufs[i][level];
			const int n = int(buf.size() / MM_STRIDE);
			const int room = n > d.capacity || n + 2 * ROOM_STEP < d.capacity ? (n + ROOM_STEP - 1) / ROOM_STEP * ROOM_STEP : d.capacity;
			buf.resize(int64_t(room) * MM_STRIDE);
			d.set(scenario_, meshes_[i][level].rid, RID(), buf, box, true);
			if (d.mm.is_valid()) rs->multimesh_set_visible_instances(d.mm, n);
			shown_[i][level] = n;
		}
}

} // namespace city
