// Lorry and motorbike (Roadcraft 4.0 render/vehicleModels.ts, in boxes and panes) and the parts the heavy vehicles share
// (the bus is bus_body.cpp). The lorry: a hollow cab over the front axle, glazed in front and on both sides between its
// pillars, with its cab inside (agent_cabin: two seats, the engine cover between them, the driver at the wheel), on a
// chassis carrying the box body; the motorbike carries its rider astride, dressed like the people in cars, in a helmet.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float BUS_LAMP_Y = 0.62f, LAMP_W = 0.32f, LAMP_H = 0.14f, LAMP_X_PAD = 0.35f, MIRROR_ARM = 0.35f, MIRROR_W = 0.08f, MIRROR_H = 0.32f;
constexpr float MIRROR_D = 0.12f, MIRROR_Z_BACK = 0.25f, DOOR_SPLIT_W = 0.04f;
constexpr float MOTO_SEAT_H = 0.12f;
// motorbike proportions (shares of the axle distance or of the body half width)
constexpr float MOTO_TAIL = 0.8f, MOTO_NOSE = 0.7f, MOTO_SEAT_W = 0.8f, MOTO_BAR_W = 2.f, MOTO_BAR_Z = 0.6f, MOTO_BAR_H = 1.5f;
constexpr float MOTO_LAMP_W = 0.4f, MOTO_LAMP_D = 0.03f, HELMET_TALL = 1.1f, HELMET_LONG = 1.15f;
constexpr float CHASSIS_SHARE = 0.8f; // lorry chassis rails against the body width
constexpr float CAB_GLASS_LOW = 0.1f, CAB_GLASS_PAD = 0.12f, CHASSIS_Y = 0.55f, BUMPER_H = 0.3f, POST_W = 0.08f;
// lorry: steps, mudguards, side guards, fuel tank, rear door handles
constexpr float STEP_H = 0.06f, STEP_D = 0.35f, STEP_OUT = 0.06f, GUARD_PAD = 0.06f, GUARD_T = 0.05f, SIDE_GUARD_H = 0.25f;
constexpr float SIDE_GUARD_Y = 0.45f, TANK_L = 1.1f, TANK_H = 0.5f, TANK_D = 0.45f, TANK_Y = 0.5f, GRAB_W = 0.05f, HANDLE_H = 0.9f;
constexpr float GRAB_Y = 1.6f, HANDLE_X = 0.15f, CAB_DOOR_SEAM = 0.03f;
constexpr float WHEEL_GAP_M = 0.02f; // the cab's floor pan clear of the wheels' inner faces

struct Lorry {
	float L = 0, W = 0, H = 0, r = 0, hw = 0, hl = 0, cab_back = 0, cab_bottom = 0, belt = 0, cab_top = 0, box_bottom = 0, floor = 0, wall = 0, ww = 0, inset = 0;
	std::vector<float> axles;
};

Lorry read_lorry(const Dictionary &d) {
	Lorry k;
	k.L = d["length"]; k.W = d["width"]; k.H = d["height"]; k.r = d["wheel"]; k.ww = d["wheel_w"]; k.inset = d["wheel_inset_m"];
	k.hw = k.W * 0.5f; k.hl = k.L * 0.5f; k.cab_back = k.hl - k.L * float(d["cab_share"]);
	k.cab_bottom = d["cab_bottom"]; k.belt = d["belt"]; k.cab_top = d["cab_top"]; k.box_bottom = d["box_bottom"];
	k.floor = d["cab_floor"]; k.wall = d["wall_m"];
	k.axles = heavy_axles(d, k.L);
	return k;
}

