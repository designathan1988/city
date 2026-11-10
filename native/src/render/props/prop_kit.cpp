// Builds the street furniture meshes from their data parts and copies them into chunk meshes.
#include "render/props/prop_kit.h"

#include "core/color.h"
#include "core/math.h"
#include "core/units.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HORIZONTAL_EPS = 1e-6f;
constexpr int LIGHT_SIDES = 18, BEAM_SEGMENTS = 4; // Volumetric Light Beam: 18 sides, at least 3 segments for a smooth fall
constexpr float BEAM_LENS = 0.075f; // the cut apex over the radius at the ground: the lens, some 0.28 m of 3.7
constexpr float LIGHT_BEAM_PART = 2.f;
const Color WHITE(1, 1, 1, 1);

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

// A cylinder narrowing from r0 at its base to r1 at its top, its top closed.
void frustum(MeshBuilder &b, const Vector3 &base, float r0, float r1, float h, int seg, const Color &col, const Vector2 &t2) {
	const Vector3 top = base + Vector3(0, h, 0), up(0, 1, 0);
	const float lean = (r0 - r1) / h;
	for (int i = 0; i < seg; i++) {
		const float a0 = TAU_F * i / seg, a1 = TAU_F * (i + 1) / seg;
		const Vector3 d0(std::cos(a0), 0, std::sin(a0)), d1(std::cos(a1), 0, std::sin(a1));
		const Vector3 n0 = (d0 + up * lean).normalized(), n1 = (d1 + up * lean).normalized(), nn = (n0 + n1).normalized();
		const int i0 = b.add(base + d0 * r0, n0, col, Vector2(), t2), i1 = b.add(base + d1 * r0, n1, col, Vector2(), t2);
		const int i2 = b.add(top + d1 * r1, n1, col, Vector2(), t2), i3 = b.add(top + d0 * r1, n0, col, Vector2(), t2);
		b.tri(i0, i1, i2, nn);
		b.tri(i0, i2, i3, nn);
		const int c0 = b.add(top, up, col, Vector2(), t2), c1 = b.add(top + d0 * r1, up, col, Vector2(), t2), c2 = b.add(top + d1 * r1, up, col, Vector2(), t2);
		b.tri(c0, c1, c2, up);
	}
}

// A bar of section width x height from a to b.
void beam(MeshBuilder &b, const Vector3 &from, const Vector3 &to, float w, float h, const Color &col, const Vector2 &t2) {
	const Vector3 along = (to - from).normalized();
	const Vector3 side = (std::fabs(along.y) > 1.f - HORIZONTAL_EPS ? Vector3(1, 0, 0) : along.cross(Vector3(0, 1, 0)).normalized()) * (w * 0.5f);
	const Vector3 up = side.cross(along).normalized() * (h * 0.5f);
	const Vector3 c[4] = { side + up, up - side, -side - up, side - up };
	for (int i = 0; i < 4; i++) {
		const Vector3 p = c[i], q = c[(i + 1) % 4];
		b.quad(from + p, from + q, to + q, to + p, col, (p + q).normalized(), nullptr, t2);
	}
	b.quad(from + c[0], from + c[1], from + c[2], from + c[3], col, -along, nullptr, t2);
	b.quad(to + c[0], to + c[1], to + c[2], to + c[3], col, along, nullptr, t2);
}

// One part in `col`, its UV2 `t2` (its PropTag; a lens's Lens or a pole's PolePart in y).
void shape(MeshBuilder &b, const Dictionary &p, const Color &col, const Vector2 &t2, PackedStringArray &errors) {
	String shape = p.get("shape", "");
	if (shape == "box") b.box(vec3(p["min"]), vec3(p["max"]), col, t2, true);
	else if (shape == "cylinder") b.cylinder(vec3(p["base"]), float(p["radius"]), float(p["height"]), int(p["segments"]), col, true, t2);
	else if (shape == "blob") b.blob(vec3(p["center"]), vec3(p["radius"]), col, t2);
	else if (shape == "frustum") frustum(b, vec3(p["base"]), float(p["radius"]), float(p["top_radius"]), float(p["height"]), int(p["segments"]), col, t2);
	else if (shape == "beam") beam(b, vec3(p["from"]), vec3(p["to"]), float(p["width"]), float(p["thick"]), col, t2);
	else errors.push_back(String("streets.json props: forma desconhecida '") + shape + "'");
}

