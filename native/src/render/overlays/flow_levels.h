// The congestion map's levels (overlay_renderer.h): a texture of a texel a road segment id, the segment's mean speed over its
// limit laid on the ramp between jam_share and free_share (overlays.json), which the shader of the carriageways reads (render/
// shaders/overlay_flow.gdshader). The levels are kept as last sent: a publication of the traffic's speeds sends the texture only
// when a level moved, into the texture it has, and a new one is made only when the segments outgrow it.
#pragma once

#include "sim/state/pose_buffer.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <cstdint>
#include <vector>

namespace city {

class FlowLevels {
public:
	// `material`: the one the carriageways are drawn with, told the texture and its row width; the ramp's ends as speed shares.
	void init(const godot::RID &material, float free_share, float jam_share);
	void reset(); // another city: every street free until its traffic says
	void fit(int slots); // a texel for every segment id under `slots` (before a mesh that carries them is drawn)
	// This publication's speeds made levels: the bytes sent to the GPU, 0 when no level moved.
	int64_t refresh(const PoseFrame &f);
	uint8_t level_of(float share) const; // a speed share on the ramp: 0 jammed .. the top free
	// Tests: the refreshes run and the sends made, the segments of the last publication, the levels as sent with their texture, and
	// the ramp's level of five speeds (stopped, jam_share, half way, free_share, free).
	void probe(godot::Dictionary &d) const;

private:
	void send();
	godot::RID material_;
	godot::Ref<godot::ImageTexture> texture_;
	std::vector<uint8_t> level_; // as last sent: a row of the texture after the other
	int rows_ = 0, segments_ = 0;
	int64_t refreshes_ = 0, sends_ = 0;
	float free_share_ = 1, jam_share_ = 0;
};

} // namespace city
