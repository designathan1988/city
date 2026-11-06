// The cabin of any vehicle behind its see-through glass: a lining facing in (the shell's own faces are seen from behind
// there and culled) with its doorways, floor, liner and dashboard; the seats in their styles (car, truck cab, bus chair). Who sits
// in them is not here: the cooked people, posed by PeopleRenderer at the hip points this leaves in CabinLook::spots.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr int RIM_SIDES = 16;          // segments of a steering wheel's rim
constexpr float SPOKE_ANGLES[3] = { 1.82f, -1.82f, 3.14159f }; // its spokes from the rim's top, radians
constexpr float SPOKE_W_M = 0.04f, SPOKE_T_M = 0.018f, BOSS_W_M = 0.13f, BOSS_L_M = 0.05f;
constexpr float SEAT_NOSE_M = 0.04f, SEAT_NOSE_DROP_M = 0.05f, SEAT_LIP_M = 0.03f, SEAT_CREASE_M = 0.05f, SEAT_LAP_M = 0.06f; // a cushion's front, a backrest's foot
constexpr float BACK_TOP_ROUND_M = 0.02f, BACK_TOP_IN_M = 0.01f;
constexpr float REST_DEPTH_M = 0.1f, HEADREST_ROD_M = 0.015f, HEADREST_ROD_SHARE = 0.25f;
constexpr float DASH_LIP_M = 0.02f, DASH_FOOT_M = 0.12f, DASH_WAIST = 0.5f;
constexpr float ROW_EPS_M = 0.01f;
constexpr int WHEEL_KEYS = 5; // agents.json cabin.seats.*.wheel

bool load_style(const Dictionary &d, SeatStyle &s) {
	s.hip_m = d["hip_m"]; s.cushion_m = d["cushion_m"]; s.width_m = d["cushion_w_m"]; s.depth_m = d["cushion_d_m"]; s.back_h_m = d["back_h_m"];
	s.back_d_m = d["back_d_m"]; s.base_share = d["base_share"];
	s.recline = d["recline"];
	s.headrest = agent_v3(d["headrest_m"]); s.handle = agent_v2(d["handle_m"]); s.handle_colour = agent_hex(d["handle"]);
	Array w = d["wheel"];
	if (w.size() == WHEEL_KEYS) { s.wheel_fwd_m = w[0]; s.wheel_up_m = w[1]; s.wheel_tilt = w[2]; s.wheel_r_m = w[3]; s.column_m = w[4]; }
	return s.hip_m > 0 && s.width_m > 0 && w.size() == WHEEL_KEYS;
}

void wall(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &in, const Color &col) {
	mb.quad(a, b, c, d, col, in, nullptr, agent_tag(AgentTag::FIXED));
}

