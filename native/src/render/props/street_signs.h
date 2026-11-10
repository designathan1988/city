// The plates standing in a chunk (Roadcraft render/signLayer.ts): the rules' signs of its junctions, the signs placed on its
// footways and the name plates of the streets named, each a post and a plate of its shape in the chunk's mesh, its face
// showing its cell of the sheet (render/props/sign_atlas.h), its back bare metal.
#pragma once

#include "render/data_reader.h"
#include "render/mesh_builder.h"
#include "render/props/sign_atlas.h"
#include "render/props/street_props.h"

namespace city {

// A plate's look (data/render/streets.json "signs"): size, centre's height, points of its outline (0: a rectangle).
struct PlateLook {
	float w = 0, h = 0, z = 0, turn = 0;
	int points = 0;
};

struct SignLook {
	int sheet_w = 0, sheet_h = 0, cell = 0, blade_w = 0, blade_h = 0; // a layer of the pictures and its cells, pixels
	PlateLook plate[SIGN_TYPES];
	float post_radius = 0, face_out = 0, blade_half = 0;
	int post_sides = 0;
	godot::Color post, back;
	bool load(const DataValue &t, godot::PackedStringArray &errors);
};

void place_signs(const CityView &s, const StreetLook &L, const SignLook &K, SignAtlas &atlas, int64_t key, MeshBuilder &mb);

} // namespace city
