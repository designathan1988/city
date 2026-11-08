// The land of a scenic city, see scenic_land.h.
#include "sim/terrain/scenic_land.h"

#include "core/random.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int RELIEF_OCTAVES = 3;
constexpr uint32_t RELIEF_SALT = 0x5ce41cU;
constexpr float WATER_WALL_SHARE = 0.5f; // the water stands this share up the channel's walls
constexpr float SHORE_SAMPLE_M = 8; // a lake's rim is read this often for its lowest land
} // namespace

ScenicLandField::Box ScenicLandField::box_of(const Polyline &line, float reach) {
	Box b{ V2(BIG, BIG), V2(-BIG, -BIG) };
	for (const V2 &p : line) {
		b.lo = V2(std::min(b.lo.x, p.x - reach), std::min(b.lo.y, p.y - reach));
		b.hi = V2(std::max(b.hi.x, p.x + reach), std::max(b.hi.y, p.y + reach));
	}
	return b;
}

ScenicLandField::ScenicLandField(const ScenicLand &def, uint32_t seed) : def_(def), seed_(seed) {
	for (const ScenicRidge &r : def_.ridges) ridge_box_.push_back(box_of(r.line, r.half_width_m));
	for (const ScenicRiver &r : def_.rivers) {
		river_box_.push_back(box_of(r.line, r.half_width_m + r.wall_m + r.valley_m));
		river_length_.push_back(std::max(1.f, polyline_length(r.line)));
	}
	// A lake stands drop_m under the lowest land of its shore, so no shore lies under its water.
	for (const ScenicLake &l : def_.lakes) {
		lake_box_.push_back(box_of(l.shore, l.shore_m));
		double low = NO_ROOM;
		for (size_t k = 0; k < l.shore.size(); k++) {
			const V2 a = l.shore[k], b = l.shore[(k + 1) % l.shore.size()];
			const int steps = std::max(1, int(std::ceil((b - a).len() / SHORE_SAMPLE_M)));
			for (int i = 0; i < steps; i++) low = std::min(low, plateau(a + (b - a) * (float(i) / float(steps))));
		}
		lake_level_.push_back(float(low) - l.drop_m);
	}
}

double ScenicLandField::plateau(V2 p) const {
	double h = def_.base_m + def_.relief_m * fbm(p.x / def_.relief_cell_m, p.y / def_.relief_cell_m, seed_ ^ RELIEF_SALT, RELIEF_OCTAVES);
	for (size_t i = 0; i < def_.ridges.size(); i++) {
		if (!ridge_box_[i].near(p)) continue;
		const ScenicRidge &r = def_.ridges[i];
		h += r.height_m * smooth01(1 - polyline_distance(r.line, p) / r.half_width_m);
	}
	return h;
}

float ScenicLandField::river_level(size_t i, float along) const {
	const ScenicRiver &r = def_.rivers[i];
	return r.level_m + (r.level_end_m - r.level_m) * std::min(1.f, along / river_length_[i]);
}

double ScenicLandField::height(double x, double z) const {
	const V2 p{ float(x), float(z) };
	double h = plateau(p);
	for (size_t i = 0; i < def_.rivers.size(); i++) {
		if (!river_box_[i].near(p)) continue;
		const ScenicRiver &r = def_.rivers[i];
		float along = 0;
		const float d = polyline_distance(r.line, p, &along), level = river_level(i, along);
		const double bed = level - r.depth_m, bank = level + r.freeboard_m;
		double shaped;
		if (d <= r.half_width_m) shaped = bed;
		else if (d <= r.half_width_m + r.wall_m) shaped = bed + (bank - bed) * (d - r.half_width_m) / r.wall_m;
		else shaped = bank + (h - bank) * smooth01((d - r.half_width_m - r.wall_m) / r.valley_m);
		h = std::min(h, shaped);
	}
	for (size_t i = 0; i < def_.lakes.size(); i++) {
		if (!lake_box_[i].near(p)) continue;
		const ScenicLake &l = def_.lakes[i];
		const float edge = ring_edge_distance(l.shore, p), level = lake_level_[i];
		const double shaped = ring_holds(l.shore, p) ? level - l.depth_m * smooth01(edge / l.shore_m) : level + (h - level) * smooth01(edge / l.shore_m);
		h = std::min(h, shaped);
	}
	return h;
}

bool ScenicLandField::water(double x, double z, float &level) const {
	const V2 p{ float(x), float(z) };
	for (size_t i = 0; i < def_.rivers.size(); i++) {
		if (!river_box_[i].near(p)) continue;
		const ScenicRiver &r = def_.rivers[i];
		float along = 0;
		if (polyline_distance(r.line, p, &along) > r.half_width_m + r.wall_m * WATER_WALL_SHARE) continue;
		level = river_level(i, along);
		return true;
	}
	for (size_t i = 0; i < def_.lakes.size(); i++) {
		if (!lake_box_[i].near(p) || !ring_holds(def_.lakes[i].shore, p)) continue;
		level = lake_level_[i];
		return true;
	}
	return false;
}

} // namespace city
