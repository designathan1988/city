// The town's solids as the atmosphere pass reads them to take the sky out of its streets: a window of the land round the
// view, a texel holding the height of what stands there and the highest thing within reach of it (a pixel with nothing over it
// within reach reads no more). A job lays the building volumes; the main thread only uploads them, within the frame's ledger.
#pragma once

#include "render/async_slot.h"
#include "render/frame_admission.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <vector>

namespace city {

class CanyonField {
public:
	// Called each frame: starts a job when the buildings changed or the view left the window, takes a finished one in.
	void sync(const CityState &s, float cam_x, float cam_z, FrameAdmission &f);
	// { any, texture, window } for render/sky_view.gd.
	godot::Dictionary view() const;
	uint64_t view_revision() const { return view_revision_; } // moves whenever view() would say something else (RenderWorld::sync drives sync)
	// The building chunks that changed: the field is made again only when one of them meets the lots its window reads (the one
	// drawn or the one a job is making).
	void touch(const std::vector<int64_t> &chunks);
	void reset();

private:
	struct Raster {
		godot::PackedByteArray texels;
		float ox = 0, oz = 0;
		bool any = false;
	};
	// What a job keeps for the next one: its buffers.
	struct Scratch {
		std::vector<float> cells, rows, near, line, pad, ahead, behind;
	};
	void work(const CityState &s, float ox, float oz, uint64_t generation);
	bool reads(int64_t chunk, float ox, float oz) const;
	godot::Ref<godot::ImageTexture> texture_;
	AsyncSlot<Raster, Scratch> slot_;
	uint64_t revision_ = 0, asked_ = ~uint64_t(0), seen_ = ~uint64_t(0);
	uint64_t view_revision_ = 0; // main thread only
	float ox_ = 0, oz_ = 0, asked_ox_ = 0, asked_oz_ = 0;
	bool any_ = false, asking_ = false, drawn_ = false;
};

} // namespace city
