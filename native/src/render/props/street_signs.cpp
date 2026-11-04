// The plates of a chunk, see street_signs.h.
#include "render/props/street_signs.h"

#include "core/color.h"
#include "core/units.h"
#include "render/props/prop_kit.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
const char *const PLATE_KEYS[SIGN_TYPES] = { "stop", "yield", "speed", "noParking", "noEntry", "pedestrian", "school", "direction", "street", "info" };
const Vector3 UP(0, 1, 0);
const Vector2 TAG_FIXED(float(int(PropTag::FIXED)), 0), TAG_PLATE(float(int(PropTag::PLATE)), 0);

// The outline of a plate in its own plane, unit square coordinates in -0.5 .. 0.5.
void outline(const PlateLook &p, std::vector<Vector2> &out) {
	if (p.points == 0) {
		out = { Vector2(-0.5f, -0.5f), Vector2(0.5f, -0.5f), Vector2(0.5f, 0.5f), Vector2(-0.5f, 0.5f) };
		return;
	}
	for (int k = 0; k < p.points; k++) {
		const float a = (p.turn + float(k) / float(p.points)) * TAU_F;
		out.push_back(Vector2(std::cos(a), std::sin(a)) * 0.5f);
	}
}

// A plate fanned round its centre at `at`: `side` its width direction, its face looking along `face`.
void plate(MeshBuilder &mb, const PlateLook &p, const SignLook &K, const Vector3 &at, const Vector3 &side, const Vector3 &face, const Rect2 &cell, bool painted) {
	std::vector<Vector2> ring;
	outline(p, ring);
	const auto corner = [&](const Vector2 &q) { return at + side * (q.x * p.w) + UP * (q.y * p.h); };
	const auto uv = [&](const Vector2 &q) { return cell.position + Vector2(q.x + 0.5f, 0.5f - q.y) * cell.size; };
	const Color col = painted ? Color(1, 1, 1) : K.back;
	const Vector2 tag = painted ? TAG_PLATE : TAG_FIXED;
	const int centre = mb.add(at, face, col, uv(Vector2()), tag);
	std::vector<int> rim;
	for (const Vector2 &q : ring) rim.push_back(mb.add(corner(q), face, col, uv(q), tag));
	for (size_t k = 0; k < rim.size(); k++) mb.tri(centre, rim[k], rim[(k + 1) % rim.size()], face);
}

void stand(MeshBuilder &mb, const SignLook &K, SignAtlas &atlas, const StreetLook &L, const SignStand &s) {
	const PlateLook &p = K.plate[int(s.type)];
	const Vector3 foot(s.x, s.y + L.h_sidewalk, s.z), face(s.face_x, 0, s.face_z), side(s.face_z, 0, -s.face_x);
	const bool blades = s.type == SignType::STREET; // a street's name stands on top of its post, read from both sides
	mb.cylinder(foot, K.post_radius, blades ? p.z - p.h * 0.5f : p.z + p.h * 0.5f, K.post_sides, K.post, true, TAG_FIXED);
	const Rect2 cell = atlas.cell(s.type, s.text);
	const Vector3 centre = foot + UP * p.z;
	if (blades) {
		plate(mb, p, K, centre + face * K.blade_half, side, face, cell, true);
		plate(mb, p, K, centre - face * K.blade_half, -side, -face, cell, true);
		return;
	}
	plate(mb, p, K, centre + face * K.face_out, side, face, cell, true);
	plate(mb, p, K, centre + face * (K.face_out - K.blade_half), side, -face, cell, false);
}
} // namespace

bool SignLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("signs")) {
		errors.push_back("streets.json: falta signs");
		return false;
	}
	const Dictionary d = t["signs"], plates = d["plates"];
	post_radius = d["post_radius_m"]; post_sides = d["post_sides"]; face_out = d["face_out_m"]; blade_half = d["blade_half_m"];
	post = rgba8_to_color(parse_hex_color(String(d["post_color"]).utf8().get_data()));
	back = rgba8_to_color(parse_hex_color(String(d["back_color"]).utf8().get_data()));
	for (int k = 0; k < SIGN_TYPES; k++) {
		if (!plates.has(PLATE_KEYS[k])) {
			errors.push_back(String("streets.json signs.plates: falta ") + PLATE_KEYS[k]);
			continue;
		}
		const Dictionary p = plates[PLATE_KEYS[k]];
		plate[k] = { float(p["w_m"]), float(p["h_m"]), float(p["z_m"]), float(p["turn"]), int(p["outline_points"]) };
	}
	return errors.is_empty();
}

void place_signs(const CityState &s, const StreetLook &L, const SignLook &K, SignAtlas &atlas, int64_t key, MeshBuilder &mb) {
	std::vector<SignStand> stands;
	const float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[size_t(n)];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key) traffic::junction_signs(s, n, stands);
	});
	s.street.items_in(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int id) {
		const StreetItem &it = s.street.items.items[size_t(id)];
		if (it.kind == ItemKind::SIGN && chunk_of(it.x, it.z) == key) placed_sign(s.roads, it, stands);
	});
	for (const StreetItem &it : s.street.items.items) { // a street's name stands along its whole street: few, and each plate by its own chunk
		if (!it.alive || it.kind != ItemKind::STREETNAME) continue;
		const FootwayHit h = footway_at(s.roads, it.x, it.z, s.catalog.furniture.poles.footway_catch_m);
		if (!h.ok) continue;
		std::vector<SignStand> names;
		name_plates(s.roads, it, street_chain(s.roads, h.seg), names);
		for (const SignStand &n : names)
			if (chunk_of(n.x, n.z) == key) stands.push_back(n);
	}
	for (const SignStand &st : stands) stand(mb, K, atlas, L, st);
}

} // namespace city