// The walls stand lining_inset_m in from the shell on every side (never in the plane of a panel's inner face, which
// would fight it for the depth); the floor and the liner run out to the shell, under and over the gap.
void lining(MeshBuilder &mb, const Cabin &c, const CabinLook &l) {
	const float in = l.lining_inset_m, w = c.half_w - in, zf = c.z_front - in, zb = c.z_back + in, y0 = c.floor, yr = c.roof - l.liner_drop_m;
	const Color &col = c.paint.lining;
	for (int side : { -1, 1 }) {
		const float x = side * w;
		const std::vector<Vector2> spans = side < 0 ? spans_between(zb, zf, c.doors) : std::vector<Vector2>{ Vector2(zb, zf) };
		for (const Vector2 &s : spans)
			wall(mb, Vector3(x, y0, s.x), Vector3(x, y0, s.y), Vector3(x, c.side_top, s.y), Vector3(x, c.side_top, s.x), Vector3(-side, 0, 0), col);
	}
	wall(mb, Vector3(-w, y0, zb), Vector3(w, y0, zb), Vector3(w, c.back_top, zb), Vector3(-w, c.back_top, zb), Vector3(0, 0, 1), col);
	wall(mb, Vector3(-w, y0, zf), Vector3(w, y0, zf), Vector3(w, c.front_top, zf), Vector3(-w, c.front_top, zf), Vector3(0, 0, -1), col);
	const float hw = c.half_w, z0 = c.z_back, z1 = c.z_front;
	wall(mb, Vector3(-hw, y0, z0), Vector3(hw, y0, z0), Vector3(hw, y0, z1), Vector3(-hw, y0, z1), Vector3(0, 1, 0), c.paint.floor);
	wall(mb, Vector3(-w, yr, z0), Vector3(w, yr, z0), Vector3(w, yr, z1), Vector3(-w, yr, z1), Vector3(0, -1, 0), col);
	if (c.dash_d > 0) { // from the shell back by its depth, over the front wall: its top, the lip over the driver's knees, the foot well
		const float top = c.front_top, low = top - c.dash_h, rear = z1 - c.dash_d;
		agent_extrude(mb, { Vector2(z1, top), Vector2(rear + DASH_LIP_M, top), Vector2(rear, top - DASH_LIP_M), Vector2(rear + DASH_LIP_M, top - c.dash_h * DASH_WAIST),
							  Vector2(rear + DASH_FOOT_M, low), Vector2(z1, low) },
				std::max(-w, w - c.dash_w), w, c.paint.dash, AgentTag::FIXED);
		if (c.console.x > 0) agent_box(mb, Vector3(-c.console.x * 0.5f, y0, c.console_back), Vector3(c.console.x * 0.5f, y0 + c.console.y, rear + DASH_FOOT_M), c.paint.dash, AgentTag::FIXED);
	}
}

// A car's or a cab's seat: the cushion's side profile and the backrest's leaning back from it, run across; the headrest on its rods.
void reclined_seat(MeshBuilder &mb, const Seat &s, const CabinLook &l, const CabinPaint &p) {
	const SeatStyle &st = *s.style;
	const float hip = s.floor + st.hip_m, hw = st.width_m * 0.5f, zf = s.z + st.depth_m * 0.5f, low = hip - st.cushion_m;
	const float back = s.z - l.person.torso.z * 0.5f, sn = std::sin(st.recline), cs = std::cos(st.recline), foot = back - st.back_d_m * cs;
	const float top_z = back - st.back_h_m * sn, top_y = hip + st.back_h_m * cs;
	agent_extrude(mb, { Vector2(zf - SEAT_NOSE_M, hip - SEAT_LIP_M), Vector2(zf, hip - SEAT_NOSE_DROP_M), Vector2(zf - SEAT_NOSE_M, low), Vector2(foot, low), Vector2(back, hip), Vector2(back + SEAT_LAP_M, hip) },
			s.x - hw, s.x + hw, p.seat, AgentTag::FIXED);
	agent_extrude(mb, { Vector2(foot, low), Vector2(foot - st.back_h_m * sn, top_y - BACK_TOP_ROUND_M), Vector2(top_z + BACK_TOP_IN_M, top_y), Vector2(back, hip + SEAT_CREASE_M), Vector2(back, hip) },
			s.x - hw, s.x + hw, p.seat, AgentTag::FIXED);
	if (st.base_share > 0) agent_box(mb, Vector3(s.x - hw * st.base_share, s.floor, zf - st.depth_m), Vector3(s.x + hw * st.base_share, low, zf - SEAT_NOSE_M), p.seat, AgentTag::FIXED);
	const Vector3 &h = st.headrest;
	if (h.y <= 0) return;
	const float gap = h.z, hy = top_y + gap * cs, hz = top_z - gap * sn - st.back_d_m * cs * 0.5f, rod = h.x * HEADREST_ROD_SHARE;
	agent_box(mb, Vector3(s.x - h.x * 0.5f, hy + rod * 0, hz - REST_DEPTH_M * 0.5f), Vector3(s.x + h.x * 0.5f, hy + h.y, hz + REST_DEPTH_M * 0.5f), p.seat, AgentTag::FIXED);
	for (int side : { -1, 1 })
		agent_box(mb, Vector3(s.x + side * rod - HEADREST_ROD_M * 0.5f, top_y - BACK_TOP_ROUND_M, hz - HEADREST_ROD_M * 0.5f), Vector3(s.x + side * rod + HEADREST_ROD_M * 0.5f, hy, hz + HEADREST_ROD_M * 0.5f),
				p.dash, AgentTag::FIXED);
}

