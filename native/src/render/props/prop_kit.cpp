// Builds the street furniture meshes from their data parts and copies them into chunk meshes.
#include "render/props/prop_kit.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace city {

namespace {
constexpr float HORIZONTAL_EPS = 1e-6f;

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

// One part in `col`, its UV2 `t2` (the PropTag of a prop's part, the Lens of a lens).
void shape(MeshBuilder &b, const Dictionary &p, const Color &col, const Vector2 &t2, PackedStringArray &errors) {
	String shape = p.get("shape", "");
	if (shape == "box") b.box(vec3(p["min"]), vec3(p["max"]), col, t2, true);
	else if (shape == "cylinder") b.cylinder(vec3(p["base"]), float(p["radius"]), float(p["height"]), int(p["segments"]), col, true, t2);
	else if (shape == "blob") b.blob(vec3(p["center"]), vec3(p["radius"]), col, t2);
	else errors.push_back(String("streets.json props: forma desconhecida '") + shape + "'");
}

Color part_color(const Dictionary &p) {
	return rgba8_to_color(parse_hex_color(String(p.get("color", "")).utf8().get_data()));
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
		const PropTag tag = String(p.get("glow", "")) == "night" ? PropTag::NIGHT_GLOW : PropTag::FIXED;
		shape(b, p, part_color(p), Vector2(float(int(tag)), 0), errors);
	}
	return b;
}
} // namespace

bool PropKit::build(const Dictionary &props, PackedStringArray &errors) {
	meshes_[int(PropKind::SIGNAL)] = parts(props, "signal", errors);
	meshes_[int(PropKind::LAMP)] = parts(props, "lamp", errors);
	meshes_[int(PropKind::HYDRANT)] = parts(props, "hydrant", errors);
	meshes_[int(PropKind::BIN)] = parts(props, "bin", errors);
	meshes_[int(PropKind::BENCH)] = parts(props, "bench", errors);
	if (!props.has("signal")) return false;
	const float dim = float(props.get("lens_dim", 0.f));
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
		const Color lit = part_color(p);
		shape(lenses_, p, Color(lit.r * dim, lit.g * dim, lit.b * dim, lit.a), Vector2(float(k), 0), errors);
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
