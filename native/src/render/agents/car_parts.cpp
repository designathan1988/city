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
	const CarPartsShape &K = s.shapes->car_parts;
	float z = dir * s.L * 0.5f;
	for (float walked = 0; walked < K.scan_reach_m && car_half_w(s, z / s.L) < half; walked += K.scan_step_m) z -= dir * K.scan_step_m;
	return z;
}

void mirror(MeshBuilder &mb, const CarStyle &s, const DataValue &m, int side, const AgentTrim &trim) {
	const CarPartsShape &K = trim.shapes->car_parts;
	const float z = s.doors[0] * s.L - m["back_m"].f(), y = belt_y(s, z / s.L) + m["rise_m"].f(), x = skin_x(s, z / s.L);
	const float along = m["long_m"].f(), out = m["wide_m"].f(), high = m["high_m"].f(), arm = m["stalk_m"].f(), cz = z - K.housing_back_m, cx = x + K.housing_out_m;
	agent_strut(mb, Vector3(side * (x - K.root_in_m), y - K.root_drop_m, z), Vector3(side * (x + K.tip_out_m), y, z - K.tip_back_m), arm, arm, trim.blackout, AgentTag::FIXED);
	const Vector3 back(side * cx, y - K.housing_drop_m + high * 0.5f, cz - along * 0.5f);
	agent_pod(mb, *trim.shapes, back, Vector2(out, high) * 0.5f, along, AGENT_WHITE, AgentTag::PAINT);
	const Vector2 g = agent_v2(m["glass_m"]); // high, across
	agent_pod_face(mb, *trim.shapes, back - Vector3(0, 0, K.mirror_glass_proud_m), Vector2(g.y, g.x) * 0.5f, trim.mirror, AgentTag::FIXED);
}

// A mirror on each front door by the A pillar's foot, its arm dark and its housing painted; a handle on each door under the belt.
void mirrors_and_handles(MeshBuilder &mb, const CarStyle &s, const DataValue &body, const AgentTrim &trim) {
	const CarPartsShape &K = trim.shapes->car_parts;
	const Vector3 h = agent_v3(body["car_handle_m"]); // [long, high, out]
	for (int side : { -1, 1 }) {
		mirror(mb, s, body["mirror"], side, trim);
		for (size_t d = 0; d + 1 < s.doors.size(); d++) {
			const float front = s.doors[d] * s.L, back = s.doors[d + 1] * s.L, z = back + std::min(K.handle_reach_m, (front - back) * K.handle_share);
			const float y = belt_y(s, z / s.L) - K.handle_drop_m, x = skin_x(s, z / s.L), mid = z + K.handle_ahead_m;
			agent_box(mb, Vector3(side * (x - K.handle_sink_m), y, mid - h.x * 0.5f), Vector3(side * (x + h.z), y + h.y, mid + h.x * 0.5f), trim.blackout, AgentTag::FIXED);
		}
	}
}

// Where the seats go: the driver's hip point behind the cabin's front (the ankles there) by the front leg room's reach,
// the next row's behind the front backrest by the rear one's; each seat midway between the car's middle and its side.
struct CarRows {
	float first = 0, pitch = 0, x = 0;
};

