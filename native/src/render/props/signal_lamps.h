// The lenses of the signal posts (render/props/street_props.h SignalHead), each drawn once: per chunk one MultiMesh of
// its posts, every instance a post's lenses (PropKit::lenses) with the lenses it shows lit in INSTANCE_CUSTOM (the car
// light and the walkers' stop or walk its approach publishes, PoseFrame::signals); render/shaders/props.gdshader (the
// "prop" material) draws those lit and every other lens unlit. A chunk's buffer is written again only when one of its posts changes, the
// whole of it (a few posts): writing single instances would first read the MultiMesh back from the GPU (Godot
// renderer_rd/storage_rd/mesh_storage.cpp _multimesh_make_local). A post beyond the published radius shows every lens
// unlit.
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
	// The posts of a chunk whose mesh is now drawn: their lenses drawn at once, in the states last published.
	void set_chunk(int64_t key, std::vector<SignalHead> &&heads);
	void drop_chunk(int64_t key);
	void update(const PoseFrame &f);
	int lit() const { return lit_; } // lenses lit in the buffers sent (render_cost probe)

private:
	struct Chunk {
		std::vector<SignalHead> heads;
		std::vector<uint8_t> shown; // per post: the lenses its instance shows lit (car lens | walker lens << 4)
		godot::AABB box; // the posts' feet, grown by the lenses' reach from them
		MultiMeshDraw draw;
		int lit = 0;
	};
	uint8_t showing(const SignalHead &h) const;
	void write(Chunk &c) const;
	godot::RID scenario_, material_;
	OwnedMesh lenses_;
	float reach_ = 0; // farthest lens from a post's foot
	std::unordered_map<int64_t, Chunk> chunks_;
	std::vector<SignalRecord> signals_; // the last published lights, by ds
	int lit_ = 0;
	uint64_t publication_ = 0;
	bool has_publication_ = false;
};

} // namespace city
