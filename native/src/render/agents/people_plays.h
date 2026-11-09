// The figures' plays as the people shaders read them (people_pose.gdshaderinc): a float texture of PEOPLE_PLAYS texels a row, a row a figure
// drawn this frame (a walker near, a seated person; what it holds and its helmet read its row), each texel (clip, phase, weight, 0), the
// heaviest first and the rest of weight zero. Refilled every frame and sent whole; it grows by doubling when a frame draws more figures
// (the lab's citizenRow: a figure's own row of its palette pass).
#pragma once

#include "render/agents/people_gait.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <vector>

namespace city {

class FigurePlays {
public:
	void begin() { used_ = 0; }
	// A figure's row: its plays written; the row's number for its instances.
	int put(const Play *plays, int n);
	// Sends the frame's rows; when the texture had to grow, every material that poses a person is pointed at the new one.
	void upload(const godot::RID *materials, int count);
	void reset() { texture_.unref(); }
	godot::RID rid() const { return texture_.is_valid() ? texture_->get_rid() : godot::RID(); }

private:
	godot::PackedByteArray texels_; // rows * PEOPLE_PLAYS texels of four floats
	int rows_ = 0, used_ = 0;
	godot::Ref<godot::ImageTexture> texture_;
};

} // namespace city
