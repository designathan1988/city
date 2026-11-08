// The look of the blast effects (data/render/effects.json: the blast itself and the tints of what it throws; data/render/aftermath.json:
// what it leaves on the street; data/render/gore.json: what a body opened or cut shows), read once by EffectWorld::init: strict, a missing key is reported. Style values: how the effects
// look, never what a blow does to the city (that is data/catalog/blast.json).
#pragma once

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <vector>

namespace city {

// A flame source the blast leaves burning (the 4.0's `fires`): lasts for_s + min(for_cap_s, for_k * fireball radius) seconds, puffs rate_hz flames a second
// of size min(size_cap_m, size_k * radius) (no cap when 0), height_m over the ground.
struct SourceDef {
	float for_s = 0, for_k = 0, for_cap_s = 0, rate_hz = 0, size_k = 0, size_cap_m = 0, height_m = 0;
};

// `count` puffs of `kind` over a spread, at a height, each of a size and a life (kind -1: the soil's dust).
struct BurstDef {
	int kind = 0, count = 0;
	float height = 0, spread_m = 0, spread_r = 0, size_m = 0, size_r = 0, life = 0;
};

// The colours (sRGB) of what a blow throws and leaves, by what it is (effects.json "tints").
struct Tints {
	godot::Color asphalt, concrete, earth, brick, glass, wood, metal, charred, leaf, paint, dirt;
};

// The ground a crater heaved (aftermath.json "heave", render/effects/heave.h). Distances out from the crater's centre are shares of
// its reach (the crater quad's half, Marks::CRATER_SCALE times its hole). Plates: plates_base + reach / plates_per_m of them, at most
// plates_most, none where `crowded` plates of earlier craters already lie within crowd_r reaches; each out at plate_in + plate_span x
// sqrt(u), plate_w_min + plate_w_span x u wide times min(scale_most, scale_base + reach / scale_grow_m), plate_depth_* of its width deep,
// plate_thick_* thick, tipped tilt_min + tilt_span x u about the way round and turned at random, its middle plate_lift of its width
// over the ground; dirt_* of the dirt over its tint and shade_* of light. Slabs at the lip: slabs_base + reach x slabs_per_m, out at
// slab_in + slab_span x u (slab_jitter rad off their even turn), slab_min_m + slab_span_m x u across (slab_earth_scale on earth),
// slab_thick_* thick, slab_depth_* of their length deep, tipped slab_tilt_*, sunk slab_embed of their height, one in
// slab_concrete_every of a paved lip concrete (the kerb's). A clod every clod_every plates. outline_r: the plate's jagged edge, a radius
// a corner, its faces bevelled bevel_m. Its colours are the tints'.
struct HeaveLook {
	int plates_base = 0, plates_most = 0, crowded = 0, slabs_base = 0, clod_every = 0, slab_concrete_every = 0;
	float plates_per_m = 0, crowd_r = 0, plate_in = 0, plate_span = 0, plate_w_min = 0, plate_w_span = 0, scale_base = 0, scale_grow_m = 0, scale_most = 0;
	float plate_depth_min = 0, plate_depth_span = 0, plate_thick_hard = 0, plate_thick_earth = 0, plate_lift = 0, tilt_min = 0, tilt_span = 0;
	float dirt_min = 0, dirt_span = 0, shade_min = 0, shade_span = 0, concrete_share = 0;
	float slabs_per_m = 0, slab_in = 0, slab_span = 0, slab_jitter = 0, slab_min_m = 0, slab_span_m = 0, slab_earth_scale = 0, slab_thick_hard = 0, slab_thick_earth = 0;
	float slab_depth_min = 0, slab_depth_span = 0, slab_tilt_min = 0, slab_tilt_span = 0, slab_embed = 0;
	float clod_min_m = 0, clod_span_m = 0, clod_h_m = 0, bevel_m = 0;
	std::vector<float> outline_r;
	godot::Color asphalt, concrete, earth, dirt; // sRGB
};

// A wire torn loose (aftermath.json "rope", render/effects/ropes.h): `points` points a wire, at most `most` kept; its links `slack` of
// the length they had; `damp` of the speed kept a step, `passes` passes of the lengths a step; on the ground `lift_m` over it and
// `friction` of its speed kept. Its loose end, while within crackle_near_m of the ground and live (crackle_min_s + crackle_span_s x u),
// crackles at crackle_hz (crackle_count spark puffs over crackle_spread_m, crackle_size_m, crackle_life_s), and at flash_chance of them flashes the arc's
// colour (flash_energy, flash_s). Its loose end thrown kick_m a second out from the burst and kick_lift of that up; the fallen end at
// drop_share of the seat height; still after settle_s.
struct RopeLook {
	int points = 0, most = 0, passes = 0, crackle_count = 0;
	float slack = 0, damp = 0, lift_m = 0, friction = 0, crackle_min_s = 0, crackle_span_s = 0, crackle_hz = 0, crackle_near_m = 0;
	float crackle_spread_m = 0, crackle_size_m = 0, crackle_life_s = 0, flash_chance = 0, flash_energy = 0, flash_s = 0;
	float kick_m = 0, kick_lift = 0, drop_share = 0, settle_s = 0;
};

// A pole knocked out (aftermath.json "pole"): its top crown_m over its wires; whole it falls turning fall_spin_min + fall_spin_span x u rad/s
// about the way across its throw, snapped at cut_min + cut_span x u of its height its top turns snap_spin_*; its arm (arm_section_m square)
// reaches arm_over_m past the last seat, its lamp's head (lamp_head_m) lamp_reach_share out along the arm; a signal post's head post_head_m.
struct PoleLook {
	float crown_m = 0, fall_spin_min = 0, fall_spin_span = 0, snap_spin_min = 0, snap_spin_span = 0, cut_min = 0, cut_span = 0;
	float arm_section_m = 0, arm_over_m = 0, lamp_reach_share = 0;
	godot::Vector3 lamp_head_m, post_head_m;
};

// An electric arc (aftermath.json "arc"): every gap_min_s + gap_span_s x u, `sparks` spark puffs over spread_m (size_m, life_s) and a
// flash of `color` (energy, range_m, flash_s). A knocked pole arcs pole_arc_min_s + pole_arc_span_s x u seconds, a signal post
// signal_arc_min_s + signal_arc_span_s x u.
struct ArcLook {
	int sparks = 0;
	float gap_min_s = 0, gap_span_s = 0, spread_m = 0, size_m = 0, life_s = 0, energy = 0, range_m = 0, flash_s = 0;
	float pole_arc_min_s = 0, pole_arc_span_s = 0, signal_arc_min_s = 0, signal_arc_span_s = 0;
	godot::Color color;
};

// What a body opened or torn shows (gore.json "gore"). Guts: chains of
// `links` links, link_min_m + link_span_m x u each, coiled coil_m round as they leave (coil_turn rad a link) and drop_share of a link
// lower each, gut_r_min_m + gut_r_span_m x u thick, a tint of gut_tints; held to the wound, or at hold_chance torn loose after hold_min_s +
// hold_span_s x u; `damp` of their speed kept a step, `passes` passes of their lengths, `friction` of it on the ground, still under
// still_mm a step for still_s. Organs: organ_min_m + organ_span_m x u across, organ_long_span longer, organ_flat_min + organ_flat_span x u
// as thick, thrown organ_scatter_ms across and organ_lift_min_ms + organ_lift_span_ms x u up, a tint of organ_tints. A bone out of a
// stump: bone_r_m round in bone_tint, bone_len_m long (head_bone_len_m from a neck), the cuff of flesh round it cuff_r_m round and
// cuff_len_m long in cuff_tint; on a piece piece_bone_len_m (head_piece_bone_len_m); it shows on the body at stump_bone_chance, on the
// piece for a draw between piece_bone_from and piece_bone_to. A blast opens a torn body at open_torn_chance, a dead one at
// open_dead_chance, opened_organs_min + opened_organs_span x u out of it (torn_organs_* out of one torn apart), a second chain from the pelvis at pelvis_chance with pelvis_kick_share of
// the kick, belly_kick_ms. A piece cut off flies at piece_speed_min_ms + piece_speed_power_ms x max(power_least, power), times
// speed_jitter_min + speed_jitter_span x u, piece_spread_ms across, piece_lift_min_ms + piece_lift_span_ms x u up times lift_base + power;
// shot off it drops near: shot_speed_min_ms + shot_speed_span_ms x u, shot_spread_ms, shot_lift_min_ms + shot_lift_span_ms x u.
struct GoreLook {
	int links = 0, passes = 0, opened_organs_min = 0, opened_organs_span = 0, torn_organs_min = 0, torn_organs_span = 0;
	float link_min_m = 0, link_span_m = 0, coil_m = 0, coil_turn = 0, drop_share = 0, gut_r_min_m = 0, gut_r_span_m = 0;
	float hold_chance = 0, hold_min_s = 0, hold_span_s = 0, damp = 0, friction = 0, still_mm = 0, still_s = 0;
	float organ_min_m = 0, organ_span_m = 0, organ_long_span = 0, organ_flat_min = 0, organ_flat_span = 0, organ_scatter_ms = 0, organ_lift_min_ms = 0, organ_lift_span_ms = 0;
	float bone_r_m = 0, bone_len_m = 0, head_bone_len_m = 0, piece_bone_len_m = 0, head_piece_bone_len_m = 0, cuff_r_m = 0, cuff_len_m = 0;
	float stump_bone_chance = 0, piece_bone_from = 0, piece_bone_to = 0, open_torn_chance = 0, open_dead_chance = 0, pelvis_chance = 0, pelvis_kick_share = 0, belly_kick_ms = 0;
	float piece_speed_min_ms = 0, piece_speed_power_ms = 0, power_least = 0, speed_jitter_min = 0, speed_jitter_span = 0, piece_spread_ms = 0, piece_lift_min_ms = 0;
	float piece_lift_span_ms = 0, lift_base = 0, shot_speed_min_ms = 0, shot_speed_span_ms = 0, shot_spread_ms = 0, shot_lift_min_ms = 0, shot_lift_span_ms = 0;
	godot::Color bone_tint, cuff_tint; // sRGB
	std::vector<godot::Color> gut_tints, organ_tints;
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
	float ring_life = 0, ring_height = 0, ring_reach_r = 0; // the ring races out to ring_reach_r fireball radii
	Tints tints;
	HeaveLook heave;
	RopeLook rope;
	ArcLook arc;
	float leak_rate = 0, leak_size = 0, leak_life = 0; // a burst pipe's spurt: puffs a second, their size and life
	PoleLook pole;
	// A tree brought down (aftermath.json "tree"): its stump stump_min + stump_span x u of its height, charred; its trunk trunk_r_share of
	// its height round, thrown throw_share of the push and turning spin_min + spin_span x u rad/s across its way, burning burn_min_s +
	// burn_span_s x u; `chips` at the break (chip_r_m round, chip_len_min_m + chip_len_span_m x u long); its crown
	// leaves_per_m clumps a metre of its width, leaf_min_m + leaf_span_m x u across and leaf_thick_m thick, out of crown_at of its
	// height, scattered leaf_scatter_ms. A street tree, whose size the street does not say, is street_height_m by street_width_m.
	struct TreeLook {
		float stump_min = 0, stump_span = 0, trunk_r_share = 0, throw_share = 0, spin_min = 0, spin_span = 0, burn_min_s = 0, burn_span_s = 0;
		float chip_r_m = 0, chip_len_min_m = 0, chip_len_span_m = 0, leaves_per_m = 0, leaf_min_m = 0, leaf_span_m = 0;
		float leaf_thick_m = 0, crown_at = 0, leaf_scatter_ms = 0, street_height_m = 0, street_width_m = 0;
		int chips = 0;
	} tree;
	GoreLook gore;
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
	float body_life = 0, push_ms = 0, lift_ms = 0, tracer_life = 0, tracer_m = 0, flash_m = 0, body_lead = 0; // bodies, tracers and the muzzle

	bool load(const godot::Dictionary &t, const godot::Dictionary &aftermath, const godot::Dictionary &gore_table, godot::PackedStringArray &errors);

private:
	void load_look(const godot::Dictionary &t, godot::PackedStringArray &errors); // fire, ring, flash, shake, shrapnel
	void load_things(const godot::Dictionary &t, godot::PackedStringArray &errors); // soot, debris, geysers, blood, paper
	void load_heave(const godot::Dictionary &t, godot::PackedStringArray &errors); // aftermath.json heave, in the tints
	void load_wires(const godot::Dictionary &t, godot::PackedStringArray &errors); // aftermath.json rope, arc, leak, pole
	void load_gore(const godot::Dictionary &t, godot::PackedStringArray &errors); // gore.json
};

} // namespace city
