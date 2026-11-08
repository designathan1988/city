// The foliage texture, made once from data/render/streets.json "trees.atlas": three square regions side by side, grey so
// each card takes its own tone from the vertex colour. Left, a leaf cluster for a card seen from afar and beside it the same with more, smaller leaves for one up close (leaves as small lenses, denser at the heart,
// each lighter on one half and darker at its rim, so it reads as a leaf and not as a smudge); right, a palm frond (a
// rachis with leaflets swept towards its tip). Alpha is the leaf's cover; mipmaps are generated from it.
#pragma once

#include "core/shader_contracts.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

// Each region's place in the atlas along u (the shader maps a card's 0..1 corner into its region): ATLAS_REGION_W, LEAF_REGION_U and
// FROND_REGION_U, core/shader_contracts.h's.

class LeafAtlas {
public:
	bool build(const godot::Dictionary &atlas, godot::PackedStringArray &errors);
	godot::RID texture() const { return texture_.is_valid() ? texture_->get_rid() : godot::RID(); }
	// The picture the texture was made from, handed over once (the tree pictures paint from it): the CPU copy that already
	// exists, never read back from the GPU.
	godot::Ref<godot::Image> take_image() {
		godot::Ref<godot::Image> out = image_;
		image_.unref();
		return out;
	}

private:
	godot::Ref<godot::ImageTexture> texture_;
	godot::Ref<godot::Image> image_;
};

} // namespace city
