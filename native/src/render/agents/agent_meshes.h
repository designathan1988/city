// Low-poly meshes of the traffic agents, built once at startup from project/data/render/agents.json: lofted car bodies
// (sedan, hatch, SUV, van, pickup), bus, truck, motorbike with rider (pedestrians are the cooked people, people_asset.h). Model frame: +Z forward,
// +X the driver's left, Y up, origin = body centre on the road. UV2.x tags each part for render/shaders/agents.gdshader
// (AgentTag); UV2.y = 1 marks wheel vertices (UV = their hub), turned by the shader, and UV2.y = 2 a person's (Limb::SEAT,
// UV = the seat's place in the order the seats fill, -1 the driver, and the vehicle's passenger seats): shown or folded
// away by the shader from the vehicle's load and order. Each vehicle comes at three levels: near, its glass a surface of
// its own (split_tag) drawn see-through by render/shaders/agent_glass.gdshader over its cabin (agent_cabin); the body, no
// cabin and the glass opaque; far, the body's outline alone (agents.json far_radius_m: past it no lamp, grille, plate or
// mirror is a pixel), fewer stations, rows and wheel sides.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <functional>

namespace city {

// The levels a vehicle is built at, nearest first; agents.json car_body detail arrays and wheel_sides hold one entry each.
constexpr int AGENT_CABIN_LEVEL = 0, AGENT_BODY_LEVEL = 1, AGENT_FAR_LEVEL = 2, AGENT_LEVELS = 3;

enum class AgentTag : int { FIXED = 0, PAINT = 1, HEAD = 2, TAIL = 3, GLASS = 4, SKIN, CLOTH, HAIR, HELMET, LEGS, HAIR_LONG, SIGN, ROOF, GLOSS };
// Parts that move in the shader: WHEEL turns about X round its hub (UV = hub y, z) by the roll angle; SEAT is a person's.
enum class Limb : int { NONE = 0, WHEEL = 1, SEAT = 2 };
constexpr float DRIVER_ORDER = -1.f; // UV.x of the driver's parts: always there

// Fixed colours of the parts that are not painted (agents.json "trim").
struct AgentTrim {
	godot::Color tyre, hub, under, blackout, grille, plate, glass, headlamp, tail, truck_box, chrome, roof, indicator, mirror;
};

// A person sitting (agents.json "cabin.person"): torso, arm, thigh and foot as [w, h, d] (the thigh's d from the hip to
// the knee, the foot's from the heel to the toes) and long hair falling behind the head [w, below the head's middle, d]
// (HAIR_LONG: only the long cut keeps it).
struct Person {
	godot::Vector3 torso, arm, thigh, foot, long_hair;
	godot::Color shoes;
	float shin_w = 0, head_r = 0, neck_m = 0, hair_drop = 0, hair_back_m = 0;
};
// A kind of seat (agents.json "cabin.seats"): hip point over its floor, cushion, backrest, the base under the cushion as
// a share of it, a headrest over the backrest [w, h, gap] and a grab handle on top of it [share of the width, thick]
// (none when zero) and, for a driver, the steering wheel from the hip point: forward, up, tilt back from upright, radius
// and its column's length into the dashboard.
struct SeatStyle {
	float hip_m = 0, cushion_m = 0, width_m = 0, depth_m = 0, back_h_m = 0, back_d_m = 0, base_share = 0;
	float wheel_fwd_m = 0, wheel_up_m = 0, wheel_tilt = 0, wheel_r_m = 0, column_m = 0;
	float recline = 0; // the backrest's lean from upright, radians; 0 a square seat
	godot::Vector3 headrest;
	godot::Vector2 handle;
	godot::Color handle_colour;
};
// Colours of one kind of cabin (agents.json "cars_cabin" / "bus.interior" / "truck.interior").
struct CabinPaint {
	godot::Color lining, floor, seat, dash;
};
struct CabinLook {
	float lining_inset_m = 0, liner_drop_m = 0, rim_m = 0;
	Person person;
	SeatStyle car, cab, chair;
	bool load(const godot::Dictionary &d);
};
// A person's joints in the model frame: the hip point (where they sit), the middle of the shoulders, the left (+X) knee,
// ankle and, when they hold something, hand (the right ones mirrored about the hips); a helmet's radii, or hair.
struct Figure {
	godot::Vector3 hip, shoulders, knee, ankle, hand, helmet;
	bool holds = false;
};
// One seat: hip point (x, z) over the floor it stands on.
struct Seat {
	float x = 0, z = 0, floor = 0;
	const SeatStyle *style = nullptr;
	bool driver = false;
};
// The inside of one body: floor and roof (under it the liner), the shell's inner half width and ends (z; the lining stands
// lining_inset_m in from them), the tops of its side, front and back walls (the glass starts there), the dashboard and
// the doorways on the kerb side (-X; z spans, no wall).
struct Cabin {
	float floor = 0, roof = 0, half_w = 0, z_front = 0, z_back = 0, side_top = 0, front_top = 0, back_top = 0;
	float dash_d = 0, dash_h = 0, dash_w = 0; // the dashboard from the driver's side wall (+X) inwards
	godot::Vector2 console; // between the front seats from the dashboard back: wide, high (zero: none)
	float console_back = 0; // the z it ends at
	std::vector<godot::Vector2> doors;
	CabinPaint paint;
};
// A hollow lower body (bus, truck cab): side walls `wall` thick from y0 to y1 over [z0, z1], cut over the axles (wheel
// arches up to arch_top) and open on the kerb side over the doorways; the floor pan, pan_half_w wide each side (clear of
// the wheels), from y0 to under `floor`.
struct Tub {
	float half_w = 0, wall = 0, y0 = 0, y1 = 0, z0 = 0, z1 = 0, floor = 0, arch_r = 0, arch_top = 0, pan_half_w = 0;
	std::vector<float> axles;
	std::vector<godot::Vector2> doors;
};

inline godot::Vector2 agent_tag(AgentTag t, Limb l = Limb::NONE) { return godot::Vector2(float(int(t)), float(int(l))); }
inline const godot::Color AGENT_WHITE(1, 1, 1, 1);

godot::Color agent_hex(const godot::Variant &v);
// [x, y, z] / [x, y] from agents.json (zero when the list is not that long).
godot::Vector3 agent_v3(const godot::Variant &v);
godot::Vector2 agent_v2(const godot::Variant &v);
AgentTrim read_trim(const godot::Dictionary &t);
CabinPaint read_cabin_paint(const godot::Dictionary &d);
// A box with a tag; colour white for tinted tags.
void agent_box(MeshBuilder &mb, const godot::Vector3 &mn, const godot::Vector3 &mx, const godot::Color &c, AgentTag tag, Limb limb = Limb::NONE);
// A bar from a to b, `w` across (along X where it can be) and `d` the other way: limbs, torsos, steering wheels.
void agent_strut(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, float w, float d, const godot::Color &c, AgentTag tag,
		Limb limb = Limb::NONE);
// A pane of glass a-b-c-d facing `out`.
void agent_pane(MeshBuilder &mb, const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &d, const godot::Vector3 &out,
		const AgentTrim &trim);
// A wheel turning about X: tyre tread, sidewall and hubcap on the outer face (`side` = +1 left, -1 right).
void agent_wheel(MeshBuilder &mb, const godot::Vector3 &centre, float r, float width, int sides, int side, const AgentTrim &trim);
// The same near: a tyre with a rounded shoulder, its rim sunk in it and five spokes.
void agent_wheel_near(MeshBuilder &mb, const godot::Vector3 &centre, float r, float width, int sides, int side, const AgentTrim &trim);
// A thin tyre round a rim, both sides alike, and `spokes` wires from the hub (a bicycle's).
void agent_spoked_wheel(MeshBuilder &mb, const godot::Vector3 &centre, float r, float width, int sides, int spokes, const AgentTrim &trim);
// A convex side profile (z, y) run across from x0 to x1, with its two ends.
void agent_extrude(MeshBuilder &mb, const std::vector<godot::Vector2> &profile, float x0, float x1, const godot::Color &c, AgentTag tag);
// A slab from x0 to x1 and y0 to y1 over [z0, z1], raised to arch_top over each axle (wheel-arch cut-outs).
void agent_arched_box(MeshBuilder &mb, float x0, float x1, float y0, float y1, float z0, float z1, const std::vector<float> &axles_z, float arch_r,
		float arch_top, const godot::Color &c, AgentTag tag);
void agent_tub(MeshBuilder &mb, const Tub &t, const AgentTrim &trim);
// [z0, z1] less the spans of `gaps` (sorted or not).
std::vector<godot::Vector2> spans_between(float z0, float z1, const std::vector<godot::Vector2> &gaps);

// agent_cabin.cpp: the lining, dashboard, seats and people of `c`; the seats fill in their order, the drivers' always.
// Returns the passenger seats.
int agent_cabin(MeshBuilder &mb, const Cabin &c, const std::vector<Seat> &seats, const CabinLook &l);
// The driver's hip point z in `c`: their toes on the pedals at the front lining (seated() below: the thigh level to the
// knee, the shin straight down, the foot ahead of the heel).
float pedal_hip_z(const Cabin &c, const CabinLook &l);
// The dashboard of `c` in front of `driver`: from the front wall back to the steering wheel's hub and down to the top of
// the knees, `width` in from the driver's side wall.
void dash_for(Cabin &c, const Seat &driver, const CabinLook &l, float width);
// A person (Limb::SEAT parts: shirt, trousers, shoes, skin, hair or helmet) posed by `f`.
void agent_person(MeshBuilder &mb, const Person &p, const Figure &f);
// The parts of the person from vertex `from` on fill the seat that is `order`-th to fill (DRIVER_ORDER: always there).
void seat_order(MeshBuilder &mb, int64_t from, float order);
// Seats in rows from z_first back by `pitch` while z >= z_last, one at each x of a row; `keep` may raise a seat (its
// floor) or leave it out.
void seat_rows(std::vector<Seat> &out, float z_first, float z_last, float pitch, const std::vector<float> &xs, float floor, const SeatStyle &style,
		const std::function<bool(Seat &)> &keep = nullptr);
// The triangles of `src` tagged `tag` into `only`, the rest into `rest`.
void split_tag(const MeshBuilder &src, AgentTag tag, MeshBuilder &rest, MeshBuilder &only);

// Each builder draws the cabin when given one (the near level) and returns its passenger seats (-1: the data's cabin is
// inside out).
// car_body.cpp: one passenger car style (agents.json "cars"[i]) with the shared "car_body" and "cars_cabin" settings, at
// `level` (AGENT_*_LEVEL; the cabin, given only at the cabin level).
int build_car(MeshBuilder &mb, const godot::Dictionary &car, const godot::Dictionary &body, const godot::Dictionary &inside, const CabinLook *cabin,
		const AgentTrim &trim, int level);
// bus_body.cpp
int build_bus(MeshBuilder &mb, const godot::Dictionary &d, int sides, const CabinLook *cabin, const AgentTrim &trim);
// heavy_meshes.cpp
int build_truck(MeshBuilder &mb, const godot::Dictionary &d, int sides, const CabinLook *cabin, const AgentTrim &trim);
// Whoever rides a two-wheeler astride (agents.json rider joints), in a helmet of these radii.
void agent_rider(MeshBuilder &mb, const Person &rider, const godot::Dictionary &pose, const godot::Vector3 &helmet);
int build_moto(MeshBuilder &mb, const godot::Dictionary &d, int sides, const Person &rider, const AgentTrim &trim);
int build_bicycle(MeshBuilder &mb, const godot::Dictionary &d, int sides, const Person &rider, const AgentTrim &trim);
// A tyre from its designation [section width mm, aspect ratio %, rim diameter inches] (ISO 4000-1, ETRTO): its radius
// (half the rim's diameter plus the sidewall, width times aspect) and width, metres.
godot::Vector2 agent_tyre(const godot::Variant &designation);
// Parts the heavy vehicles share (heavy_meshes.cpp).
// Where the wheels go: the axles (z) from the front overhang and the wheelbases, the tracks (centre to centre of the
// tyres, front axle and the others) and, behind the front axle, twin tyres dual_m apart (agents.json bus / truck).
struct Running {
	std::vector<float> axles;
	float r = 0, w = 0, front_track = 0, rear_track = 0, dual = 0;
};
Running heavy_running(const godot::Dictionary &d, float length);
void heavy_wheels(MeshBuilder &mb, const Running &g, int sides, const AgentTrim &trim);
// Headlamps and tail lamps on the ends (agents.json lamps_m: their centres' height over the road, their outer edges in
// from the sides, their faces [w, h]).
void heavy_lamps(MeshBuilder &mb, const godot::Dictionary &lamps, float width, float length, const AgentTrim &trim);
// A mirror each side on an arm out from the body at z: the glass's bottom at y, its outer edge `out` beyond the side.
void heavy_mirrors(MeshBuilder &mb, float half_w, float z, float y, float out, const AgentTrim &trim);
constexpr float HEAVY_PROUD_M = 0.01f, HEAVY_GLASS_INSET_M = 0.02f, HEAVY_ARCH_PAD_M = 0.07f, HEAVY_PILLAR_M = 0.12f;

} // namespace city
