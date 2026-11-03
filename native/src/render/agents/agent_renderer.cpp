// Builds the agent meshes once, pushes the palettes to the "agent" material and refills the instance buffers every
// frame: transform from the interpolated pose (+Z forward, +X left), INSTANCE_CUSTOM = (paint r, g, b, wheel roll
// angle in radians + BRAKE_FLAG when braking) for vehicles (the Compatibility renderer keeps custom data as halfs) and
// the instance colour's alpha a seed of the vehicle (who sits in it); people go to PeopleRenderer.
#include "render/agents/agent_renderer.h"

#include "render/agents/agent_meshes.h"

#include "core/random.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float AABB_HALF_H = 500.f;
constexpr int CAR_MODELS = int(VehicleType::MOTO); // sedan .. pickup come from agents.json "cars"
constexpr uint32_t LOOK_TYPE_MASK = 0xFFu, LOOK_PAINT_SHIFT = 8, LOOK_BRAKE_SHIFT = 16;
constexpr float BRAKE_FLAG = 8.f; // added to the roll angle (0..TAU) when the brake light is on; agents.gdshader
constexpr uint32_t SEED_SALT = 0x5ea7u;
// agents.gdshader palette array sizes
constexpr int CLOTHES_MAX = 16, HAIR_MAX = 8, SKIN_MAX = 8;

PackedColorArray palette(const Variant &list, int max, const char *name, PackedStringArray &errors) {
	PackedColorArray out;
	for (const Variant &c : Array(list)) out.push_back(agent_hex(c));
	if (out.is_empty() || out.size() > max) errors.push_back(String("agents.json: paleta ") + name + " vazia ou longa demais");
	while (out.size() < max) out.push_back(out.is_empty() ? Color() : out[0]);
	return out;
}

} // namespace

bool AgentRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("agent", errors);
	glass_ = ctx.material("agent_glass", errors);
	Dictionary t = ctx.table("agents", errors);
	if (!errors.is_empty()) return false;
	AgentTrim trim = read_trim(t["trim"]);
	Array cars = t["cars"];
	Dictionary body = t["car_body"];
	if (cars.size() != CAR_MODELS) {
		errors.push_back("agents.json: esperado um carro por estilo");
		return false;
	}
	CabinLook cabin;
	if (!cabin.load(t["cabin"])) {
		errors.push_back("agents.json: cabin incompleto");
		return false;
	}
	int sides = int(body["wheel_sides"]);
	RenderingServer *rs = RenderingServer::get_singleton();
	for (int i = 0; i < VEHICLE_TYPES; i++) {
		MeshBuilder mb;
		Dictionary d;
		if (i < CAR_MODELS) build_car(mb, d = cars[i], body, cabin, trim);
		else if (i == int(VehicleType::MOTO)) build_moto(mb, d = t["moto"], sides, trim);
		else if (i == int(VehicleType::BUS)) build_bus(mb, d = t["bus"], sides, trim);
		else build_truck(mb, d = t["truck"], sides, trim);
		wheel_r_[i] = float(d["wheel"]);
		// a car's glass is see-through over its cabin; the heavy vehicles have no cabin yet and keep it opaque
		MeshBuilder shell, glass;
		if (i < CAR_MODELS) split_tag(mb, AgentTag::GLASS, shell, glass);
		const MeshBuilder &opaque = i < CAR_MODELS ? shell : mb;
		meshes_[i].reset();
		meshes_[i].rid = rs->mesh_create();
		opaque.commit_to(meshes_[i].rid);
		rs->mesh_surface_set_material(meshes_[i].rid, 0, material_);
		if (!glass.empty()) {
			glass.commit_to(meshes_[i].rid);
			rs->mesh_surface_set_material(meshes_[i].rid, 1, glass_);
		}
	}
	if (!push_palettes(t, errors)) return false;
	for (const Variant &c : Array(t["paints"])) paints_.push_back(agent_hex(c));
	vehicle_r_ = float(t["vehicle_radius_m"]);
	people_r_ = float(t["people_radius_m"]);
	jump_m_ = float(t["wheel_jump_m"]);
	if (paints_.empty()) errors.push_back("agents.json: sem pinturas");
	if (!errors.is_empty()) return false;
	return people_.init(ctx, t["people"], errors);
}

bool AgentRenderer::push_palettes(const Dictionary &t, PackedStringArray &errors) {
	const Dictionary people = t["people"], occupants = t["occupants"];
	RenderingServer *rs = RenderingServer::get_singleton();
	const Array clothes = people["clothes"], hair = people["hair"], skin = occupants["skin"], fill = occupants["seat_fill"];
	rs->material_set_param(material_, "clothes", palette(clothes, CLOTHES_MAX, "clothes", errors));
	rs->material_set_param(material_, "hair", palette(hair, HAIR_MAX, "hair", errors));
	rs->material_set_param(material_, "skin", palette(skin, SKIN_MAX, "skin", errors));
	rs->material_set_param(material_, "clothes_n", int(clothes.size()));
	rs->material_set_param(material_, "hair_n", int(hair.size()));
	rs->material_set_param(material_, "skin_n", int(skin.size()));
	if (fill.size() != CABIN_SEATS) errors.push_back("agents.json: occupants.seat_fill precisa de um valor por banco");
	else rs->material_set_param(material_, "seat_fill", Vector4(float(fill[0]), float(fill[1]), float(fill[2]), float(fill[3])));
	return errors.is_empty();
}

void AgentRenderer::reset() {
	for (MultiMeshDraw &d : draws_) d.free();
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
	if (!material_.is_valid() || !meshes_[0].valid()) return;
	PackedFloat32Array bufs[VEHICLE_TYPES];
	const float r2 = vehicle_r_ * vehicle_r_;
	drawn_vehicles_ = 0;
	for (const AgentRecord &rec : f.vehicles) {
		Pose p = record_pose(rec, at);
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const bool brake = (rec.look & (1u << LOOK_BRAKE_SHIFT)) != 0;
		if (type < 0 || type >= VEHICLE_TYPES) continue;
		float spin = roll(int(rec.id), p, wheel_r_[type]);
		float dx = p.x - cam.x, dz = p.z - cam.z;
		if (dx * dx + dz * dz > r2) continue;
		const Color &c = paints_[size_t(paint) % paints_.size()];
		push_agent(bufs[type], p, 1.f, c.r, c.g, c.b, spin + (brake ? BRAKE_FLAG : 0.f), hash_float(rec.id * SEED_SALT));
		drawn_vehicles_++;
	}
	drawn_people_ = people_.update(f, cam, at, people_r_);
	AABB box(Vector3(cam.x - vehicle_r_, cam.y - AABB_HALF_H, cam.z - vehicle_r_), Vector3(vehicle_r_ * 2, AABB_HALF_H * 2, vehicle_r_ * 2));
	// no override: each surface carries its material (the body and a car's glass)
	for (int i = 0; i < VEHICLE_TYPES; i++) draws_[i].set(scenario_, meshes_[i].rid, RID(), bufs[i], box, true);
}

} // namespace city