// The cab: hollow under the belt over its floor, the back wall, the roof band, the glass and its pillars.
void cab(MeshBuilder &mb, const Lorry &k, const AgentTrim &trim) {
	Tub t;
	t.half_w = k.hw; t.wall = k.wall; t.y0 = k.cab_bottom; t.y1 = k.belt; t.z0 = k.cab_back; t.z1 = k.hl; t.floor = k.floor;
	t.axles = k.axles; // the front wheel's arch under the cab (the others are behind it)
	t.arch_r = k.r + HEAVY_ARCH_PAD_M;
	t.arch_top = 2 * k.r + HEAVY_ARCH_PAD_M;
	t.pan_half_w = k.hw - k.inset - k.ww - WHEEL_GAP_M;
	agent_tub(mb, t, trim);
	agent_box(mb, Vector3(-k.hw, k.cab_bottom, k.hl - k.wall), Vector3(k.hw, k.belt, k.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-k.hw, k.cab_bottom, k.cab_back), Vector3(k.hw, k.cab_top, k.cab_back + CAB_GLASS_PAD), AGENT_WHITE, AgentTag::PAINT);
	const float top = k.cab_top - CAB_GLASS_LOW, zg = k.hl - HEAVY_GLASS_INSET_M, xg = k.hw - HEAVY_PILLAR_M, door = k.cab_back + CAB_GLASS_PAD * 2;
	agent_box(mb, Vector3(-k.hw, top, k.cab_back), Vector3(k.hw, k.cab_top, k.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_pane(mb, Vector3(-xg, k.belt, zg), Vector3(xg, k.belt, zg), Vector3(xg, top, zg), Vector3(-xg, top, zg), Vector3(0, 0, 1), trim);
	for (int side : { -1, 1 }) {
		const float x = side * (k.hw - HEAVY_GLASS_INSET_M), z0 = k.cab_back + CAB_GLASS_PAD, z1 = k.hl - HEAVY_PILLAR_M;
		agent_pane(mb, Vector3(x, k.belt, z0), Vector3(x, k.belt, z1), Vector3(x, top, z1), Vector3(x, top, z0), Vector3(side, 0, 0), trim);
		agent_box(mb, Vector3(side * xg, k.belt, z1), Vector3(side * k.hw, top, k.hl), AGENT_WHITE, AgentTag::PAINT);
		// the door's rear edge, from the step to the roof
		agent_box(mb, Vector3(side * (k.hw - HEAVY_PROUD_M), k.cab_bottom, door), Vector3(side * (k.hw + HEAVY_PROUD_M), top, door + CAB_DOOR_SEAM), trim.blackout,
				AgentTag::FIXED);
	}
}

int cab_inside(MeshBuilder &mb, const Lorry &k, const Dictionary &in, const CabinLook &l) {
	Cabin c;
	c.floor = k.floor;
	c.roof = k.cab_top - CAB_GLASS_LOW;
	c.half_w = k.hw - k.wall;
	c.z_front = k.hl - k.wall;
	c.z_back = k.cab_back + CAB_GLASS_PAD;
	c.side_top = c.front_top = k.belt;
	c.back_top = c.roof;
	const Vector3 dash = agent_v3(in["dash_m"]); // depth, height, width
	c.dash_d = dash.x; c.dash_h = dash.y; c.dash_w = dash.z;
	c.paint = read_cabin_paint(in);
	const float z = k.cab_back + float(in["seat_from_back_m"]), x = k.hw - float(in["seat_from_side_m"]);
	std::vector<Seat> seats;
	seat_rows(seats, z, z, 1.f, { x, -x }, k.floor, l.cab);
	seats[0].driver = true;
	const Vector3 cover = agent_v3(in["console_m"]); // the engine cover between the seats: width, height, depth
	const float cz = z + float(in["console_ahead_m"]);
	agent_box(mb, Vector3(-cover.x * 0.5f, k.floor, cz - cover.z * 0.5f), Vector3(cover.x * 0.5f, k.floor + cover.y, cz + cover.z * 0.5f), c.paint.dash,
			AgentTag::FIXED);
	return agent_cabin(mb, c, seats, l);
}

// Chassis rails, bumper, box body with its corner posts, steps, mudguards, side guards, tank, door seam and handles.
void running_gear(MeshBuilder &mb, const Lorry &k, const AgentTrim &trim) {
	const float hw = k.hw, hl = k.hl, r = k.r;
	agent_box(mb, Vector3(-hw * CHASSIS_SHARE, CHASSIS_Y, -hl), Vector3(hw * CHASSIS_SHARE, k.box_bottom, hl - HEAVY_PROUD_M), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-hw, k.cab_bottom - BUMPER_H, hl - BUMPER_H), Vector3(hw, k.cab_bottom, hl + HEAVY_PROUD_M), trim.grille, AgentTag::FIXED);
	const float box_front = k.cab_back - HEAVY_ARCH_PAD_M * 2;
	agent_box(mb, Vector3(-hw, k.box_bottom, -hl), Vector3(hw, k.H, box_front), trim.truck_box, AgentTag::FIXED);
	for (float z : { -hl, box_front - POST_W })
		for (int side : { -1, 1 })
			agent_box(mb, Vector3(side * hw - HEAVY_PROUD_M - POST_W * (side > 0), k.box_bottom, z),
					Vector3(side * hw + HEAVY_PROUD_M + POST_W * (side < 0), k.H, z + POST_W), trim.chrome, AgentTag::FIXED);
	for (int side : { -1, 1 }) {
		const float x = side * hw;
		agent_box(mb, Vector3(x - side * STEP_D * 0.5f - STEP_OUT, k.cab_bottom - BUMPER_H, (hl + k.cab_back) * 0.5f),
				Vector3(x + STEP_OUT * side, k.cab_bottom - BUMPER_H + STEP_H, hl - BUMPER_H), trim.chrome, AgentTag::FIXED);
		for (float z : k.axles) {
			const float zr = r + GUARD_PAD;
			agent_box(mb, Vector3(x - side * (k.inset + k.ww), 2 * r + GUARD_PAD, z - zr), Vector3(x, 2 * r + GUARD_PAD + GUARD_T, z + zr),
					trim.blackout, AgentTag::FIXED);
		}
		agent_box(mb, Vector3(x - side * POST_W, SIDE_GUARD_Y, k.axles[1] + r + GUARD_PAD), Vector3(x, SIDE_GUARD_Y + SIDE_GUARD_H, box_front - r), trim.chrome,
				AgentTag::FIXED);
		agent_box(mb, Vector3(-HANDLE_X * side - GRAB_W * 0.5f, GRAB_Y, -hl - HEAVY_PROUD_M * 2), Vector3(-HANDLE_X * side + GRAB_W * 0.5f, GRAB_Y + HANDLE_H, -hl),
				trim.chrome, AgentTag::FIXED);
	}
	const float tank = k.axles[0] - r - GUARD_PAD - TANK_L;
	agent_box(mb, Vector3(hw - TANK_D, TANK_Y, tank - TANK_L), Vector3(hw, TANK_Y + TANK_H, tank), trim.chrome, AgentTag::FIXED);
	agent_box(mb, Vector3(-DOOR_SPLIT_W * 0.5f, k.box_bottom, -hl - HEAVY_PROUD_M * 2), Vector3(DOOR_SPLIT_W * 0.5f, k.H, -hl), trim.blackout, AgentTag::FIXED);
}

} // namespace

