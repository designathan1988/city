// Tree placement per 256 m chunk (see tree_place.h).
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/random.h"
#include "core/seeded.h"
#include "sim/buildings/b40/civic.h"
#include "render/trees/garden_place.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/building_store.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t WILD_SALT = 0x51ed27u, PARK_SALT = 0x9a2cu, STREET_SALT = 0x57ee7u; // distinct salts: woods, parks and streets draw apart
// The woods' hash streams of one cell (Roadcraft's numbers: the same tree in the same spot).
constexpr uint32_t WOOD_KEEP = 41, WOOD_X = 43, WOOD_Z = 44, WOOD_SIZE = 45, WOOD_YAW = 47, WOOD_MODEL = 48, WOOD_KIND = 49;
constexpr float NEIGHBOUR_LEAST_M = 1e-3f; // nearer than this two trees give no direction
constexpr int PARK_TRIES = 4; // draws per wanted tree: spots on paths, courts and fountains are skipped
constexpr float GRADE_PROBE = 2.f, LIST_MARGIN = 80.f;
// Hash streams of one candidate: keep, position x and z, height, width, species, yaw, tint.
enum Stream : uint32_t { KEEP = 0, POS_X, POS_Z, HEIGHT, WIDTH, SPECIES, YAW, TINT, STREAMS };

uint32_t cell_hash(int ix, int iz, uint32_t salt) { return hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), salt); }
float h01(uint32_t h, Stream s) { return hash_float(h + s); }

} // namespace

namespace {
constexpr double GRID_CELL_M = 32.0; // of the grid filing a chunk's roads and lots
// Room added to every filed box and every query square: the float sums of the exact tests (lot axes turned by sin and cos) never
// fall outside it, so the grid hands each test a superset of the roads and lots that can pass it.
constexpr double GRID_SLACK_M = 1.0;
constexpr double ROOT_TWO = 1.4142135623730951;

int grid_index(double v, double origin, int n) {
	const double i = std::floor((v - origin) / GRID_CELL_M);
	return int(std::clamp(i, 0.0, double(n - 1)));
}
} // namespace

void ChunkGround::Grid::file(int index, double minx, double minz, double maxx, double maxz) {
	const int i0 = grid_index(minx - GRID_SLACK_M, x0, n), i1 = grid_index(maxx + GRID_SLACK_M, x0, n);
	const int j0 = grid_index(minz - GRID_SLACK_M, z0, n), j1 = grid_index(maxz + GRID_SLACK_M, z0, n);
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) cells[size_t(j) * size_t(n) + size_t(i)].push_back(index);
	count = std::max(count, index + 1);
}

namespace {
// The indices a query has tested already (each is filed in every cell its reach touches): the thread's own, so a ground several
// jobs share is only read. A query is a stamp; an index tested under it carries it.
struct Asked {
	std::vector<uint32_t> seen;
	uint32_t stamp = 0;
};
thread_local Asked asked;
} // namespace

template <typename F>
bool ChunkGround::Grid::any(double x, double z, double reach, F &&test) const {
	if (n == 0) return false;
	const double r = std::max(0.0, reach) + GRID_SLACK_M;
	const int i0 = grid_index(x - r, x0, n), i1 = grid_index(x + r, x0, n), j0 = grid_index(z - r, z0, n), j1 = grid_index(z + r, z0, n);
	if (asked.seen.size() < size_t(count)) asked.seen.resize(size_t(count), 0);
	if (++asked.stamp == 0) {
		std::fill(asked.seen.begin(), asked.seen.end(), 0);
		asked.stamp = 1;
	}
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++)
			for (int index : cells[size_t(j) * size_t(n) + size_t(i)]) {
				if (asked.seen[size_t(index)] == asked.stamp) continue;
				asked.seen[size_t(index)] = asked.stamp;
				if (test(index)) return true;
			}
	return false;
}

size_t ChunkGround::bytes() const {
	size_t b = sizeof(ChunkGround) + (segs.capacity() + lots.capacity()) * sizeof(int) + eco.at.capacity() * sizeof(Community);
	for (const Grid *g : { &seg_grid, &lot_grid }) {
		b += g->cells.capacity() * sizeof(std::vector<int>);
		for (const std::vector<int> &c : g->cells) b += c.capacity() * sizeof(int);
	}
	return b;
}

float ground_reach(const Catalog &catalog) {
	const EcologyDef &d = catalog.nature.ecology;
	return std::max(LIST_MARGIN, float(d.mean_cells + 2) * d.grid_m); // compute_ecology: mean_cells + 1 corners, and one for its sample
}

