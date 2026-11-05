// Pictures of the trees for their far level: each species drawn once, on the CPU, from a few elevations of the view into
// one atlas (rows the species, columns the elevations), with its colour and its normal, from the very model the near
// level draws: its body's faces, its leaf cards as the atlas' tufts, its fronds. A far tree is then one quad showing the
// frame nearest the view, lit by the picture's normal, so a wood far away reads as crowns and not as flat cards.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

class TreePictures {
public:
	// `spec`: streets.json trees.pictures; `species`: how many rows; `leaves`: the leaf atlas' image.
	bool begin(const godot::Dictionary &spec, int species, const godot::Ref<godot::Image> &leaves, godot::PackedStringArray &errors);
	void draw(int row, const MeshBuilder &body, const MeshBuilder &cards, const godot::Color &tone);
	void finish();
	godot::RID albedo() const { return albedo_.is_valid() ? albedo_->get_rid() : godot::RID(); }
	godot::RID normal() const { return normal_.is_valid() ? normal_->get_rid() : godot::RID(); }
	int frames() const { return int(elevations_.size()); }
	int rows() const { return rows_; }
	float frame_share() const { return share_; }
	float top_elevation_deg() const { return elevations_.empty() ? 0.f : elevations_.back(); }

private:
	struct Frame; // one elevation's depth, colour and normal buffers
	void frame_body(Frame &f, const MeshBuilder &m) const;
	void frame_cards(Frame &f, const MeshBuilder &m) const;
	void store(const Frame &f, int row, int col, const godot::Color &tone);
	int px_ = 0, rows_ = 0;
	float share_ = 1;
	float shade_ = 1; // the light a crown keeps from its own shadow and its neighbours, which a picture cannot cast
	std::vector<float> elevations_;
	godot::Ref<godot::Image> leaves_;
	godot::PackedByteArray colour_, normals_;
	godot::Ref<godot::ImageTexture> albedo_, normal_;
};

} // namespace city