std::vector<float> heavy_axles(const Dictionary &d, float L) {
	std::vector<float> zs;
	Array a = d["axles"];
	for (int i = 0; i < a.size(); i++) zs.push_back(float(a[i]) * L);
	return zs;
}

void heavy_wheels(MeshBuilder &mb, const Dictionary &d, const std::vector<float> &zs, float W, int sides, const AgentTrim &trim) {
	const float r = d["wheel"], ww = d["wheel_w"], x = W * 0.5f - float(d["wheel_inset_m"]);
	for (float z : zs)
		for (int side : { -1, 1 }) agent_wheel(mb, Vector3(side * x, r, z), r, ww, sides, side, trim);
}

void heavy_lamps(MeshBuilder &mb, float W, float L, float tail_y, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		float x = side * (W * 0.5f - LAMP_X_PAD);
		agent_box(mb, Vector3(x - LAMP_W * 0.5f, BUS_LAMP_Y, L * 0.5f - HEAVY_PROUD_M), Vector3(x + LAMP_W * 0.5f, BUS_LAMP_Y + LAMP_H, L * 0.5f + HEAVY_PROUD_M),
				trim.headlamp, AgentTag::HEAD);
		agent_box(mb, Vector3(x - LAMP_W * 0.5f, tail_y, -L * 0.5f - HEAVY_PROUD_M), Vector3(x + LAMP_W * 0.5f, tail_y + LAMP_H, -L * 0.5f + HEAVY_PROUD_M),
				trim.tail, AgentTag::TAIL);
	}
}

