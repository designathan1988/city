// A passenger car's ends, parts and cabin (car_shape.h): the ends closed in level bands (the bumper face dark, painted
// above it), the lamps, grille and plates on them, a mirror on each front door and a handle on each door (fine level),
// and the cabin behind the glass (agent_cabin.cpp) between the windscreen and rear screen bases, under the belt and roof
// lines: rows of two seats, the driver's on the left.
#include "render/agents/car_shape.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float CAR_LAMP_W = 0.28f, LAMP_H = 0.08f, LAMP_X = 0.65f, TAIL_W = 0.26f, TAIL_H = 0.1f, TAIL_X = 0.7f;
constexpr float PART_DEPTH_M = 0.03f, PART_PROUD_M = 0.01f, GRILLE_X = 0.6f, GRILLE_LOW_M = 0.1f, GRILLE_H = 0.15f;
constexpr float PLATE_W = 0.52f, PLATE_H = 0.11f, PLATE_Y_M = 0.22f;
constexpr float HANDLE_BACK_M = 0.08f; // a door handle ahead of its door's back edge
// SAE J1100 counts 254 mm (10 in) more in its leg room dimensions (L33, L51) than the leg's own reach.
constexpr float SAE_EXTRA_M = 0.254f;

float belt_y(const CarStyle &s, float xf) {
	Vector2 r[RING];
	car_ring(s, xf, r);
	return r[RING_BELT].y;
}

// A mirror on each front door just behind the A pillar's foot, its arm dark and its housing painted; a handle on each
// door under the belt, ahead of the door's back edge.
void mirrors_and_handles(MeshBuilder &mb, const CarStyle &s, const Dictionary &body, const AgentTrim &trim) {
	const Array wm = body["wing_mirror_m"]; // the housing [out, high, long], its arm out from the door
	const Vector3 m(wm[0], wm[1], wm[2]), h = agent_v3(body["car_handle_m"]); // the handle [long, high, out]
	const float arm = float(wm[3]);
	float doors[3];
	car_door_edges(s, doors);
	const float zm = doors[0] * s.L - m.z, ym = belt_y(s, doors[0]), hw = car_half_w(s, doors[0]);
	const float b_pillar = float(body["b_pillar_m"]);
	for (int side : { -1, 1 }) {
		agent_box(mb, Vector3(side * hw, ym, zm), Vector3(side * (hw + arm), ym + m.y * 0.5f, zm + m.z * 0.5f), trim.blackout, AgentTag::FIXED);
		agent_box(mb, Vector3(side * (hw + arm), ym, zm), Vector3(side * (hw + arm + m.x), ym + m.y, zm + m.z), AGENT_WHITE, AgentTag::PAINT);
		for (float back : { doors[1] * s.L - b_pillar * 0.5f, doors[2] * s.L }) {
			const float xf = back / s.L, x = car_half_w(s, xf), y = belt_y(s, xf) - h.y * 2, z = back - HANDLE_BACK_M;
			agent_box(mb, Vector3(side * x, y, z - h.x), Vector3(side * (x + h.z), y + h.y, z), trim.blackout, AgentTag::FIXED);
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
	c.roof = std::min(car_top_at(s, std::min(s.screen[0], s.screen[1])), car_top_at(s, back)) - s.glass_top;
	if (!s.rear_glass) c.back_top = c.roof;
	c.half_w = std::min(car_half_w(s, c.z_front / s.L), car_half_w(s, c.z_back / s.L));
	c.paint = read_cabin_paint(inside);
	return c;
}

} // namespace

void car_cap(MeshBuilder &mb, const CarStyle &s, float xf, float dir, float bumper, const AgentTrim &trim) {
	Vector2 r[RING];
	car_ring(s, xf, r);
	const float z = xf * s.L;
	auto band = [&](const Vector2 &lo, const Vector2 &hi, AgentTag t) {
		mb.quad(Vector3(-lo.x, lo.y, z), Vector3(lo.x, lo.y, z), Vector3(hi.x, hi.y, z), Vector3(-hi.x, hi.y, z), t == AgentTag::PAINT ? AGENT_WHITE : trim.under,
				Vector3(0, 0, dir), nullptr, agent_tag(t));
	};
	for (int k = 1; k + 1 < RING; k++) { // ring[0] is the middle of the floor: band 0 has no height
		const Vector2 &lo = r[k], &hi = r[k + 1];
		if (hi.y <= bumper || lo.y >= bumper) {
			band(lo, hi, hi.y <= bumper ? AgentTag::FIXED : AgentTag::PAINT);
			continue;
		}
		const Vector2 cut = lo.lerp(hi, (bumper - lo.y) / (hi.y - lo.y));
		band(lo, cut, AgentTag::FIXED);
		band(cut, hi, AgentTag::PAINT);
	}
}

void car_details(MeshBuilder &mb, const CarStyle &s, const Dictionary &body, bool fine, const AgentTrim &trim) {
	const float lamp_y = body["lamp_y"], tail_y = body["tail_y"];
	const float zf = s.L * 0.5f, zb = -zf, hf = car_half_w(s, 0.5f), hb = car_half_w(s, -0.5f), bot = s.clear;
	const float yl = bot + (car_top_at(s, 0.5f) - bot) * lamp_y, yt = bot + (car_top_at(s, -0.5f) - bot) * tail_y;
	for (int side : { -1, 1 }) {
		const float xl = side * hf * LAMP_X, xt = side * hb * TAIL_X;
		agent_box(mb, Vector3(xl - CAR_LAMP_W * 0.5f, yl - LAMP_H * 0.5f, zf - PART_DEPTH_M), Vector3(xl + CAR_LAMP_W * 0.5f, yl + LAMP_H * 0.5f, zf + PART_PROUD_M),
				trim.headlamp, AgentTag::HEAD);
		agent_box(mb, Vector3(xt - TAIL_W * 0.5f, yt - TAIL_H * 0.5f, zb - PART_PROUD_M), Vector3(xt + TAIL_W * 0.5f, yt + TAIL_H * 0.5f, zb + PART_DEPTH_M),
				trim.tail, AgentTag::TAIL);
	}
	const float gy = bot + GRILLE_LOW_M;
	agent_box(mb, Vector3(-hf * GRILLE_X, gy, zf - PART_DEPTH_M), Vector3(hf * GRILLE_X, gy + GRILLE_H, zf + PART_PROUD_M), trim.grille, AgentTag::FIXED);
	for (float z : { zf, zb })
		agent_box(mb, Vector3(-PLATE_W * 0.5f, bot + PLATE_Y_M - PLATE_H, z - PART_DEPTH_M), Vector3(PLATE_W * 0.5f, bot + PLATE_Y_M, z + PART_PROUD_M),
				trim.plate, AgentTag::FIXED);
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
	return agent_cabin(mb, c, seats, l);
}

} // namespace city
