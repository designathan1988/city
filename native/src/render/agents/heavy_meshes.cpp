// Truck and motorbike, and the parts the heavy vehicles share (the bus is bus_body.cpp). The truck (agents.json "truck",
// its sources in the _doc): a hollow cab over the front wheel, glazed in front and on both sides between its pillars, with
// its cab inside (agent_cabin: the driver at the wheel, toes on the pedals, a passenger beside), the wind deflector on its
// roof up to the box; the chassis under the box body behind it, the fuel tank on the chassis. The motorbike carries its
// rider astride, dressed like the people in cars, in a helmet.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float LITRES_PER_M3 = 1000.f;
constexpr float MIRROR_W = 0.08f, MIRROR_H = 0.32f, MIRROR_D = 0.12f, DOOR_SPLIT_W = 0.04f, CAB_DOOR_SEAM = 0.03f;
constexpr float TAIL_LENS_H_M = 0.05f;
// motorbike proportions (shares of the axle distance or of the body half width)
constexpr float MOTO_LAMP_D = 0.03f;

struct Truck {
	float L = 0, hl = 0, chw = 0, cab_back = 0, cab_bottom = 0, floor = 0, belt = 0, glass_top = 0, cab_top = 0, wall = 0;
	float box_front = 0, box_hw = 0, box_bottom = 0, box_top = 0, rails_hw = 0;
	Running g;
};

Truck read_truck(const Dictionary &d) {
	Truck k;
	k.L = d["length"]; k.hl = k.L * 0.5f; k.wall = d["wall_m"];
	k.g = heavy_running(d, k.L);
	k.chw = float(d["cab_w"]) * 0.5f;
	k.rails_hw = float(d["rails_m"]) * 0.5f;
	k.cab_back = k.g.axles[0] - float(d["body_from_axle_m"]);
	k.cab_bottom = 2 * k.g.r + HEAVY_ARCH_PAD_M; // over the front wheel
	k.floor = k.cab_bottom + k.wall;
	k.cab_top = d["cab_top"];
	const Vector2 lines = agent_v2(d["cab_lines"]); // the belt and the glass's top, shares of the cab's height
	k.belt = k.cab_bottom + (k.cab_top - k.cab_bottom) * lines.x;
	k.glass_top = k.cab_bottom + (k.cab_top - k.cab_bottom) * lines.y;
	const Array box = d["box_m"]; // length from the back, width, bottom (the chassis' platform), top
	k.box_front = -k.hl + float(box[0]);
	k.box_hw = float(box[1]) * 0.5f;
	k.box_bottom = box[2];
	k.box_top = box[3];
	return k;
}

// The cab: hollow under the belt over its floor, the blind back, the roof over the glass, the windscreen and side glass
// between their pillars, the door's back edge; the bumper under the cab's front down to the axle's height.
void cab(MeshBuilder &mb, const Truck &k, const AgentTrim &trim) {
	Tub t;
	t.half_w = k.chw; t.wall = k.wall; t.y0 = k.cab_bottom; t.y1 = k.belt; t.z0 = k.cab_back; t.z1 = k.hl; t.floor = k.floor;
	t.axles = k.g.axles;
	t.arch_r = k.g.r + HEAVY_ARCH_PAD_M;
	t.arch_top = k.cab_bottom; // the cab clears the wheel: nothing to cut
	t.pan_half_w = k.rails_hw;
	agent_tub(mb, t, trim);
	agent_box(mb, Vector3(-k.chw, k.cab_bottom, k.hl - k.wall), Vector3(k.chw, k.belt, k.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-k.chw, k.cab_bottom, k.cab_back), Vector3(k.chw, k.cab_top, k.cab_back + k.wall), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-k.chw, k.glass_top, k.cab_back), Vector3(k.chw, k.cab_top, k.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-k.chw, k.g.r, k.hl - k.wall), Vector3(k.chw, k.cab_bottom, k.hl + HEAVY_PROUD_M), trim.grille, AgentTag::FIXED);
	const float zg = k.hl - HEAVY_GLASS_INSET_M, xg = k.chw - HEAVY_PILLAR_M, door = k.cab_back + HEAVY_PILLAR_M;
	agent_pane(mb, Vector3(-xg, k.belt, zg), Vector3(xg, k.belt, zg), Vector3(xg, k.glass_top, zg), Vector3(-xg, k.glass_top, zg), Vector3(0, 0, 1), trim);
	for (int side : { -1, 1 }) {
		const float x = side * (k.chw - HEAVY_GLASS_INSET_M), z1 = k.hl - HEAVY_PILLAR_M;
		agent_pane(mb, Vector3(x, k.belt, door), Vector3(x, k.belt, z1), Vector3(x, k.glass_top, z1), Vector3(x, k.glass_top, door), Vector3(side, 0, 0), trim);
		agent_box(mb, Vector3(side * xg, k.belt, z1), Vector3(side * k.chw, k.glass_top, k.hl), AGENT_WHITE, AgentTag::PAINT);
		agent_box(mb, Vector3(side * (k.chw - k.wall), k.belt, k.cab_back), Vector3(side * k.chw, k.glass_top, door), AGENT_WHITE, AgentTag::PAINT);
		agent_box(mb, Vector3(side * (k.chw - HEAVY_PROUD_M), k.cab_bottom, door), Vector3(side * (k.chw + HEAVY_PROUD_M), k.glass_top, door + CAB_DOOR_SEAM),
				trim.blackout, AgentTag::FIXED);
	}
}

