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
constexpr int POD_SIDES = 12, POD_RINGS = 4; // a pod's turns round and rings along: the sizes of its arrays below
constexpr float ALONG_X = 0.01f;   // a bar this close to X (its square off X) takes Y for its width
constexpr float MM_PER_M = 1000.f, PERCENT = 100.f, M_PER_INCH = 0.0254f; // the inch: 25.4 mm exactly (1959)
} // namespace

Color agent_hex(const DataValue &v) { return rgba8_to_color(v.colour()); }
Vector3 agent_v3(const DataValue &v) { return v.v3(); }
Vector2 agent_v2(const DataValue &v) { return v.v2(); }

Vector2 agent_tyre(const DataValue &designation) {
	const Vector3 t = agent_v3(designation); // section width mm, aspect ratio %, rim diameter in
	const float w = t.x / MM_PER_M;
	return Vector2(t.z * M_PER_INCH * 0.5f + w * t.y / PERCENT, w);
}

AgentTrim read_trim(const DataValue &t) {
	AgentTrim r;
	r.tyre = agent_hex(t["tyre"]);
	r.hub = agent_hex(t["hub"]);
	r.under = agent_hex(t["under"]);
	r.blackout = agent_hex(t["blackout"]);
	r.grille = agent_hex(t["grille"]);
	r.plate = agent_hex(t["plate"]);
	r.glass = agent_hex(t["glass"]);
	r.indicator = agent_hex(t["indicator"]);
	r.mirror = agent_hex(t["mirror"]);
	r.headlamp = agent_hex(t["headlamp"]);
	r.tail = agent_hex(t["tail"]);
	r.truck_box = agent_hex(t["truck_box"]);
	r.chrome = agent_hex(t["chrome"]);
	r.roof = agent_hex(t["roof"]);
	return r;
}

CabinPaint read_cabin_paint(const DataValue &d) {
	CabinPaint p;
	p.lining = agent_hex(d["lining"]);
	p.floor = agent_hex(d["floor"]);
	p.seat = agent_hex(d["seat"]);
	p.dash = agent_hex(d["dash"]);
	return p;
}

// Round to the eye and flat at the sides: a superellipse, at one of its POD_SIDES turns.
static Vector2 pod_turn(const PartsShape &K, int i) {
	const float a = TAU_F * float(i) / float(POD_SIDES), c = std::cos(a), s = std::sin(a), e = 2.f / K.pod_squareness;
	return Vector2(std::copysign(std::pow(std::fabs(c), e), c), std::copysign(std::pow(std::fabs(s), e), s));
}

void agent_pod_face(MeshBuilder &mb, const AgentShapes &shapes, const Vector3 &centre, const Vector2 &half, const Color &c, AgentTag tag) {
	const PartsShape &K = shapes.agent_parts;
	const Vector3 out(0, 0, -1);
	const Vector2 t2 = agent_tag(tag);
	const int hub = mb.add(centre, out, c, Vector2(), t2);
	int first = -1, last = -1;
	for (int i = 0; i < POD_SIDES; i++) {
		const Vector2 r = pod_turn(K, i) * half;
		const int v = mb.add(centre + Vector3(r.x, r.y, 0), out, c, Vector2(), t2);
		if (last >= 0) mb.tri(hub, last, v, out);
		else first = v;
		last = v;
	}
	mb.tri(hub, last, first, out);
}