void seat_shell(MeshBuilder &mb, const Seat &s, const CabinLook &l, const CabinPaint &p) {
	const SeatStyle &st = *s.style;
	if (st.recline > 0) {
		reclined_seat(mb, s, l, p);
		return;
	}
	const float hip = s.floor + st.hip_m, hw = st.width_m * 0.5f, hd = st.depth_m * 0.5f, low = hip - st.cushion_m;
	if (st.base_share > 0)
		agent_box(mb, Vector3(s.x - hw * st.base_share, s.floor, s.z - hd * st.base_share), Vector3(s.x + hw * st.base_share, low, s.z + hd * st.base_share), p.seat,
				AgentTag::FIXED);
	agent_box(mb, Vector3(s.x - hw, low, s.z - hd), Vector3(s.x + hw, hip, s.z + hd), p.seat, AgentTag::FIXED);
	const float back = s.z - l.person.torso.z * 0.5f, top = hip + st.back_h_m;
	agent_box(mb, Vector3(s.x - hw, low, back - st.back_d_m), Vector3(s.x + hw, top, back), p.seat, AgentTag::FIXED);
	const Vector3 &h = st.headrest;
	if (h.y > 0)
		agent_box(mb, Vector3(s.x - h.x * 0.5f, top + h.z, back - st.back_d_m), Vector3(s.x + h.x * 0.5f, top + h.z + h.y, back), p.seat, AgentTag::FIXED);
	const float grip = hw * st.handle.x, t = st.handle.y;
	if (t > 0)
		agent_box(mb, Vector3(s.x - grip, top, back - st.back_d_m * 0.5f - t * 0.5f), Vector3(s.x + grip, top + t, back - st.back_d_m * 0.5f + t * 0.5f),
				st.handle_colour, AgentTag::FIXED);
}

// A point on the driver's steering wheel: `a` radians round the rim from its top, or the hub and the column's end.
struct Wheel {
	Vector3 hub, top, column;
	float r = 0;
	Vector3 rim(float a) const { return hub + Vector3(std::sin(a) * r, 0, 0) + top * (std::cos(a) * r); }
};

// The steering wheel's hub: wheel_fwd_m ahead of the backrest (where the person's back is), wheel_up_m over the hip point.
float hub_z(const Seat &s, const CabinLook &l) { return s.z - l.person.torso.z * 0.5f + s.style->wheel_fwd_m; }

Wheel wheel_of(const Seat &s, const CabinLook &l) {
	const SeatStyle &st = *s.style;
	Wheel w;
	w.hub = Vector3(s.x, s.floor + st.hip_m + st.wheel_up_m, hub_z(s, l));
	// leant back by the tilt, its top away from the driver; the column runs on down and forward into the dash
	w.top = Vector3(0, std::cos(st.wheel_tilt), std::sin(st.wheel_tilt));
	w.column = w.hub + Vector3(0, -std::sin(st.wheel_tilt), std::cos(st.wheel_tilt)) * st.column_m;
	w.r = st.wheel_r_m;
	return w;
}

void steering(MeshBuilder &mb, const Wheel &w, const CabinLook &l, const CabinPaint &p) {
	for (int i = 0; i < RIM_SIDES; i++)
		agent_strut(mb, w.rim(TAU_F * i / RIM_SIDES), w.rim(TAU_F * (i + 1) / RIM_SIDES), l.rim_m, l.rim_m, p.dash, AgentTag::FIXED);
	for (float a : SPOKE_ANGLES) agent_strut(mb, w.hub, w.rim(a), SPOKE_W_M, SPOKE_T_M, p.dash, AgentTag::FIXED);
	agent_strut(mb, w.hub, w.column, l.rim_m, l.rim_m, p.dash, AgentTag::FIXED);
	agent_strut(mb, w.hub, w.hub + (w.column - w.hub).normalized() * BOSS_L_M, BOSS_W_M, BOSS_W_M, p.dash, AgentTag::FIXED);
}

