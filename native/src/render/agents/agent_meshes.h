// Low-poly meshes of the traffic agents, built once at startup from project/data/render/agents.json: lofted car bodies
// (sedan, hatch, SUV, van, pickup), bus, truck, motorbike with rider (pedestrians are the cooked people, people_asset.h). Model frame: +Z forward,
// +X the driver's left, Y up, origin = body centre on the road. UV2.x tags each part for render/shaders/agents.gdshader
// (AgentTag); UV2.y = 1 marks wheel vertices (UV = their hub), turned by the shader.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

enum class AgentTag : int { FIXED = 0, PAINT = 1, HEAD = 2, TAIL = 3, GLASS = 4 };
// Parts that move in the shader: WHEEL turns about X round its hub (UV = hub y, z) by INSTANCE_CUSTOM.b radians.
enum class Limb : int { NONE = 0, WHEEL = 1 };

// Fixed colours of the parts that are not painted (agents.json "trim").
struct AgentTrim {
	godot::Color tyre, hub, under, blackout, grille, plate, glass, headlamp, tail, truck_box, chrome, roof, rider, rider_legs, shoes;
};

inline godot::Vector2 agent_tag(AgentTag t, Limb l = Limb::NONE) { return godot::Vector2(float(int(t)), float(int(l))); }
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

// car_body.cpp: one passenger car style (agents.json "cars"[i]) with the shared "car_body" settings.
void build_car(MeshBuilder &mb, const godot::Dictionary &car, const godot::Dictionary &body, const AgentTrim &trim);
// heavy_meshes.cpp
void build_bus(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);
void build_truck(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);
void build_moto(MeshBuilder &mb, const godot::Dictionary &d, int sides, const AgentTrim &trim);

} // namespace city
