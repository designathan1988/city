// The lit lenses of the signal posts (render/props/street_props.h SignalHead): per chunk the posts its furniture mesh
// carries (dim lenses, PropKit), and over them the lens each post's approach shows now (PoseFrame::signals: the car
// light and the walkers' stop or walk), one MultiMesh per lens. The buffers are written again only when a post's
// lenses change; a post beyond the published radius stays dim.
#pragma once

#include "render/props/prop_kit.h"
#include "render/props/street_props.h"
#include "render/rs_util.h"
#include "sim/state/pose_buffer.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

class SignalLamps {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const PropKit &kit);
	void reset();
	void set_chunk(int64_t key, std::vector<SignalHead> &&heads); // the posts of a chunk whose mesh is now drawn
	void drop_chunk(int64_t key);
	void update(const PoseFrame &f);
	int lit() const { return lit_; } // lenses lit at the last update (render_cost probe)

private:
	godot::RID scenario_, material_;
	OwnedMesh lens_[int(Lens::COUNT)];
	MultiMeshDraw draw_[int(Lens::COUNT)];
	std::unordered_map<int64_t, std::vector<SignalHead>> chunks_;
	std::vector<uint8_t> shown_; // per post, in chunk order: the lenses drawn (car lens | walker lens << 4)
	bool posts_changed_ = true;
	int lit_ = 0;
	float reach_ = 0; // farthest lens from a post's foot
};

} // namespace city
