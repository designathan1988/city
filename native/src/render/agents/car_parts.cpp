// A passenger car's ends, parts and cabin (car_shape.h): the ends closed dark, the lamps on the surface of the nose and the
// tail, the grille and plates over the ends, a mirror on each front door and a handle on each door (fine level), and the
// cabin behind the glass (agent_cabin.cpp) between the windscreen and rear screen bases, under the belt and roof lines:
// rows of two seats, the driver's on the left.
#include "render/agents/car_shape.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float LAMP_DEPTH_M = 0.04f, LAMP_SINK_M = 0.005f, INDICATOR_SINK_M = 0.01f; // a lens, and how far into the surface it starts
constexpr float INDICATOR_DROP_M = 0.03f, INDICATOR_RISE_M = 0.02f; // the front's under the lamp, the rear's over it
constexpr float SCAN_STEP_M = 0.005f, SCAN_REACH_M = 0.6f; // the walk in from an end to the surface at a half width
constexpr float NOSE_BACK_M = 0.02f, TAIL_BACK_M = 0.05f; // where the end's height is read, clear of its roll
constexpr float GRILLE_RISE_M = 0.03f, GRILLE_SINK_M = 0.012f, GRILLE_LOW = 0.5f; // over the middle of the nose
constexpr float BUMPER_SHARE = 0.3f; // of an end's height, up from its foot
constexpr float PLATE_THICK_M = 0.012f, PLATE_PROUD_M = 0.009f, PLATE_FRONT = 0.45f, PLATE_REAR = 0.55f; // up from the bottom to the grille, the tail lamps
constexpr float HANDLE_SINK_M = 0.007f, HANDLE_DROP_M = 0.09f, HANDLE_REACH_M = 0.2f, HANDLE_SHARE = 0.25f, HANDLE_AHEAD_M = 0.06f;
constexpr float ROOT_IN_M = 0.02f, ROOT_DROP_M = 0.03f, TIP_BACK_M = 0.04f, TIP_OUT_M = 0.1f; // a mirror's arm, out of the door to its housing
constexpr float HOUSING_BACK_M = 0.06f, HOUSING_DROP_M = 0.02f, HOUSING_OUT_M = 0.17f; // its housing's middle from the door
constexpr float MIRROR_GLASS_PROUD_M = 0.003f; // off the housing's back, or the two fight for the pixel
// SAE J1100 counts 254 mm (10 in) more in its leg room dimensions (L33, L51) than the leg's own reach.
constexpr float SAE_EXTRA_M = 0.254f;

float belt_y(const CarStyle &s, float xf) {
	Vector2 r[RING];
	car_ring(s, xf, r);
	return r[RING_BELT].y;
}

float skin_x(const CarStyle &s, float xf) {
	Vector2 r[RING];
	car_ring(s, xf, r);
	return r[RING_BELT].x;
}

// The z of the surface `half` out from the middle: walking in from the nose (dir 1) or the tail (dir -1).
float end_surface(const CarStyle &s, float half, float dir) {
	float z = dir * s.L * 0.5f;
	for (float walked = 0; walked < SCAN_REACH_M && car_half_w(s, z / s.L) < half; walked += SCAN_STEP_M) z -= dir * SCAN_STEP_M;
	return z;
}

void mirror(MeshBuilder &mb, const CarStyle &s, const Dictionary &m, int side, const AgentTrim &trim) {
	const float z = s.doors[0] * s.L - float(m["back_m"]), y = belt_y(s, z / s.L) + float(m["rise_m"]), x = skin_x(s, z / s.L);
	const float along = m["long_m"], out = m["wide_m"], high = m["high_m"], arm = m["stalk_m"], cz = z - HOUSING_BACK_M, cx = x + HOUSING_OUT_M;
	agent_strut(mb, Vector3(side * (x - ROOT_IN_M), y - ROOT_DROP_M, z), Vector3(side * (x + TIP_OUT_M), y, z - TIP_BACK_M), arm, arm, trim.blackout, AgentTag::FIXED);
	const Vector3 back(side * cx, y - HOUSING_DROP_M + high * 0.5f, cz - along * 0.5f);
	agent_pod(mb, back, Vector2(out, high) * 0.5f, along, AGENT_WHITE, AgentTag::PAINT);
	const Vector2 g = agent_v2(m["glass_m"]); // high, across
	agent_pod_face(mb, back - Vector3(0, 0, MIRROR_GLASS_PROUD_M), Vector2(g.y, g.x) * 0.5f, trim.mirror, AgentTag::FIXED);
}

