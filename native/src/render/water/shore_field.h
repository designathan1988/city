// The water each piece of land stands by, as the terrain shader reads it: a texel per water sample holding the level of
// the water over it, or of the water a bank within reach looks at (the sea, a lake, a river alike). The beach follows that
// level and not the sea's. A job works it out from the map; the main thread only uploads it.
#pragma once

#include "sim/state/city_state.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <atomic>
#include <mutex>

namespace city {

class ShoreField {
public:
	// Called each frame: starts a job when the land or the water changed, takes a finished one in.
	void sync(const CityState &s);
	// { revision, texture, grid } for render/shore_view.gd; revision 0 until the first levels are in.
	godot::Dictionary view() const;
	void touch() { revision_++; } // the land or the water changed
	void reset();

private:
	struct Raster {
		godot::PackedByteArray texels;
		int side = 0;
		float half = 0, step = 1;
	};
	void work(const CityState &s, uint64_t revision);
	godot::Ref<godot::ImageTexture> texture_;
	mutable std::mutex mutex_;
	Raster done_;
	bool has_done_ = false;
	std::atomic<bool> busy_{ false };
	std::atomic<uint64_t> revision_{ 0 };
	uint64_t seen_ = ~uint64_t(0);
	int side_ = 0, uploads_ = 0;
	float half_ = 0, step_ = 1;
};

} // namespace city
