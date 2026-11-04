// The plant communities by site (see ecology.h). The mixes follow the descriptions of each fitofisionomia: gallery
// forest along water, veredas in wet valley floors, cerradão on lower slopes, campo rupestre on ridges; flooded forest
// on low ground by the rivers; capões in the hollows of the pampa; the flooded fields and cordilheiras of the pantanal.
#include "sim/nature/ecology.h"

#include "core/math.h"
#include "core/random.h"
#include "core/units.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SEED_MOD = 997u; // a seed moves the patches by up to this many cells
constexpr float SHIFT_X = 1.731f, SHIFT_Z = 2.113f;
constexpr uint32_t PATCH_SALT = 0x70617463u, FRAY_SALT = 0x66726179u;
constexpr int STRATA_MIX = 6; // the most communities one biome mixes
// The shares of a mix (each site reading): the sides of a smoothstep, in 0..1 of the patch or valley.
constexpr float VEREDA_DEEP_FROM = 0.15f, VEREDA_DEEP_TO = 0.55f, VEREDA_PATCH_FROM = 0.3f, VEREDA_PATCH_TO = 0.6f, VEREDA_WATER_NEAR_M = 24.f, VEREDA_WATER_FAR_M = 128.f;
constexpr float DENSE_DEEP_FROM = 0.1f, DENSE_DEEP_TO = 0.5f, DENSE_DEEP_SHARE = 0.8f, DENSE_PATCH_FROM = 0.62f, DENSE_PATCH_TO = 0.85f, DENSE_PATCH_SHARE = 0.6f;
constexpr float RUPESTRE_RIDGE = 0.9f, RUPESTRE_SANDSTONE = 0.45f, CERRADO_FLOOR = 0.05f;
constexpr float RIDGE_ALTITUDE_M = 24.f, PATCH_ALTITUDE_M = 20.f;
constexpr float VARZEA_WATER_NEAR_M = 16.f, VARZEA_WATER_FAR_M = 88.f, VARZEA_VALLEY = 0.4f;
constexpr float CAATINGA_RIDGE = 0.35f, CAPAO_PATCH_FROM = 0.62f, CAPAO_PATCH_TO = 0.8f, CAPAO_DEEP_FROM = 0.05f, CAPAO_DEEP_TO = 0.4f, CAPAO_RIPARIAN = 0.9f, PAMPA_RIDGE = 0.2f;
constexpr float LOW_VALLEY_BASE = 0.35f, LOW_VALLEY = 1.2f, LOW_RIDGE_FROM = 0.25f, LOW_RIDGE_TO = 0.6f, LOW_WATER_NEAR_M = 8.f, LOW_WATER_FAR_M = 88.f;
constexpr float CORDILHEIRA_PATCH_FROM = 0.55f, CORDILHEIRA_PATCH_TO = 0.75f, CORDILHEIRA_LOW = 0.7f, PANTANAL_RIPARIAN = 0.35f, PANTANAL_REST = 0.6f;
constexpr float RAGGED_BASE = 0.8f, RAGGED_SPAN = 0.4f, SHRUB_BASE = 1.2f, SHRUB_SPAN = 0.4f;
constexpr float NO_WEIGHT = 1e-6f, MIN_WEIGHT = 0.01f, BYTE_MAX = 255.f;
constexpr float FAR_FROM_WATER_M = 1e9f;
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
	Mix m;
	switch (b) {
		case Biome::CERRADO: {
			float vereda = s.flat * smooth(VEREDA_DEEP_FROM, VEREDA_DEEP_TO, s.valley) * (1.f - smooth(VEREDA_WATER_NEAR_M, VEREDA_WATER_FAR_M, s.water)) * smooth(VEREDA_PATCH_FROM, VEREDA_PATCH_TO, s.patch);
			float dense = clamp01(smooth(DENSE_DEEP_FROM, DENSE_DEEP_TO, s.valley) * DENSE_DEEP_SHARE + smooth(DENSE_PATCH_FROM, DENSE_PATCH_TO, s.patch) * DENSE_PATCH_SHARE);
			float rupestre = clamp01(s.ridge * RUPESTRE_RIDGE + s.sandstone * RUPESTRE_SANDSTONE * s.flat) * (1.f - riparian);
			m.add(riparian * open, Formation::GALLERY);
			m.add(vereda * (1.f - riparian) * open, Formation::VEREDA);
			m.add(dense * (1.f - riparian) * (1.f - vereda) * open, Formation::CERRADAO);
			m.add(rupestre * open, Formation::RUPESTRE);
			m.add(std::max(0.f, 1.f - riparian - vereda - dense - rupestre) * open + CERRADO_FLOOR, Formation::CERRADO);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::ATLANTIC: {
			float high = smooth(d.forest_line_from_m, d.forest_line_to_m, s.altitude + s.ridge * RIDGE_ALTITUDE_M + (s.patch - 0.5f) * PATCH_ALTITUDE_M);
			m.add((1.f - high) * open + riparian, Formation::OMBROPHILOUS);
			m.add(high * open * (1.f - riparian), Formation::HIGH_GRASSLAND);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::AMAZON: {
			float varzea = (1.f - smooth(VARZEA_WATER_NEAR_M, VARZEA_WATER_FAR_M, s.water)) * clamp01(VARZEA_VALLEY + s.valley);
			m.add((1.f - varzea) * open, Formation::TERRA_FIRME);
			m.add(varzea * open, Formation::VARZEA);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		case Biome::CAATINGA:
			m.add(riparian * open, Formation::CAATINGA_RIVER);
			m.add((1.f - riparian) * open, Formation::THORN_SCRUB);
			m.add(cliff + s.ridge * CAATINGA_RIDGE, Formation::OUTCROP);
			break;
		case Biome::PAMPA: {
			float capao = smooth(CAPAO_PATCH_FROM, CAPAO_PATCH_TO, s.patch) * smooth(CAPAO_DEEP_FROM, CAPAO_DEEP_TO, s.valley);
			m.add(riparian * open * CAPAO_RIPARIAN, Formation::CAPAO);
			m.add(capao * (1.f - riparian) * open, Formation::CAPAO);
			m.add((1.f - capao) * (1.f - riparian) * open, Formation::CAMPOS);
			m.add(cliff + s.ridge * PAMPA_RIDGE + s.basalt * PAMPA_RIDGE, Formation::OUTCROP);
			break;
		}
		case Biome::PANTANAL: {
			float low = std::max(s.flat * clamp01(LOW_VALLEY_BASE + s.valley * LOW_VALLEY) * (1.f - smooth(LOW_RIDGE_FROM, LOW_RIDGE_TO, s.ridge)), 1.f - smooth(LOW_WATER_NEAR_M, LOW_WATER_FAR_M, s.water));
			float cord = smooth(CORDILHEIRA_PATCH_FROM, CORDILHEIRA_PATCH_TO, s.patch) * (1.f - low * CORDILHEIRA_LOW);
			m.add(riparian * open * PANTANAL_RIPARIAN, Formation::CORDILHEIRA);
			m.add(low * (1.f - cord) * open, Formation::FLOODED_FIELD);
			m.add(cord * open, Formation::CORDILHEIRA);
			m.add(std::max(0.f, 1.f - low - cord) * open * PANTANAL_REST, Formation::CERRADO);
			m.add(cliff, Formation::OUTCROP);
			break;
		}
		default: return Community{};
	}
	Community c = m.blend(d);
	float jitter = RAGGED_BASE + RAGGED_SPAN * s.fray;
	c.canopy = clamp01(c.canopy * jitter);
	c.trees = clamp01(c.trees * jitter);
	c.shrub = clamp01(c.shrub * (SHRUB_BASE - SHRUB_SPAN * s.fray));
	return c;
}

// Distance to the nearest wet corner, metres.
void water_distance(const std::vector<uint8_t> &wet, int side, float cell, std::vector<float> &d) {
	d.resize(wet.size());
	for (size_t i = 0; i < wet.size(); i++) d[i] = wet[i] ? 0.f : FAR_FROM_WATER_M;
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
			s.patch = 0.5f + 0.5f * vnoise(wx / d.patch_m + sx, wz / d.patch_m + sz, PATCH_SALT);
			s.fray = 0.5f + 0.5f * vnoise(wx / d.fray_m + sz, wz / d.fray_m - sx, FRAY_SALT);
			int ix = hf.ix_of(wx), iz = hf.ix_of(wz);
			const PaintCell &pc = paint.cell(ix, iz);
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
