// Shared pieces of the agent meshes: colours from agents.json, tagged boxes, wheels and arched lower bodies.
#include "render/agents/agent_meshes.h"

#include "core/color.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HUB_SHARE = 0.62f, HUB_PROUD_M = 0.01f, RIM_SHARE = 0.5f, SPOKE_SHADE = 0.45f;
} // namespace

Color agent_hex(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

AgentTrim read_trim(const Dictionary &t) {
	AgentTrim r;
	r.tyre = agent_hex(t["tyre"]);
	r.hub = agent_hex(t["hub"]);
	r.under = agent_hex(t["under"]);
	r.blackout = agent_hex(t["blackout"]);
	r.grille = agent_hex(t["grille"]);
	r.plate = agent_hex(t["plate"]);
	r.glass = agent_hex(t["glass"]);
	r.headlamp = agent_hex(t["headlamp"]);
	r.tail = agent_hex(t["tail"]);
	r.truck_box = agent_hex(t["truck_box"]);
	r.chrome = agent_hex(t["chrome"]);
	r.roof = agent_hex(t["roof"]);
	r.rider = agent_hex(t["rider"]);
	r.rider_legs = agent_hex(t["rider_legs"]);
	r.shoes = agent_hex(t["shoes"]);
	return r;
}

void agent_box(MeshBuilder &mb, const Vector3 &mn, const Vector3 &mx, const Color &c, AgentTag tag, Limb limb) {
	Vector3 lo(std::min(mn.x, mx.x), std::min(mn.y, mx.y), std::min(mn.z, mx.z)), hi(std::max(mn.x, mx.x), std::max(mn.y, mx.y), std::max(mn.z, mx.z));
	mb.box(lo, hi, c, agent_tag(tag, limb), true);
}

void agent_wheel(MeshBuilder &mb, const Vector3 &c, float r, float w, int sides, int side, const AgentTrim &trim) {
	const float xo = c.x + side * w * 0.5f, xi = c.x - side * w * 0.5f, xh = xo + side * HUB_PROUD_M;
	const Vector2 t2 = agent_tag(AgentTag::FIXED, Limb::WHEEL), hub(c.y, c.z);
	const Vector2 uvs[4] = { hub, hub, hub, hub };
	const Vector3 face(float(side), 0, 0);
	const Color spoke = trim.hub * SPOKE_SHADE;
	for (int i = 0; i < sides; i++) {
		float a0 = TAU_F * i / sides, a1 = TAU_F * (i + 1) / sides, am = (a0 + a1) * 0.5f;
		Vector3 d0(0, std::cos(a0), std::sin(a0)), d1(0, std::cos(a1), std::sin(a1)), dm(0, std::cos(am), std::sin(am));
		Vector3 o0 = Vector3(0, c.y, c.z) + d0 * r, o1 = Vector3(0, c.y, c.z) + d1 * r;
		Vector3 h0 = Vector3(0, c.y, c.z) + d0 * (r * HUB_SHARE), h1 = Vector3(0, c.y, c.z) + d1 * (r * HUB_SHARE);
		Vector3 m0 = Vector3(0, c.y, c.z) + d0 * (r * RIM_SHARE), m1 = Vector3(0, c.y, c.z) + d1 * (r * RIM_SHARE);
		mb.quad(o0 + Vector3(xi, 0, 0), o0 + Vector3(xo, 0, 0), o1 + Vector3(xo, 0, 0), o1 + Vector3(xi, 0, 0), trim.tyre, dm, uvs, t2);
		mb.quad(h0 + Vector3(xo, 0, 0), o0 + Vector3(xo, 0, 0), o1 + Vector3(xo, 0, 0), h1 + Vector3(xo, 0, 0), trim.tyre, face, uvs, t2);
		// rim ring, then alternating spoke / gap sectors so the turning wheel reads at a distance
		mb.quad(m0 + Vector3(xh, 0, 0), h0 + Vector3(xh, 0, 0), h1 + Vector3(xh, 0, 0), m1 + Vector3(xh, 0, 0), trim.hub, face, uvs, t2);
		int k0 = mb.add(Vector3(xh, c.y, c.z), face, i % 2 ? spoke : trim.hub, hub, t2);
		int k1 = mb.add(m0 + Vector3(xh, 0, 0), face, i % 2 ? spoke : trim.hub, hub, t2);
		int k2 = mb.add(m1 + Vector3(xh, 0, 0), face, i % 2 ? spoke : trim.hub, hub, t2);
		mb.tri(k0, k1, k2, face);
	}
}

void agent_arched_box(MeshBuilder &mb, float hw, float y0, float y1, float len, const std::vector<float> &axles, float arch_r, float arch_top,
		const Color &c, AgentTag tag) {
	std::vector<float> zs = axles;
	std::sort(zs.begin(), zs.end());
	float z = -len * 0.5f;
	for (float a : zs) {
		float lo = std::max(z, a - arch_r), hi = std::min(len * 0.5f, a + arch_r);
		if (lo > z) agent_box(mb, Vector3(-hw, y0, z), Vector3(hw, y1, lo), c, tag);
		if (arch_top < y1 && hi > lo) agent_box(mb, Vector3(-hw, arch_top, lo), Vector3(hw, y1, hi), c, tag);
		z = std::max(z, hi);
	}
	if (z < len * 0.5f) agent_box(mb, Vector3(-hw, y0, z), Vector3(hw, y1, len * 0.5f), c, tag);
}

} // namespace city
