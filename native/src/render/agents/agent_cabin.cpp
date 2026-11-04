// The cabin of any vehicle behind its see-through glass (Roadcraft 4.0 carBody.ts / vehicleModels.ts: seats and people at
// their hip points, the driver's hands on the wheel): a lining facing in (the shell's own faces are seen from behind there
// and culled) with its doorways, floor, liner and dashboard; the seats in their styles (car, truck cab, bus chair); in
// each a sitting person whose parts carry, for the shader, the seat's place in the order the seats fill and the
// vehicle's passenger seats (UV).
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float SHOULDER_DROP = 0.06f; // arms hang from this far under the top of the torso
constexpr int RIM_SIDES = 8;           // segments of a steering wheel's rim
constexpr float HANDS = 1.05f;         // the hands on the rim at ten to two: radians from its top
constexpr float ROW_EPS_M = 0.01f;
constexpr int WHEEL_KEYS = 5; // agents.json cabin.seats.*.wheel

bool load_style(const Dictionary &d, SeatStyle &s) {
	s.hip_m = d["hip_m"]; s.cushion_m = d["cushion_m"]; s.width_m = d["cushion_w_m"]; s.depth_m = d["cushion_d_m"]; s.back_h_m = d["back_h_m"];
	s.back_d_m = d["back_d_m"]; s.base_share = d["base_share"];
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
	if (c.dash_d > 0) // from the shell back by its depth, over the front wall
		agent_box(mb, Vector3(std::max(-w, w - c.dash_w), c.front_top - c.dash_h, z1 - c.dash_d), Vector3(w, c.front_top, z1), c.paint.dash, AgentTag::FIXED);
}

void seat_shell(MeshBuilder &mb, const Seat &s, const CabinLook &l, const CabinPaint &p) {
	const SeatStyle &st = *s.style;
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

Wheel wheel_of(const Seat &s) {
	const SeatStyle &st = *s.style;
	Wheel w;
	w.hub = Vector3(s.x, s.floor + st.hip_m + st.wheel_up_m, s.z + st.wheel_fwd_m);
	// leant back by the tilt, its top away from the driver; the column runs on down and forward into the dash
	w.top = Vector3(0, std::cos(st.wheel_tilt), std::sin(st.wheel_tilt));
	w.column = w.hub + Vector3(0, -std::sin(st.wheel_tilt), std::cos(st.wheel_tilt)) * st.column_m;
	w.r = st.wheel_r_m;
	return w;
}

void steering(MeshBuilder &mb, const Wheel &w, const CabinLook &l, const CabinPaint &p) {
	for (int i = 0; i < RIM_SIDES; i++)
		agent_strut(mb, w.rim(TAU_F * i / RIM_SIDES), w.rim(TAU_F * (i + 1) / RIM_SIDES), l.rim_m, l.rim_m, p.dash, AgentTag::FIXED);
	agent_strut(mb, w.hub, w.column, l.rim_m, l.rim_m, p.dash, AgentTag::FIXED);
}

// Somebody sitting in `s` facing +Z: thighs level on the cushion, shins straight down to the floor.
Figure seated(const Person &p, const Seat &s) {
	const float hip = s.floor + s.style->hip_m, kx = s.x + p.thigh.x * 0.5f, kz = s.z + p.thigh.z;
	Figure f;
	f.hip = Vector3(s.x, hip, s.z);
	f.shoulders = Vector3(s.x, hip + p.torso.y, s.z);
	f.knee = Vector3(kx, hip + p.thigh.y * 0.5f, kz);
	f.ankle = Vector3(kx, s.floor + p.foot.y, kz);
	return f;
}

} // namespace

