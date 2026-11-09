// How run down a building looks by its age (Roadcraft world/buildings/types.ts decayOf): its wear share, in steps, from the city
// minute it was built and the clock, and the city minute that share next moves, when the render draws its chunk again
// (building_renderer.cpp). The share is drawn by the walls' stains (building_common.gdshaderinc weather), the far and mid walls'
// tone, the rain streaks (facade_bay.cpp) and a balcony's things (facade_props.cpp).
#pragma once

#include "render/buildings/look_shapes_levels.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city::b40 {

struct Wear {
	float share = 0; // 0 new .. 1 run down, in steps of 1 / steps
	double next_min = 0; // the city minute the share moves to its next step; WEAR_NEVER_MIN once it is run down
};

constexpr double WEAR_NEVER_MIN = 1e300; // a share that never moves again: farther than any city minute

// `built` and `now` city minutes, `span_min` the minutes from the first stains to run down (data/catalog/growth.json decay_span_min).
Wear wear_of(const WearLook &K, double built, double now, double span_min);

// The tone of a wall worn `share` where no stain is drawn a pixel (an instance's colour, the far level's), over the tone it has new:
// the stains' darkening by far_k of the share, never under `stain_floor`, the plain finishes' own stain share (materials.json
// weather_stain), which every level already draws (Roadcraft finishes.ts FAR_FINISH).
float wear_tone(const WearLook &K, float share, float stain_floor);

// The wear as the building renderer draws it: the look's rule (building_look.json shapes.wear) and the plain finishes' own stain
// share (materials.json _shared finishes weather_stain[0]).
struct WearRule {
	const WearLook *look = nullptr;
	float stain_floor = 0;

	bool load(const WearLook &K, const godot::Dictionary &materials, godot::PackedStringArray &errors);
	// A building's share at `now`, its next step folded into `due`.
	float share(double built, double now, double span_min, double &due) const;
	float tone(float share) const { return wear_tone(*look, share, stain_floor); }
};

} // namespace city::b40
