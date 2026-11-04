// Shared pieces of the agent meshes: colours from agents.json, tagged boxes and bars, glass panes, wheels and the hollow
// lower bodies of the heavy vehicles (walls with wheel arches and doorways over a floor pan).
#include "render/agents/agent_meshes.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HUB_SHARE = 0.62f, HUB_PROUD_M = 0.01f, RIM_SHARE = 0.5f, SPOKE_SHADE = 0.45f;
constexpr float PAN_GAP_M = 0.02f; // the floor pan's top under the cabin floor
constexpr float ALONG_X = 0.01f;   // a bar this close to X (its square off X) takes Y for its width
constexpr float MM_PER_M = 1000.f, PERCENT = 100.f, M_PER_INCH = 0.0254f; // the inch: 25.4 mm exactly (1959)
} // namespace

Color agent_hex(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

Vector3 agent_v3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

Vector2 agent_v2(const Variant &v) {
	Array a = v;
	return a.size() == 2 ? Vector2(float(a[0]), float(a[1])) : Vector2();
}

Vector2 agent_tyre(const Variant &designation) {
	const Vector3 t = agent_v3(designation); // section width mm, aspect ratio %, rim diameter in
	const float w = t.x / MM_PER_M;
	return Vector2(t.z * M_PER_INCH * 0.5f + w * t.y / PERCENT, w);
}

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
	return r;
}

CabinPaint read_cabin_paint(const Dictionary &d) {
	CabinPaint p;
	p.lining = agent_hex(d["lining"]);
	p.floor = agent_hex(d["floor"]);
	p.seat = agent_hex(d["seat"]);
	p.dash = agent_hex(d["dash"]);
	return p;
}

void agent_box(MeshBuilder &mb, const Vector3 &mn, const Vector3 &mx, const Color &c, AgentTag tag, Limb limb) {
	Vector3 lo(std::min(mn.x, mx.x), std::min(mn.y, mx.y), std::min(mn.z, mx.z)), hi(std::max(mn.x, mx.x), std::max(mn.y, mx.y), std::max(mn.z, mx.z));
	mb.box(lo, hi, c, agent_tag(tag, limb), true);
}

void agent_strut(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, float w, float thick, const Color &c, AgentTag tag, Limb limb) {
	const Vector3 d = (b - a).normalized();
	Vector3 u = Vector3(1, 0, 0) - d * d.x; // X square to the bar
	if (u.length_squared() < ALONG_X) u = Vector3(0, 1, 0) - d * d.y;
	u = u.normalized() * (w * 0.5f);
	const Vector3 v = d.cross(u).normalized() * (thick * 0.5f);
	const Vector2 t2 = agent_tag(tag, limb);
	const Vector3 ring[4] = { u + v, u - v, -u - v, -u + v };
	for (int i = 0; i < 4; i++) {
		const Vector3 &p = ring[i], &q = ring[(i + 1) % 4];
		mb.quad(a + p, b + p, b + q, a + q, c, p + q, nullptr, t2);
	}
	mb.quad(a + ring[0], a + ring[1], a + ring[2], a + ring[3], c, -d, nullptr, t2);
	mb.quad(b + ring[0], b + ring[1], b + ring[2], b + ring[3], c, d, nullptr, t2);
}

void agent_pane(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, const AgentTrim &trim) {
	mb.quad(a, b, c, d, trim.glass, out, nullptr, agent_tag(AgentTag::GLASS));
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

void agent_arched_box(MeshBuilder &mb, float x0, float x1, float y0, float y1, float z0, float z1, const std::vector<float> &axles, float arch_r,
		float arch_top, const Color &c, AgentTag tag) {
	std::vector<float> zs = axles;
	std::sort(zs.begin(), zs.end());
	float z = z0;
	for (float a : zs) {
		float lo = clampf(a - arch_r, z, z1), hi = clampf(a + arch_r, z, z1);
		if (lo > z) agent_box(mb, Vector3(x0, y0, z), Vector3(x1, y1, lo), c, tag);
		if (arch_top < y1 && hi > lo) agent_box(mb, Vector3(x0, arch_top, lo), Vector3(x1, y1, hi), c, tag);
		z = std::max(z, hi);
	}
	if (z < z1) agent_box(mb, Vector3(x0, y0, z), Vector3(x1, y1, z1), c, tag);
}

std::vector<Vector2> spans_between(float z0, float z1, const std::vector<Vector2> &gaps) {
	std::vector<Vector2> g = gaps, out;
	std::sort(g.begin(), g.end(), [](const Vector2 &a, const Vector2 &b) { return a.x < b.x; });
	float z = z0;
	for (const Vector2 &s : g) {
		if (s.x > z) out.push_back(Vector2(z, std::min(s.x, z1)));
		z = std::max(z, s.y);
		if (z >= z1) break;
	}
	if (z < z1) out.push_back(Vector2(z, z1));
	return out;
}

void agent_tub(MeshBuilder &mb, const Tub &t, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		const float xa = side * (t.half_w - t.wall), xb = side * t.half_w;
		const std::vector<Vector2> spans = side < 0 ? spans_between(t.z0, t.z1, t.doors) : std::vector<Vector2>{ Vector2(t.z0, t.z1) };
		for (const Vector2 &s : spans)
			agent_arched_box(mb, std::min(xa, xb), std::max(xa, xb), t.y0, t.y1, s.x, s.y, t.axles, t.arch_r, t.arch_top, AGENT_WHITE, AgentTag::PAINT);
	}
	agent_box(mb, Vector3(-t.pan_half_w, t.y0, t.z0), Vector3(t.pan_half_w, t.floor - PAN_GAP_M, t.z1), trim.under, AgentTag::FIXED);
}

} // namespace city
