// Tree placement per 256 m chunk (see tree_place.h).
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/random.h"
#include "render/buildings/civic.h"
#include "sim/buildings/building_store.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t WILD_SALT = 0x51ed27u, PARK_SALT = 0x9a2cu, STREET_SALT = 0x57ee7u, FAR_SALT = 0xfa12u, FIELD_SEED = 4242u;
constexpr int PARK_TRIES = 4; // draws per wanted tree: spots on paths, courts and fountains are skipped
constexpr float GRADE_PROBE = 2.f, LIST_MARGIN = 80.f;
constexpr float THRESHOLD_GAP = 1e-4f; // the bands' thresholds never meet
constexpr int WATER_DIRS = 8; // directions the water is looked for, at half and at the whole of `water_m`
// Hash streams of one candidate: keep, position x and z, height, width, species, yaw, tint.
enum Stream : uint32_t { KEEP = 0, POS_X, POS_Z, HEIGHT, WIDTH, SPECIES, YAW, TINT, STREAMS };

float smoothstep(float a, float b, float x) {
	float t = clampf((x - a) / std::max(b - a, THRESHOLD_GAP), 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}

uint32_t cell_hash(int ix, int iz, uint32_t salt) { return hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), salt); }
float h01(uint32_t h, Stream s) { return hash_float(h + s); }

struct Lists {
	std::vector<int> segs, lots;
};

Lists lists(const CityState &s, int64_t key) {
	Lists l;
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int sid) { l.segs.push_back(sid); });
	s.buildings.query(x0 - LIST_MARGIN, z0 - LIST_MARGIN, x0 + CHUNK + LIST_MARGIN, z0 + CHUNK + LIST_MARGIN, [&](int id) { l.lots.push_back(id); });
	return l;
}

bool near_road(const CityState &s, const Lists &l, float x, float z, float r) {
	for (int sid : l.segs) {
		double t, d;
		closest_t(s.roads.segs[sid].q, x, z, t, d);
		if (d < s.catalog.profile(s.roads.segs[sid].profile).half() + r) return true;
	}
	return false;
}

bool in_lot(const CityState &s, const Lists &l, float margin, float x, float z) {
	for (int id : l.lots) {
		const Building &b = s.buildings.items[id];
		float dx = x - b.x, dz = z - b.z;
		float lx = dx * std::cos(b.rot) - dz * std::sin(b.rot), lz = dx * std::sin(b.rot) + dz * std::cos(b.rot);
		if (std::fabs(lx) < b.w * 0.5f + margin && std::fabs(lz) < b.d * 0.5f + margin) return true;
	}
	return false;
}

float grade_at(const Heightfield &hf, float x, float z) {
	return std::hypot(hf.height_at(x + GRADE_PROBE, z) - hf.height_at(x - GRADE_PROBE, z), hf.height_at(x, z + GRADE_PROBE) - hf.height_at(x, z - GRADE_PROBE)) / (2 * GRADE_PROBE);
}

// The woods field at (x, z): the patches, raised on steep ground and along water.
float field(const CityState &s, const TreeLook &T, float x, float z) {
	float f = fbm(x / T.mass_m, z / T.mass_m, FIELD_SEED ^ s.seed, T.octaves);
	f += T.slope_gain * smoothstep(T.slope_from, T.slope_to, grade_at(s.terrain, x, z));
	if (T.water_gain > 0)
		for (int k = 0; k < WATER_DIRS; k++) {
			float a = TAU_F * k / WATER_DIRS, c = std::cos(a), sn = std::sin(a);
			if (s.terrain.is_water(x + c * T.water_m * 0.5f, z + sn * T.water_m * 0.5f, 0.f) || s.terrain.is_water(x + c * T.water_m, z + sn * T.water_m, 0.f)) {
				f += T.water_gain;
				break;
			}
		}
	return f;
}

WoodBand band_of(const TreeLook &T, float f) { return f > T.t_inner ? WoodBand::INNER : f > T.t_edge ? WoodBand::EDGE : WoodBand::OPEN; }

// How deep in a wood the field is: 0 at its outer edge, 1 from the inner threshold on.
float depth_of(const TreeLook &T, float f) { return clampf((f - T.t_edge) / std::max(T.t_inner - T.t_edge, THRESHOLD_GAP), 0.f, 1.f); }

Color tint_of(const TreeLook &T, float r) {
	float k = 1.f + (r * 2.f - 1.f) * T.tint_vary;
	return Color(k, k, k, 1);
}

bool ground_ok(const CityState &s, const TreeLook &T, float x, float z, float margin, float &y) {
	const Heightfield &hf = s.terrain;
	if (!hf.in_bounds(x, z, margin)) return false;
	y = hf.height_at(x, z);
	return y >= T.min_y && !hf.is_water(x, z, 0.f) && grade_at(hf, x, z) <= T.max_grade;
}

