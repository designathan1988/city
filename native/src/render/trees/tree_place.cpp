// Tree placement per 256 m chunk (see tree_place.h).
#include "render/trees/tree_place.h"

#include "core/color.h"
#include "core/random.h"
#include "render/buildings/civic.h"
#include "sim/buildings/building_store.h"

#include <godot_cpp/variant/array.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t WILD_SALT = 0x51ed27u, PARK_SALT = 0x9a2cu, NOISE_SEED = 4242u;
constexpr int PARK_TRIES = 4; // draws per wanted tree: spots on paths, courts and fountains are skipped
constexpr int NOISE_OCTAVES = 2;
constexpr float GRADE_PROBE = 2.f, LIST_MARGIN = 80.f;
constexpr uint32_t YAW_HASH_SALT = 5;

uint32_t cell_hash(int ix, int iz, uint32_t salt) { return hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), salt); }

// The segments and lots that can affect this chunk (its bounds + LIST_MARGIN), queried once per chunk instead of once
// per candidate tree.
void chunk_segments(const CityState &s, int64_t key, std::vector<int> &out) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN,
			[&](int sid) { out.push_back(sid); });
}

void chunk_lots(const CityState &s, int64_t key, std::vector<int> &out) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.buildings.query(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN,
			[&](int id) { out.push_back(id); });
}

bool near_road(const CityState &s, const std::vector<int> &segs, float x, float z, float r) {
	for (int sid : segs) {
		double t, d;
		closest_t(s.roads.segs[sid].q, x, z, t, d);
		if (d < s.catalog.profile(s.roads.segs[sid].profile).half() + r) return true;
	}
	return false;
}

bool in_lot(const CityState &s, const std::vector<int> &lots, const TreeLook &T, float x, float z) {
	for (int id : lots) {
		const Building &b = s.buildings.items[id];
		float dx = x - b.x, dz = z - b.z;
		float lx = dx * std::cos(b.rot) - dz * std::sin(b.rot), lz = dx * std::sin(b.rot) + dz * std::cos(b.rot);
		if (std::fabs(lx) < b.w * 0.5f + T.lot_margin && std::fabs(lz) < b.d * 0.5f + T.lot_margin) return true;
	}
	return false;
}

int pick_species(const TreeLook &T, float r) {
	if (r < T.shrub_share) return T.shrub;
	return r < T.shrub_share + T.conifer_share ? T.conifer : T.broadleaf;
}

void wild(const CityState &s, const TreeLook &T, const TreeKit &kit, const std::vector<int> &segs, const std::vector<int> &lots,
		int64_t key, std::vector<TreeSpot> &out) {
	const Heightfield &hf = s.terrain;
	int n = int(CHUNK / T.grid_m), cx = key_x(key), cz = key_y(key);
	for (int j = 0; j < n; j++)
		for (int i = 0; i < n; i++) {
			int ix = cx * n + i, iz = cz * n + j;
			uint32_t h = cell_hash(ix, iz, WILD_SALT ^ s.seed);
			float x = (ix + 0.5f) * T.grid_m + (hash_float(h) - 0.5f) * 2 * T.jitter_m;
			float z = (iz + 0.5f) * T.grid_m + (hash_float(h + 1) - 0.5f) * 2 * T.jitter_m;
			float keep = (T.keep_base + T.keep_noise * std::max(0.f, fbm(x / T.noise_m, z / T.noise_m, NOISE_SEED ^ s.seed, NOISE_OCTAVES) + T.noise_bias)) * T.keep_scale;
			if (hash_float(h + 2) > keep || !hf.in_bounds(x, z, T.grid_m)) continue;
			float y = hf.height_at(x, z);
			if (y < T.min_y || hf.is_water(x, z, 0.f)) continue;
			float grade = (std::fabs(hf.height_at(x + GRADE_PROBE, z) - hf.height_at(x - GRADE_PROBE, z)) +
								  std::fabs(hf.height_at(x, z + GRADE_PROBE) - hf.height_at(x, z - GRADE_PROBE))) /
					(4 * GRADE_PROBE);
			if (grade > T.max_grade) continue;
			int sp = pick_species(T, hash_float(h + 3));
			float scale = T.scale_min + T.scale_span * hash_float(h + 4);
			if (near_road(s, segs, x, z, kit.species(sp).crown_r * scale + T.road_margin) || in_lot(s, lots, T, x, z)) continue;
			out.push_back({ Vector3(x, y, z), scale, TAU_F * hash_float(h + YAW_HASH_SALT), sp });
		}
}