Color part_color(const Dictionary &p) {
	return rgba8_to_color(parse_hex_color(String(p.get("color", "")).utf8().get_data()));
}

PropTag part_tag(const Dictionary &p) {
	const String look = p.get("glow", "");
	if (look == "night") return PropTag::NIGHT_GLOW;
	if (!bool(p.get("wood", false))) return PropTag::FIXED;
	return String(p.get("shape", "")) == "box" ? PropTag::PLANK : PropTag::WOOD; // sawn, or a trunk
}

// Where on itself a part's vertices lie, for the surface the shader gives it: a plank's length and across it, anything else its height off the ground (props.gdshader).
void mark_surface(MeshBuilder &b, int64_t from, PropTag tag) {
	for (int64_t i = from; i < b.v.size(); i++) {
		const Vector3 &v = b.v[i];
		b.uv.set(i, tag == PropTag::PLANK ? Vector2(v.x, v.y + v.z) : Vector2(0, v.y));
	}
}

// A prop's parts, its lenses (parts with "lens") left to PropKit::lenses.
MeshBuilder parts(const Dictionary &props, const char *name, PackedStringArray &errors) {
	MeshBuilder b;
	if (!props.has(name)) {
		errors.push_back(String("streets.json props: falta ") + name);
		return b;
	}
	Array list = props[name];
	for (int i = 0; i < list.size(); i++) {
		const Dictionary p = list[i];
		if (p.has("lens")) continue;
		const int64_t from = b.v.size();
		shape(b, p, part_color(p), Vector2(float(int(part_tag(p))), float(p.get("part", 0))), errors);
		mark_surface(b, from, part_tag(p));
	}
	return b;
}

// The longest side of a part: what decides whether its shadow reads.
float part_size(const Dictionary &p) {
	const String kind = p.get("shape", "");
	if (kind == "box") {
		const Vector3 d = vec3(p["max"]) - vec3(p["min"]);
		return std::max({ std::fabs(d.x), std::fabs(d.y), std::fabs(d.z) });
	}
	if (kind == "cylinder" || kind == "frustum") return std::max(float(p["height"]), 2.f * float(p["radius"]));
	return BIG;
}

// What a far prop casts: its parts long enough to show in a shadow, round ones with fewer sides. The parts keep their marks, so
// the shader that shows or hides them per instance goes on working.
MeshBuilder cast_parts(const Dictionary &props, const char *name, const Dictionary &cast, PackedStringArray &errors) {
	MeshBuilder b;
	const Array list = props[name];
	const float least = cast["least_m"];
	const int sides = cast["segments"];
	for (int i = 0; i < list.size(); i++) {
		Dictionary p = Dictionary(list[i]).duplicate();
		if (p.has("lens") || part_size(p) < least) continue;
		if (p.has("segments")) p["segments"] = std::min(int(p["segments"]), sides);
		shape(b, p, part_color(p), Vector2(float(int(part_tag(p))), float(p.get("part", 0))), errors);
	}
	return b;
}

// A wire: `chords` straight pieces of a tube of `sides` sides round the x axis from 0 to 1, flat-shaded per side. UV is the
// wire's height and its seat along an urban pole's arm, UV2.y its seat along a rural pole's (or WIRE_SECONDARY_MARK), which
// props.gdshader hangs from the poles by their kinds.
void wire_tube(MeshBuilder &b, const Vector2 &place, float rural_seat, const Color &colour, float radius, int chords, int sides) {
	const Vector2 t2(float(int(PropTag::WIRE)), rural_seat);
	const Vector2 uvs[4] = { place, place, place, place };
	for (int s = 0; s < chords; s++) {
		const float t0 = float(s) / chords, t1 = float(s + 1) / chords;
		for (int k = 0; k < sides; k++) {
			const float a0 = TAU_F * k / sides, a1 = TAU_F * (k + 1) / sides;
			const Vector3 r0(0, std::cos(a0) * radius, std::sin(a0) * radius), r1(0, std::cos(a1) * radius, std::sin(a1) * radius);
			b.quad(Vector3(t0, 0, 0) + r0, Vector3(t1, 0, 0) + r0, Vector3(t1, 0, 0) + r1, Vector3(t0, 0, 0) + r1, colour, (r0 + r1).normalized(), uvs, t2);
		}
	}
}

