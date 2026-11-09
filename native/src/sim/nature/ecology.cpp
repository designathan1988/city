// The plant communities by site (see ecology.h). The mixes follow the descriptions of each fitofisionomia: gallery
// forest along water, veredas in wet valley floors, cerradão on lower slopes, campo rupestre on ridges; flooded forest
// on low ground by the rivers; capões in the hollows of the pampa; the flooded fields and cordilheiras of the pantanal.
#include "sim/nature/ecology.h"

#include "core/math.h"
#include "core/random.h"
#include "core/seeded.h"
#include "core/units.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SEED_MOD = 997u; // a seed moves the patches by up to this many cells
constexpr float SHIFT_X = 1.731f, SHIFT_Z = 2.113f; // cells a seed's step moves the patches, apart so the axes do not repeat together
constexpr int STRATA_MIX = 6; // the most communities one biome mixes
constexpr float NO_WEIGHT = 1e-6f, MIN_WEIGHT = 0.01f, BYTE_MAX = 255.f; // a sum of nothing; a paint under a hundredth counts for none; a byte's top
constexpr float UNREACHED_M = 1e9f; // farther than any cell: not reached yet
constexpr int CHAMFER_STRAIGHT = 3, CHAMFER_DIAGONAL = 4; // two-pass chamfer distance (Borgefors 1986)

float smooth(float a, float b, float x) { return smooth01((x - a) / (b - a)); }
float clamp01(float v) { return clampf(v, 0.f, 1.f); }

struct Site {
	float water = 0, valley = 0, ridge = 0, flat = 0, steep = 0, altitude = 0, sandstone = 0, basalt = 0, patch = 0, fray = 0;
};

void add_to(Community &o, const Community &c, float k) {
	o.canopy += c.canopy * k; o.trees += c.trees * k; o.emergent += c.emergent * k; o.shrub += c.shrub * k; o.palm += c.palm * k;
	o.cactus += c.cactus * k; o.grass += c.grass * k; o.dry += c.dry * k; o.wet += c.wet * k; o.rocky += c.rocky * k;
}

// Communities of one biome and their weights; `blend` makes them one that sums to their mean.
struct Mix {
	std::array<float, STRATA_MIX> w{};
	std::array<Formation, STRATA_MIX> f{};
	int n = 0;
	void add(float weight, Formation formation) {
		w[size_t(n)] = weight;
		f[size_t(n)] = formation;
		n++;
	}
	Community blend(const EcologyDef &d) const {
		float total = 0;
		for (int i = 0; i < n; i++) total += std::max(0.f, w[size_t(i)]);
		Community out;
		if (total <= NO_WEIGHT) return out;
		for (int i = 0; i < n; i++)
			if (w[size_t(i)] > 0) add_to(out, d.formations[size_t(f[size_t(i)])], w[size_t(i)] / total);
		return out;
	}
};

