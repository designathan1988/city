// The plates of a chunk, see street_signs.h.
#include "render/props/street_signs.h"

#include "core/color.h"
#include "core/units.h"
#include "render/props/prop_kit.h"
#include "sim/street/street_names.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
const char *const PLATE_KEYS[SIGN_TYPES] = { "stop", "yield", "speed", "noParking", "noEntry", "pedestrian", "school", "direction", "street", "info" };
const Vector3 UP(0, 1, 0);
const Vector2 TAG_FIXED(float(int(PropTag::FIXED)), 0), TAG_PLATE(float(int(PropTag::PLATE)), 0), TAG_POST(float(int(PropTag::SIGN_POST)), 0);

// The outline of a plate in its own plane, filling the unit square -0.5 .. 0.5: w_m and h_m are the plate's real measures
// (across the flats of an octagon, the side of a triangle). project/render/sign_sheet.gd paints the same polygon.
void outline(const PlateLook &p, std::vector<Vector2> &out) {
	if (p.points == 0) {
		out = { Vector2(-0.5f, -0.5f), Vector2(0.5f, -0.5f), Vector2(0.5f, 0.5f), Vector2(-0.5f, 0.5f) };
		return;
	}
	Vector2 low(INFINITY, INFINITY), high(-INFINITY, -INFINITY);
	for (int k = 0; k < p.points; k++) {
		const float a = (p.turn + float(k) / float(p.points)) * TAU_F;
		out.push_back(Vector2(std::cos(a), std::sin(a)));
		low = low.min(out.back());
		high = high.max(out.back());
	}
	const Vector2 across = high - low, middle = (low + high) * 0.5f;
	for (Vector2 &q : out) q = (q - middle) / across;
}

// A plate fanned round its centre at `at`: `side` its width direction, its face looking along `face`.
void plate(MeshBuilder &mb, const PlateLook &p, const SignLook &K, const Vector3 &at, const Vector3 &side, const Vector3 &face, const SignCell &cell, bool painted) {
	std::vector<Vector2> ring;
	outline(p, ring);
	const auto corner = [&](const Vector2 &q) { return at + side * (q.x * p.w) + UP * (q.y * p.h); };
	const auto uv = [&](const Vector2 &q) { return cell.uv.position + Vector2(q.x + 0.5f, 0.5f - q.y) * cell.uv.size; };
	const Color col = painted ? Color(1, 1, 1) : K.back;
	const Vector2 tag = painted ? Vector2(TAG_PLATE.x, float(cell.layer)) : TAG_FIXED; // UV2.y: its layer
	const int centre = mb.add(at, face, col, uv(Vector2()), tag);
	std::vector<int> rim;
	for (const Vector2 &q : ring) rim.push_back(mb.add(corner(q), face, col, uv(q), tag));
	for (size_t k = 0; k < rim.size(); k++) mb.tri(centre, rim[k], rim[(k + 1) % rim.size()], face);
}

void stand(MeshBuilder &mb, const SignLook &K, SignAtlas &atlas, const StreetLook &L, const SignStand &s) {
	const PlateLook &p = K.plate[int(s.type)];
	const Vector3 foot(s.x, s.y + L.h_sidewalk, s.z), face(s.face_x, 0, s.face_z), side(s.face_z, 0, -s.face_x);
	const bool blades = s.type == SignType::STREET; // a street's name stands on top of its post, read from both sides
	mb.cylinder(foot, K.post_radius, blades ? p.z - p.h * 0.5f : p.z + p.h * 0.5f, K.post_sides, K.post, true, TAG_POST);
	const SignCell cell = atlas.cell(s.type, s.text);
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

bool SignLook::load(const DataValue &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const DataValue d = t["signs"], plates = d["plates"];
	sheet_w = d["sheet_w_px"].whole(); sheet_h = d["sheet_h_px"].whole(); cell = d["plate_cell_px"].whole(); blade_w = d["blade_cell_w_px"].whole(); blade_h = d["blade_cell_h_px"].whole();
	post_radius = d["post_radius_m"].f(); post_sides = d["post_sides"].whole(); face_out = d["face_out_m"].f(); blade_half = d["blade_half_m"].f();
	post = rgba8_to_color(d["post_color"].colour());
	back = rgba8_to_color(d["back_color"].colour());
	for (int k = 0; k < SIGN_TYPES; k++) {
		const DataValue p = plates[PLATE_KEYS[k]];
		plate[k] = { p["w_m"].f(), p["h_m"].f(), p["z_m"].f(), p["rotation"].f(), p["outline_points"].whole() };
	}
	return errors.size() == before;
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
	// a street's name stands along its whole street: its plates kept by chunk in StreetWorks::names, in the order they were made
	s.street.names.in_chunk(key, [&](const SignStand &n) { stands.push_back(n); });
	for (const SignStand &st : stands) stand(mb, K, atlas, L, st);
}

} // namespace city