// The wind deflector: a wedge from the roof's front edge up to the box's top at the cab's back, as wide as the cab.
void deflector(MeshBuilder &mb, const Truck &k) {
	const float x = k.chw, y0 = k.cab_top, y1 = k.box_top, zb = k.cab_back, zf = k.hl;
	const Vector2 paint = agent_tag(AgentTag::PAINT);
	mb.quad(Vector3(-x, y1, zb), Vector3(x, y1, zb), Vector3(x, y0, zf), Vector3(-x, y0, zf), AGENT_WHITE, Vector3(0, zf - zb, y1 - y0), nullptr, paint);
	mb.quad(Vector3(-x, y0, zb), Vector3(x, y0, zb), Vector3(x, y1, zb), Vector3(-x, y1, zb), AGENT_WHITE, Vector3(0, 0, -1), nullptr, paint);
	for (int side : { -1, 1 })
		mb.quad(Vector3(side * x, y0, zb), Vector3(side * x, y0, zf), Vector3(side * x, y1, zb), Vector3(side * x, y1, zb), AGENT_WHITE, Vector3(side, 0, 0),
				nullptr, paint);
}

// Two seats either side of the cab's middle, the driver's on +X with their toes on the pedals, the dashboard across.
int cab_inside(MeshBuilder &mb, const Truck &k, const Dictionary &in, const CabinLook &l) {
	Cabin c;
	c.floor = k.floor;
	c.roof = k.glass_top;
	c.half_w = k.chw - k.wall;
	c.z_front = k.hl - k.wall;
	c.z_back = k.cab_back + k.wall;
	c.side_top = c.front_top = k.belt;
	c.back_top = c.roof;
	c.paint = read_cabin_paint(in);
	const float x = (c.half_w - l.lining_inset_m) * 0.5f;
	std::vector<Seat> seats;
	seat_rows(seats, pedal_hip_z(c, l), pedal_hip_z(c, l), 1.f, { x, -x }, c.floor, l.cab);
	seats[0].driver = true;
	dash_for(c, seats[0], l, 2 * (c.half_w - l.lining_inset_m));
	return agent_cabin(mb, c, seats, l);
}

// The chassis (rails as wide as the sheet's, from the axles' height to the platform), the box with its rear doors' split,
// the fuel tank on the chassis behind the cab (its volume over its length and depth).
void chassis_and_box(MeshBuilder &mb, const Truck &k, const Dictionary &d, const AgentTrim &trim) {
	agent_box(mb, Vector3(-k.rails_hw, k.g.r, -k.hl), Vector3(k.rails_hw, k.box_bottom, k.cab_back), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-k.box_hw, k.box_bottom, -k.hl), Vector3(k.box_hw, k.box_top, k.box_front), trim.truck_box, AgentTag::FIXED);
	agent_box(mb, Vector3(-DOOR_SPLIT_W * 0.5f, k.box_bottom, -k.hl - HEAVY_PROUD_M), Vector3(DOOR_SPLIT_W * 0.5f, k.box_top, -k.hl), trim.blackout,
			AgentTag::FIXED);
	const Vector2 tank = agent_v2(d["fuel_tank_m"]); // length, depth
	const float h = float(d["fuel_l"]) / LITRES_PER_M3 / (tank.x * tank.y);
	agent_box(mb, Vector3(k.rails_hw, k.box_bottom - h, k.cab_back - tank.x), Vector3(k.rails_hw + tank.y, k.box_bottom, k.cab_back), trim.chrome,
			AgentTag::FIXED);
}

} // namespace

