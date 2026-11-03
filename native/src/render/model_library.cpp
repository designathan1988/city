// Unit masses for instanced buildings and the data-driven model builder.
#include "render/model_library.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

const Color WHITE(1, 1, 1, 1);

Vector3 vec3(const Variant &v) {
	Array a = v;
	return a.size() == 3 ? Vector3(float(a[0]), float(a[1]), float(a[2])) : Vector3();
}

void build_mass(MeshBuilder &b) {
	const float h = 0.5f;
	const Vector3 p[BOX_CORNERS] = { Vector3(-h, 0, -h), Vector3(h, 0, -h), Vector3(h, 0, h), Vector3(-h, 0, h),
		Vector3(-h, 1, -h), Vector3(h, 1, -h), Vector3(h, 1, h), Vector3(-h, 1, h) };
	const Vector2 side_uv[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
	for (int f = 0; f < BOX_FACES - 1; f++) { // no bottom: masses sit on their foundation
		const int *q = BOX_FACE_CORNERS[f];
		const Vector3 out = Vector3(float(BOX_FACE_NORMALS[f][0]), float(BOX_FACE_NORMALS[f][1]), float(BOX_FACE_NORMALS[f][2]));
		MassFace tag = out.y > 0.f ? MassFace::TOP : (out.x != 0.f ? MassFace::SIDE_X : MassFace::SIDE_Z);
		b.quad(p[q[0]], p[q[1]], p[q[2]], p[q[3]], WHITE, out, side_uv, Vector2(float(int(tag)), 0));
	}
}

void build_gable(MeshBuilder &b) {
	const float h = 0.5f;
	const Vector3 l0(-h, 0, -h), l1(h, 0, -h), r0(-h, 0, h), r1(h, 0, h), t0(-h, 1, 0), t1(h, 1, 0);
	b.quad(l0, l1, t1, t0, WHITE, Vector3(0, h, -1));
	b.quad(r1, r0, t0, t1, WHITE, Vector3(0, h, 1));
	for (int side : { -1, 1 }) {
		Vector3 out(float(side), 0, 0);
		const Vector3 &a = side < 0 ? l0 : l1, &c = side < 0 ? r0 : r1, &t = side < 0 ? t0 : t1;
		int i0 = b.add(a, out, WHITE), i1 = b.add(c, out, WHITE), i2 = b.add(t, out, WHITE);
		b.tri(i0, i1, i2, out);
	}
}

void build_hip(MeshBuilder &b) {
	const float h = 0.5f, ridge = 0.25f;
	const Vector3 c0(-h, 0, -h), c1(h, 0, -h), c2(h, 0, h), c3(-h, 0, h), t0(-ridge, 1, 0), t1(ridge, 1, 0);
	b.quad(c0, c1, t1, t0, WHITE, Vector3(0, 1, -1));
	b.quad(c2, c3, t0, t1, WHITE, Vector3(0, 1, 1));
	for (int side : { -1, 1 }) {
		Vector3 out = Vector3(float(side), h, 0).normalized();
		const Vector3 &a = side < 0 ? c3 : c1, &c = side < 0 ? c0 : c2, &t = side < 0 ? t0 : t1;
		int i0 = b.add(a, out, WHITE), i1 = b.add(c, out, WHITE), i2 = b.add(t, out, WHITE);
		b.tri(i0, i1, i2, out);
	}
}

bool build_part(MeshBuilder &b, const Dictionary &part, PackedStringArray &errors, const String &where) {
	String mat = part.get("material", "");
	Color col = WHITE;
	SurfaceTag tag = SurfaceTag::FIXED;
	if (mat == "tint") tag = SurfaceTag::TINT;
	else if (mat == "glow") tag = SurfaceTag::GLOW;
	else col = rgba8_to_color(parse_hex_color(mat.utf8().get_data()));
	const Vector2 t2(float(int(tag)), 0);
	String shape = part.get("shape", "");
	if (shape == "box") b.box(vec3(part["min"]), vec3(part["max"]), col, t2, true);
	else if (shape == "cylinder") b.cylinder(vec3(part["base"]), float(part["radius"]), float(part["height"]), int(part["segments"]), col, bool(part["cap"]), t2);
	else if (shape == "cone") b.cone(vec3(part["base"]), float(part["radius"]), float(part["height"]), int(part["segments"]), col, t2);
	else if (shape == "blob") b.blob(vec3(part["center"]), vec3(part["radius"]), col, t2);
	else {
		errors.push_back(where + String(": forma desconhecida '") + shape + String("'"));
		return false;
	}
	return true;
}

} // namespace

bool ModelLibrary::build(const Dictionary &models_json, PackedStringArray &errors) {
	MeshBuilder mb, gb, hb;
	build_mass(mb);
	build_gable(gb);
	build_hip(hb);
	mass_box = mb.commit();
	roof_gable = gb.commit();
	roof_hip = hb.commit();
	Dictionary defs = models_json.get("models", Dictionary());
	Array names = defs.keys();
	for (int i = 0; i < names.size(); i++) {
		String name = names[i];
		MeshBuilder b;
		Array parts = defs[name];
		for (int k = 0; k < parts.size(); k++) build_part(b, parts[k], errors, "models." + name);
		models[std::string(name.utf8().get_data())] = b.commit();
	}
	return errors.is_empty();
}

RID ModelLibrary::model(const char *name) const {
	auto it = models.find(name);
	return it == models.end() || it->second.is_null() ? RID() : it->second->get_rid();
}

} // namespace city
