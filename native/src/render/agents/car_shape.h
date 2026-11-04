// The shape of a passenger-car style (internal to car_body.cpp and car_parts.cpp): its dimensions and lines from
// agents.json "cars"[i] and "car_body", the roof line through its points by monotone cubic interpolation (Fritsch &
// Carlson 1980: no overshoot between the points), the plan rounded at the nose and the tail, the wheel arches, and the
// half ring of each station: from the floor's middle round the bottom corner, up the slightly bulging flank to the belt,
// the side glass leaning in, round the roof edge to the crown.
#pragma once

#include "render/agents/agent_meshes.h"

namespace city {

// Points of the half ring, floor middle to roof middle: bottom (0-1), bottom corner (1-3), sill (3-4), flank (4-7, narrowing
// from its middle), the side glass with its frame (7-10), the roof's rounded edge (10-14), the crown (14-17).
constexpr int RING = 18;
constexpr int RING_SILL = 3, RING_BELT = 7, RING_FRAME_TOP = 9, RING_PANE_TOP = 10, RING_ROOF = 14;
constexpr float CAR_ARCH_PAD_M = 0.07f; // a wheel arch's room round its tyre

struct CarStyle {
	float L = 0, W = 0, H = 0, r = 0, tw = 0, clear = 0; // tw: the tyre's width
	float axles[2] = {}, track[2] = {}; // front and rear: the axle (share of L from the middle), the track (metres)
	std::vector<float> top_x, top_y, top_m; // the roof line, x ascending (share of L), y (share of H), slopes
	std::vector<float> doors; // door edges, front to back (shares)
	float screen[2] = {}, rear[2] = {}, belt[2] = {}, nose_r = 0, tail_r = 0; // screen: base, top; rear: top, base
	float glass_front = 0, glass_back = 0; // the side glass (shares)
	float lean = 0; // the glasshouse: metres in per metre up
	float lift[2] = {}, tuck[2] = {}; // the bumpers rise by lift over their last tuck metres: nose, tail
	float roll[3] = {}; // the ends round in over their last roll[0] metres, by roll[1] across and a share roll[2] of the height
	float sill = 0, crown[2] = {}, bottom_round = 0, bulge = 0, frame = 0, pillar_foot = 0, roof_r = 0, tail_lamp_h = 0;
	float bed_from = -1; // a pick-up's load bed from here back (share); -1 none
	bool rear_glass = true; // "rear": [] is a blind back (the van)
};

CarStyle read_car_style(const godot::Dictionary &car, const godot::Dictionary &body);
float car_top_at(const CarStyle &s, float xf);
float car_half_w(const CarStyle &s, float xf);
void car_ring(const CarStyle &s, float xf, godot::Vector2 out[RING]);
bool car_within(float x, const float r[2]);
// Ahead of the front wheel arch or behind the rear one: the bumpers' overhang.
bool car_overhang(const CarStyle &s, float xf);
// The underside's height at xf, leaving the wheel arches out: the bumpers tucked up at the ends.
float car_low_at(const CarStyle &s, float xf);

// car_parts.cpp: the ends closed dark, the lamps, grille, plates, mirrors and door handles; the cabin behind the glass
// (passengers, -1 when the data's cabin is inside out).
void car_cap(MeshBuilder &mb, const CarStyle &s, float xf, float dir, const AgentTrim &trim);
void car_details(MeshBuilder &mb, const CarStyle &s, const godot::Dictionary &body, bool fine, const AgentTrim &trim);
int car_cabin(MeshBuilder &mb, const CarStyle &s, const godot::Dictionary &inside, const CabinLook &l);

} // namespace city
