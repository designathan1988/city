// A car's cabin behind its see-through glass (Roadcraft 4.0 carBody.ts: seats and people at their hip points): a dark
// lining facing in (the shell's own faces are seen from behind there and culled), the dashboard, the seats in rows
// from the windscreen back, and in each seat a sitting person whose parts carry the seat's index for the shader.
#include "render/agents/agent_meshes.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float SHOULDER_DROP = 0.06f; // arms hang from this far under the top of the torso

// A wall rectangle facing `in` (the cabin's inside).
void lining(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &in, const Color &col) {
	mb.quad(a, b, c, d, col, in, nullptr, agent_tag(AgentTag::FIXED));
}

void seat_box(MeshBuilder &mb, const Vector3 &mn, const Vector3 &mx, AgentTag tag, int seat, const Color &col) {
	mb.box(Vector3(std::min(mn.x, mx.x), std::min(mn.y, mx.y), std::min(mn.z, mx.z)),
			Vector3(std::max(mn.x, mx.x), std::max(mn.y, mx.y), std::max(mn.z, mx.z)), col, seat_tag(tag, seat), true);
}

// The person in seat `k`, hip point (x, hip, z), facing +Z.
void occupant(MeshBuilder &mb, const CabinLook &l, int k, float x, float hip, float z) {
	const Vector3 &t = l.torso;
	const Color white(1, 1, 1, 1);
	const float top = hip + t.y;
	seat_box(mb, Vector3(x - t.x * 0.5f, hip, z - t.z * 0.5f), Vector3(x + t.x * 0.5f, top, z + t.z * 0.5f), AgentTag::CLOTH, k, white);
	for (int side : { -1, 1 }) {
		const float ax = x + side * (t.x + l.arm.x) * 0.5f;
		seat_box(mb, Vector3(ax - l.arm.x * 0.5f, top - SHOULDER_DROP - l.arm.y, z - l.arm.z * 0.5f),
				Vector3(ax + l.arm.x * 0.5f, top - SHOULDER_DROP, z + l.arm.z * 0.5f), AgentTag::CLOTH, k, white);
	}
	const float neck = l.head_r * 0.5f, head_y = top + l.neck_m + l.head_r;
	seat_box(mb, Vector3(x - neck, top, z - neck), Vector3(x + neck, top + l.neck_m + l.head_r * 0.5f, z + neck), AgentTag::SKIN, k, white);
	mb.blob(Vector3(x, head_y, z), Vector3(l.head_r, l.head_r, l.head_r), white, seat_tag(AgentTag::SKIN, k));
	const float hr = l.head_r * (1 + l.hair_drop);
	mb.blob(Vector3(x, head_y + l.head_r * l.hair_drop, z - l.hair_back_m), Vector3(hr, hr * (1 - l.hair_drop), hr), white,
			seat_tag(AgentTag::HAIR, k));
}

} // namespace

bool CabinLook::load(const Dictionary &d) {
	auto v3 = [](const Variant &v) {
		Array a = v;
		return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
	};
	seat_spread = d["seat_spread"]; front_hip_m = d["front_hip_m"]; row_pitch_m = d["row_pitch_m"]; back_clear_m = d["back_clear_m"];
	hip_m = d["hip_m"]; lining_inset_m = d["lining_inset_m"]; liner_drop_m = d["liner_drop_m"]; cushion_m = d["cushion_m"];
	seat_w_m = d["cushion_width_m"]; seat_d_m = d["cushion_depth_m"]; back_h_m = d["back_h_m"]; back_d_m = d["back_d_m"]; dash_d_m = d["dash_d_m"];
	dash_h_m = d["dash_h_m"]; torso = v3(d["torso_m"]); arm = v3(d["arm_m"]); head_r = d["head_r_m"]; neck_m = d["neck_m"];
	hair_drop = d["hair_drop"]; hair_back_m = d["hair_back_m"]; rows = d["rows"];
	return rows > 0 && torso.y > 0;
}

void agent_cabin(MeshBuilder &mb, const Cabin &c, const CabinLook &l, const AgentTrim &trim) {
	const float w = c.half_w - l.lining_inset_m, zf = c.z_front, zb = c.z_back, y0 = c.floor, y1 = c.belt, yr = c.roof - l.liner_drop_m;
	const Color &in = trim.interior;
	lining(mb, Vector3(-w, y0, zb), Vector3(-w, y0, zf), Vector3(-w, y1, zf), Vector3(-w, y1, zb), Vector3(1, 0, 0), in);
	lining(mb, Vector3(w, y0, zb), Vector3(w, y0, zf), Vector3(w, y1, zf), Vector3(w, y1, zb), Vector3(-1, 0, 0), in);
	lining(mb, Vector3(-w, y0, zb), Vector3(w, y0, zb), Vector3(w, y1, zb), Vector3(-w, y1, zb), Vector3(0, 0, 1), in);
	lining(mb, Vector3(-w, y0, zf), Vector3(w, y0, zf), Vector3(w, y1, zf), Vector3(-w, y1, zf), Vector3(0, 0, -1), in);
	lining(mb, Vector3(-w, y0, zb), Vector3(w, y0, zb), Vector3(w, y0, zf), Vector3(-w, y0, zf), Vector3(0, 1, 0), in);
	lining(mb, Vector3(-w, yr, zb), Vector3(w, yr, zb), Vector3(w, yr, zf), Vector3(-w, yr, zf), Vector3(0, -1, 0), in);
	agent_box(mb, Vector3(-w, y1 - l.dash_h_m, zf - l.dash_d_m), Vector3(w, y1, zf), in, AgentTag::FIXED);
	const float hip = y0 + l.hip_m, x = c.half_w * 2 * l.seat_spread;
	int k = 0;
	for (int row = 0; row < l.rows && k < CABIN_SEATS; row++) {
		const float z = zf - l.front_hip_m - row * l.row_pitch_m;
		if (z - l.torso.z * 0.5f - l.back_d_m < zb + l.back_clear_m) break; // no room for this row's backrest
		for (int side : { 1, -1 }) { // the driver's side (+X, the left) first
			const float sx = side * x;
			agent_box(mb, Vector3(sx - l.seat_w_m * 0.5f, hip - l.cushion_m, z - l.seat_d_m * 0.5f),
					Vector3(sx + l.seat_w_m * 0.5f, hip, z + l.seat_d_m * 0.5f), trim.seat, AgentTag::FIXED);
			const float back = z - l.torso.z * 0.5f;
			agent_box(mb, Vector3(sx - l.seat_w_m * 0.5f, hip, back - l.back_d_m), Vector3(sx + l.seat_w_m * 0.5f, hip + l.back_h_m, back),
					trim.seat, AgentTag::FIXED);
			occupant(mb, l, k++, sx, hip, z);
		}
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
