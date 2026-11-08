// What stands on the open ground under the trees (data/render/cover.json): rocks and scrub as lumps of flat faces, cactus
// as columns, grass tufts (green, and the straw of the dry grass) as blades and wildflowers as a stem under a bloom, each
// kind in a few variants one unit tall,
// drawn with the tree bodies' shader (render/shaders/vegetation.gdshader), so wind and the fade with distance come with
// it. A kind says how far apart its plants may stand and how big they grow; where they stand is cover_renderer.cpp's.
#pragma once

#include "render/data_reader.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <array>
#include <vector>

namespace city {

enum class CoverPiece : int { ROCKS, SCRUB, CACTUS, TUFTS, FLOWERS, DRY_GRASS, COUNT };
constexpr int COVER_PIECES = int(CoverPiece::COUNT);

struct CoverLook {
	float spacing_m = 1, least = 0, size0 = 0, size1 = 0, tall = 1, sink = 0, tint_vary = 0, road_margin_m = 0;
	bool casts = false, sways = false;
	std::vector<godot::Ref<godot::ArrayMesh>> variants;
};

class CoverKit {
public:
	bool build(const DataValue &table, godot::PackedStringArray &errors); // cover.json, through the strict reader's values
	const CoverLook &look(CoverPiece p) const { return looks_[size_t(p)]; }
	float reach_m = 0, fade_m = 0, tuft_share = 0, dry_share = 0;
	float max_grade = 0, grade_probe_m = 1; // the steepest ground a plant stands on, read this far either side

private:
	std::array<CoverLook, COVER_PIECES> looks_;
};

} // namespace city
