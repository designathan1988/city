// Tree placement per 256 m chunk (see tree_place.h).
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/random.h"
#include "core/seeded.h"
#include "render/buildings/civic.h"
#include "sim/buildings/building_store.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t WILD_SALT = 0x51ed27u, PARK_SALT = 0x9a2cu, STREET_SALT = 0x57ee7u;
// The woods' hash streams of one cell (Roadcraft's numbers: the same tree in the same spot).
constexpr uint32_t WOOD_KEEP = 41, WOOD_X = 43, WOOD_Z = 44, WOOD_SIZE = 45, WOOD_YAW = 47, WOOD_MODEL = 48, WOOD_KIND = 49;
constexpr int PARK_TRIES = 4; // draws per wanted tree: spots on paths, courts and fountains are skipped
constexpr float GRADE_PROBE = 2.f, LIST_MARGIN = 80.f;
// Hash streams of one candidate: keep, position x and z, height, width, species, yaw, tint.
enum Stream : uint32_t { KEEP = 0, POS_X, POS_Z, HEIGHT, WIDTH, SPECIES, YAW, TINT, STREAMS };

uint32_t cell_hash(int ix, int iz, uint32_t salt) { return hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), salt); }
float h01(uint32_t h, Stream s) { return hash_float(h + s); }

} // namespace

ChunkGround chunk_ground(const CityState &s, int64_t key) {
	ChunkGround l;
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int sid) { l.segs.push_back(sid); });
	s.buildings.query(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int id) { l.lots.push_back(id); });
	if (s.nature.nature.region >= 0 || s.nature.ground.any_cover()) l.eco = compute_ecology(s.terrain, s.nature.ground, s.nature.nature, s.catalog.nature.ecology, x0, z0, CHUNK);
	return l;
}

bool near_road(const CityState &s, const ChunkGround &l, float x, float z, float r) {
	for (int sid : l.segs) {
		double t, d;
		closest_t(s.roads.segs[sid].q, x, z, t, d);
		if (d < s.catalog.profile(s.roads.segs[sid].profile).half() + r) return true;
	}
	return false;
}

bool in_lot(const CityState &s, const ChunkGround &l, float margin, float x, float z) {
	for (int id : l.lots) {
		const Building &b = s.buildings.items[id];
		float dx = x - b.x, dz = z - b.z;
		float lx = dx * std::cos(b.rot) - dz * std::sin(b.rot), lz = dx * std::sin(b.rot) + dz * std::cos(b.rot);
		if (std::fabs(lx) < b.w * 0.5f + margin && std::fabs(lz) < b.d * 0.5f + margin) return true;
	}
	return false;
}

