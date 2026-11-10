// Low-poly trees, one unit tall (an instance's scale is its height in metres), built once per species from
// data/render/streets.json "trees": a faceted body (tapered trunk, limbs and a shaded crown heart, each face one tone
// between a dark and a lit one by how much it faces the sky) under leaf cards that face the camera, palm fronds as
// strips, and one far card for every species. The vertex layout below is the contract with
// render/shaders/vegetation.gdshaderinc:
//   UV2.x  part (TreePart): body, leaf card or frond strip (far cards are render/trees/far_cards.h's own mesh);
//   UV2.y  a card's half size, in model units (0 for the rest);
//   UV     a card's corner (0..1) inside its region of the leaf atlas, or a frond's atlas coordinate;
//   COLOR  rgb the sRGB tone, a the card's roll as a share of a turn (1 for the rest).
#pragma once

#include "render/mesh_builder.h"
#include "render/trees/leaf_atlas.h"
#include "render/trees/tree_part.h"
#include "render/trees/tree_pictures.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

// A far card is centred on the middle of its tree; how far it reaches is the pictures' frame (tree_pictures.h).
constexpr float FAR_CARD_CENTRE = 0.5f;

struct TreeSpecies {
	godot::String name;
	// Each made whole with the material it is drawn in (surface_data.h): the body and its caster in the body's, the cards in the cards'.
	OwnedMesh body, cards; // no cards mesh when the species has none
	OwnedMesh caster; // the body simplified: what it throws on the ground, swaying as it does
	float crown_reach = 0; // crown radius as a share of the height (keeps trees off the roads)
	bool cards_cast = false; // a palm's fronds are its crown and throw its shadow; other cards do not
	godot::Color far_tone; // the crown's mean tone (sRGB), for its far card
};

class TreeKit {
public:
	// `body_material`, `card_material`: what the species' meshes are made with.
	bool build(const godot::Dictionary &trees, const godot::RID &body_material, const godot::RID &card_material, godot::PackedStringArray &errors);
	int count() const { return int(species_.size()); }
	const TreeSpecies &species(int i) const { return species_[size_t(i)]; }
	int find(const godot::String &name) const; // -1 when absent
	const LeafAtlas &atlas() const { return atlas_; }
	const TreePictures &pictures() const { return pictures_; }

private:
	std::vector<TreeSpecies> species_;
	LeafAtlas atlas_;
	TreePictures pictures_;
};

} // namespace city
