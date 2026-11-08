// The models of bay_props.json made into meshes, part by part. A part's tone goes in the vertex colour, over the instance's colour; the
// alpha marks the condenser's front, which building_props.gdshader draws.
#include "render/buildings/bay_props.h"

#include "core/color.h"
#include "core/math.h"
#include "core/seeded.h"

#include <godot_cpp/variant/string.hpp>

#include <cmath>
#include <string>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float FRONT_MARK = 0.f, MIN_LENGTH = 1e-5f, LEAF_ARC = 0.75f, LEAF_BASE_LIFT = 0.05f, LEAF_TIP_FILL = 1.15f, LEAF_TIP_START = 0.08f;
constexpr float LEAF_RIM = 0.004f, BLADE_LONG = 1.4f, BLADE_NARROW = 0.55f, HALF = 0.5f;
constexpr const char *MODEL_NAMES[BAY_PROPS] = { "ac", "pot", "foliage", "bush", "chair", "table", "airer", "dish", "lounger", "bike" };

// [low, high]
float low(const DataValue &v) { return v.list(2).size() == 2 ? v[0].f() : 0.f; }
float high(const DataValue &v) { return v.list(2).size() == 2 ? v[1].f() : 0.f; }
Color tone_of(const DataValue &p) {
	const Vector3 t = p["tone"].v3();
	return Color(t.x, t.y, t.z, p.has("front") && p["front"].flag() ? FRONT_MARK : 1.f);
}
float between(Mulberry32 &r, const DataValue &range) { return low(range) + float(r.next()) * (high(range) - low(range)); }
uint32_t seed_of(const DataValue &p) { return uint32_t(int64_t(p["seed"].num())); }

// A beam from `a` to `b`: `width` across (level where it can be), `thick` the other way.
void bar(MeshBuilder &m, const Vector3 &a, const Vector3 &b, float width, float thick, const Color &col) {
	const Vector3 axis = b - a;
	if (axis.length() < MIN_LENGTH) return;
	const Vector3 along = axis.normalized();
	const Vector3 pick = std::abs(along.z) > 1.f - MIN_LENGTH ? Vector3(1, 0, 0) : Vector3(0, 0, 1);
	const Vector3 side = pick.cross(along).normalized() * (width * HALF), up = along.cross(side).normalized() * (thick * HALF);
	const Vector3 c[4] = { -side - up, side - up, side + up, -side + up };
	for (int i = 0; i < 4; i++) {
		const Vector3 &p = c[i], &q = c[(i + 1) % 4];
		m.quad(a + p, a + q, b + q, b + p, col, (p + q).normalized());
	}
	m.quad(a + c[0], a + c[1], a + c[2], a + c[3], col, -along);
	m.quad(b + c[0], b + c[1], b + c[2], b + c[3], col, along);
}

// Round, its radius going from r0 at `base` to r1 at the top: the side alone.
void frustum(MeshBuilder &m, const Vector3 &base, float r0, float r1, float h, int seg, const Color &col) {
	int first = int(m.v.size());
	for (int i = 0; i <= seg; i++) {
		const float a = TAU_F * float(i) / float(seg);
		const Vector3 n(std::cos(a), 0, std::sin(a));
		m.add(base + n * r0, n, col);
		m.add(base + n * r1 + Vector3(0, h, 0), n, col);
	}
	for (int i = 0; i < seg; i++) {
		const int k = first + i * 2;
		const Vector3 out = m.n[k];
		m.tri(k, k + 1, k + 2, out);
		m.tri(k + 1, k + 3, k + 2, out);
	}
}

// A flat ring around `c` in the plane of `e0`, `e1`, r0 == 0: a disc.
void ring(MeshBuilder &m, const Vector3 &c, const Vector3 &e0, const Vector3 &e1, float r0, float r1, int seg, const Color &col) {
	const Vector3 out = e0.cross(e1);
	for (int i = 0; i < seg; i++) {
		const float a = TAU_F * float(i) / float(seg), b = TAU_F * float(i + 1) / float(seg);
		const Vector3 u = e0 * std::cos(a) + e1 * std::sin(a), w = e0 * std::cos(b) + e1 * std::sin(b);
		m.quad(c + u * r0, c + u * r1, c + w * r1, c + w * r0, col, out);
	}
}

// Leaves arching out of the soil, each a blade twisted about its spine in `steps` lengths.
void rosette(MeshBuilder &m, const DataValue &p) {
	Mulberry32 r{ seed_of(p) };
	const int leaves = p["leaves"].whole(), steps = p["joints"].whole();
	const Color tint = tone_of(p);
	for (int k = 0; k < leaves; k++) {
		const float yaw = TAU_F * float(k) / float(leaves) + float(r.next()) * high(p["twist"]) * HALF;
		const float length = between(r, p["length"]), rise = between(r, p["rise"]), width = between(r, p["width"]), dark = between(r, p["shades"]);
		const float twist = between(r, p["twist"]);
		const Color col(tint.r * dark, tint.g * dark, tint.b * dark);
		const Vector3 dir(std::cos(yaw), 0, std::sin(yaw)), side = Vector3(-dir.z * std::cos(twist), std::sin(twist), dir.x * std::cos(twist));
		Vector3 last[2];
		for (int s = 0; s <= steps; s++) {
			const float t = float(s) / float(steps);
			const float half = width * std::sin(PI_F * std::min(1.f, t * LEAF_TIP_FILL + LEAF_TIP_START)) * HALF + LEAF_RIM;
			const Vector3 spine = dir * (length * t) + Vector3(0, rise * std::sin(t * PI_F * LEAF_ARC) + LEAF_BASE_LIFT, 0);
			const Vector3 next[2] = { spine - side * half, spine + side * half };
			if (s > 0) m.quad(last[0], last[1], next[1], next[0], col, (next[1] - last[0]).cross(next[0] - last[1]).normalized() + Vector3(0, MIN_LENGTH, 0));
			last[0] = next[0];
			last[1] = next[1];
		}
	}
}