namespace {
float grade_at(const Heightfield &hf, float x, float z, float probe) {
	return std::hypot(hf.height_at(x + probe, z) - hf.height_at(x - probe, z), hf.height_at(x, z + probe) - hf.height_at(x, z - probe)) / (2 * probe);
}

// The woods' density at (x, z): the patches (the map's north is -z), raised on steep ground, by the ecosystem and by painted forest.
float density(const CityState &s, const TreeLook &T, const ChunkGround &l, float x, float z) {
	float d = T.density_of(x, -z);
	d += T.slope_gain * clampf((grade_at(s.terrain, x, z, T.slope_probe_m) - T.slope_from) / (T.slope_to - T.slope_from), 0.f, 1.f);
	if (l.eco.side > 0) d += T.eco_gain * wood_share(l.eco.near(x, z), s.catalog.nature.ecology);
	return d + T.forest_gain * s.nature.ground.cover_at(CoverKind::FOREST, x, z);
}

Color tint_of(const TreeLook &T, float r) {
	float k = 1.f + (r * 2.f - 1.f) * T.tint_vary;
	return Color(k, k, k, 1);
}

bool ground_ok(const CityState &s, const TreeLook &T, float x, float z, float margin, float &y) {
	const Heightfield &hf = s.terrain;
	if (!hf.in_bounds(x, z, margin)) return false;
	y = hf.height_at(x, z);
	return y >= T.min_y && !hf.is_water(x, z, 0.f) && grade_at(hf, x, z, GRADE_PROBE) <= T.max_grade;
}

// The woods' cells over [from, to) on one axis. The land is laid in plates of `plate_m`, each with its lattice from its own corner, and a chunk lies
// in one plate; `hash0` is what a cell's index adds to reach its hash's index.
struct PlateCells {
	int first = 0, last = -1, hash0 = 0;
	float corner = 0;
};

PlateCells plate_cells(const TreeLook &T, float from, float to) {
	PlateCells c;
	float centre = std::round((from + to) * 0.5f / T.plate_m) * T.plate_m;
	c.corner = centre - T.plate_m * 0.5f;
	c.hash0 = int(std::lround(centre / T.grid_m));
	c.first = std::max(0, int(std::ceil((from - c.corner) / T.grid_m - 0.5f)));
	c.last = std::min(int(std::floor(T.plate_m / T.grid_m)) - 1, int(std::ceil((to - c.corner) / T.grid_m - 0.5f)) - 1);
	return c;
}

// The trees of the woods and the open land whose cell's centre lies in the chunk. The lattice runs on the map's own axes (its north is -z).
// `lean`: work out the lean, which no far card draws.
void wild(const CityState &s, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key, bool lean, std::vector<TreeInstance> &out) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	PlateCells px = plate_cells(T, x0, x0 + CHUNK), py = plate_cells(T, -(z0 + CHUNK), -z0);
	for (int j = py.first; j <= py.last; j++)
		for (int i = px.first; i <= px.last; i++) {
			int gi = i + px.hash0, gj = j + py.hash0;
			float roll = float(woods_hash(gi, gj, WOOD_KEEP));
			if (roll >= T.most) continue; // above the highest chance: its density is never read
			float d = density(s, T, l, px.corner + (i + 0.5f) * T.grid_m, -(py.corner + (j + 0.5f) * T.grid_m));
			WoodBand b = T.band_at(d);
			float keep = T.keep_at(d, b);
			if (roll >= keep) continue;
			float x = px.corner + (i + 0.5f + (float(woods_hash(gi, gj, WOOD_X)) - 0.5f) * T.jitter) * T.grid_m;
			float z = -(py.corner + (j + 0.5f + (float(woods_hash(gi, gj, WOOD_Z)) - 0.5f) * T.jitter) * T.grid_m);
			float y;
			if (!ground_ok(s, T, x, z, T.grid_m, y) || s.nature.planted.cleared(x, z)) continue;
			const BandLook &B = T.band[size_t(b)];
			int sp = B.mix.pick(float(woods_hash(gi, gj, WOOD_KIND)));
			uint32_t h = cell_hash(gi, gj, WILD_SALT);
			if (l.eco.side > 0 && h01(h, TINT) < l.eco.near(x, z).palm) sp = T.planted_species.back();
			float crowd = std::min(1.f, keep * T.crowd_per_keep), size = B.h0 + (B.h1 - B.h0) * float(woods_hash(gi, gj, WOOD_SIZE));
			float height = size * (T.open_tall + T.crowd_tall * crowd);
			float width = size * (T.open_wide - T.crowd_narrow * crowd) * (1.f + (float(woods_hash(gi, gj, WOOD_MODEL)) * 2.f - 1.f) * T.width_vary);
			if (near_road(s, l, x, z, kit.species(sp).crown_reach * width + T.road_margin) || in_lot(s, l, T.lot_margin, x, z)) continue;
			TreeInstance t{ Vector3(x, y, z), height, width, TAU_F * float(woods_hash(gi, gj, WOOD_YAW)), 0, 0, sp, tint_of(T, h01(h, TINT)) };
			float k = T.edge_lean * (1.f - crowd) * std::min(1.f, crowd * T.lean_rise);
			if (lean && k > 0) { // to the open side: down the density's slope
				float gx = density(s, T, l, x + T.grid_m, z) - density(s, T, l, x - T.grid_m, z), gz = density(s, T, l, x, z + T.grid_m) - density(s, T, l, x, z - T.grid_m);
				float g = std::sqrt(gx * gx + gz * gz);
				if (g > 0) { t.lean_x = -gx / g * k; t.lean_z = -gz / g * k; }
			}
			out.push_back(t);
		}
}