void heavy_mirrors(MeshBuilder &mb, float hw, float z, float y, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		float x0 = side * hw, x1 = side * (hw + MIRROR_ARM);
		agent_box(mb, Vector3(std::min(x0, x1), y + MIRROR_H - MIRROR_W, z - MIRROR_W * 0.5f), Vector3(std::max(x0, x1), y + MIRROR_H, z + MIRROR_W * 0.5f),
				trim.blackout, AgentTag::FIXED);
		agent_box(mb, Vector3(x1 - MIRROR_W * 0.5f, y, z - MIRROR_D * 0.5f), Vector3(x1 + MIRROR_W * 0.5f, y + MIRROR_H, z + MIRROR_D * 0.5f), trim.blackout,
				AgentTag::FIXED);
	}
}

int build_truck(MeshBuilder &mb, const Dictionary &d, int sides, const CabinLook *look, const AgentTrim &trim) {
	const Lorry k = read_lorry(d);
	const int passengers = look ? cab_inside(mb, k, d["interior"], *look) : 0;
	cab(mb, k, trim);
	running_gear(mb, k, trim);
	heavy_mirrors(mb, k.hw, k.hl - MIRROR_Z_BACK, k.belt, trim);
	heavy_lamps(mb, k.W, k.L, k.box_bottom, trim);
	heavy_wheels(mb, d, k.axles, k.W, sides, trim);
	return passengers;
}

int build_moto(MeshBuilder &mb, const Dictionary &d, int sides, const Person &rider, const AgentTrim &trim) {
	const float r = float(d["wheel"]), ww = float(d["wheel_w"]), az = float(d["axle_z"]), helmet = float(d["helmet_r"]);
	Array b = d["body"];
	const float bw = float(b[0]) * 0.5f, by0 = float(b[1]), by1 = float(b[2]);
	const float tail = -az * MOTO_TAIL, nose = az * MOTO_NOSE, lamp_w = bw * MOTO_LAMP_W;
	for (float z : { -az, az }) agent_wheel(mb, Vector3(0, r, z), r, ww, sides, 1, trim);
	agent_box(mb, Vector3(-bw, by0, tail), Vector3(bw, by1, nose), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-bw * MOTO_SEAT_W, by1, -az * MOTO_NOSE), Vector3(bw * MOTO_SEAT_W, by1 + MOTO_SEAT_H, 0), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-bw * MOTO_BAR_W, by1 + MOTO_SEAT_H, az * MOTO_BAR_Z), Vector3(bw * MOTO_BAR_W, by1 + MOTO_SEAT_H * MOTO_BAR_H, nose), trim.blackout,
			AgentTag::FIXED);
	agent_box(mb, Vector3(-lamp_w, by1 - LAMP_H, nose), Vector3(lamp_w, by1, nose + MOTO_LAMP_D), trim.headlamp, AgentTag::HEAD);
	agent_box(mb, Vector3(-lamp_w, by1 - LAMP_H, tail - MOTO_LAMP_D), Vector3(lamp_w, by1, tail), trim.tail, AgentTag::TAIL);
	// the rider astride (agents.json moto.rider): on the saddle, leaning to the grips, feet by the engine, in a helmet
	const Dictionary pose = d["rider"];
	Figure f;
	f.hip = agent_v3(pose["hip"]);
	f.shoulders = agent_v3(pose["shoulders"]);
	f.knee = agent_v3(pose["knee"]);
	f.ankle = agent_v3(pose["ankle"]);
	f.hand = agent_v3(pose["hand"]);
	f.holds = true;
	f.helmet = Vector3(helmet, helmet * HELMET_TALL, helmet * HELMET_LONG);
	const int64_t from = mb.v.size();
	agent_person(mb, rider, f);
	seat_order(mb, from, DRIVER_ORDER);
	return 0;
}

} // namespace city
