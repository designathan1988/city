// The water each piece of land stands by, as the terrain shader reads it: a texel per water sample holding the level of
// the water over it, or of the water a bank within reach looks at (the sea, a lake, a river alike). The beach follows that
// level and not the sea's. A job works it out from the map; the main thread only uploads it, within the frame's ledger.
#pragma once

#include "render/async_slot.h"
#include "render/frame_admission.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <vector>

namespace city {

class ShoreField {
public:
	// Called each frame: starts a job when the land or the water changed, takes a finished one in.
	void sync(const CityState &s, FrameAdmission &f);
	// { revision, texture, grid } for render/shore_view.gd; revision 0 until the first levels are in.
	godot::Dictionary view() const;
	uint64_t view_revision() const { return uploads_; } // view()'s revision, read alone: a new view is in when it moves
	void touch() { revision_++; } // the land or the water changed
	void reset();

private:
	struct Raster {
		godot::PackedByteArray texels;
		int side = 0;
		float half = 0, step = 1;
	};
	// The job's buffers, kept for the next one.
	struct Scratch {
		std::vector<float> level, ground;
		std::vector<int> frontier, next;
		std::vector<std::pair<int, float>> set;
		std::vector<uint32_t> seen;
		uint32_t stamp = 0;
	};
	void work(const CityState &s, uint64_t generation);
	godot::Ref<godot::ImageTexture> texture_;
	AsyncSlot<Raster, Scratch> slot_;
	uint64_t revision_ = 0, asked_ = ~uint64_t(0), seen_ = ~uint64_t(0);
	int side_ = 0, uploads_ = 0;
	float half_ = 0, step_ = 1;
};

} // namespace city
