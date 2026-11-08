// streets.json "trees.placement" → TreeLook (see tree_place.h), through the strict reader (render/data_reader.h). The chances of the bands scale to `woods_per_ha`: the patches
// are stationary, so one sample of them, taken once, gives the land's mean chance.
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/seeded.h"
#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HA_M2 = 10000.f;
constexpr float THRESHOLD_GAP = 1e-6f; // a land with no chance at all keeps the scale finite
// The sample of the patches: a square of CAL_SIDE² points CAL_STEP_M apart, past the largest patch's correlation.
constexpr int CAL_SIDE = 200;
constexpr float CAL_STEP_M = 97.f;
// The noise streams of the patches, one by scale (Roadcraft's numbers: the woods stand where they stand there).
constexpr uint32_t PATCH_SALTS[WOOD_PATCHES] = { 7, 9, 11 };
const char *const BAND_KEYS[WOOD_BANDS] = { "inner", "edge", "open" };

constexpr int PAIR = 2; // [low, high]
} // namespace

float TreeLook::density_of(double x, double y) const {
	double patch = 0;
	for (int k = 0; k < WOOD_PATCHES; k++) patch += patch_weight[size_t(k)] * woods_noise(x, y, patch_m[size_t(k)], PATCH_SALTS[k]);
	return clampf((float(patch) - mass_from) / mass_span, 0.f, 1.f) * mass_gain - density_floor;
}

WoodBand TreeLook::band_at(float d) const {
	return d > band[size_t(WoodBand::INNER)].from ? WoodBand::INNER : d > band[size_t(WoodBand::EDGE)].from ? WoodBand::EDGE : WoodBand::OPEN;
}

float TreeLook::keep_at(float d, WoodBand b) const {
	const BandLook &B = band[size_t(b)];
	if (b == WoodBand::OPEN && d <= B.from) return B.lone * scale;
	float top = b == WoodBand::EDGE ? band[size_t(WoodBand::INNER)].from : B.from; // only the edge ramps up to a band above it
	float t = top > B.from ? clampf((d - B.from) / (top - B.from), 0.f, 1.f) : 0.f;
	return (B.keep0 + (B.keep1 - B.keep0) * t) * scale;
}

// The tier's budget of trees against the land's plan: it thins the woods only as far as the plan goes over it, as the 4.0's treeScale does.
float TreeLook::tier_share(float tier) const {
	return std::min(1.f, over_planned * tier) / scale;
}

int SpeciesMix::pick(float r) const {
	for (size_t i = 0; i < weight.size(); i++)
		if (r < weight[i]) return species[i];
	return species.empty() ? 0 : species.back();
}

bool SpeciesMix::load(DataReader r, const Dictionary &mix, const TreeKit &kit) {
	species.clear();
	weight.clear();
	float total = 0;
	const Array keys = mix.keys();
	std::vector<float> share;
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		share.push_back(float(r.num(mix, key.utf8().get_data())));
		total += share.back();
	}
	if (total <= 0) {
		r.fail("the species mix is empty");
		return false;
	}
	float run = 0;
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		const int sp = kit.find(key);
		if (sp < 0) r.fail(String("unknown species '") + key + String("'"));
		run += share[size_t(i)] / total;
		species.push_back(std::max(sp, 0));
		weight.push_back(run);
	}
	return true;
}

