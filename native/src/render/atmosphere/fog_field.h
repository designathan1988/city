// The painted fog as the atmosphere pass reads it: a window of the land round the view, a texel holding the fog's
// density, the ground under it, the bank's height and the speed it drifts (a float texture, the shader reads it repeating
// so the window can follow the view). A job works the texels out from the dabs; the main thread only uploads them.
#pragma once

#include "sim/state/city_state.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <atomic>
#include <mutex>

namespace city {

class FogField {
public:
	// Called each frame: starts a job when the dabs changed or the view left the window, takes a finished one in.
	void sync(const CityState &s, float cam_x, float cam_z);
	// { any, texture, slab, window } for render/sky_view.gd.
	godot::Dictionary view() const;
	uint64_t view_revision() const { return view_revision_; } // moves whenever view() would say something else (RenderWorld::sync drives sync)
	void reset();

private:
	struct Raster {
		godot::PackedByteArray texels;
		float low = 0, high = 0, ox = 0, oz = 0, side = 1;
		bool any = false;
	};
	void work(const CityState &s, float ox, float oz, uint64_t revision);
	godot::Ref<godot::ImageTexture> texture_;
	mutable std::mutex mutex_;
	Raster done_;
	bool has_done_ = false;
	std::atomic<bool> busy_{ false };
	uint64_t seen_ = ~uint64_t(0);
	uint64_t view_revision_ = 0; // main thread only
	float ox_ = 0, oz_ = 0, side_ = 1, low_ = 0, high_ = 0;
	bool any_ = false;
};

} // namespace city