// A mirror on each front door by the A pillar's foot, its arm dark and its housing painted; a handle on each door under the belt.
void mirrors_and_handles(MeshBuilder &mb, const CarStyle &s, const Dictionary &body, const AgentTrim &trim) {
	const Vector3 h = agent_v3(body["car_handle_m"]); // [long, high, out]
	for (int side : { -1, 1 }) {
		mirror(mb, s, body["mirror"], side, trim);
		for (size_t d = 0; d + 1 < s.doors.size(); d++) {
			const float front = s.doors[d] * s.L, back = s.doors[d + 1] * s.L, z = back + std::min(HANDLE_REACH_M, (front - back) * HANDLE_SHARE);
			const float y = belt_y(s, z / s.L) - HANDLE_DROP_M, x = skin_x(s, z / s.L), mid = z + HANDLE_AHEAD_M;
			agent_box(mb, Vector3(side * (x - HANDLE_SINK_M), y, mid - h.x * 0.5f), Vector3(side * (x + h.z), y + h.y, mid + h.x * 0.5f), trim.blackout, AgentTag::FIXED);
		}
	}
}

// Where the seats go: the driver's hip point behind the cabin's front (the ankles there) by the front leg room's reach,
// the next row's behind the front backrest by the rear one's; each seat midway between the car's middle and its side.
struct CarRows {
	float first = 0, pitch = 0, x = 0;
};

CarRows rows_of(const Cabin &c, const Dictionary &inside, const CabinLook &l) {
	const Vector2 leg = agent_v2(inside["leg_room_m"]); // L33, L51
	const float back = l.person.torso.z * 0.5f + l.car.back_d_m; // the hip point to the backrest's back
	CarRows r;
	r.first = c.z_front - (leg.x - SAE_EXTRA_M);
	r.pitch = back + leg.y - SAE_EXTRA_M;
	r.x = (c.half_w - l.lining_inset_m) * 0.5f;
	return r;
}

// The inside of the body: from the floor over the sill to the lowest belt line and under the roof, from the windscreen
// base back to the rear screen's base or, on a blind back, to a bulkhead behind the front seats' backrests up to the roof.
Cabin cabin_of(const CarStyle &s, const Dictionary &inside, const CabinLook &l) {
	Cabin c;
	c.floor = s.clear + s.sill;
	c.side_top = c.front_top = c.back_top = std::min(s.belt[0], s.belt[1]) * s.H;
	c.z_front = std::max(s.screen[0], s.screen[1]) * s.L;
	const float bulkhead = rows_of(c, inside, l).first - l.person.torso.z * 0.5f - l.car.back_d_m - l.lining_inset_m;
	c.z_back = s.rear_glass ? std::min(s.rear[0], s.rear[1]) * s.L : bulkhead;
	// under the roof between the screens' top edges (at their bases the line is the bonnet's and the boot's)
	const float back = s.rear_glass ? std::max(s.rear[0], s.rear[1]) : c.z_back / s.L;
	c.roof = std::min(car_top_at(s, std::min(s.screen[0], s.screen[1])), car_top_at(s, back)) - s.roof_r;
	if (!s.rear_glass) c.back_top = c.roof;
	c.half_w = std::min(car_half_w(s, c.z_front / s.L), car_half_w(s, c.z_back / s.L));
	c.paint = read_cabin_paint(inside);
	return c;
}

} // namespace

void car_cap(MeshBuilder &mb, const CarStyle &s, float xf, float dir, const AgentTrim &trim) {
	Vector2 r[RING];
	car_ring(s, xf, r);
	const float z = xf * s.L;
	float low = r[1].y, top = r[1].y;
	for (int k = 1; k < RING; k++) {
		low = std::min(low, r[k].y);
		top = std::max(top, r[k].y);
	}
	const float bumper = low + (top - low) * BUMPER_SHARE;
	for (int k = 1; k + 1 < RING; k++) { // ring[0] is the middle of the floor: band 0 has no height
		const Vector2 &lo = r[k], &hi = r[k + 1];
		// the end is the body's own panel; only the bumper's foot is the black of the trim
		const bool foot = (lo.y + hi.y) * 0.5f < bumper;
		mb.quad(Vector3(-lo.x, lo.y, z), Vector3(lo.x, lo.y, z), Vector3(hi.x, hi.y, z), Vector3(-hi.x, hi.y, z), foot ? trim.blackout : AGENT_WHITE, Vector3(0, 0, dir), nullptr,
				agent_tag(foot ? AgentTag::GLOSS : AgentTag::PAINT));
	}
}