// Small leaves on a mound, pointing out of it.
void mound(MeshBuilder &m, const DataValue &p) {
	Mulberry32 r{ seed_of(p) };
	const Color tint = tone_of(p);
	const float y0 = p["centre"].f(), out = low(p["reach"]), up = high(p["reach"]);
	const int leaves = p["leaves"].whole();
	for (int k = 0; k < leaves; k++) {
		const float yaw = float(r.next()) * TAU_F, pitch = between(r, p["lean"]), size = between(r, p["size"]), dark = between(r, p["shades"]);
		const Vector3 n(std::cos(yaw) * std::cos(pitch), std::sin(pitch), std::sin(yaw) * std::cos(pitch));
		const Vector3 c(n.x * out, y0 + n.y * up, n.z * out);
		const Vector3 t = n.cross(Vector3(0, 1, 0)).normalized() * (size * BLADE_NARROW), b = n.cross(t).normalized() * (size * BLADE_LONG);
		m.quad(c - b, c + t, c + b, c - t, Color(tint.r * dark, tint.g * dark, tint.b * dark), n);
	}
}

// A spherical cap of `cap` radians, its pole tilted `tilt` about x from straight up.
void bowl(MeshBuilder &m, const DataValue &p) {
	const Color col = tone_of(p);
	const float rad = p["radius"].f(), cap = p["cap"].f(), tilt = p["tilt"].f();
	const int seg = p["segments"].whole(), rings = p["rings"].whole();
	const Vector3 centre = p["centre"].v3();
	auto dir = [&](int ring_at, int i) {
		const float phi = cap * float(ring_at) / float(rings), th = TAU_F * float(i) / float(seg);
		const Vector3 d(std::sin(phi) * std::cos(th), std::cos(phi), std::sin(phi) * std::sin(th));
		return Vector3(d.x, d.y * std::cos(tilt) - d.z * std::sin(tilt), d.y * std::sin(tilt) + d.z * std::cos(tilt));
	};
	for (int k = 0; k < rings; k++)
		for (int i = 0; i < seg; i++) {
			const Vector3 a = dir(k, i), b = dir(k, i + 1), c = dir(k + 1, i + 1), d = dir(k + 1, i);
			m.quad(centre + a * rad, centre + b * rad, centre + c * rad, centre + d * rad, col, (a + c).normalized());
		}
}

void part(MeshBuilder &m, const DataValue &p) {
	const String shape = p["shape"].text();
	const Color col = tone_of(p);
	const Vector3 up(0, 0, 1), right(1, 0, 0), front(0, 1, 0);
	if (shape == "box") m.box(p["min"].v3(), p["max"].v3(), col, Vector2(), true);
	else if (shape == "bar") bar(m, p["from"].v3(), p["to"].v3(), p["width"].f(), p.has("thick") ? p["thick"].f() : p["width"].f(), col);
	else if (shape == "frustum") frustum(m, p["base"].v3(), p["r0"].f(), p["r1"].f(), p["height"].f(), p["segments"].whole(), col);
	else if (shape == "disc") ring(m, p["centre"].v3(), up, right, 0, p["radius"].f(), p["segments"].whole(), col);
	else if (shape == "ring") ring(m, p["centre"].v3(), right, front, p["r0"].f(), p["r1"].f(), p["segments"].whole(), col);
	else if (shape == "blob") m.blob(p["center"].v3(), p["radius"].v3(), col);
	else if (shape == "rosette") rosette(m, p);
	else if (shape == "mound") mound(m, p);
	else if (shape == "bowl") bowl(m, p);
	else p["shape"].fail(String("unknown shape '") + shape + String("'"));
}

std::vector<uint32_t> colours(const DataValue &list) {
	std::vector<uint32_t> out;
	for (int i = 0; i < list.size(); i++) out.push_back(list[i].colour());
	return out;
}
} // namespace

bool BayPropLook::load(const DataValue &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const DataValue list = t["models"], c = t["colours"], s = t["shares"];
	for (int k = 0; k < BAY_PROPS; k++) {
		const DataValue parts = list[MODEL_NAMES[k]];
		models[size_t(k)] = MeshBuilder(); // a second read is the table again
		for (int i = 0; i < parts.size(); i++) part(models[size_t(k)], parts[i]);
	}
	ac_cases = colours(c["ac_cases"]);
	pots = colours(c["pots"]);
	greens = colours(c["greens"]);
	plastic = colours(c["plastic"]);
	fine = colours(c["fine"]);
	loungers = colours(c["loungers"]);
	bikes = colours(c["bikes"]);
	ac_old = c["ac_old"].colour();
	dish = c["dish"].colour();
	airer = c["airer"].colour();
#define X(name) share.name = s[#name].f();
	BAY_PROP_SHARES(X)
#undef X
	return errors.size() == before;
}

} // namespace city::b40