// How somebody sits in `s`: by the kind of seat, whether they drive and, in a car, whether they are in the row of the driver.
SeatPose pose_of(const Seat &s, const Seat &first, const CabinLook &l) {
	if (s.style == &l.car) return s.driver ? SeatPose::CAR_DRIVER : (s.z >= first.z - ROW_EPS_M ? SeatPose::CAR_FRONT : SeatPose::CAR_REAR);
	if (s.style == &l.cab) return s.driver ? SeatPose::CAB_DRIVER : SeatPose::CAB_RIDE;
	return SeatPose::CHAIR;
}

} // namespace

float pedal_hip_z(const Cabin &c, const CabinLook &l) {
	const Person &p = l.person;
	return c.z_front - l.lining_inset_m - (p.thigh.z - p.shin_w * 0.5f + p.foot.z);
}

void dash_for(Cabin &c, const Seat &driver, const CabinLook &l, float width) {
	const SeatStyle &st = *driver.style;
	c.dash_d = c.z_front - hub_z(driver, l);
	c.dash_h = c.front_top - (driver.floor + st.hip_m + l.person.thigh.y);
	c.dash_w = width;
}

bool CabinLook::load(const Dictionary &d) {
	lining_inset_m = d["lining_inset_m"]; liner_drop_m = d["liner_drop_m"]; rim_m = d["steering_rim_m"];
	const Dictionary p = d["person"], seats = d["seats"];
	person.torso = agent_v3(p["torso_m"]); person.thigh = agent_v3(p["thigh_m"]); person.foot = agent_v3(p["foot_m"]);
	person.shin_w = p["shin_w_m"];
	const bool styles = load_style(seats["car"], car) && load_style(seats["cab"], cab) && load_style(seats["chair"], chair);
	return styles && person.torso.y > 0 && person.thigh.z > 0 && rim_m > 0;
}

int agent_cabin(MeshBuilder &mb, const Cabin &c, const std::vector<Seat> &seats, const CabinLook &l) {
	lining(mb, c, l);
	int passengers = 0;
	for (const Seat &s : seats) {
		seat_shell(mb, s, l, c.paint);
		if (s.driver && s.style->wheel_r_m > 0) steering(mb, wheel_of(s, l), l, c.paint);
		if (l.spots) l.spots->push_back({ Vector3(s.x, s.floor + s.style->hip_m, s.z), pose_of(s, seats.front(), l), s.driver ? DRIVER_SEAT : passengers });
		if (!s.driver) passengers++;
	}
	return passengers;
}

void seat_rows(std::vector<Seat> &out, float z_first, float z_last, float pitch, const std::vector<float> &xs, float floor, const SeatStyle &style,
		const std::function<bool(Seat &)> &keep) {
	for (int row = 0; z_first - row * pitch >= z_last - ROW_EPS_M; row++)
		for (float x : xs) {
			Seat s;
			s.x = x;
			s.z = z_first - row * pitch;
			s.floor = floor;
			s.style = &style;
			if (!keep || keep(s)) out.push_back(s);
		}
}

void split_tag(const MeshBuilder &src, AgentTag tag, MeshBuilder &rest, MeshBuilder &only) {
	const float want = float(int(tag));
	std::vector<int> to_rest(size_t(src.v.size()), -1), to_only(size_t(src.v.size()), -1);
	for (int64_t i = 0; i + 2 < src.idx.size(); i += 3) {
		const int a = src.idx[i];
		const bool mine = src.uv2[a].x == want;
		MeshBuilder &dst = mine ? only : rest;
		std::vector<int> &map = mine ? to_only : to_rest;
		for (int64_t j = i; j < i + 3; j++) {
			const int v = src.idx[j];
			if (map[size_t(v)] < 0) map[size_t(v)] = dst.add(src.v[v], src.n[v], src.c[v], src.uv[v], src.uv2[v]);
			dst.idx.push_back(map[size_t(v)]);
		}
	}
}

} // namespace city
