// What the ecosystem leaves on the ground, as the terrain shader reads it: a texel per corner of the ecology's grid (nature.json
// ecology.site_grid_m) holding the dry herbs (grass times its dry share), the waterlogged ground and the bare soil where neither
// herbs nor trees cover it (Roadcraft render/plateTexels.ts ecologyCornersInto). Each chunk's corners are worked out by the same
// compute_ecology call, over the same window, as the trees and the cover read (render/trees/tree_place.cpp chunk_ground), so the
// straw on the ground and the plants over it agree. A job redoes the chunks an edit reached and their neighbours (the ecology's
// window reaches mean_cells + 1 corners past a chunk, under one chunk); the main thread only uploads, within the frame's ledger.
#pragma once

#include "render/async_slot.h"
#include "render/frame_admission.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <cstdint>
#include <vector>

namespace city {

class EcologyRaster {
public:
	// Called each frame: starts a job when chunks are waiting, takes a finished one in.
	void sync(const CityState &s, FrameAdmission &f);
	// The chunks whose landscape changed (the change log's nature chunks): they and the chunks round them are worked out again.
	void touch(const std::vector<int64_t> &chunks);
	// { revision, texture, grid } for render/ecology_view.gd; revision 0 until the first raster is in.
	godot::Dictionary view() const;
	uint64_t view_revision() const { return uploads_; }
	void reset(); // a new map: every chunk again, on a raster of its size

private:
	struct Raster {
		godot::PackedByteArray texels;
		int side = 0;
		float half = 0, step = 1;
	};
	// The raster the jobs keep between them: a job writes the chunks it was given over what the last one left.
	struct Scratch {
		std::vector<uint8_t> texels;
		int side = 0;
		float half = 0, step = 0;
	};
	void work(const CityState &s, uint64_t generation, std::vector<int64_t> chunks, bool whole);
	godot::Ref<godot::ImageTexture> texture_;
	AsyncSlot<Raster, Scratch> slot_;
	std::vector<int64_t> waiting_; // chunks to work out again, deduplicated when a job takes them
	bool whole_ = true; // every chunk of the map waits (a new map, or a raster not made yet)
	int side_ = 0;
	uint64_t uploads_ = 0;
	float half_ = 0, step_ = 1;
};

} // namespace city