Community biome_community(Biome b, const Site &s, const EcologyDef &d) {
	float riparian = 1.f - smooth(d.riparian_near_m + d.riparian_near_patch_m * s.patch, d.riparian_far_m + d.riparian_far_patch_m * s.patch, s.water);
	float cliff = s.steep, open = 1.f - cliff;
	const EcologyMixDef &x = d.mix;
	Mix m;
	switch (b) {
		case Biome::CERRADO: {
			float vereda = s.flat * smooth(x.vereda_deep_from, x.vereda_deep_to, s.valley) * (1.f - smooth(x.vereda_water_near_m, x.vereda_water_far_m, s.water)) * smooth(x.vereda_patch_from, x.vereda_patch_to, s.patch);
			float dense = clamp01(smooth(x.dense_deep_from, x.dense_deep_to, s.valley) * x.dense_deep_share + smooth(x.dense_patch_from, x.dense_patch_to, s.patch) * x.dense_patch_share);
			float rupestre = clamp01(s.ridge * x.rupestre_ridge + s.sandstone * x.rupestre_sandstone * s.flat) * (1.f - riparian);
			m.add(riparian * open, Formation::GALLERY);
			m.add(vereda * (1.f - riparian) * open, Formation::VEREDA);
			m.add(dense * (1.f - riparian) * (1.f - vereda) * open, Formation::CERRADAO);
			m.add(rupestre * open, Formation::RUPESTRE);
			m.add(std::max(0.f, 1.f - riparian - vereda - dense - rupestre) * open + x.cerrado_floor, Formation::CERRADO);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::ATLANTIC: {
			float high = smooth(d.forest_line_from_m, d.forest_line_to_m, s.altitude + s.ridge * x.ridge_altitude_m + (s.patch - 0.5f) * x.patch_altitude_m);
			m.add((1.f - high) * open + riparian, Formation::OMBROPHILOUS);
			m.add(high * open * (1.f - riparian), Formation::HIGH_GRASSLAND);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::AMAZON: {
			float varzea = (1.f - smooth(x.varzea_water_near_m, x.varzea_water_far_m, s.water)) * clamp01(x.varzea_valley + s.valley);
			m.add((1.f - varzea) * open, Formation::TERRA_FIRME);
			m.add(varzea * open, Formation::VARZEA);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::CAATINGA:
			m.add(riparian * open, Formation::CAATINGA_RIVER);
			m.add((1.f - riparian) * open, Formation::THORN_SCRUB);
			m.add(cliff + s.ridge * x.caatinga_ridge, Formation::OUTCROP);
			break;
		case Biome::PAMPA: {
			float capao = smooth(x.capao_patch_from, x.capao_patch_to, s.patch) * smooth(x.capao_deep_from, x.capao_deep_to, s.valley);
			m.add(riparian * open * x.capao_riparian, Formation::CAPAO);
			m.add(capao * (1.f - riparian) * open, Formation::CAPAO);
			m.add((1.f - capao) * (1.f - riparian) * open, Formation::CAMPOS);
			m.add(cliff + s.ridge * x.pampa_ridge + s.basalt * x.pampa_basalt, Formation::OUTCROP);
			break;
		}
		case Biome::PANTANAL: {
			float low = std::max(s.flat * clamp01(x.low_valley_base + s.valley * x.low_valley) * (1.f - smooth(x.low_ridge_from, x.low_ridge_to, s.ridge)), 1.f - smooth(x.low_water_near_m, x.low_water_far_m, s.water));
			float cord = smooth(x.cordilheira_patch_from, x.cordilheira_patch_to, s.patch) * (1.f - low * x.cordilheira_low);
			m.add(riparian * open * x.pantanal_riparian, Formation::CORDILHEIRA);
			m.add(low * (1.f - cord) * open, Formation::FLOODED_FIELD);
			m.add(cord * open, Formation::CORDILHEIRA);
			m.add(std::max(0.f, 1.f - low - cord) * open * x.pantanal_rest, Formation::CERRADO);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		default: return Community{};
	}
	Community c = m.blend(d);
	float jitter = x.ragged_base + x.ragged_span * s.fray;
	c.canopy = clamp01(c.canopy * jitter);
	c.trees = clamp01(c.trees * jitter);
	c.shrub = clamp01(c.shrub * (x.shrub_base - x.shrub_span * s.fray));
	return c;
}

// Distance to the nearest wet corner, metres.
void water_distance(const std::vector<uint8_t> &wet, int side, float cell, std::vector<float> &d) {
	d.resize(wet.size());
	for (size_t i = 0; i < wet.size(); i++) d[i] = wet[i] ? 0.f : UNREACHED_M;
	auto relax = [&](int x, int y, int dx, int dy, int w) {
		int nx = x + dx, ny = y + dy;
		if (nx < 0 || ny < 0 || nx >= side || ny >= side) return;
		float &v = d[size_t(y) * side + x];
		v = std::min(v, d[size_t(ny) * side + nx] + float(w));
	};
	for (int y = 0; y < side; y++)
		for (int x = 0; x < side; x++) {
			relax(x, y, -1, 0, CHAMFER_STRAIGHT); relax(x, y, 0, -1, CHAMFER_STRAIGHT);
			relax(x, y, -1, -1, CHAMFER_DIAGONAL); relax(x, y, 1, -1, CHAMFER_DIAGONAL);
		}
	for (int y = side - 1; y >= 0; y--)
		for (int x = side - 1; x >= 0; x--) {
			relax(x, y, 1, 0, CHAMFER_STRAIGHT); relax(x, y, 0, 1, CHAMFER_STRAIGHT);
			relax(x, y, 1, 1, CHAMFER_DIAGONAL); relax(x, y, -1, 1, CHAMFER_DIAGONAL);
		}
	for (float &v : d) v *= cell / CHAMFER_STRAIGHT;
}

// The mean of the heights round each corner over a square of `radius` corners: a separable box blur.
void local_mean(const std::vector<float> &h, int side, int radius, std::vector<float> &out) {
	std::vector<float> rows(h.size());
	out.resize(h.size());
	for (int y = 0; y < side; y++)
		for (int x = 0; x < side; x++) {
			float sum = 0;
			int n = 0;
			for (int k = std::max(0, x - radius); k <= std::min(side - 1, x + radius); k++, n++) sum += h[size_t(y) * side + k];
			rows[size_t(y) * side + x] = sum / float(n);
		}
	for (int y = 0; y < side; y++)
		for (int x = 0; x < side; x++) {
			float sum = 0;
			int n = 0;
			for (int k = std::max(0, y - radius); k <= std::min(side - 1, y + radius); k++, n++) sum += rows[size_t(k) * side + x];
			out[size_t(y) * side + x] = sum / float(n);
		}
}
} // namespace

const Community &EcologyField::near(float x, float z) const {
	int ix = clampi(int(std::lround((x - x0) / cell)), 0, side - 1), iz = clampi(int(std::lround((z - z0) / cell)), 0, side - 1);
	return at[size_t(iz) * side + ix];
}

float wood_share(const Community &c, const EcologyDef &d) {
	return std::min(1.f, c.canopy * d.canopy_wood + c.trees * d.trees_wood + c.emergent * d.emergent_wood);
}

EcologyField compute_ecology(const Heightfield &hf, const GroundPaint &paint, const NatureSettings &nature, const EcologyDef &d, float x0, float z0, float span) {
	EcologyField f;
	f.cell = d.grid_m;
	int inner = int(std::ceil(span / d.grid_m)) + 1, m = d.mean_cells + 1, side = inner + 2 * m;
	float gx = x0 - float(m) * d.grid_m, gz = z0 - float(m) * d.grid_m;
	std::vector<float> h(size_t(side) * side), dist, mean;
	std::vector<uint8_t> wet(h.size());
	for (int y = 0; y < side; y++)
		for (int x = 0; x < side; x++) {
			float wx = gx + float(x) * d.grid_m, wz = gz + float(y) * d.grid_m;
			h[size_t(y) * side + x] = hf.in_bounds(wx, wz) ? hf.base_height_at(wx, wz) : hf.water_level;
			wet[size_t(y) * side + x] = hf.in_bounds(wx, wz) && hf.is_water(wx, wz, 0.f) ? 1 : 0;
		}
	water_distance(wet, side, d.grid_m, dist);
	local_mean(h, side, d.mean_cells, mean);
	f.side = inner;
	f.x0 = x0;
	f.z0 = z0;
	f.at.resize(size_t(inner) * inner);
	float sx = float(nature.seed % SEED_MOD) * SHIFT_X, sz = float((nature.seed / SEED_MOD) % SEED_MOD) * SHIFT_Z;
	for (int j = 0; j < inner; j++)
		for (int i = 0; i < inner; i++) {
			int x = i + m, y = j + m;
			size_t k = size_t(y) * side + x;
			float wx = gx + float(x) * d.grid_m, wz = gz + float(y) * d.grid_m;
			float hx = (h[k + 1] - h[k - 1]) / (2 * d.grid_m), hz = (h[k + size_t(side)] - h[k - size_t(side)]) / (2 * d.grid_m);
			float slope = std::atan(std::hypot(hx, hz)) * RAD_TO_DEG, relative = h[k] - mean[k];
			Site s;
			s.water = dist[k];
			s.valley = smooth(d.low_m, d.high_m, -relative);
			s.ridge = smooth(d.low_m, d.high_m, relative);
			s.flat = 1.f - smooth(d.flat_from_deg, d.flat_to_deg, slope);
			s.steep = smooth(d.steep_from_deg, d.steep_to_deg, slope);
			s.altitude = h[k] - hf.water_level;
			s.patch = float(woods_noise(wx / d.patch_m + sx, sz - wz / d.patch_m, 1.0, 0)); // the map's north is -z
			s.fray = float(woods_noise(wx / d.fray_m + sz, -wz / d.fray_m - sx, 1.0, 0));
			int ix = hf.ix_of(wx), iz = hf.ix_of(wz);
			const PaintCell &pc = paint.cell(ix, iz);
			const float rock = pc.amount / BYTE_MAX; // the rock the land is made of: the painted geology, granite where none was
			s.sandstone = pc.surface == d.sandstone_surface ? rock : 0.f;
			s.basalt = pc.surface == d.basalt_surface ? rock : 0.f;
			float painted = pc.biome ? pc.biome_amount / BYTE_MAX : 0.f, own = 1.f - painted;
			Community total;
			float sum = 0;
			if (pc.biome && painted >= MIN_WEIGHT) { add_to(total, biome_community(Biome(pc.biome - 1), s, d), painted); sum += painted; }
			if (nature.region >= 0 && own >= MIN_WEIGHT) { add_to(total, biome_community(Biome(nature.region), s, d), own); sum += own; }
			Community &out = f.at[size_t(j) * inner + i];
			if (sum > 0) add_to(out, total, 1.f / sum);
			if (wet[k]) out.canopy = out.trees = out.emergent = out.shrub = out.palm = out.cactus = out.rocky = 0;
		}
	return f;
}

} // namespace city
