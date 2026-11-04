// The shape of a passenger-car style (internal to car_body.cpp and car_parts.cpp): its dimensions and lines from
// agents.json "cars"[i] and "car_body", the roof line through its points by monotone cubic interpolation (Fritsch &
// Carlson 1980: no overshoot between the points), the plan rounded at the nose and the tail, the wheel arches, and the
// half ring of each station: from the floor's middle round the bottom corner, up the slightly bulging flank to the belt,
// the side glass leaning in, round the roof edge to the crown.
#pragma once

#include "render/agents/agent_meshes.h"

namespace city {

// Points of the half ring, floor middle to roof middle: bottom (0-1), bottom corner (1-3), sill (3-4), flank (4-6, the
// bulge at 5), side glass (6-7), roof edge (7-9), roof (9-10).
constexpr int RING = 11;
constexpr int RING_SILL = 3, RING_BELT = 6, RING_PANE_TOP = 7;
constexpr float CAR_ARCH_PAD_M = 0.07f; // a wheel arch's room round its tyre

struct CarStyle {
	float L = 0, W = 0, H = 0, r = 0, tw = 0, clear = 0; // tw: the tyre's width
	float axles[2] = {}, track[2] = {}; // front and rear: the axle (share of L from the middle), the track (metres)
	std::vector<float> top_x, top_y, top_m; // the roof line, x ascending (share of L), y (share of H), slopes
	float screen[2] = {}, rear[2] = {}, side[2] = {}, belt[2] = {}, nose_r = 0, tail_r = 0;
	float sill = 0, tumble = 0, glass_top = 0, crown = 0, bottom_round = 0, bulge = 0;
	bool rear_glass = true; // "rear": [] is a blind back (the van)
};

CarStyle read_car_style(const godot::Dictionary &car, const godot::Dictionary &body);
float car_top_at(const CarStyle &s, float xf);
float car_half_w(const CarStyle &s, float xf);
void car_ring(const CarStyle &s, float xf, godot::Vector2 out[RING]);
bool car_within(float x, const float r[2]);
// Ahead of the front wheel arch or behind the rear one: the bumpers' overhang.
bool car_overhang(const CarStyle &s, float xf);
// The doors' edges (the windscreen base, the B pillar half way along the side glass, the side glass's back) as shares.
void car_door_edges(const CarStyle &s, float out[3]);

// car_parts.cpp: the ends closed in level bands (the bumper face dark up to `bumper` over the road), the lamps, grille,
// plates, mirrors and door handles; the cabin behind the glass (passengers, -1 when the data's cabin is inside out).
void car_cap(MeshBuilder &mb, const CarStyle &s, float xf, float dir, float bumper, const AgentTrim &trim);
void car_details(MeshBuilder &mb, const CarStyle &s, const godot::Dictionary &body, bool fine, const AgentTrim &trim);
int car_cabin(MeshBuilder &mb, const CarStyle &s, const godot::Dictionary &inside, const CabinLook &l);

} // namespace city