// The woods' and the open land's trees of the chunk (only the open band's when `open_only`).
void wild(const CityState &s, const TreeLook &T, const TreeKit &kit, const Lists &l, int64_t key, bool open_only, std::vector<TreeInstance> &out) {
	int n = int(CHUNK / T.grid_m), cx = key_x(key), cz = key_y(key);
	// The highest chance any band gives: a candidate drawn above it stands in no band, so its field is never read.
	float most = T.band[size_t(WoodBand::OPEN)].keep;
	if (!open_only)
		for (const BandLook &B : T.band) most = std::max(most, B.keep);
	for (int j = 0; j < n; j++)
		for (int i = 0; i < n; i++) {
			int ix = cx * n + i, iz = cz * n + j;
			uint32_t h = cell_hash(ix, iz, WILD_SALT ^ s.seed);
			if (h01(h, KEEP) >= most) continue;
			float x = (ix + 0.5f + (h01(h, POS_X) - 0.5f) * T.jitter) * T.grid_m, z = (iz + 0.5f + (h01(h, POS_Z) - 0.5f) * T.jitter) * T.grid_m;
			float f = field(s, T, x, z);
			WoodBand b = band_of(T, f);
			const BandLook &B = T.band[size_t(b)];
			if ((open_only && b != WoodBand::OPEN) || h01(h, KEEP) >= B.keep) continue;
			float y;
			if (!ground_ok(s, T, x, z, T.grid_m, y)) continue;
			int sp = B.mix.pick(h01(h, SPECIES));
			float c = b == WoodBand::OPEN ? 0.f : depth_of(T, f);
			float height = (B.h0 + (B.h1 - B.h0) * h01(h, HEIGHT)) * (1.f + T.crowd_tall * c);
			float width = height * (1.f + (h01(h, WIDTH) * 2.f - 1.f) * T.width_vary) * (1.f - T.crowd_narrow * c);
			if (near_road(s, l, x, z, kit.species(sp).crown_reach * width + T.road_margin) || in_lot(s, l, T.lot_margin, x, z)) continue;
			TreeInstance t{ Vector3(x, y, z), height, width, TAU_F * h01(h, YAW), 0, 0, sp, tint_of(T, h01(h, TINT)) };
			if (b == WoodBand::EDGE) { // lean to the open side: down the field's slope
				float gx = field(s, T, x + T.grid_m, z) - field(s, T, x - T.grid_m, z), gz = field(s, T, x, z + T.grid_m) - field(s, T, x, z - T.grid_m);
				float g = std::sqrt(gx * gx + gz * gz), k = T.edge_lean * (1.f - c);
				if (g > 0) { t.lean_x = -gx / g * k; t.lean_z = -gz / g * k; }
			}
			out.push_back(t);
		}
}

void parks(const CityState &s, const TreeLook &T, const Lists &l, int64_t key, std::vector<TreeInstance> &out) {
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

FarCard far_of(const TreeInstance &t, const TreeKit &kit) {
	const TreeSpecies &sp = kit.species(t.species);
	return { t.p, t.height, t.width * sp.far_width, sp.far_tone.srgb_to_linear() * t.tint, t.species };
}
} // namespace

void place_trees(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<TreeInstance> &out) {
	Lists l = lists(s, key);
	street(s, L, T, key, out);
	parks(s, T, l, key, out);
	wild(s, T, kit, l, key, false, out);
}

void place_far(const CityState &s, const StreetLook &L, const TreeLook &T, const TreeKit &kit, int64_t key,
		std::vector<FarCard> &out) {
	Lists l = lists(s, key);
	std::vector<TreeInstance> lone;
	street(s, L, T, key, lone);
	parks(s, T, l, key, lone);
	wild(s, T, kit, l, key, true, lone);
	for (const TreeInstance &t : lone) out.push_back(far_of(t, kit));
	int n = int(CHUNK / T.far_cell_m), cx = key_x(key), cz = key_y(key);
	float half = T.far_cell_m * 0.5f;
	for (int j = 0; j < n; j++)
		for (int i = 0; i < n; i++) {
			int ix = cx * n + i, iz = cz * n + j;
			uint32_t h = cell_hash(ix, iz, FAR_SALT ^ s.seed);
			float x = (ix + 0.5f + (h01(h, POS_X) - 0.5f) * T.far_jitter) * T.far_cell_m, z = (iz + 0.5f + (h01(h, POS_Z) - 0.5f) * T.far_jitter) * T.far_cell_m;
			float f = field(s, T, x, z);
			WoodBand b = band_of(T, f);
			if (b == WoodBand::OPEN) continue;
			float y;
			if (!ground_ok(s, T, x, z, half, y) || near_road(s, l, x, z, half + T.road_margin) || in_lot(s, l, T.lot_margin + half, x, z)) continue;
			const BandLook &B = T.band[size_t(b)];
			int sp = B.mix.pick(h01(h, SPECIES));
			float height = (B.h0 + (B.h1 - B.h0) * h01(h, HEIGHT)) * (1.f + T.crowd_tall * depth_of(T, f));
			out.push_back({ Vector3(x, y, z), height, T.far_cell_m * T.far_spread * std::sqrt(B.cover) * (1.f + (h01(h, WIDTH) * 2.f - 1.f) * T.width_vary),
					kit.species(sp).far_tone.srgb_to_linear() * tint_of(T, h01(h, TINT)), sp });
		}
}

} // namespace city
