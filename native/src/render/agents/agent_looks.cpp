// What the agents read of agents.json once, at boot: the paints pushed to the "agent" material, how full each type runs, the
// parts' sizes and the meshes built from them; the frame's work is in agent_renderer.cpp.
#include "render/agents/agent_renderer.h"
#include "core/shader_contracts/agents.h"

#include "render/agents/agent_lod.h"
#include "render/agents/agent_meshes.h"
#include "render/shape_reader.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace godot;

namespace city {

namespace {

// agents.gdshader palette array sizes
constexpr int HALF_EXACT = 2048; // whole numbers a half float holds exactly: the order (stride * seats + first) below it
constexpr int MIN_STRIDE = 2;    // a shuffled order skips at least one seat (stride 1 fills them front to back)

PackedColorArray palette(const DataValue &list, int max) {
	PackedColorArray out;
	for (int i = 0; i < list.size(); i++) out.push_back(agent_hex(list[i]));
	if (out.is_empty() || out.size() > max) list.fail(String("between 1 and ") + String::num_int64(max) + String(" colours"));
	while (out.size() < max) out.push_back(out.is_empty() ? Color() : out[0]);
	return out;
}

// agents.json occupants.load key of a vehicle type (the motorbike carries its rider alone).
const char *load_key(int type) {
	if (type < CAR_MODELS || type == WAGON_MODEL) return "car";
	if (type == int(VehicleType::BUS)) return "bus";
	return type == int(VehicleType::TRUCK) ? "truck" : nullptr;
}

} // namespace

bool AgentRenderer::push_palettes(const DataValue &t, const std::vector<uint32_t> &lines, PackedStringArray &errors) {
	const int64_t before = errors.size();
	RenderingServer *rs = RenderingServer::get_singleton();
	const DataValue paints = t["styles"][REGIONAL_STYLE]["paints"];
	// as numbers: a Color array would be turned linear on the way (the parts' vertex colours are not)
	PackedVector3Array raw;
	for (const Color &c : palette(paints, AGENT_PAINTS)) raw.push_back(Vector3(c.r, c.g, c.b));
	for (size_t i = 0; i < lines.size() && i < size_t(AGENT_LINE_PAINTS); i++) { // after them: a bus's line (traffic_state.h LINE_PAINT)
		const Color c = rgb8_to_color(lines[i]);
		raw.push_back(Vector3(c.r, c.g, c.b));
	}
	rs->material_set_param(material_, "paints", raw);
	const DataValue roof = t["car_body"]["black_roof"];
	rs->material_set_param(material_, "black_roof", agent_hex(roof["colour"]));
	rs->material_set_param(material_, "black_roof_share", roof["share"].f());
	paints_ = paints.size();
	return errors.size() == before;
}

bool AgentRenderer::read_loads(const DataValue &loads, PackedStringArray &errors) {
	const int64_t before = errors.size();
	for (int i = 0; i < AGENT_MODELS; i++) {
		Load &l = load_[i];
		const char *key = load_key(i);
		if (!key || l.seats == 0) continue;
		const DataValue d = loads[key];
		l.shuffle = d["shuffle"].flag();
		float sum = 0;
		const DataValue bands = d["bands"];
		l.bands.clear(); // a second read is the table again
		l.strides.clear();
		for (int b = 0; b < bands.size(); b++) l.bands.push_back(sum += bands[b].f());
		for (float &b : l.bands) b /= sum;
		if (l.bands.empty() || sum <= 0) d.fail("no bands");
		const int widest = (HALF_EXACT - l.seats) / l.seats; // the largest stride whose order a half float still holds
		if (l.shuffle && widest < MIN_STRIDE) d.fail("too many seats for the order a half float holds");
		for (int s = MIN_STRIDE; l.shuffle && s <= std::min(l.seats - MIN_STRIDE, widest); s++)
			if (std::gcd(s, l.seats) == 1) l.strides.push_back(s);
		if (l.strides.empty()) l.strides.push_back(1);
	}
	return errors.size() == before;
}

namespace {
constexpr uint32_t WRECK_SEED = 2654435761u; // each model buckled its own way

// agents.json's table of a vehicle that is no car.
const char *vehicle_key(int type) {
	if (type == int(VehicleType::MOTO)) return "moto";
	if (type == int(VehicleType::BICYCLE)) return "bicycle";
	return type == int(VehicleType::BUS) ? "bus" : "truck";
}
} // namespace

// The part builders' sizes: every block of AGENT_SHAPE_BLOCKS, each field under its own name (render/shape_reader.h).
bool AgentRenderer::read_shapes(const Dictionary &table, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader top{ errors, "agents.json" };
	DataReader r = top.in("shapes");
	const Dictionary t = top.table(table, "shapes");
	AgentShapes &out = shapes_;
	AGENT_SHAPE_BLOCKS(SHAPE_READ_BLOCK)
	return errors.size() == before;
}

bool AgentRenderer::build_meshes(const DataValue &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	AgentTrim trim = read_trim(t["trim"]);
	trim.shapes = &shapes_;
	const DataValue cars = t["cars"], body = t["car_body"], inside = t["cars_cabin"];
	cars.list(CAR_MODELS + 1); // one car a style, and the wagon
	CabinLook look;
	look.shapes = &shapes_;
	if (!look.load(t["cabin"])) t["cabin"].fail("a cabin row does not read");
	for (const char *key : { "wheel_sides", "stations", "arch_steps", "end_steps", "end_roll_steps" }) // one entry per level and one for the fleet's shadow (agent_meshes.h)
		body[key].list(LEVELS + 1);
	if (errors.size() != before) return false;
	const DataValue wheel_sides = body["wheel_sides"], far_shadow = t["far_shadow"], lod = t["lod"];
	const float cabin_error = lod["cabin_error_m"].f(), body_error = lod["body_error_m"].f(), far_error = lod["far_error_m"].f();
	MeshBuilder hull;
	build_car(hull, cars[0], body, inside, nullptr, trim, AGENT_HULL_LEVEL);
	fleet_shadow_.init(scenario_, shadow_, simplify_parts(hull, far_error), far_shadow["sector_m"].f(), far_shadow["inset"].f(), AGENT_MODELS);
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < LEVELS; level++) {
			MeshBuilder mb;
			const bool car = i < CAR_MODELS || i == WAGON_MODEL;
			const DataValue d = car ? cars[i == WAGON_MODEL ? CAR_MODELS : i] : t[vehicle_key(i)];
			const int sides = wheel_sides[level].whole();
			const CabinLook *l = level == CABIN_LEVEL ? &look : nullptr;
			look.spots = &seats_[i];
			int seats = 0;
			if (car) seats = build_car(mb, d, body, inside, l, trim, level);
			else if (i == int(VehicleType::MOTO)) seats = build_moto(mb, d, sides, trim);
			else if (i == int(VehicleType::BICYCLE)) seats = build_bicycle(mb, d, sides, trim);
			else if (i == int(VehicleType::BUS)) seats = build_bus(mb, d, sides, l, trim);
			else seats = build_truck(mb, d, sides, l, trim);
			wheel_r_[i] = agent_tyre(d["tyre"]).x; // the motorbike's: its rear wheel
			wheelbase_[i] = d["wheelbase_m"].f();
			if (level == CABIN_LEVEL && (i == int(VehicleType::MOTO) || i == int(VehicleType::BICYCLE))) {
				astride_[i] = true;
				seats_[i].push_back({ agent_v3(d["rider"]["hip"]), i == int(VehicleType::MOTO) ? SeatPose::MOTO : SeatPose::BICYCLE, DRIVER_SEAT });
			}
			if (level == FAR_LEVEL) fleet_shadow_.set_model(i, mb);
			if (level == BODY_LEVEL) wrecks_[size_t(i)] = agent_wreck(shapes_, mb, uint32_t(i + 1) * WRECK_SEED);
			if (level == CABIN_LEVEL) {
				const AABB box = mesh_outline(mb);
				reach_m_[i] = box.position.abs().max(box.get_end().abs()).length();
			}
			if (seats < 0) d.fail("its cabin is inside out (its back ahead of its front)");
			if (level == CABIN_LEVEL) {
				load_[i].seats = seats;
				commit_mesh(i, MID_DRAW, simplify_parts(mb, cabin_error));
				commit_mesh(i, level, mb);
			} else {
				commit_mesh(i, level, level == BODY_LEVEL ? simplify_parts(mb, body_error) : simplify_whole(mb, far_error)); // far, a shell of few pixels: the parts need not keep their borders
			}
		}
	return errors.size() == before;
}

} // namespace city
