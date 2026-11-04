// The look of the blast effects (data/render/effects.json), read once by EffectWorld::init: strict, a missing key is
// reported. Style values: how the effects look, never what a blow does to the city (that is data/catalog/blast.json).
#pragma once

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

// `count` puffs of `kind` over a spread, at a height, each of a size and a life (kind -1: the soil's dust).
// A flame source the blast leaves burning (the 4.0's `fires`): lasts for_s + min(for_cap_s, for_k * fireball radius) seconds, puffs rate_hz flames a second
// of size min(size_cap_m, size_k * radius) (no cap when 0), height_m over the ground.
struct SourceDef {
	float for_s = 0, for_k = 0, for_cap_s = 0, rate_hz = 0, size_k = 0, size_cap_m = 0, height_m = 0;
};

struct BurstDef {
	int kind = 0, count = 0;
	float height = 0, spread_m = 0, spread_r = 0, size_m = 0, size_r = 0, life = 0;
};

struct EffectParams {
	int pool = 0;
	float wind_x = 0, wind_z = 0, fireball_base = 0, fireball_per_force = 0;
	std::vector<BurstDef> bursts;
	std::vector<SourceDef> sources;
	// What each puff of a source is (flame, a chimney of dark smoke, a light smoke).
	float flame_base = 0, flame_k = 0, dying_s = 0, dying_floor = 0, puff_height = 0, chimney_height = 0, chimney_spread = 0, light_chance = 0, light_height = 0, light_spread_k = 0, light_size = 0, light_life = 0;
	float burn_fire_rate = 0, burn_building_rate = 0, flame_size = 0, flame_life = 0, smoke_size = 0, smoke_life = 0, column_chance = 0;
	int flame_kind = 0, smoke_kind = 0;
	float ring_life = 0, ring_height = 0;
	godot::Color flash_color;
	float flash_energy = 0, flash_range_r = 0, flash_range_min = 0, flash_life = 0, flash_decay = 0, flash_height = 0;
	float shake_base = 0, shake_per_r = 0, shake_decay = 0;
	int shrap_base = 0;
	float shrap_per_r = 0, shrap_speed_min = 0, shrap_speed_span = 0, shrap_lift_min = 0, shrap_lift_span = 0, shrap_size_min = 0, shrap_size_span = 0;
	int soot_base = 0, soot_max = 0, soot_kept = 0;
	float soot_per_step = 0, soot_reach_r = 0, soot_size_min = 0, soot_size_span = 0, soot_size_r = 0;
	int debris_max = 0;
	float gravity = 0, restitution = 0, friction = 0, rest_s = 0, damp_ground = 0;
	float geyser_s = 0, geyser_fade = 0, geyser_rate = 0, geyser_size = 0;
	int blood_max = 0;
	float pool_spread_s = 0, pool_size = 0, splat_m = 0, drop_m = 0;
	int paper_count = 0;
	float paper_size = 0, paper_life = 0, paper_spread = 0;
	int down_dust_count = 0;
	float down_dust_size = 0, down_dust_life = 0, down_dust_spread = 0;
	float body_life = 0, push_ms = 0, lift_ms = 0, limb_push_ms = 0, tracer_life = 0, tracer_m = 0, flash_m = 0, body_lead = 0; // bodies, tracers and the muzzle

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);

private:
	void load_look(const godot::Dictionary &t, godot::PackedStringArray &errors); // fire, ring, flash, shake, shrapnel
	void load_things(const godot::Dictionary &t, godot::PackedStringArray &errors); // soot, debris, geysers, blood, paper
};

} // namespace city
