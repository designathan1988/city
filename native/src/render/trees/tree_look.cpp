// streets.json "trees.placement" → TreeLook (see tree_place.h). The bands' cover becomes a chance per candidate cell
// (cover × cell area ÷ the mean crown's area), and the woods field's thresholds are the quantiles of the field that
// leave the asked land shares above them: the noise is stationary, so one sample of it, taken once, sets them.
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/random.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HA_M2 = 10000.f;
// The calibration sample: a square of CAL_SIDE² points CAL_STEP mass lengths apart (far beyond the field's correlation,
// so the points are close to independent), with its own seed: the field's distribution does not depend on the seed.
constexpr int CAL_SIDE = 160;
constexpr float CAL_STEP = 0.73f;
constexpr uint32_t CAL_SEED = 0x7a11u;
const char *const BAND_KEYS[WOOD_BANDS] = { "inner", "edge", "open" };

float at(const Variant &v, int i) {
	Array a = v;
	return a.size() > i ? float(a[i]) : 0.f;
}

// The field value above which `share` of the land lies.
float quantile_above(std::vector<float> &sorted, float share) {
	size_t i = size_t(std::clamp((1.f - share) * float(sorted.size() - 1), 0.f, float(sorted.size() - 1)));
	return sorted[i];
}

// The mix's mean crown radius over its mean height, the species weighted by their share.
float mean_reach(const SpeciesMix &m, const TreeKit &kit) {
	float sum = 0, prev = 0;
	for (size_t i = 0; i < m.species.size(); i++) {
		sum += (m.weight[i] - prev) * kit.species(m.species[i]).crown_reach;
		prev = m.weight[i];
	}
	return sum;
}
} // namespace

int SpeciesMix::pick(float r) const {
	for (size_t i = 0; i < weight.size(); i++)
		if (r < weight[i]) return species[i];
	return species.empty() ? 0 : species.back();
}

bool SpeciesMix::load(const Dictionary &mix, const TreeKit &kit, const String &where, PackedStringArray &errors) {
	species.clear();
	weight.clear();
	float total = 0;
	Array keys = mix.keys();
	for (int i = 0; i < keys.size(); i++) total += float(mix[keys[i]]);
	if (total <= 0) {
		errors.push_back("streets.json trees " + where + ": mistura de espécies vazia");
		return false;
	}
	float run = 0;
	for (int i = 0; i < keys.size(); i++) {
		int sp = kit.find(String(keys[i]));
		if (sp < 0) errors.push_back("streets.json trees " + where + ": espécie desconhecida " + String(keys[i]));
		run += float(mix[keys[i]]) / total;
		species.push_back(std::max(sp, 0));
		weight.push_back(run);
	}
	return errors.is_empty();
}

bool TreeLook::load(const Dictionary &t, const TreeKit &kit, PackedStringArray &errors) {
	if (!t.has("placement")) {
		errors.push_back("streets.json trees: falta placement");
		return false;
	}
	Dictionary p = t["placement"], bands = p.get("bands", Dictionary());
	near_m = p["near_m"]; fade_m = p["fade_m"]; pop_m = p["pop_m"]; far_m = p["far_m"]; far_cell_m = p["far_cell_m"]; far_spread = p["far_spread"]; far_jitter = p["far_jitter"];
	far_shadows = p["far_shadows"]; grid_m = p["grid_m"]; jitter = p["jitter"]; mass_m = p["mass_m"]; octaves = p["octaves"];
	inner_share = p["wood_inner_share"]; edge_share = p["wood_edge_share"]; open_per_ha = p["open_per_ha"];
	slope_from = at(p["slope_grade"], 0); slope_to = at(p["slope_grade"], 1); slope_gain = p["slope_gain"];
	water_m = p["riparian_m"]; water_gain = p["riparian_gain"];
	crowd_tall = p["crowd_tall"]; crowd_narrow = p["crowd_narrow"]; edge_lean = p["edge_lean"]; width_vary = p["width_vary"];
	tint_vary = p["tint_vary"]; min_y = p["min_y_m"]; max_grade = p["max_grade"]; road_margin = p["road_margin_m"]; lot_margin = p["lot_margin_m"];
	park_m2_per_tree = p["park_m2_per_tree"]; park_edge = p["park_edge_m"]; park_max = p["park_max"];
	park_h0 = at(p["park_height_m"], 0); park_h1 = at(p["park_height_m"], 1); street_height_m = p["street_height_m"];
	park.load(p["park_mix"], kit, "park_mix", errors);
	street.load(p["street_mix"], kit, "street_mix", errors);
	for (int b = 0; b < WOOD_BANDS; b++) {
		Dictionary d = bands.get(BAND_KEYS[b], Dictionary());
		BandLook &L = band[size_t(b)];
		L.h0 = at(d["height_m"], 0);
		L.h1 = at(d["height_m"], 1);
		L.mix.load(d.get("mix", Dictionary()), kit, String("bands.") + BAND_KEYS[b], errors);
		if (b == int(WoodBand::OPEN)) {
			L.keep = open_per_ha * grid_m * grid_m / HA_M2;
			continue;
		}
		L.cover = d["cover"];
		float crown = mean_reach(L.mix, kit) * 0.5f * (L.h0 + L.h1);
		L.keep = std::min(1.f, L.cover * grid_m * grid_m / std::max(PI_F * crown * crown, grid_m));
	}
	std::vector<float> f;
	f.reserve(size_t(CAL_SIDE) * CAL_SIDE);
	for (int j = 0; j < CAL_SIDE; j++)
		for (int i = 0; i < CAL_SIDE; i++) f.push_back(fbm(i * CAL_STEP, j * CAL_STEP, CAL_SEED, octaves));
	std::sort(f.begin(), f.end());
	t_inner = quantile_above(f, inner_share);
	t_edge = quantile_above(f, inner_share + edge_share);
	return errors.is_empty() && grid_m > 0 && mass_m > 0 && far_cell_m > 0;
}

} // namespace city