CarRows rows_of(const Cabin &c, const DataValue &inside, const CabinLook &l) {
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
Cabin cabin_of(const CarStyle &s, const DataValue &inside, const CabinLook &l) {
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
	const CarPartsShape &K = trim.shapes->car_parts;
	Vector2 r[RING];
	car_ring(s, xf, r);
	const float z = xf * s.L;
	float low = r[1].y, top = r[1].y;
	for (int k = 1; k < RING; k++) {
		low = std::min(low, r[k].y);
		top = std::max(top, r[k].y);
	}
	const float bumper = low + (top - low) * K.bumper_share;
	for (int k = 1; k + 1 < RING; k++) { // ring[0] is the middle of the floor: band 0 has no height
		const Vector2 &lo = r[k], &hi = r[k + 1];
		// the end is the body's own panel; only the bumper's foot is the black of the trim
		const bool foot = (lo.y + hi.y) * 0.5f < bumper;
		mb.quad(Vector3(-lo.x, lo.y, z), Vector3(lo.x, lo.y, z), Vector3(hi.x, hi.y, z), Vector3(-hi.x, hi.y, z), foot ? trim.blackout : AGENT_WHITE, Vector3(0, 0, dir), nullptr,
				agent_tag(foot ? AgentTag::GLOSS : AgentTag::PAINT));
	}
}

// A lens `w` wide and `h` high with its middle at (x, y) on the end `dir`, both sides.
// A lens on each side (+X the driver's left), tagged `right` and `left` by the side it stands on (an indicator flashes on its own).
static void lens_pair(MeshBuilder &mb, const CarStyle &s, float half, float y, float w, float h, float dir, float sink, const Color &c, AgentTag right, AgentTag left) {
	const CarPartsShape &K = s.shapes->car_parts;
	const float z = end_surface(s, half, dir) - dir * sink;
	for (int side : { -1, 1 }) {
		const float x = side * half;
		agent_box(mb, Vector3(x - w * 0.5f, y - h * 0.5f, z - K.lamp_depth_m * 0.5f), Vector3(x + w * 0.5f, y + h * 0.5f, z + K.lamp_depth_m * 0.5f), c, side < 0 ? right : left);
	}
}

static void lens_pair(MeshBuilder &mb, const CarStyle &s, float half, float y, float w, float h, float dir, float sink, const Color &c, AgentTag tag) {
	lens_pair(mb, s, half, y, w, h, dir, sink, c, tag, tag);
}

void car_details(MeshBuilder &mb, const CarStyle &s, const DataValue &body, bool fine, const AgentTrim &trim) {
	const CarPartsShape &K = trim.shapes->car_parts;
	const float low_f = car_low_at(s, 0.5f), low_b = car_low_at(s, -0.5f), zf = s.L * 0.5f, zb = -zf;
	const float nose_y = car_top_at(s, 0.5f - K.nose_back_m / s.L), tail_top = car_top_at(s, -0.5f + K.tail_back_m / s.L);
	const float lamp_y = low_f + (nose_y - low_f) * body["lamp_y"].f(), tail_y = low_b + (tail_top - low_b) * body["tail_y"].f();
	const Vector3 head = agent_v3(body["front_lens_m"]), ind = agent_v3(body["indicator_m"]);
	const Vector2 tail = agent_v2(body["rear_lens_m"]);
	lens_pair(mb, s, s.W * 0.5f - head.x, lamp_y, head.z, head.y, 1.f, K.lamp_sink_m, trim.headlamp, AgentTag::HEAD);
	lens_pair(mb, s, s.W * 0.5f - tail.x, tail_y, tail.y, s.tail_lamp_h, -1.f, K.lamp_sink_m, trim.tail, AgentTag::TAIL);
	lens_pair(mb, s, s.W * 0.5f - ind.x, lamp_y - K.indicator_drop_m, ind.z, ind.y, 1.f, K.indicator_sink_m, trim.indicator, AgentTag::INDICATOR_RIGHT, AgentTag::INDICATOR_LEFT);
	lens_pair(mb, s, s.W * 0.5f - ind.x, tail_y + K.indicator_rise_m, ind.z, ind.y, -1.f, K.indicator_sink_m, trim.indicator, AgentTag::INDICATOR_RIGHT, AgentTag::INDICATOR_LEFT);
	const Vector3 g = agent_v3(body["grille_m"]); // the share of the nose's half width across, high, deep
	const float gy = low_f + (nose_y - low_f) * K.grille_low + K.grille_rise_m, gw = car_half_w(s, 0.5f - K.nose_back_m / s.L) * g.x * 0.5f;
	agent_box(mb, Vector3(-gw, gy - g.y * 0.5f, zf - K.grille_sink_m - g.z * 0.5f), Vector3(gw, gy + g.y * 0.5f, zf - K.grille_sink_m + g.z * 0.5f), trim.grille, AgentTag::FIXED);
	const Vector2 p = agent_v2(body["car_plate_m"]);
	const float py[2] = { low_f + (gy - low_f) * K.plate_front, low_b + (tail_y - low_b) * K.plate_rear }, pz[2] = { zf + K.plate_proud_m, zb - K.plate_proud_m };
	for (int i = 0; i < 2; i++)
		agent_box(mb, Vector3(-p.x * 0.5f, py[i] - p.y * 0.5f, pz[i] - K.plate_thick_m * 0.5f), Vector3(p.x * 0.5f, py[i] + p.y * 0.5f, pz[i] + K.plate_thick_m * 0.5f), trim.plate, AgentTag::FIXED);
	if (fine) mirrors_and_handles(mb, s, body, trim);
}

int car_cabin(MeshBuilder &mb, const CarStyle &s, const DataValue &inside, const CabinLook &l) {
	Cabin c = cabin_of(s, inside, l);
	if (c.z_back >= c.z_front) return -1;
	const CarRows r = rows_of(c, inside, l);
	const float room = c.z_back + l.lining_inset_m + l.person.torso.z * 0.5f + l.car.back_d_m; // a backrest still fits
	std::vector<Seat> seats;
	seat_rows(seats, r.first, std::max(r.first - (inside["rows"].whole() - 1) * r.pitch, room), r.pitch, { r.x, -r.x }, c.floor, l.car);
	if (seats.empty()) return -1;
	seats[0].driver = true;
	dash_for(c, seats[0], l, 2 * c.half_w);
	c.console = agent_v2(inside["console_m"]);
	c.console_back = seats[0].z - l.person.torso.z * 0.5f;
	return agent_cabin(mb, c, seats, l);
}

} // namespace city
