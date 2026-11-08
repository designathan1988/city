// The painted fog as the atmosphere pass reads it: a window of the land round the view, a texel holding the fog's
// density, the ground under it, the bank's height and the speed it drifts (a float texture, the shader reads it repeating
// so the window can follow the view). A job works the texels out from the dabs, copied with the ground it needs while it holds
// the map for a moment; the main thread only uploads them, within the frame's ledger. A texel keeps its place modulo the
// window, so a job remakes only what changed since the last one: the strips the window entered, the ground the terrain edits
// touched and the reach of the dabs that changed (the rest is the same texel the whole window would compute again).
#pragma once

#include "render/async_slot.h"
#include "render/frame_admission.h"
#include "sim/state/city_state.h"
#include "sim/terrain/height_patch.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <unordered_map>
#include <vector>

namespace city {

constexpr int LINGER_SHARE = 8; // the view goes this share of a field's window from where it was made before it is made again

class FogField {
public:
	// Called each frame: starts a job when the dabs changed or the view left the window, takes a finished one in.
	void sync(const CityState &s, float cam_x, float cam_z, FrameAdmission &f);
	// The terrain samples a frame's edits changed (ChangeLog::terrain_rects): their texels' ground is remade by the next job.
	void touch_terrain(const CityState &s, const std::vector<int> &rects);
	// { any, texture, slab, window } for render/sky_view.gd.
	godot::Dictionary view() const;
	uint64_t view_revision() const { return view_revision_; } // moves whenever view() would say something else (RenderWorld::sync drives sync)
	void reset();

	// A rectangle of world texel indices, [i0, i1) x [j0, j1).
	struct Span {
		int i0 = 0, j0 = 0, i1 = 0, j1 = 0;
	};

private:
	struct Raster {
		godot::PackedByteArray texels;
		float low = 0, high = 0, ox = 0, oz = 0, side = 1;
		bool any = false;
	};
	// What one job leaves the next: the dabs of the revision it read and their buckets (indices into `dabs`, ascending: the order
	// they fold in), and its window's texels (each with the ground and top it gave the slab).
	struct Scratch {
		uint64_t revision = ~uint64_t(0);
		std::vector<FogDab> dabs;
		std::unordered_map<int64_t, std::vector<int>> buckets;
		bool held = false; // the texels below are a window's
		int kx = 0, kz = 0; // that window's first texel, world indices
		godot::PackedByteArray texels;
		std::vector<float> ground, top;
		std::vector<uint8_t> on;
		// The slab of each row of texels (by its row in `texels`): its lowest ground and highest top under fog, and whether any is.
		std::vector<float> row_low, row_high;
		std::vector<uint8_t> row_any;
	};
	void work(const CityState &s, float ox, float oz, uint64_t revision, std::vector<Span> terrain, uint64_t generation);
	// The dabs of the revision taken in: only those past the first changed one leave their buckets and come into them, so each
	// bucket keeps the order a whole filing gives. `changed`: the texels the changed dabs reach; false when none changed.
	static bool refile(Scratch &k, const std::vector<FogDab> &now, float texel, Span &changed);
	// Where the job paints: a window and the ground under it, and the spans of it to paint (each with its ground patch).
	struct Paint {
		float ox = 0, oz = 0, half = 0, water = 0;
		int kx = 0, kz = 0;
		std::vector<Span> spans;
		std::vector<HeightPatch> patches;
	};
	// The spans' texels painted from the dabs and the ground; false when a reset came on the way (nothing of the window is kept).
	bool paint(Scratch &k, const Paint &p, const FogDef &def, uint64_t generation) const;
	// The slab over the window into `r`: each row's (worked out again where a span wrote it), then the rows'.
	static void slab(Scratch &k, const Paint &p, bool full, float texel, Raster &r);
	godot::Ref<godot::ImageTexture> texture_;
	AsyncSlot<Raster, Scratch> slot_;
	std::vector<Span> terrain_; // the ground touched since the last job asked (world texel indices)
	uint64_t seen_ = ~uint64_t(0), asked_ = ~uint64_t(0);
	uint64_t view_revision_ = 0; // main thread only
	float ox_ = 0, oz_ = 0, side_ = 1, low_ = 0, high_ = 0;
	bool any_ = false;
};

} // namespace city