float tree_reach(const TreeLook &T) {
	// wild(): the cells crowd_reach_m round the chunk, a cell's jitter, then the grade's probes round a tree
	return T.crowd_reach_m + 2 * T.grid_m + std::max(T.slope_probe_m, GRADE_PROBE);
}

ChunkGround chunk_ground(const CityView &s, int64_t key) {
	ChunkGround l;
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int sid) { l.segs.push_back(sid); });
	s.buildings.query(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int id) { l.lots.push_back(id); });
	if (s.nature.nature.region >= 0 || s.nature.ground.any_cover()) l.eco = compute_ecology(s.terrain, s.nature.ground, s.nature.nature, s.catalog.nature.ecology, x0, z0, CHUNK);
	const int n = int(std::ceil((double(CHUNK) + 2.0 * double(LIST_MARGIN)) / GRID_CELL_M));
	for (ChunkGround::Grid *g : { &l.seg_grid, &l.lot_grid }) {
		g->x0 = double(x0) - double(LIST_MARGIN);
		g->z0 = double(z0) - double(LIST_MARGIN);
		g->n = n;
		g->cells.assign(size_t(n) * size_t(n), {});
	}
	for (size_t i = 0; i < l.segs.size(); i++) { // the curve stands inside its control points' hull; a road reaches its half width past it
		const RoadSegment &g = s.roads.segs[size_t(l.segs[i])];
		const double half = double(s.catalog.profile(g.profile).half());
		l.seg_grid.file(int(i), std::min({ g.q.x0, g.q.cx, g.q.x1 }) - half, std::min({ g.q.z0, g.q.cz, g.q.z1 }) - half,
				std::max({ g.q.x0, g.q.cx, g.q.x1 }) + half, std::max({ g.q.z0, g.q.cz, g.q.z1 }) + half);
	}
	for (size_t i = 0; i < l.lots.size(); i++) { // a lot's corners stand within its half diagonal of its centre
		const Building &b = s.buildings.items[size_t(l.lots[i])];
		const double r = std::hypot(double(b.w) * 0.5, double(b.d) * 0.5);
		l.lot_grid.file(int(i), double(b.x) - r, double(b.z) - r, double(b.x) + r, double(b.z) + r);
	}
	return l;
}

// A point within half + r of a curve lies within that of the curve's hull box: the square of side r round it meets the filed box.
bool near_road(const CityView &s, const ChunkGround &l, float x, float z, float r) {
	return l.seg_grid.any(double(x), double(z), double(r), [&](int i) {
		const int sid = l.segs[size_t(i)];
		double t, d;
		closest_t(s.roads.segs[sid].q, x, z, t, d);
		return d < s.catalog.profile(s.roads.segs[sid].profile).half() + r;
	});
}

// A point inside a lot grown by `margin` lies within its half diagonal plus margin x sqrt 2 of its centre.
bool in_lot(const CityView &s, const ChunkGround &l, float margin, float x, float z) {
	return l.lot_grid.any(double(x), double(z), double(std::max(0.f, margin)) * ROOT_TWO, [&](int i) {
		const Building &b = s.buildings.items[l.lots[size_t(i)]];
		float dx = x - b.x, dz = z - b.z;
		float lx = dx * std::cos(b.rot) - dz * std::sin(b.rot), lz = dx * std::sin(b.rot) + dz * std::cos(b.rot);
		return std::fabs(lx) < b.w * 0.5f + margin && std::fabs(lz) < b.d * 0.5f + margin;
	});
}