// Every wire of a span (streets.json wires): the primary ones at y_m[0], the i-th on seat i along the arm (z_m on an urban pole,
// rural_z_m on a rural one), then the secondary ones at the rest of y_m, on the urban rack (secondary_z_m).
void wire_mesh(MeshBuilder &b, const Dictionary &wires, int chords, int sides) {
	const Array ys = wires["y_m"], urban = wires["z_m"], rural = wires["rural_z_m"];
	const Color colour = rgba8_to_color(parse_hex_color(String(wires["color"]).utf8().get_data()));
	const float radius = wires["radius_m"];
	for (int k = 0; k < urban.size(); k++) wire_tube(b, Vector2(float(ys[0]), float(urban[k])), float(rural[k]), colour, radius, chords, sides);
	for (int i = 1; i < ys.size(); i++) wire_tube(b, Vector2(float(ys[i]), float(wires["secondary_z_m"])), WIRE_SECONDARY_MARK, colour, radius, chords, sides);
}

void light_mesh(MeshBuilder &b) {
	const Vector3 up(0, 1, 0);
	// The beam: a unit cone cut at the lens, in rings so the fall along it stays smooth; apex up, foot on the ground.
	std::vector<int> ring(size_t(LIGHT_SIDES) * size_t(BEAM_SEGMENTS + 1));
	for (int s = 0; s <= BEAM_SEGMENTS; s++) {
		const float k = float(s) / BEAM_SEGMENTS, radius = BEAM_LENS + (1.f - BEAM_LENS) * k;
		for (int i = 0; i < LIGHT_SIDES; i++) {
			const float a = TAU_F * i / LIGHT_SIDES;
			const Vector3 d(std::cos(a), 0, std::sin(a));
			ring[size_t(s * LIGHT_SIDES + i)] = b.add(d * radius + up * (1.f - k), (d + up * (1.f - BEAM_LENS)).normalized(), WHITE, Vector2(0, 1.f - k), Vector2(0, LIGHT_BEAM_PART));
		}
	}
	for (int s = 0; s < BEAM_SEGMENTS; s++)
		for (int i = 0; i < LIGHT_SIDES; i++) {
			const int j = (i + 1) % LIGHT_SIDES, a = ring[size_t(s * LIGHT_SIDES + i)], c = ring[size_t(s * LIGHT_SIDES + j)];
			const int d = ring[size_t((s + 1) * LIGHT_SIDES + i)], e = ring[size_t((s + 1) * LIGHT_SIDES + j)];
			const float mid = TAU_F * (i + 0.5f) / LIGHT_SIDES;
			const Vector3 out(std::cos(mid), 1.f - BEAM_LENS, std::sin(mid));
			b.tri(a, d, c, out);
			b.tri(c, d, e, out);
		}
}
} // namespace

// A steel sheet bent along x, from -0.5 to 0.5: `section` its points across, [y, z] in the unit box; both faces seen.
void sheet_mesh(MeshBuilder &b, const Array &section) {
	const Vector2 t2(float(int(PropTag::FINISHED)), 0);
	const float half = 0.5f;
	for (int i = 0; i + 1 < section.size(); i++) {
		const Array p = section[i], q = section[i + 1];
		const Vector3 a(-half, float(p[0]), float(p[1])), c(-half, float(q[0]), float(q[1]));
		const Vector3 d(half, c.y, c.z), e(half, a.y, a.z);
		const Vector3 out = (c - a).cross(Vector3(1, 0, 0)).normalized();
		b.quad(a, c, d, e, WHITE, out, nullptr, t2);
		b.quad(a, c, d, e, WHITE, -out, nullptr, t2);
	}
}