void parks(const CityState &s, const TreeLook &T, const std::vector<int> &lots, int64_t key, std::vector<TreeSpot> &out) {
	for (int id : lots) {
		const Building &b = s.buildings.items[size_t(id)];
		if (!b40::is_open_lot(b.fn) || chunk_of(b.x, b.z) != key) continue;
		int n = std::min(T.park_max, int(b.w * b.d / T.park_m2_per_tree));
		Vector3 ax(std::cos(b.rot), 0, -std::sin(b.rot)), az(std::sin(b.rot), 0, std::cos(b.rot));
		for (int k = 0, placed = 0; k < n * PARK_TRIES && placed < n; k++) {
			uint32_t h = hash_combine(b.seed, uint32_t(k) + PARK_SALT);
			float lx = (hash_float(h) - 0.5f) * std::max(0.f, b.w - 2 * T.park_edge), lz = (hash_float(h + 1) - 0.5f) * std::max(0.f, b.d - 2 * T.park_edge);
			if (!b40::open_lot_tree_ok(b.fn, b.w, b.d, lx, b.d / 2 - lz)) continue; // lot frame y runs from the street (+z side)
			Vector3 p = Vector3(b.x, 0, b.z) + ax * lx + az * lz;
			p.y = s.terrain.height_at(p.x, p.z);
			out.push_back({ p, T.scale_min + T.scale_span * hash_float(h + 2), TAU_F * hash_float(h + 3), T.broadleaf });
			placed++;
		}
	}
}
} // namespace

bool TreeLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("placement")) {
		errors.push_back("streets.json trees: falta placement");
		return false;
	}
	Dictionary p = t["placement"];
	view_m = p["view_m"]; near_m = p["near_m"]; grid_m = p["grid_m"]; jitter_m = p["jitter_m"];
	keep_base = p["keep_base"]; keep_noise = p["keep_noise"]; keep_scale = p["keep_scale"]; noise_m = p["noise_m"]; noise_bias = p["noise_bias"];
	min_y = p["min_y_m"]; max_grade = p["max_grade"]; road_margin = p["road_margin_m"]; lot_margin = p["lot_margin_m"]; lot_reach = p["lot_reach_m"];
	scale_min = p["scale_min"]; scale_span = p["scale_span"]; park_m2_per_tree = p["park_m2_per_tree"]; park_edge = p["park_edge_m"];
	park_max = p["park_max"]; build_per_frame = p["build_per_frame"];
	broadleaf = p["broadleaf"]; conifer = p["conifer"]; shrub = p["shrub"]; shrub_share = p["shrub_share"]; conifer_share = p["conifer_share"];
	Array tl = p["tints"];
	for (int i = 0; i < tl.size(); i++) tints.push_back(rgba8_to_color(parse_hex_color(String(tl[i]).utf8().get_data())));
	if (tints.empty()) tints.push_back(Color(1, 1, 1));
	return true;
}

void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<TreeSpot> &out) {
	std::vector<int> segs, lots;
	chunk_segments(s, key, segs);
	chunk_lots(s, key, lots);
	place_street_trees(s, L, key, out);
	for (TreeSpot &t : out) t.species = T.broadleaf;
	parks(s, T, lots, key, out);
	wild(s, T, kit, segs, lots, key, out);
}

} // namespace city