bool TreeLook::load(const Dictionary &t, const TreeKit &kit, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r = DataReader{ errors, "streets.json.trees" }.in("placement");
	const Dictionary p = DataReader{ errors, "streets.json.trees" }.table(t, "placement");
	auto num = [&](const char *key) { return float(r.num(p, key)); };
	auto pair = [&](const char *key, float &low, float &high) {
		const Array a = r.list(p, key, PAIR);
		low = float(r.in(key).num_at(a, 0));
		high = float(r.in(key).num_at(a, 1));
	};
	near_m = num("near_m"); fade_m = num("fade_m"); far_m = num("far_m");
	far_shadows = r.integer(p, "far_shadows"); grid_m = num("grid_m"); jitter = num("jitter"); plate_m = num("lattice_span_m");
	const Array pm = r.list(p, "patch_m", WOOD_PATCHES), pw = r.list(p, "patch_weight", WOOD_PATCHES);
	for (int k = 0; k < WOOD_PATCHES; k++) {
		patch_m[size_t(k)] = float(r.in("patch_m").num_at(pm, k));
		patch_weight[size_t(k)] = float(r.in("patch_weight").num_at(pw, k));
	}
	mass_from = num("mass_from"); mass_span = num("mass_span"); mass_gain = num("mass_gain"); density_floor = num("density_floor");
	pair("slope_grade", slope_from, slope_to); slope_gain = num("slope_gain"); slope_probe_m = num("slope_probe_m");
	woods_per_ha = num("woods_per_ha"); crowd_reach_m = num("crowd_reach_m"); crowd_full = num("crowd_full");
	crowd_tall = num("crowd_tall"); crowd_narrow = num("crowd_narrow"); edge_lean = num("edge_lean"); lean_rise = num("lean_rise"); width_vary = num("width_vary");
	open_tall = num("open_tall"); open_wide = num("open_wide");
	tint_vary = num("tint_vary"); min_y = num("min_y_m"); max_grade = num("max_grade"); road_margin = num("road_margin_m"); lot_margin = num("lot_margin_m");
	park_m2_per_tree = num("park_m2_per_tree"); park_edge = num("park_edge_m"); park_max = r.integer(p, "park_max");
	pair("park_height_m", park_h0, park_h1); street_height_m = num("street_height_m");
	eco_gain = num("eco_gain"); eco_mean = num("eco_mean"); forest_gain = num("forest_gain");
	const Array planted = r.list(p, "planted_models");
	planted_species.clear(); // a second read is the table again, not more of it
	for (int i = 0; i < planted.size(); i++) planted_species.push_back(std::max(kit.find(r.in("planted_models").text_at(planted, i)), 0));
	shrub_species = kit.find("shrub");
	hedge_species = kit.find("hedge");
	flowering_species = { kit.find("shrub_flowering"), kit.find("shrub_flowering_cream"), kit.find("shrub_flowering_red") };
	bed_species = { kit.find("flower_bed"), kit.find("flower_bed_cream"), kit.find("flower_bed_red") };
	if (shrub_species < 0 || hedge_species < 0 || *std::min_element(flowering_species.begin(), flowering_species.end()) < 0 || *std::min_element(bed_species.begin(), bed_species.end()) < 0)
		r.fail("the gardens' species are missing (shrub, shrub_flowering*, hedge, flower_bed*)");
	park.load(r.in("park_mix"), r.table(p, "park_mix"), kit);
	street.load(r.in("street_mix"), r.table(p, "street_mix"), kit);
	const Dictionary bands = r.table(p, "bands");
	most = 1; // as a first read starts it (tree_place.h)
	for (int b = 0; b < WOOD_BANDS; b++) {
		DataReader rb = r.in("bands").in(BAND_KEYS[b]);
		const Dictionary d = r.in("bands").table(bands, BAND_KEYS[b]);
		BandLook &L = band[size_t(b)];
		L.from = float(rb.num(d, "from"));
		const Array keep = rb.list(d, "keep", PAIR), size = rb.list(d, "size_m", PAIR);
		L.keep0 = float(rb.in("keep").num_at(keep, 0));
		L.keep1 = float(rb.in("keep").num_at(keep, 1));
		L.lone = float(rb.num_or(d, "lone", 0.0));
		L.h0 = float(rb.in("size_m").num_at(size, 0));
		L.h1 = float(rb.in("size_m").num_at(size, 1));
		L.mix.load(rb.in("mix"), rb.table(d, "mix"), kit);
		most = std::max(most, std::max(L.keep0, L.keep1));
	}
	if (grid_m <= 0) r.fail("grid_m: not above 0");
	if (mass_span <= 0) r.fail("mass_span: not above 0");
	if (slope_to <= slope_from) r.fail("slope_grade: the top not above the foot");
	if (crowd_full <= 0) r.fail("crowd_full: not above 0");
	if (errors.size() != before) return false;
	double sum = 0;
	for (int j = 0; j < CAL_SIDE; j++)
		for (int i = 0; i < CAL_SIDE; i++) {
			float d = density_of(i * CAL_STEP_M, j * CAL_STEP_M) + eco_gain * eco_mean;
			sum += keep_at(d, band_at(d));
		}
	const float cell = grid_m * grid_m, planned = float(sum) / float(CAL_SIDE * CAL_SIDE);
	over_planned = woods_per_ha * cell / HA_M2 / std::max(planned, THRESHOLD_GAP);
	scale = std::min(1.f, over_planned);
	most *= scale;
	return true;
}

} // namespace city