bool PropKit::build(const Dictionary &props, const Dictionary &wires, const Array &beam_section, PackedStringArray &errors) {
	meshes_[int(PropKind::SIGNAL)] = parts(props, "signal", errors);
	meshes_[int(PropKind::HYDRANT)] = parts(props, "hydrant", errors);
	meshes_[int(PropKind::BIN)] = parts(props, "bin", errors);
	meshes_[int(PropKind::BENCH)] = parts(props, "bench", errors);
	meshes_[int(PropKind::POSTBOX)] = parts(props, "postbox", errors);
	meshes_[int(PropKind::PHONE)] = parts(props, "phone", errors);
	meshes_[int(PropKind::DRAIN)] = parts(props, "drain", errors);
	meshes_[int(PropKind::TREE_PIT)] = parts(props, "tree_pit", errors);
	meshes_[int(PropKind::MEADOW)] = parts(props, "meadow", errors);
	pole_ = parts(props, "pole", errors);
	if (!props.has("pole_cast")) errors.push_back("streets.json props: falta pole_cast");
	if (!errors.is_empty()) return false;
	const Dictionary cast = props["pole_cast"];
	pole_cast_ = cast_parts(props, "pole", cast, errors);
	cast_from_ = cast["from_m"];
	for (const char *key : { "y_m", "z_m", "rural_z_m", "secondary_z_m" })
		if (!wires.has(key)) errors.push_back(String("streets.json wires: falta ") + key);
	if (!errors.is_empty()) return false;
	if (Array(wires["z_m"]).size() != Array(wires["rural_z_m"]).size() || Array(wires["y_m"]).is_empty()) {
		errors.push_back("streets.json wires: z_m e rural_z_m com tamanhos diferentes, ou y_m vazio");
		return false;
	}
	wire_mesh(wire_near_, wires, int(wires["near_chords"]), int(wires["near_sides"]));
	wire_mesh(wire_far_, wires, int(wires["far_chords"]), int(wires["far_sides"]));
	box_.box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f), WHITE, Vector2(float(int(PropTag::FINISHED)), 0), true);
	sheet_mesh(beam_, beam_section);
	light_mesh(light_);
	if (!props.has("signal")) return false;
	const Array dark = props["lens_dark"]; // each lens unlit: its colour and its own faint glow
	bool seen[int(Lens::COUNT)] = {};
	Array list = props["signal"];
	for (int i = 0; i < list.size(); i++) {
		const Dictionary p = list[i];
		if (!p.has("lens")) continue;
		const int k = int(p["lens"]);
		if (k < 0 || k >= int(Lens::COUNT)) {
			errors.push_back("streets.json props.signal: lente fora de 0..4");
			continue;
		}
		const Array row = dark[k];
		shape(lenses_, p, Color::html(String(row[0])), Vector2(float(int(PropTag::LENS)), float(k)), errors);
		seen[k] = true;
	}
	for (bool s : seen)
		if (!s) errors.push_back("streets.json props.signal: falta uma das lentes 0..4");
	return errors.is_empty();
}

void PropKit::append(MeshBuilder &dst, const MeshBuilder &src, const Basis &b, const Vector3 &o) {
	int base = int(dst.v.size());
	for (int64_t i = 0; i < src.v.size(); i++)
		dst.add(b.xform(src.v[i]) + o, b.xform(src.n[i]).normalized(), src.c[i], src.uv[i], src.uv2[i]);
	for (int64_t i = 0; i < src.idx.size(); i++) dst.idx.push_back(src.idx[i] + base);
}

Basis PropKit::facing(const Vector3 &face) {
	Vector3 z(face.x, 0, face.z);
	if (z.length_squared() < HORIZONTAL_EPS) z = Vector3(0, 0, 1);
	z = z.normalized();
	Vector3 y(0, 1, 0), x = y.cross(z);
	return Basis(x, y, z);
}

} // namespace city
