// The models of bay_props.json made into meshes, part by part. A part's tone goes in the vertex colour, over the instance's colour; the
// alpha marks the condenser's front, which building_props.gdshader draws.
#include "render/buildings/bay_props.h"

#include "core/color.h"
#include "core/math.h"
#include "core/seeded.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>
#include <string>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float FRONT_MARK = 0.f, MIN_LENGTH = 1e-5f, LEAF_ARC = 0.75f, LEAF_BASE_LIFT = 0.05f, LEAF_TIP_FILL = 1.15f, LEAF_TIP_START = 0.08f;
constexpr float LEAF_RIM = 0.004f, BLADE_LONG = 1.4f, BLADE_NARROW = 0.55f, HALF = 0.5f;
constexpr const char *MODEL_NAMES[BAY_PROPS] = { "ac", "pot", "foliage", "bush", "chair", "table", "airer", "dish", "lounger", "bike" };

Vector3 vec3(const Variant &v) {
	const Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}
float low(const Variant &v) { return float(Array(v)[0]); }
float high(const Variant &v) { return float(Array(v)[1]); }
Color tone_of(const Dictionary &p) {
	const Vector3 t = vec3(p["tone"]);
	return Color(t.x, t.y, t.z, bool(p.get("front", false)) ? FRONT_MARK : 1.f);
}
float between(Mulberry32 &r, const Variant &range) { return low(range) + float(r.next()) * (high(range) - low(range)); }

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
void rosette(MeshBuilder &m, const Dictionary &p) {
	Mulberry32 r{ uint32_t(int64_t(p["seed"])) };
	const int leaves = int(p["leaves"]), steps = int(p["joints"]);
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
void mound(MeshBuilder &m, const Dictionary &p) {
	Mulberry32 r{ uint32_t(int64_t(p["seed"])) };
	const Color tint = tone_of(p);
	const float y0 = float(p["centre"]), out = low(p["reach"]), up = high(p["reach"]);
	for (int k = 0; k < int(p["leaves"]); k++) {
		const float yaw = float(r.next()) * TAU_F, pitch = between(r, p["lean"]), size = between(r, p["size"]), dark = between(r, p["shades"]);
		const Vector3 n(std::cos(yaw) * std::cos(pitch), std::sin(pitch), std::sin(yaw) * std::cos(pitch));
		const Vector3 c(n.x * out, y0 + n.y * up, n.z * out);
		const Vector3 t = n.cross(Vector3(0, 1, 0)).normalized() * (size * BLADE_NARROW), b = n.cross(t).normalized() * (size * BLADE_LONG);
		m.quad(c - b, c + t, c + b, c - t, Color(tint.r * dark, tint.g * dark, tint.b * dark), n);
	}
}

// A spherical cap of `cap` radians, its pole tilted `tilt` about x from straight up.
void bowl(MeshBuilder &m, const Dictionary &p) {
	const Color col = tone_of(p);
	const float rad = float(p["radius"]), cap = float(p["cap"]), tilt = float(p["tilt"]);
	const int seg = int(p["segments"]), rings = int(p["rings"]);
	const Vector3 centre = vec3(p["centre"]);
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

void part(MeshBuilder &m, const Dictionary &p, PackedStringArray &errors) {
	const String shape = p.get("shape", "");
	const Color col = tone_of(p);
	const Vector3 up(0, 0, 1), right(1, 0, 0), front(0, 1, 0);
	if (shape == "box") m.box(vec3(p["min"]), vec3(p["max"]), col, Vector2(), true);
	else if (shape == "bar") bar(m, vec3(p["from"]), vec3(p["to"]), float(p["width"]), float(p.get("thick", p["width"])), col);
	else if (shape == "frustum") frustum(m, vec3(p["base"]), float(p["r0"]), float(p["r1"]), float(p["height"]), int(p["segments"]), col);
	else if (shape == "disc") ring(m, vec3(p["centre"]), up, right, 0, float(p["radius"]), int(p["segments"]), col);
	else if (shape == "ring") ring(m, vec3(p["centre"]), right, front, float(p["r0"]), float(p["r1"]), int(p["segments"]), col);
	else if (shape == "blob") m.blob(vec3(p["center"]), vec3(p["radius"]), col);
	else if (shape == "rosette") rosette(m, p);
	else if (shape == "mound") mound(m, p);
	else if (shape == "bowl") bowl(m, p);
	else errors.push_back(String("bay_props.json: forma desconhecida '") + shape + "'");
}

std::vector<uint32_t> colours(const Array &list) {
	std::vector<uint32_t> out;
	for (int i = 0; i < list.size(); i++) out.push_back(parse_hex_color(String(list[i]).utf8().get_data()));
	return out;
}
uint32_t colour(const Variant &v) { return parse_hex_color(String(v).utf8().get_data()); }
} // namespace

bool BayPropLook::load(const Dictionary &t, PackedStringArray &errors) {
	const Dictionary list = t.get("models", Dictionary()), c = t.get("colours", Dictionary()), s = t.get("shares", Dictionary());
	for (int k = 0; k < BAY_PROPS; k++) {
		if (!list.has(MODEL_NAMES[k])) {
			errors.push_back(String("bay_props.json: falta o modelo ") + MODEL_NAMES[k]);
			continue;
		}
		const Array parts = list[MODEL_NAMES[k]];
		for (int i = 0; i < parts.size(); i++) part(models[size_t(k)], parts[i], errors);
	}
	ac_cases = colours(c.get("ac_cases", Array()));
	pots = colours(c.get("pots", Array()));
	greens = colours(c.get("greens", Array()));
	plastic = colours(c.get("plastic", Array()));
	fine = colours(c.get("fine", Array()));
	loungers = colours(c.get("loungers", Array()));
	bikes = colours(c.get("bikes", Array()));
	ac_old = colour(c.get("ac_old", ""));
	dish = colour(c.get("dish", ""));
	airer = colour(c.get("airer", ""));
#define X(name) share.name = float(s.get(#name, 0));
	BAY_PROP_SHARES(X)
#undef X
	return errors.is_empty();
}

} // namespace city::b40
