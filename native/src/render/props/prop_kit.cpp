// Builds the street furniture meshes from their data parts and copies them into chunk meshes.
#include "render/props/prop_kit.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int LENS_NONE = -1, LENS_RED = 0, LENS_GREEN = 2;
constexpr float HORIZONTAL_EPS = 1e-6f;

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

// One part; a lens (part "lens": 0 red, 1 amber, 2 green) is lit when it is `lit`, else dimmed by `dim`.
void part(MeshBuilder &b, const Dictionary &p, int lit, float dim, PackedStringArray &errors) {
	Color col = rgba8_to_color(parse_hex_color(String(p.get("color", "")).utf8().get_data()));
	PropTag tag = String(p.get("glow", "")) == "night" ? PropTag::NIGHT_GLOW : PropTag::FIXED;
	if (p.has("lens")) {
		bool on = int(p["lens"]) == lit;
		tag = on ? PropTag::LIT : PropTag::FIXED;
		if (!on) col = Color(col.r * dim, col.g * dim, col.b * dim, col.a);
	}
	const Vector2 t2(float(int(tag)), 0);
	String shape = p.get("shape", "");
	if (shape == "box") b.box(vec3(p["min"]), vec3(p["max"]), col, t2, true);
	else if (shape == "cylinder") b.cylinder(vec3(p["base"]), float(p["radius"]), float(p["height"]), int(p["segments"]), col, true, t2);
	else if (shape == "blob") b.blob(vec3(p["center"]), vec3(p["radius"]), col, t2);
	else errors.push_back(String("streets.json props: forma desconhecida '") + shape + "'");
}

MeshBuilder parts(const Dictionary &props, const char *name, int lit, PackedStringArray &errors) {
	MeshBuilder b;
	if (!props.has(name)) {
		errors.push_back(String("streets.json props: falta ") + name);
		return b;
	}
	float dim = float(props.get("lens_dim", 0.f));
	Array list = props[name];
	for (int i = 0; i < list.size(); i++) part(b, list[i], lit, dim, errors);
	return b;
}
} // namespace

bool PropKit::build(const Dictionary &props, PackedStringArray &errors) {
	meshes_[int(PropKind::SIGNAL_RED)] = parts(props, "signal", LENS_RED, errors);
	meshes_[int(PropKind::SIGNAL_GREEN)] = parts(props, "signal", LENS_GREEN, errors);
	meshes_[int(PropKind::LAMP)] = parts(props, "lamp", LENS_NONE, errors);
	meshes_[int(PropKind::HYDRANT)] = parts(props, "hydrant", LENS_NONE, errors);
	meshes_[int(PropKind::BIN)] = parts(props, "bin", LENS_NONE, errors);
	meshes_[int(PropKind::BENCH)] = parts(props, "bench", LENS_NONE, errors);
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
