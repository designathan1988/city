// What hangs on a facade and stands on a balcony, from data/render/bay_props.json: one model of each kind, built once and drawn as
// instances, and what they are painted and how often each comes (facade_props.cpp places them). Model axes: x along the wall, y up,
// z out of it.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace city::b40 {

// The first BAY_PROP_SHADOWED cast shadows: the rest are too thin or too low for the sun's texels.
enum BayProp : uint8_t { BAY_PROP_AC, BAY_PROP_POT, BAY_PROP_FOLIAGE, BAY_PROP_BUSH, BAY_PROP_CHAIR, BAY_PROP_TABLE, BAY_PROP_AIRER, BAY_PROP_DISH,
	BAY_PROP_LOUNGER, BAY_PROP_BIKE, BAY_PROPS };
constexpr int BAY_PROP_SHADOWED = BAY_PROP_CHAIR;

// bay_props.json "shares": the chance of each thing, or the top of its slice of one draw.
#define BAY_PROP_SHARES(X) \
	X(split_home) X(split_shop) X(split_works) X(dish) X(ac_old) X(old_popular) X(office_plant) X(bare) X(one_thing) X(two_things) X(its_kind) \
	X(one_chair) X(one_plant) X(one_airer) X(set_table) X(lounger_high) X(kind_airer) X(kind_bike) X(four_pots) X(loggia_plant) X(loggia_chair) \
	X(loggia_lounger)

struct BayPropShares {
#define X(name) float name = 0;
	BAY_PROP_SHARES(X)
#undef X
};

struct BayPropLook {
	std::array<MeshBuilder, BAY_PROPS> models;
	std::vector<uint32_t> ac_cases, pots, greens, plastic, fine, loungers, bikes;
	uint32_t ac_old = 0, dish = 0, airer = 0;
	BayPropShares share;

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

} // namespace city::b40