namespace {
float grade_at(const GroundRead &hf, float x, float z, float probe) {
	return std::hypot(hf.height_at(x + probe, z) - hf.height_at(x - probe, z), hf.height_at(x, z + probe) - hf.height_at(x, z - probe)) / (2 * probe);
}

// The woods' density at (x, z): the patches (the map's north is -z), raised on steep ground, by the ecosystem and by painted forest.
float density(const CityView &s, const TreeLook &T, const ChunkGround &l, float x, float z) {
	float d = T.density_of(x, -z);
	d += T.slope_gain * clampf((grade_at(s.terrain, x, z, T.slope_probe_m) - T.slope_from) / (T.slope_to - T.slope_from), 0.f, 1.f);
	if (l.eco.side > 0) d += T.eco_gain * wood_share(l.eco.near(x, z), s.catalog.nature.ecology);
	return d + T.forest_gain * s.nature.ground.cover_at(CoverKind::FOREST, x, z);
}

Color tint_of(const TreeLook &T, float r) {
	float k = 1.f + (r * 2.f - 1.f) * T.tint_vary;
	return Color(k, k, k, 1);
}

bool ground_ok(const CityView &s, const TreeLook &T, float x, float z, float margin, float &y) {
	const GroundRead &hf = s.terrain;
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

// A tree of the woods before its neighbours are counted.
struct WildTree {
	float x = 0, y = 0, z = 0, size = 0, stretch = 0, yaw = 0;
	int species = 0;
	Color tint;
	bool own = false; // its cell's centre lies in the chunk: the rest are its neighbours only
};

// The cell's tree, if it holds one.
// `skip`: a clearing left out (the trees that stood before a blow cut it), -1 none.
bool wild_tree(const CityView &s, const TreeLook &T, const ChunkGround &l, const PlateCells &px, const PlateCells &py, int i, int j, WildTree &w, int skip) {
	int gi = i + px.hash0, gj = j + py.hash0;
	float roll = float(woods_hash(gi, gj, WOOD_KEEP));
	if (roll >= T.most) return false; // above the highest chance: its density is never read
	float d = density(s, T, l, px.corner + (i + 0.5f) * T.grid_m, -(py.corner + (j + 0.5f) * T.grid_m));
	WoodBand b = T.band_at(d);
	if (roll >= T.keep_at(d, b) * T.share.load(std::memory_order_relaxed)) return false;
	w.x = px.corner + (i + 0.5f + (float(woods_hash(gi, gj, WOOD_X)) - 0.5f) * T.jitter) * T.grid_m;
	w.z = -(py.corner + (j + 0.5f + (float(woods_hash(gi, gj, WOOD_Z)) - 0.5f) * T.jitter) * T.grid_m);
	if (!ground_ok(s, T, w.x, w.z, T.grid_m, w.y) || s.nature.planted.cleared(w.x, w.z, skip)) return false;
	const BandLook &B = T.band[size_t(b)];
	w.species = B.mix.pick(float(woods_hash(gi, gj, WOOD_KIND)));
	float r = h01(cell_hash(gi, gj, WILD_SALT), TINT);
	if (l.eco.side > 0 && r < l.eco.near(w.x, w.z).palm) w.species = T.planted_species.back();
	w.size = B.h0 + (B.h1 - B.h0) * float(woods_hash(gi, gj, WOOD_SIZE));
	w.stretch = 1.f + (float(woods_hash(gi, gj, WOOD_MODEL)) * 2.f - 1.f) * T.width_vary;
	w.yaw = TAU_F * float(woods_hash(gi, gj, WOOD_YAW));
	w.tint = tint_of(T, r);
	return true;
}

// The trees of the woods and the open land whose cell's centre lies in the chunk. The lattice runs on the map's own axes (its north is -z). A tree's
// crowd is its neighbours, each weighing less with distance, out to crowd_reach_m: deep in a wood it stands taller and narrower, and at a wood's edge
// it leans away from them. They are read within the plate.
void wild(const CityView &s, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key, std::vector<TreeInstance> &out, int skip = -1) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	PlateCells px = plate_cells(T, x0, x0 + CHUNK), py = plate_cells(T, -(z0 + CHUNK), -z0);
	int margin = int(std::ceil(T.crowd_reach_m / T.grid_m)), cells = int(std::floor(T.plate_m / T.grid_m));
	std::vector<WildTree> trees;
	for (int j = std::max(0, py.first - margin); j <= std::min(cells - 1, py.last + margin); j++)
		for (int i = std::max(0, px.first - margin); i <= std::min(cells - 1, px.last + margin); i++) {
			WildTree w;
			if (!wild_tree(s, T, l, px, py, i, j, w, skip)) continue;
			w.own = i >= px.first && i <= px.last && j >= py.first && j <= py.last;
			trees.push_back(w);
		}
	std::sort(trees.begin(), trees.end(), [](const WildTree &a, const WildTree &b) { return a.x < b.x; });
	size_t from = 0;
	for (size_t k = 0; k < trees.size(); k++) {
		const WildTree &w = trees[k];
		while (trees[from].x < w.x - T.crowd_reach_m) from++;
		if (!w.own) continue;
		float near = 0, ax = 0, az = 0;
		for (size_t q = from; q < trees.size() && trees[q].x <= w.x + T.crowd_reach_m; q++) {
			float ex = trees[q].x - w.x, ez = trees[q].z - w.z, d = std::sqrt(ex * ex + ez * ez);
			if (q == k || d > T.crowd_reach_m || d < NEIGHBOUR_LEAST_M) continue;
			float weight = 1.f - d / T.crowd_reach_m;
			near += weight;
			ax += ex / d * weight;
			az += ez / d * weight;
		}
		float crowd = std::min(1.f, near / T.crowd_full);
		float height = w.size * (T.open_tall + T.crowd_tall * crowd), width = w.size * (T.open_wide - T.crowd_narrow * crowd) * w.stretch;
		if (near_road(s, l, w.x, w.z, kit.species(w.species).crown_reach * width + T.road_margin) || in_lot(s, l, T.lot_margin, w.x, w.z)) continue;
		TreeInstance t{ Vector3(w.x, w.y, w.z), height, width, w.yaw, 0, 0, w.species, w.tint };
		float a = std::sqrt(ax * ax + az * az), lean = T.edge_lean * (1.f - crowd) * std::min(1.f, crowd * T.lean_rise);
		if (a > NEIGHBOUR_LEAST_M) { t.lean_x = -ax / a * lean; t.lean_z = -az / a * lean; }
		out.push_back(t);
	}
}

// The lots' plants: each lot's garden, and the grove of a park or a square that has not been edited.
void parks(const CityView &s, const TreeLook &T, const ChunkGround &l, int64_t key, GardenView view, std::vector<TreeInstance> &out) {
	for (int id : l.lots) {
		const Building &b = s.buildings.items[size_t(id)];
		if (chunk_of(b.x, b.z) != key) continue;
		place_garden(s, T, b, view, out);
		if (b.body != 0 || !b40::is_open_lot(b.fn)) continue;
		int n = std::min(T.park_max, int(b.w * b.d / T.park_m2_per_tree));
		Vector3 ax(std::cos(b.rot), 0, -std::sin(b.rot)), az(std::sin(b.rot), 0, std::cos(b.rot));
		for (int k = 0, placed = 0; k < n * PARK_TRIES && placed < n; k++) {
			uint32_t h = hash_combine(b.seed, uint32_t(k) + PARK_SALT);
			float lx = (h01(h, POS_X) - 0.5f) * std::max(0.f, b.w - 2 * T.park_edge), lz = (h01(h, POS_Z) - 0.5f) * std::max(0.f, b.d - 2 * T.park_edge);
			if (!b40::open_lot_tree_ok(s.designs, b.fn, b.w, b.d, lx, b.d / 2 - lz)) continue; // lot frame y runs from the street (+z side)
			Vector3 p = Vector3(b.x, 0, b.z) + ax * lx + az * lz;
			p.y = s.terrain.height_at(p.x, p.z);
			float height = T.park_h0 + (T.park_h1 - T.park_h0) * h01(h, HEIGHT);
			out.push_back({ p, height, height * (1.f + (h01(h, WIDTH) * 2.f - 1.f) * T.width_vary), TAU_F * h01(h, YAW), 0, 0,
					T.park.pick(h01(h, SPECIES)), tint_of(T, h01(h, TINT)) });
			placed++;
		}
	}
}

void street(const CityView &s, const StreetLook &L, const TreeLook &T, int64_t key, std::vector<TreeInstance> &out) {
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
void planted(const CityView &s, const TreeLook &T, int64_t key, std::vector<TreeInstance> &out) {
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

void place_trees(const CityView &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key,
		std::vector<TreeInstance> &out) {
	street(s, L, T, key, out);
	parks(s, T, l, key, GardenView::NEAR, out);
	planted(s, T, key, out);
	wild(s, T, kit, l, key, out);
}

void place_far(const CityView &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, const ChunkGround &l, int64_t key,
		std::vector<FarCard> &out) {
	std::vector<TreeInstance> trees;
	street(s, L, T, key, trees);
	parks(s, T, l, key, GardenView::FAR, trees);
	planted(s, T, key, trees);
	wild(s, T, kit, l, key, trees);
	for (const TreeInstance &t : trees) out.push_back(far_of(t));
}

void felled_wild(const CityView &s, const TreeLook &T, const TreeKit &kit, float x, float z, float radius, int clearing, std::vector<TreeInstance> &out) {
	std::vector<TreeInstance> chunk;
	for_chunks(x - radius, z - radius, x + radius, z + radius, [&](int64_t key) {
		chunk.clear();
		wild(s, T, kit, chunk_ground(s, key), key, chunk, clearing);
		for (const TreeInstance &t : chunk)
			if (std::hypot(t.p.x - x, t.p.z - z) < radius && !s.nature.planted.cleared(t.p.x, t.p.z, clearing) && s.nature.planted.cleared(t.p.x, t.p.z)) out.push_back(t);
	});
}

} // namespace city