void agent_pod(MeshBuilder &mb, const AgentShapes &shapes, const Vector3 &centre, const Vector2 &half, float depth, const Color &c, AgentTag tag) {
	const PartsShape &K = shapes.agent_parts;
	Vector3 p[POD_RINGS + 1][POD_SIDES];
	for (int k = 0; k <= POD_RINGS; k++) {
		const float t = float(k) / float(POD_RINGS), shrink = std::pow(1.f - std::pow(t, K.pod_nose), 1.f / K.pod_nose);
		for (int i = 0; i < POD_SIDES; i++) {
			const Vector2 r = pod_turn(K, i) * half * shrink;
			p[k][i] = centre + Vector3(r.x, r.y, t * depth);
		}
	}
	const Vector2 t2 = agent_tag(tag);
	int v[POD_RINGS][POD_SIDES];
	for (int k = 0; k < POD_RINGS; k++) {
		for (int i = 0; i < POD_SIDES; i++) {
			const Vector3 around = p[k][(i + 1) % POD_SIDES] - p[k][(i + POD_SIDES - 1) % POD_SIDES], along = p[k + 1][i] - p[std::max(k - 1, 0)][i];
			Vector3 n = around.cross(along).normalized();
			if (n.dot(p[k][i] - centre - Vector3(0, 0, p[k][i].z - centre.z)) < 0) n = -n;
			v[k][i] = mb.add(p[k][i], n, c, Vector2(), t2);
		}
	}
	for (int k = 0; k + 1 < POD_RINGS; k++) {
		for (int i = 0; i < POD_SIDES; i++) {
			const int j = (i + 1) % POD_SIDES;
			const Vector3 out = p[k][i] - centre - Vector3(0, 0, p[k][i].z - centre.z);
			mb.tri(v[k][i], v[k][j], v[k + 1][j], out);
			mb.tri(v[k][i], v[k + 1][j], v[k + 1][i], out);
		}
	}
	const Vector3 forward(0, 0, 1);
	const int tip = mb.add(centre + forward * depth, forward, c, Vector2(), t2);
	for (int i = 0; i < POD_SIDES; i++) mb.tri(v[POD_RINGS - 1][i], v[POD_RINGS - 1][(i + 1) % POD_SIDES], tip, forward);
	agent_pod_face(mb, shapes, centre, half, c, tag);
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
	const PartsShape &K = trim.shapes->agent_parts;
	const float xo = c.x + side * w * 0.5f, xi = c.x - side * w * 0.5f, xh = xo + side * K.hub_proud_m;
	const Vector2 t2 = agent_wheel_tag(c.x), hub(c.y, c.z);
	const Vector2 uvs[4] = { hub, hub, hub, hub };
	const Vector3 face(float(side), 0, 0);
	const Color spoke = trim.hub * K.spoke_shade;
	for (int i = 0; i < sides; i++) {
		float a0 = TAU_F * i / sides, a1 = TAU_F * (i + 1) / sides, am = (a0 + a1) * 0.5f;
		Vector3 d0(0, std::cos(a0), std::sin(a0)), d1(0, std::cos(a1), std::sin(a1)), dm(0, std::cos(am), std::sin(am));
		Vector3 o0 = Vector3(0, c.y, c.z) + d0 * r, o1 = Vector3(0, c.y, c.z) + d1 * r;
		Vector3 h0 = Vector3(0, c.y, c.z) + d0 * (r * K.hub_share), h1 = Vector3(0, c.y, c.z) + d1 * (r * K.hub_share);
		Vector3 m0 = Vector3(0, c.y, c.z) + d0 * (r * K.rim_share), m1 = Vector3(0, c.y, c.z) + d1 * (r * K.rim_share);
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

namespace {

struct Lathe {
	MeshBuilder &mb;
	Vector3 c;
	int side, sides;
	Vector2 hub, t2;
};

// One ring of a body of revolution about X: radius rho, `axial` out along the wheel's outer side, normal (radial, axial).
std::vector<int> lathe_ring(Lathe &l, float rho, float axial, float nr, float na, const Color &col) {
	std::vector<int> out;
	for (int j = 0; j <= l.sides; j++) {
		const float a = TAU_F * j / l.sides, cy = std::cos(a), cz = std::sin(a);
		out.push_back(l.mb.add(Vector3(l.c.x + l.side * axial, l.c.y + cy * rho, l.c.z + cz * rho), Vector3(l.side * na, cy * nr, cz * nr), col, l.hub, l.t2));
	}
	return out;
}

void lathe_band(Lathe &l, const std::vector<int> &a, const std::vector<int> &b) {
	for (int j = 0; j < l.sides; j++) {
		const Vector3 mid = l.mb.n[a[size_t(j)]] + l.mb.n[b[size_t(j)]];
		l.mb.tri(a[size_t(j)], a[size_t(j) + 1], b[size_t(j) + 1], mid);
		l.mb.tri(a[size_t(j)], b[size_t(j) + 1], b[size_t(j)], mid);
	}
}

// A flat disc at `axial`, from the hub out to `rho`, facing the wheel's outer side.
void lathe_disc(Lathe &l, float rho, float axial, const Color &col) {
	const Vector3 face(float(l.side), 0, 0);
	const int mid = l.mb.add(Vector3(l.c.x + l.side * axial, l.c.y, l.c.z), face, col, l.hub, l.t2);
	const std::vector<int> ring = lathe_ring(l, rho, axial, 0, 1, col);
	for (int j = 0; j < l.sides; j++) l.mb.tri(mid, ring[size_t(j)], ring[size_t(j) + 1], face);
}

// A spoke from the hub out, narrowing to its tip, its face at `axial`.
void spoke(Lathe &l, float angle, float reach, float root, float tip_w, float axial, const Color &col) {
	const Vector3 out(0, std::cos(angle), std::sin(angle)), across(0, -out.z, out.y), face(float(l.side), 0, 0);
	const Vector3 at(l.c.x + l.side * axial, l.c.y, l.c.z), wide = across * (root * 0.5f), thin = across * (tip_w * 0.5f), tip = out * reach;
	const Vector3 back = face * -axial;
	const Vector2 uvs[4] = { l.hub, l.hub, l.hub, l.hub };
	l.mb.quad(at - wide, at + wide, at + thin + tip, at - thin + tip, col, face, uvs, l.t2);
	l.mb.quad(at + wide, at + wide + back, at + thin + tip + back, at + thin + tip, col, across, uvs, l.t2);
	l.mb.quad(at - wide, at - wide + back, at - thin + tip + back, at - thin + tip, col, -across, uvs, l.t2);
	l.mb.quad(at - thin + tip, at + thin + tip, at + thin + tip + back, at - thin + tip + back, col, out, uvs, l.t2);
}
} // namespace

void agent_wheel_near(MeshBuilder &mb, const Vector3 &c, float r, float w, int sides, int side, const AgentTrim &trim) {
	const PartsShape &K = trim.shapes->agent_parts;
	Lathe l{ mb, c, side, sides, Vector2(c.y, c.z), agent_wheel_tag(c.x) };
	const float h = r * K.tyre_section, outer = r - h, lip = outer - h * K.tyre_lip, half = w * 0.5f;
	std::vector<int> ring = lathe_ring(l, lip, -half, 0, -1, trim.tyre);
	for (int i = 0; i <= K.shoulder_steps; i++) {
		const float a = -PI_F * 0.5f + PI_F * float(i) / K.shoulder_steps, cs = std::cos(a), sn = std::sin(a);
		std::vector<int> next = lathe_ring(l, outer + cs * h, sn * half, cs, sn, trim.tyre);
		lathe_band(l, ring, next);
		ring = std::move(next);
	}
	std::vector<int> last = lathe_ring(l, lip, half, 0, 1, trim.tyre);
	lathe_band(l, ring, last);
	std::vector<int> sunk = lathe_ring(l, lip, w * K.rim_face, -1, 0, trim.tyre); // the sidewall's inner edge down to the rim
	lathe_band(l, last, sunk);
	// The rim: its lip in metal, the well dark behind the spokes and the brake's disc in the well.
	const float face = w * K.rim_face;
	lathe_disc(l, lip * K.lip_in, face, trim.under);
	lathe_disc(l, lip * K.rotor_r, face + K.rotor_deep, trim.hub * K.rotor_shade);
	std::vector<int> lip_out = lathe_ring(l, lip, face + K.hub_proud_m, 0, 1, trim.hub), lip_in = lathe_ring(l, lip * K.lip_in, face + K.hub_proud_m, 0, 1, trim.hub);
	lathe_band(l, lip_out, lip_in);
	for (int i = 0; i < K.spokes; i++) spoke(l, TAU_F * i / K.spokes, lip * (K.lip_in + K.rotor_deep), r * K.spoke_root, r * K.spoke_tip, w * K.spoke_face, trim.hub);
	Lathe boss{ mb, c, side, K.boss_sides, Vector2(c.y, c.z), agent_wheel_tag(c.x) };
	lathe_disc(boss, r * K.boss_r, w * K.boss_face, trim.hub);
}

void agent_spoked_wheel(MeshBuilder &mb, const Vector3 &c, float r, float w, int sides, int spokes, const AgentTrim &trim) {
	const PartsShape &K = trim.shapes->agent_parts;
	const float half = w * 0.5f, inner = r - w;
	for (int side : { -1, 1 }) {
		Lathe l{ mb, c, side, sides, Vector2(c.y, c.z), agent_wheel_tag(c.x) };
		std::vector<int> ring = lathe_ring(l, r - half, 0, 1, 0, trim.tyre);
		for (int i = 1; i <= K.shoulder_steps; i++) {
			const float a = PI_F * float(i) / K.shoulder_steps, cs = std::cos(a), sn = std::sin(a);
			std::vector<int> next = lathe_ring(l, r - half + cs * half, sn * half, cs, sn, trim.tyre);
			lathe_band(l, ring, next);
			ring = std::move(next);
		}
		for (int i = 0; i < spokes / 2; i++) spoke(l, TAU_F * (float(i) + (side > 0 ? 0.f : 0.5f)) / float(spokes / 2), inner, K.wire_m, K.wire_m, half * K.rim_face, trim.hub);
		Lathe boss{ mb, c, side, K.boss_sides, Vector2(c.y, c.z), agent_wheel_tag(c.x) };
		lathe_disc(boss, r * K.boss_r * K.boss_r, half, trim.hub);
	}
}

void agent_extrude(MeshBuilder &mb, const std::vector<Vector2> &p, float x0, float x1, const Color &c, AgentTag tag) {
	const Vector2 t2 = agent_tag(tag);
	const size_t n = p.size();
	Vector2 mid;
	for (const Vector2 &q : p) mid += q / float(n);
	for (size_t i = 0; i < n; i++) {
		const Vector2 &a = p[i], &b = p[(i + 1) % n], e = b - a;
		Vector2 out(e.y, -e.x);
		if (out.dot((a + b) * 0.5f - mid) < 0) out = -out;
		mb.quad(Vector3(x0, a.y, a.x), Vector3(x1, a.y, a.x), Vector3(x1, b.y, b.x), Vector3(x0, b.y, b.x), c, Vector3(0, out.y, out.x), nullptr, t2);
	}
	for (float x : { x0, x1 }) {
		const Vector3 face(x == x0 ? (x0 < x1 ? -1.f : 1.f) : (x0 < x1 ? 1.f : -1.f), 0, 0);
		for (size_t i = 1; i + 1 < n; i++) {
			const int a = mb.add(Vector3(x, p[0].y, p[0].x), face, c, Vector2(), t2), b = mb.add(Vector3(x, p[i].y, p[i].x), face, c, Vector2(), t2);
			const int d = mb.add(Vector3(x, p[i + 1].y, p[i + 1].x), face, c, Vector2(), t2);
			mb.tri(a, b, d, face);
		}
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
	const PartsShape &K = trim.shapes->agent_parts;
	for (int side : { -1, 1 }) {
		const float xa = side * (t.half_w - t.wall), xb = side * t.half_w;
		const std::vector<Vector2> spans = side < 0 ? spans_between(t.z0, t.z1, t.doors) : std::vector<Vector2>{ Vector2(t.z0, t.z1) };
		for (const Vector2 &s : spans)
			agent_arched_box(mb, std::min(xa, xb), std::max(xa, xb), t.y0, t.y1, s.x, s.y, t.axles, t.arch_r, t.arch_top, AGENT_WHITE, AgentTag::PAINT);
	}
	agent_box(mb, Vector3(-t.pan_half_w, t.y0, t.z0), Vector3(t.pan_half_w, t.floor - K.pan_gap_m, t.z1), trim.under, AgentTag::FIXED);
}

} // namespace city