Running heavy_running(const Dictionary &d, float L) {
	Running g;
	const Vector2 tyre = agent_tyre(d["tyre"]), track = agent_v2(d["track_m"]);
	g.r = tyre.x;
	g.w = tyre.y;
	g.front_track = track.x;
	g.rear_track = track.y;
	g.dual = d["dual_m"];
	float z = L * 0.5f - float(d["front_overhang_m"]);
	g.axles.push_back(z);
	for (const Variant &wb : Array(d["wheelbase_m"])) g.axles.push_back(z -= float(wb));
	return g;
}

void heavy_wheels(MeshBuilder &mb, const Running &g, int sides, const AgentTrim &trim) {
	for (size_t i = 0; i < g.axles.size(); i++) {
		const float z = g.axles[i];
		for (int side : { -1, 1 }) {
			if (i == 0) {
				agent_wheel(mb, Vector3(side * g.front_track * 0.5f, g.r, z), g.r, g.w, sides, side, trim);
				continue;
			}
			for (int twin : { -1, 1 }) // the twin tyres: the hubcap on the outer one's face
				agent_wheel(mb, Vector3(side * (g.rear_track + twin * g.dual) * 0.5f, g.r, z), g.r, g.w, sides, twin > 0 ? side : -side, trim);
		}
	}
}

void heavy_lamps(MeshBuilder &mb, const Dictionary &lamps, float W, float L, const AgentTrim &trim) {
	const Array head = lamps["head"], tail = lamps["tail"]; // [centre height, outer edge in from the side, w, h]
	for (int side : { -1, 1 })
		for (int end : { 1, -1 }) {
			const Array &l = end > 0 ? head : tail;
			const float y = l[0], w = l[2], h = l[3], x = side * (W * 0.5f - float(l[1]) - w * 0.5f), z = end * L * 0.5f;
			agent_box(mb, Vector3(x - w * 0.5f, y - h * 0.5f, z - HEAVY_PROUD_M), Vector3(x + w * 0.5f, y + h * 0.5f, z + HEAVY_PROUD_M),
					end > 0 ? trim.headlamp : trim.tail, end > 0 ? AgentTag::HEAD : AgentTag::TAIL);
		}
}

void heavy_mirrors(MeshBuilder &mb, float hw, float z, float y, float out, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		const float x1 = side * (hw + out - MIRROR_W * 0.5f);
		agent_box(mb, Vector3(side * hw, y + MIRROR_H - MIRROR_W, z - MIRROR_W * 0.5f), Vector3(x1, y + MIRROR_H, z + MIRROR_W * 0.5f), trim.blackout,
				AgentTag::FIXED);
		agent_box(mb, Vector3(x1 - MIRROR_W * 0.5f, y, z - MIRROR_D * 0.5f), Vector3(x1 + MIRROR_W * 0.5f, y + MIRROR_H, z + MIRROR_D * 0.5f), trim.blackout,
				AgentTag::FIXED);
	}
}

int build_truck(MeshBuilder &mb, const Dictionary &d, int sides, const CabinLook *look, const AgentTrim &trim) {
	const Truck k = read_truck(d);
	const int passengers = look ? cab_inside(mb, k, d["interior"], *look) : 0;
	cab(mb, k, trim);
	deflector(mb, k);
	chassis_and_box(mb, k, d, trim);
	heavy_mirrors(mb, k.chw, k.hl - HEAVY_PILLAR_M, k.belt, float(d["mirror_out_m"]), trim);
	heavy_lamps(mb, d["lamps_m"], float(d["cab_w"]), k.L, trim);
	heavy_wheels(mb, k.g, sides, trim);
	return passengers;
}

static std::vector<Vector2> profile_of(const Dictionary &part) {
	std::vector<Vector2> out;
	for (const Variant &p : Array(part["at"])) out.push_back(agent_v2(p));
	return out;
}

static void profile(MeshBuilder &mb, const Dictionary &part, const Color &c, AgentTag tag) {
	const float half = float(part["wide_m"]) * 0.5f;
	agent_extrude(mb, profile_of(part), -half, half, c, tag);
}