// A lens `w` wide and `h` high with its middle at (x, y) on the end `dir`, both sides.
static void lens_pair(MeshBuilder &mb, const CarStyle &s, float half, float y, float w, float h, float dir, float sink, const Color &c, AgentTag tag) {
	const float z = end_surface(s, half, dir) - dir * sink;
	for (int side : { -1, 1 }) {
		const float x = side * half;
		// an indicator's lens is tagged by its side: +x is the vehicle's left, its front looking along +z
		const AgentTag own = tag == AgentTag::SIGNAL_LEFT ? (side > 0 ? AgentTag::SIGNAL_LEFT : AgentTag::SIGNAL_RIGHT) : tag;
		agent_box(mb, Vector3(x - w * 0.5f, y - h * 0.5f, z - LAMP_DEPTH_M * 0.5f), Vector3(x + w * 0.5f, y + h * 0.5f, z + LAMP_DEPTH_M * 0.5f), c, own);
	}
}

void car_details(MeshBuilder &mb, const CarStyle &s, const Dictionary &body, bool fine, const AgentTrim &trim) {
	const float low_f = car_low_at(s, 0.5f), low_b = car_low_at(s, -0.5f), zf = s.L * 0.5f, zb = -zf;
	const float nose_y = car_top_at(s, 0.5f - NOSE_BACK_M / s.L), tail_top = car_top_at(s, -0.5f + TAIL_BACK_M / s.L);
	const float lamp_y = low_f + (nose_y - low_f) * float(body["lamp_y"]), tail_y = low_b + (tail_top - low_b) * float(body["tail_y"]);
	const Vector3 head = agent_v3(body["front_lens_m"]), ind = agent_v3(body["indicator_m"]);
	const Vector2 tail = agent_v2(body["rear_lens_m"]);
	lens_pair(mb, s, s.W * 0.5f - head.x, lamp_y, head.z, head.y, 1.f, LAMP_SINK_M, trim.headlamp, AgentTag::HEAD);
	lens_pair(mb, s, s.W * 0.5f - tail.x, tail_y, tail.y, s.tail_lamp_h, -1.f, LAMP_SINK_M, trim.tail, AgentTag::TAIL);
	lens_pair(mb, s, s.W * 0.5f - ind.x, lamp_y - INDICATOR_DROP_M, ind.z, ind.y, 1.f, INDICATOR_SINK_M, trim.indicator, AgentTag::SIGNAL_LEFT);
	lens_pair(mb, s, s.W * 0.5f - ind.x, tail_y + INDICATOR_RISE_M, ind.z, ind.y, -1.f, INDICATOR_SINK_M, trim.indicator, AgentTag::SIGNAL_LEFT);
	const Vector3 g = agent_v3(body["grille_m"]); // the share of the nose's half width across, high, deep
	const float gy = low_f + (nose_y - low_f) * GRILLE_LOW + GRILLE_RISE_M, gw = car_half_w(s, 0.5f - NOSE_BACK_M / s.L) * g.x * 0.5f;
	agent_box(mb, Vector3(-gw, gy - g.y * 0.5f, zf - GRILLE_SINK_M - g.z * 0.5f), Vector3(gw, gy + g.y * 0.5f, zf - GRILLE_SINK_M + g.z * 0.5f), trim.grille, AgentTag::FIXED);
	const Vector2 p = agent_v2(body["car_plate_m"]);
	const float py[2] = { low_f + (gy - low_f) * PLATE_FRONT, low_b + (tail_y - low_b) * PLATE_REAR }, pz[2] = { zf + PLATE_PROUD_M, zb - PLATE_PROUD_M };
	for (int i = 0; i < 2; i++)
		agent_box(mb, Vector3(-p.x * 0.5f, py[i] - p.y * 0.5f, pz[i] - PLATE_THICK_M * 0.5f), Vector3(p.x * 0.5f, py[i] + p.y * 0.5f, pz[i] + PLATE_THICK_M * 0.5f), trim.plate, AgentTag::FIXED);
	if (fine) mirrors_and_handles(mb, s, body, trim);
}

int car_cabin(MeshBuilder &mb, const CarStyle &s, const Dictionary &inside, const CabinLook &l) {
	Cabin c = cabin_of(s, inside, l);
	if (c.z_back >= c.z_front) return -1;
	const CarRows r = rows_of(c, inside, l);
	const float room = c.z_back + l.lining_inset_m + l.person.torso.z * 0.5f + l.car.back_d_m; // a backrest still fits
	std::vector<Seat> seats;
	seat_rows(seats, r.first, std::max(r.first - (int(inside["rows"]) - 1) * r.pitch, room), r.pitch, { r.x, -r.x }, c.floor, l.car);
	if (seats.empty()) return -1;
	seats[0].driver = true;
	dash_for(c, seats[0], l, 2 * c.half_w);
	c.console = agent_v2(inside["console_m"]);
	c.console_back = seats[0].z - l.person.torso.z * 0.5f;
	return agent_cabin(mb, c, seats, l);
}

} // namespace city