void agent_person(MeshBuilder &mb, const Person &p, const Figure &f) {
	const Vector3 &t = p.torso, &g = p.thigh;
	const Vector2 shoe = agent_tag(AgentTag::FIXED, Limb::SEAT);
	agent_strut(mb, f.hip, f.shoulders, t.x, t.z, AGENT_WHITE, AgentTag::CLOTH, Limb::SEAT);
	for (int side : { -1, 1 }) {
		auto at = [&](const Vector3 &v) { return side > 0 ? v : Vector3(2 * f.hip.x - v.x, v.y, v.z); };
		const Vector3 knee = at(f.knee), ankle = at(f.ankle);
		agent_strut(mb, Vector3(f.hip.x + side * g.x * 0.5f, f.hip.y + g.y * 0.5f, f.hip.z), knee, g.x, g.y, AGENT_WHITE, AgentTag::LEGS, Limb::SEAT);
		agent_strut(mb, knee, ankle, p.shin_w, p.shin_w, AGENT_WHITE, AgentTag::LEGS, Limb::SEAT);
		const float heel = ankle.z - p.shin_w * 0.5f;
		mb.box(Vector3(ankle.x - p.foot.x * 0.5f, ankle.y - p.foot.y, heel), Vector3(ankle.x + p.foot.x * 0.5f, ankle.y, heel + p.foot.z), p.shoes, shoe, true);
		const Vector3 shoulder(f.shoulders.x + side * (t.x + p.arm.x) * 0.5f, f.shoulders.y - SHOULDER_DROP - p.arm.x * 0.5f, f.shoulders.z);
		const Vector3 hand = f.holds ? at(f.hand) : shoulder - Vector3(0, p.arm.y, 0);
		agent_strut(mb, shoulder, hand, p.arm.x, p.arm.z, AGENT_WHITE, AgentTag::CLOTH, Limb::SEAT);
	}
	const Vector3 &s = f.shoulders;
	const float neck = p.head_r * 0.5f;
	const Vector3 head = s + Vector3(0, p.neck_m + p.head_r, 0);
	mb.box(Vector3(s.x - neck, s.y, s.z - neck), Vector3(s.x + neck, s.y + p.neck_m + p.head_r * 0.5f, s.z + neck), AGENT_WHITE,
			agent_tag(AgentTag::SKIN, Limb::SEAT), true);
	if (f.helmet.x > 0) {
		mb.blob(head, f.helmet, AGENT_WHITE, agent_tag(AgentTag::HELMET, Limb::SEAT));
		return;
	}
	mb.blob(head, Vector3(p.head_r, p.head_r, p.head_r), AGENT_WHITE, agent_tag(AgentTag::SKIN, Limb::SEAT));
	const float hr = p.head_r * (1 + p.hair_drop);
	mb.blob(head + Vector3(0, p.head_r * p.hair_drop, -p.hair_back_m), Vector3(hr, hr * (1 - p.hair_drop), hr), AGENT_WHITE,
			agent_tag(AgentTag::HAIR, Limb::SEAT));
	const Vector3 &lh = p.long_hair; // down the back of the head to the shoulders
	mb.box(Vector3(head.x - lh.x * 0.5f, head.y - lh.y, head.z - hr - p.hair_back_m), Vector3(head.x + lh.x * 0.5f, head.y, head.z - hr - p.hair_back_m + lh.z),
			AGENT_WHITE, agent_tag(AgentTag::HAIR_LONG, Limb::SEAT), true);
}

void seat_order(MeshBuilder &mb, int64_t from, float order) {
	for (int64_t i = from; i < mb.v.size(); i++) mb.uv.set(i, Vector2(order, 0));
}

bool CabinLook::load(const Dictionary &d) {
	lining_inset_m = d["lining_inset_m"]; liner_drop_m = d["liner_drop_m"]; rim_m = d["steering_rim_m"];
	const Dictionary p = d["person"], seats = d["seats"];
	person.torso = agent_v3(p["torso_m"]); person.arm = agent_v3(p["arm_m"]); person.thigh = agent_v3(p["thigh_m"]); person.foot = agent_v3(p["foot_m"]);
	person.shin_w = p["shin_w_m"]; person.head_r = p["head_r_m"]; person.neck_m = p["neck_m"]; person.hair_drop = p["hair_drop"];
	person.hair_back_m = p["hair_back_m"]; person.long_hair = agent_v3(p["long_hair_m"]); person.shoes = agent_hex(p["shoes"]);
	const bool styles = load_style(seats["car"], car) && load_style(seats["cab"], cab) && load_style(seats["chair"], chair);
	return styles && person.torso.y > 0 && person.thigh.z > 0 && rim_m > 0;
}

int agent_cabin(MeshBuilder &mb, const Cabin &c, const std::vector<Seat> &seats, const CabinLook &l) {
	const int64_t first = mb.v.size();
	lining(mb, c, l);
	int passengers = 0;
	for (const Seat &s : seats) {
		seat_shell(mb, s, l, c.paint);
		Wheel w;
		if (s.driver && s.style->wheel_r_m > 0) {
			w = wheel_of(s);
			steering(mb, w, l, c.paint);
		}
		Figure f = seated(l.person, s);
		if (s.driver && s.style->wheel_r_m > 0) {
			f.holds = true;
			f.hand = w.rim(HANDS); // the left hand at ten to two
		}
		const int64_t from = mb.v.size();
		agent_person(mb, l.person, f);
		seat_order(mb, from, s.driver ? DRIVER_ORDER : float(passengers++));
	}
	for (int64_t i = first; i < mb.v.size(); i++)
		if (mb.uv2[i].y == float(int(Limb::SEAT))) mb.uv.set(i, Vector2(mb.uv[i].x, float(passengers)));
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