void parks(const CityState &s, const TreeLook &T, const ChunkGround &l, int64_t key, std::vector<TreeInstance> &out) {
	for (int id : l.lots) {
		const Building &b = s.buildings.items[size_t(id)];
		if (!b40::is_open_lot(b.fn) || chunk_of(b.x, b.z) != key) continue;
		int n = std::min(T.park_max, int(b.w * b.d / T.park_m2_per_tree));
		Vector3 ax(std::cos(b.rot), 0, -std::sin(b.rot)), az(std::sin(b.rot), 0, std::cos(b.rot));
		for (int k = 0, placed = 0; k < n * PARK_TRIES && placed < n; k++) {
			uint32_t h = hash_combine(b.seed, uint32_t(k) + PARK_SALT);
			float lx = (h01(h, POS_X) - 0.5f) * std::max(0.f, b.w - 2 * T.park_edge), lz = (h01(h, POS_Z) - 0.5f) * std::max(0.f, b.d - 2 * T.park_edge);
			if (!b40::open_lot_tree_ok(b.fn, b.w, b.d, lx, b.d / 2 - lz)) continue; // lot frame y runs from the street (+z side)
			Vector3 p = Vector3(b.x, 0, b.z) + ax * lx + az * lz;
			p.y = s.terrain.height_at(p.x, p.z);
			float height = T.park_h0 + (T.park_h1 - T.park_h0) * h01(h, HEIGHT);
			out.push_back({ p, height, height * (1.f + (h01(h, WIDTH) * 2.f - 1.f) * T.width_vary), TAU_F * h01(h, YAW), 0, 0,
					T.park.pick(h01(h, SPECIES)), tint_of(T, h01(h, TINT)) });
			placed++;
		}
	}
}

void street(const CityState &s, const StreetLook &L, const TreeLook &T, int64_t key, std::vector<TreeInstance> &out) {
	std::vector<TreeSpot> spots;
	place_street_trees(s, L, key, spots);
	for (const TreeSpot &t : spots) {
		uint32_t h = hash_combine(hash_combine(uint32_t(int(t.p.x)), uint32_t(int(t.p.z))), STREET_SALT);
		float height = t.scale * T.street_height_m;
		out.push_back({ t.p, height, height, t.yaw, 0, 0, T.street.pick(h01(h, SPECIES)), tint_of(T, h01(h, TINT)) });
	}
}

FarCard far_of(const TreeInstance &t) { return { t.p, t.height, t.width, t.tint, t.species }; }

// The trees the player planted in the chunk, by the model each one's seed grows as.
void planted(const CityState &s, const TreeLook &T, int64_t key, std::vector<TreeInstance> &out) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	const PlantedTrees &p = s.nature.planted;
	const TreeBrushDef &d = s.catalog.nature.trees;
	p.for_trees(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int id) {
		const PlantedTree &t = p.trees[size_t(id)];
		if (!s.terrain.in_bounds(t.x, t.z) || s.terrain.is_water(t.x, t.z, 0.f)) return;
		Vector3 at(t.x, s.terrain.height_at(t.x, t.z), t.z);
		out.push_back({ at, t.height * T.open_tall, t.height * T.open_wide, t.yaw, 0, 0, T.planted_species[size_t(tree_slot(t.seed, d))], tint_of(T, t.seed) });
	});
}
} // namespace

void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<TreeInstance> &out) {
	ChunkGround l = chunk_ground(s, key);
	street(s, L, T, key, out);
	parks(s, T, l, key, out);
	planted(s, T, key, out);
	wild(s, T, kit, l, key, true, out);
}

void place_far(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<FarCard> &out) {
	ChunkGround l = chunk_ground(s, key);
	std::vector<TreeInstance> trees;
	street(s, L, T, key, trees);
	parks(s, T, l, key, trees);
	planted(s, T, key, trees);
	wild(s, T, kit, l, key, false, trees);
	for (const TreeInstance &t : trees) out.push_back(far_of(t));
}

} // namespace city
