// Low-poly meshes of the traffic agents, built once at startup from project/data/render/agents.json: lofted car bodies
// (sedan, hatch, SUV, van, pickup), bus, truck, motorbike with rider (pedestrians are the cooked people, people_asset.h). Model frame: +Z forward,
// +X the driver's left, Y up, origin = body centre on the road. UV2.x tags each part for render/shaders/agents.gdshader
// (AgentTag); UV2.y = 1 marks wheel vertices (UV = their hub), turned by the shader, and UV2.y = 2 + k the k-th seat's
// occupant (shown or folded away by the shader from the vehicle's seed). A car's glass is a surface of its own
// (split_tag), drawn see-through by render/shaders/agent_glass.gdshader over its cabin (agent_cabin).
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

enum class AgentTag : int { FIXED = 0, PAINT = 1, HEAD = 2, TAIL = 3, GLASS = 4, SKIN, CLOTH, HAIR };
// Parts that move in the shader: WHEEL turns about X round its hub (UV = hub y, z) by INSTANCE_CUSTOM.b radians;
// SEAT + k is the occupant of seat k (0 the driver).
enum class Limb : int { NONE = 0, WHEEL = 1, SEAT = 2 };
constexpr int CABIN_SEATS = 4; // agents.gdshader seat_fill

// Fixed colours of the parts that are not painted (agents.json "trim").
struct AgentTrim {
	godot::Color tyre, hub, under, blackout, grille, plate, glass, headlamp, tail, truck_box, chrome, roof, rider, rider_legs, shoes;
	godot::Color interior, seat;
};

// A car's cabin (agents.json "cabin"): its seats, the lining that hides the inside of the shell behind the glass and
// the people sitting there, sized by the style's body (Cabin).
struct CabinLook {
	float seat_spread = 0, front_hip_m = 0, row_pitch_m = 0, back_clear_m = 0, hip_m = 0, lining_inset_m = 0, liner_drop_m = 0;
	float cushion_m = 0, seat_w_m = 0, seat_d_m = 0, back_h_m = 0, back_d_m = 0, dash_d_m = 0, dash_h_m = 0;
	godot::Vector3 torso, arm; // w, h, d
	float head_r = 0, neck_m = 0, hair_drop = 0, hair_back_m = 0;
	int rows = 0;
	bool load(const godot::Dictionary &d);
};
// The inside of one body: floor, belt and roof heights, half width and the windscreen and rear screen bases (z).
struct Cabin {
	float floor = 0, belt = 0, roof = 0, half_w = 0, z_front = 0, z_back = 0;
};

inline godot::Vector2 agent_tag(AgentTag t, Limb l = Limb::NONE) { return godot::Vector2(float(int(t)), float(int(l))); }
inline godot::Vector2 seat_tag(AgentTag t, int seat) { return godot::Vector2(float(int(t)), float(int(Limb::SEAT) + seat)); }
inline const godot::Color AGENT_WHITE(1, 1, 1, 1);

godot::Color agent_hex(const godot::Variant &v);
AgentTrim read_trim(const godot::Dictionary &t);
// A box with a tag; colour white for tinted tags.
void agent_box(MeshBuilder &mb, const godot::Vector3 &mn, const godot::Vector3 &mx, const godot::Color &c, AgentTag tag, Limb limb = Limb::NONE);
// A wheel turning about X: tyre tread, sidewall and hubcap on the outer face (`side` = +1 left, -1 right).
void agent_wheel(MeshBuilder &mb, const godot::Vector3 &centre, float r, float width, int sides, int side, const AgentTrim &trim);
// Lower body sides with wheel-arch cut-outs (heavy vehicles): a box from y0 to y1, raised to arch_top over each axle.
void agent_arched_box(MeshBuilder &mb, float half_w, float y0, float y1, float len, const std::vector<float> &axles_z, float arch_r,
		float arch_top, const godot::Color &c, AgentTag tag);

// agent_cabin.cpp: the lining, dashboard, seats and occupants of `c`.
void agent_cabin(MeshBuilder &mb, const Cabin &c, const CabinLook &l, const AgentTrim &trim);
// The triangles of `src` tagged `tag` into `only`, the rest into `rest`.
void split_tag(const MeshBuilder &src, AgentTag tag, MeshBuilder &rest, MeshBuilder &only);

// car_body.cpp: one passenger car style (agents.json "cars"[i]) with the shared "car_body" settings and its cabin.
void build_car(MeshBuilder &mb, const godot::Dictionary &car, const godot::Dictionary &body, const CabinLook &cabin, const AgentTrim &trim);
// heavy_meshes.cpp
void build_bus(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);
void build_truck(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);
void build_moto(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);

} // namespace city
