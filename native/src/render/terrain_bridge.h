// The land as the terrain's shader reads it (project/render/shaders/terrain.gdshader). Terrain3D draws the clipmap's meshes and
// keeps no region of its own: in its regions the whole map stood on the CPU and again on the GPU whatever the view showed (256 x 256
// samples of height, painted ground and colour for every square kilometre: 576 regions for the 24 km land, some 430 MiB of each),
// and a region added or removed made its texture arrays again from all that stood. Here the land is two pairs of map stacks that
// live on the GPU alone, each layer written by itself:
// - the fine ground, the heights and the painted ground at every sample, for a window of regions round the view (follow): a region
//   that enters the window takes the layer of the one that left it (a region's layer is its place modulo the window's side), the
//   nearest first, within the frame's upload room;
// - the far ground, the whole map every `far_cells` samples, in tiles: what the shader reads wherever no fine region stands, on
//   the same clipmap, so the land runs whole from near to far (the clipmap's far levels sample the land that sparsely themselves).
// The authoritative heights are sim/terrain's; an edit writes the fine regions that stand and the far tiles it touched.
#pragma once

#include "sim/nature/ground_paint.h"
#include "sim/terrain/heightfield.h"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/texture2d_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <cstdint>
#include <vector>

namespace city {

class TerrainBridge {
public:
	// A new land (a map made or read): its maps are made at the next follow.
	void new_land(const Heightfield &hf, godot::Object *terrain3d);
	// The fine ground round (x, z): the window of regions moves as little as holds everything within `reach_m`, and the regions it
	// lacks are written, the nearest first, while `room` bytes last (one at least: the far ground shows the others meanwhile). A
	// new land's maps are made first: the far ground whole, a sample every `far_cells`, and the whole window. Returns the bytes sent
	// (the upload ledger's).
	int64_t follow(const Heightfield &hf, const GroundPaint &paint, godot::Object *terrain3d, float x, float z, float reach_m, int far_cells, int64_t room);
	// rects: x0,z0,x1,z1 sample rectangles from ChangeLog::terrain_rects: the heights under them. Returns the bytes sent.
	int64_t push_rects(const Heightfield &hf, const std::vector<int> &rects);
	// The painted ground under the rectangles, as control words (layout: ground_paint.gdshaderinc). Returns the bytes sent.
	int64_t push_paint(const Heightfield &hf, const GroundPaint &paint, const std::vector<int> &rects);
	int pending() const { return pending_; } // the window's regions not yet written: the ground there is still the far one
	// Tests: the height the shader reads at a world point, taken back from the GPU: "height", and "fine" (a fine region's sample).
	godot::Dictionary drawn(const Heightfield &hf, float x, float z) const;

private:
	// A stack of square maps of 32-bit samples that lives on the GPU alone, a layer written at a time.
	struct MapStack {
		godot::Ref<godot::Texture2DArray> maps;
		int side = 0, layers = 0;
		void make(int p_side, int p_layers);
		int64_t write(int layer, const godot::PackedByteArray &samples);
		float read(int layer, int x, int y) const; // tests: a sample back from the GPU, as a float
	};
	void make(const Heightfield &hf, godot::Object *terrain3d, float reach_m, int far_cells);
	int layer_of(int bx, int bz) const { return (bz % window_) * window_ + bx % window_; }
	bool held(const Heightfield &hf, int bx, int bz) const { return window_ > 0 && held_[size_t(layer_of(bx, bz))] == bz * hf.regions + bx; }
	void tell_held(godot::Object *terrain3d) const; // the shader's fine_held: which region each fine layer holds
	template <typename Layer>
	int64_t rewrite(const Heightfield &hf, const std::vector<int> &rects, MapStack &fine, MapStack &far, Layer &&layer);
	MapStack fine_heights_, fine_controls_, far_heights_, far_controls_;
	std::vector<int> held_; // by fine layer: the region it holds (bz x regions + bx), NO_REGION while none
	int window_ = 0; // the fine window's side, regions
	int from_x_ = 0, from_z_ = 0; // its first region
	int far_cells_ = 0, far_tiles_ = 0; // the land's samples a far one; the far maps' tiles a side
	int pending_ = 0;
	bool made_ = false;
};

} // namespace city
