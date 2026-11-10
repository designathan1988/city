// The land in a Terrain3D node, as far as the view needs it. Terrain3D keeps its ground in regions (256 x 256 samples of height,
// painted ground and colour, on the CPU and again on the GPU) and the map is hundreds of them: made all at once, a 640 m town held
// the 400 regions of its 20 km land (some 300 MiB of images and as much video memory). The fine ground follows the view instead
// (follow: the regions within reach of the eye, added and removed as it moves), and the whole map stands under it as two small
// maps, its heights and its painted ground every `far_cells` samples, which the terrain's shader reads wherever no region is
// (terrain_common.gdshaderinc ground_height, ground_control): the same clipmap draws both, so there is no seam between them.
// The authoritative heights are sim/terrain's; edits reach the regions that stand and the far maps.
#pragma once

#include "sim/nature/ground_paint.h"
#include "sim/terrain/heightfield.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace city {

class TerrainBridge {
public:
	// A new land: no region stands, the far maps are made again at the next follow.
	void push_all(const Heightfield &hf, godot::Object *terrain3d);
	// The fine ground round (x, z): every region within `reach_m` stands, one that left it by a region's side goes. The far maps are
	// made when the land is new, a sample every `far_cells`. Returns the bytes of the maps sent (the upload ledger's).
	int64_t follow(const Heightfield &hf, const GroundPaint &paint, godot::Object *terrain3d, float x, float z, float reach_m, int far_cells);
	// rects: x0,z0,x1,z1 sample rectangles from ChangeLog::terrain_rects. Returns the bytes of the maps sent (the upload ledger's).
	int64_t push_rects(const Heightfield &hf, godot::Object *terrain3d, const std::vector<int> &rects);
	// The painted ground under the rectangles, as Terrain3D's control map (layout: ground_paint.gdshaderinc). Returns the bytes sent.
	int64_t push_paint(const Heightfield &hf, const GroundPaint &paint, godot::Object *terrain3d, const std::vector<int> &rects);
	// The whole painted ground again (a loaded map): the standing regions' and, at the next follow, the far map's.
	void push_all_paint(const Heightfield &hf, const GroundPaint &paint, godot::Object *terrain3d);
	int regions_standing() const { return int(live_.size()); } // for the probes

private:
	void write_region(const Heightfield &hf, godot::Object *data, int bx, int bz, bool create);
	void write_control(const Heightfield &hf, const GroundPaint &paint, godot::Object *data, int bx, int bz);
	void make_far(const Heightfield &hf, const GroundPaint &paint, godot::Object *terrain3d, int far_cells);
	void far_heights_in(const Heightfield &hf, int x0, int z0, int x1, int z1);
	void far_controls_in(const GroundPaint &paint, int x0, int z0, int x1, int z1);
	int64_t send_far(godot::Ref<godot::ImageTexture> &to, const godot::PackedByteArray &bytes);
	std::unordered_set<int64_t> live_; // the regions that stand, by cell_key(bx, bz)
	godot::PackedByteArray far_h_, far_c_; // the far maps' samples: heights (float) and painted ground (control words)
	godot::Ref<godot::ImageTexture> far_heights_, far_controls_;
	int far_side_ = 0, far_cells_ = 0; // their side in samples, and the land's samples a far one; 0: to make
};

} // namespace city
