// Low-poly trees, one unit tall (an instance's scale is its height in metres), built once per species from
// data/render/streets.json "trees": a faceted body (tapered trunk, limbs and a shaded crown heart, each face one tone
// between a dark and a lit one by how much it faces the sky) under leaf cards that face the camera, palm fronds as
// strips, and one far card for every species. The vertex layout below is the contract with
// render/shaders/vegetation.gdshaderinc:
//   UV2.x  part (TreePart): body, leaf card, frond strip or far card;
//   UV2.y  a card's half size, in model units (0 for the rest);
//   UV     a card's corner (0..1) inside its region of the leaf atlas, or a frond's atlas coordinate;
//   COLOR  rgb the sRGB tone, a the card's roll as a share of a turn (1 for the rest).
#pragma once

#include "render/mesh_builder.h"
#include "render/trees/leaf_atlas.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

enum class TreePart : int { BODY = 0, CARD = 1, FROND = 2, FAR = 3 };

struct TreeSpecies {
	godot::String name;
	godot::Ref<godot::ArrayMesh> body, cards; // cards empty when the species has none
	float crown_reach = 0; // crown radius as a share of the height (keeps trees off the roads)
	float sway = 0, flutter = 0; // wind: top displacement and leaf flutter as shares of the height
	bool cards_cast = false; // a palm's fronds are its crown and throw its shadow; other cards do not
	godot::Color far_tone; // the crown's mean tone (sRGB), for its far card
	float far_width = 1; // the far card's width over the tree's height
};

class TreeKit {
public:
	bool build(const godot::Dictionary &trees, godot::PackedStringArray &errors);
	int count() const { return int(species_.size()); }
	const TreeSpecies &species(int i) const { return species_[size_t(i)]; }
	int find(const godot::String &name) const; // -1 when absent
	const godot::Ref<godot::ArrayMesh> &far_card() const { return far_card_; }
	const LeafAtlas &atlas() const { return atlas_; }

private:
	std::vector<TreeSpecies> species_;
	godot::Ref<godot::ArrayMesh> far_card_;
	LeafAtlas atlas_;
};

} // namespace city