// The machine: the paint's tank, panels and tail, the saddle, the engine and its pipe, the frame, the fork and the bars.
static void moto_frame(MeshBuilder &mb, const Dictionary &sh, float rear_z, float front_z, float wheel_r, const AgentTrim &trim) {
	const Vector2 head = agent_v2(sh["head"]), spine = agent_v2(sh["spine_to"]), pivot = agent_v2(sh["pivot"]);
	profile(mb, sh["tank"], AGENT_WHITE, AgentTag::PAINT);
	profile(mb, sh["panel"], AGENT_WHITE, AgentTag::PAINT);
	profile(mb, sh["cowl"], AGENT_WHITE, AgentTag::PAINT);
	profile(mb, sh["saddle"], trim.blackout, AgentTag::FIXED);
	profile(mb, sh["engine"], agent_hex(sh["block"]), AgentTag::FIXED);
	const float fork = sh["fork_m"], fx = sh["fork_x_m"], ax = sh["arm_x_m"], ex = sh["exhaust_x_m"], em = sh["exhaust_m"];
	agent_strut(mb, Vector3(0, head.y, head.x), Vector3(0, spine.y, spine.x), fork, fork, trim.blackout, AgentTag::FIXED);
	for (int side : { -1, 1 }) {
		agent_strut(mb, Vector3(side * fx, head.y, head.x), Vector3(side * fx, wheel_r, front_z), fork, fork, trim.chrome, AgentTag::FIXED);
		agent_strut(mb, Vector3(side * ax, pivot.y, pivot.x), Vector3(side * ax, wheel_r, rear_z), fork, fork, trim.blackout, AgentTag::FIXED);
	}
	const Array pipe = sh["exhaust"];
	for (int64_t i = 0; i + 1 < pipe.size(); i++) {
		const Vector2 a = agent_v2(pipe[i]), b = agent_v2(pipe[i + 1]);
		agent_strut(mb, Vector3(-ex, a.y, a.x), Vector3(-ex, b.y, b.x), em, em, trim.chrome, AgentTag::FIXED);
	}
}

int build_moto(MeshBuilder &mb, const Dictionary &d, int sides, const AgentTrim &trim) {
	const Vector2 rear = agent_tyre(d["tyre"]), front = agent_tyre(d["front_tyre"]);
	const float az = float(d["wheelbase_m"]) * 0.5f;
	const Dictionary sh = d["shape"];
	const Vector2 head = agent_v2(sh["head"]);
	for (int side : { -1, 1 }) {
		agent_wheel(mb, Vector3(0, rear.x, -az), rear.x, rear.y, sides, side, trim);
		agent_wheel(mb, Vector3(0, front.x, az), front.x, front.y, sides, side, trim);
	}
	moto_frame(mb, sh, -az, az, rear.x, trim);
	const Vector3 nac = agent_v3(sh["nacelle_m"]), guard = agent_v3(sh["guard_m"]), bar = agent_v3(sh["bar_m"]);
	const Vector2 lens = agent_v2(sh["lens_m"]);
	agent_box(mb, Vector3(-nac.x * 0.5f, head.y - nac.y, head.x), Vector3(nac.x * 0.5f, head.y, head.x + nac.z), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-lens.x * 0.5f, head.y - nac.y * 0.5f - lens.y * 0.5f, head.x + nac.z), Vector3(lens.x * 0.5f, head.y - nac.y * 0.5f + lens.y * 0.5f, head.x + nac.z + MOTO_LAMP_D), trim.headlamp, AgentTag::HEAD);
	for (float z : { az, -az })
		agent_box(mb, Vector3(-guard.x * 0.5f, front.x * 2 + guard.y, z - guard.z * 0.5f), Vector3(guard.x * 0.5f, front.x * 2 + guard.y * 2, z + guard.z * 0.5f), trim.blackout, AgentTag::FIXED);
	agent_strut(mb, Vector3(-bar.x, bar.y, bar.z), Vector3(bar.x, bar.y, bar.z), float(sh["grip_m"]) * 2, float(sh["grip_m"]) * 2, trim.blackout, AgentTag::FIXED);
	const Array cowl = Dictionary(sh["cowl"])["at"];
	const Vector2 tail = agent_v2(cowl[2]);
	agent_box(mb, Vector3(-lens.x * 0.5f, tail.y - TAIL_LENS_H_M, tail.x - MOTO_LAMP_D), Vector3(lens.x * 0.5f, tail.y, tail.x), trim.tail, AgentTag::TAIL);
	return 0;
}

} // namespace city
