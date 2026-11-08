// The land of a scenic city (sim/catalog/scenic.h), worked out where it is asked: the plateau rolling by its relief, the ridges
// raised over it, each river cut into its channel with the floodplain easing down to its banks, each lake sunk under the land
// round it. TerrainGen asks it in place of its own land for a scenic map; it only reads after it is made, so any thread may ask.
#pragma once

#include "sim/catalog/scenic.h"

#include <cstdint>
#include <vector>

namespace city {

class ScenicLandField {
public:
	ScenicLandField(const ScenicLand &def, uint32_t seed);
	// Ground height at (x, z).
	double height(double x, double z) const;
	// The water at (x, z): true and its level in `level` on a river or a lake.
	bool water(double x, double z, float &level) const;

private:
	struct Box {
		V2 lo, hi; // a line's bounds widened by the reach of what it shapes
		bool near(V2 p) const { return p.x >= lo.x && p.x <= hi.x && p.y >= lo.y && p.y <= hi.y; }
	};
	ScenicLand def_;
	uint32_t seed_;
	std::vector<Box> ridge_box_, river_box_, lake_box_;
	std::vector<float> river_length_, lake_level_;

	static Box box_of(const Polyline &line, float reach);
	double plateau(V2 p) const; // relief and ridges, before the water
	float river_level(size_t i, float along) const;
};

} // namespace city
