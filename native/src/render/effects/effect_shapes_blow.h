// The effects' shapes of a blow's events: the blast, the people struck, the things reached (effects.json shapes, read by effect_params.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define EFFECT_SHAPE_EFFECT_BLAST(X, XI, XD, XV, XVI) \
	X(ruin_match_m) X(spark_count_per_kpa) XI(hit_dust_min) X(hit_dust_per_kpa) X(hit_spread_m) X(hit_size_m) \
	X(hit_life_s) X(shell_lift_m) X(shell_out_share) X(shell_flat_min) X(shell_flat_span) X(hit_spark_max) \
	X(down_paper_height_m) X(down_smoke_size_scale) X(collapse_dust_high_m) X(soot_size_jitter) X(hit_spark_size) \
	X(hit_spark_life) X(hit_lift_m)
struct BlastShape {
	SHAPE_DECLARE(EFFECT_SHAPE_EFFECT_BLAST)
};

#define EFFECT_SHAPE_EFFECT_BODIES(X, XI, XD, XV, XVI) \
	X(skull_centre_m) X(trunk_centre_m) X(arm_centre_m) X(leg_centre_m) X(arm_side_m) X(leg_side_m) X(skull_w) \
	X(skull_h) X(skull_d) X(sleeve_w) X(sleeve_h) X(leg_w) X(leg_h) X(leg_d) X(walls_reach_m) X(topple_ms) \
	X(limb_scatter_ms) X(torn_push) X(no_push_below_m) X(least_life_s) X(tracer_reach_m) X(wound_spray) \
	X(wound_size_m) X(wound_life_s) X(wound_spread_m) X(spark_count) X(spark_spread_m) X(spark_size_m) \
	X(spark_life_s) XI(shards) X(shard_size_m) X(shard_thick_m) X(shard_speed_ms) X(shard_life_s)
struct BodiesShape {
	SHAPE_DECLARE(EFFECT_SHAPE_EFFECT_BODIES)
};

#define EFFECT_SHAPE_EFFECT_THINGS(X, XI, XD, XV, XVI) \
	X(mast_r) X(mast_h) X(post_rad) X(post_h) X(hydrant_r) X(hydrant_h) X(bench_l) X(bench_w) X(bench_h) X(bin_l) \
	X(bin_h) X(bench_slat_share) X(trunk_r) X(trunk_h) X(leaf_size) X(leaf_thick) X(trunk_top_share) \
	X(trunk_stump_share) X(trunk_half) X(crown_height_k) X(splinter_r) X(splinter_len_min) X(splinter_len_span) \
	XI(crown_leaves) XI(splinters) X(throw_speed) X(throw_lift) X(throw_scatter) X(thrown_slow) X(thrown_tree) \
	X(splinter_push_min) X(splinter_step_min) X(splinter_step_span) X(splinter_base_m) X(spark_puffs) \
	X(spark_spread) X(spark_size) X(spark_life) X(wreck_l) X(wreck_h) X(wreck_w) X(car_burn_s) X(car_throw_speed) \
	X(wreck_lift) X(moped_burn_s) X(trunk_burn_s) X(roll_r) X(roll_w) X(fire_puffs_car) X(fire_puffs_bike) \
	X(fire_spread_car) X(fire_spread_bike) X(fire_size) X(fire_life) X(blood_puffs) X(blood_spread_m) \
	X(blood_size_m) X(blood_life_s) X(organ_min_m) X(organ_span_m) X(organ_speed) X(organ_flat_min) X(organ_thin) \
	X(organ_thin_span) X(organ_round) XI(organs_min) XI(organs_span) X(chest_rise_m) X(scatter_blood_m) \
	X(pool_min) X(pool_span) X(pool_torn) XI(torn_pools) X(drip_scatter_m) X(drip_min_m) X(drip_span_m) X(snap_min) X(snap_span) \
	X(arm_l_m) X(arm_t_m) X(lamp_l_m) X(lamp_h_m) X(lamp_w_m) X(lamp_reach_m) X(fall_far_share) X(fall_least) X(cut_min) X(cut_span) \
	X(crown_share) X(shrub_h) XI(bits) X(bit_l_min) X(bit_l_span) X(bit_h_min) X(bit_h_span) X(bit_w_min) X(bit_w_span) X(bit_lift_m)
struct ThingsShape {
	SHAPE_DECLARE(EFFECT_SHAPE_EFFECT_THINGS)
};

// The broken ground of a crater (effect_blast.cpp crater_slabs, render/blast.ts crater of the reference game): the slab's jagged
// outline (debris.cpp), the rim of slabs heaved and tilted round the hole and the plates of the surface tipped up round it, laid
// for good, each by a share of the crater's decal radius (marks.h CRATER_SCALE times the hole's).
#define EFFECT_SHAPE_CRATER_SLABS(X, XI, XD, XV, XVI) \
	XI(outline_corners) X(outline_r) X(outline_wave) X(outline_wave_k) X(outline_wave_phase) XI(outline_notch_every) \
	X(outline_notch) X(outline_bulge) X(bevel_thick) X(bevel_size) X(rim_base) X(rim_per_m) X(rim_jitter_rad) X(rim_r_min) \
	X(rim_r_span) X(rim_size_min_m) X(rim_size_span_m) X(rim_earth_scale) X(rim_thick_hard_m) X(rim_thick_earth_m) \
	X(rim_depth_min) X(rim_depth_span) X(rim_tilt_min) X(rim_tilt_span) X(rim_out_ms) X(rim_up_ms) X(rim_lift_m) \
	XI(rim_concrete_every) X(plates_base) X(plates_per_m) XI(plates_most) XI(plates_crowd) X(plates_crowd_r) X(plate_r_min) \
	X(plate_r_span) X(plate_w_min_m) X(plate_w_span_m) X(plate_scale_min) X(plate_scale_per_m) X(plate_scale_most) \
	X(plate_d_min) X(plate_d_span) X(plate_thick_hard_m) X(plate_thick_earth_m) X(plate_tilt_min) X(plate_tilt_span) \
	X(plate_lift_share) X(plate_concrete_chance) XV(dirt_tint) X(dirt_min) X(dirt_span) X(shade_min) X(shade_span) \
	XI(clod_every) X(clod_min_m) X(clod_span_m) X(clod_h_m) X(clod_lift_m)
struct CraterSlabsShape {
	SHAPE_DECLARE(EFFECT_SHAPE_CRATER_SLABS)
};

// The guts and bones of an opened body (gore.cpp; the reference game's render/gore.ts and ragdoll.ts openBelly, boneOut).
#define EFFECT_SHAPE_GORE(X, XI, XD, XV, XVI) \
	XI(links) X(seg_min_m) X(seg_span_m) X(radius_min_m) X(radius_span_m) X(hold_chance) X(hold_min_s) X(hold_span_s) \
	X(coil_m) X(coil_turn) X(coil_drop) X(damp) XI(iterations) X(ground_keep) X(still_m) X(still_s) X(step_s) X(step_most_s) \
	XI(most_segments) XI(most_bones) X(bone_r_m) X(cuff_r_m) X(cuff_len_m) X(bone_len_m) X(bone_head_len_m) X(bone_chance) \
	X(pelvis_chance) X(pelvis_kick) X(shot_kick_ms) X(torn_kick_ms) X(torn_open_chance) X(dead_open_chance) XV(gut_tints) \
	XV(bone_tint) XV(cuff_tint)
struct GoreShape {
	SHAPE_DECLARE(EFFECT_SHAPE_GORE)
};

// The wires torn off felled poles (wires.cpp; the reference game's blast.ts wire and stepRope): a rope of Verlet points pinned at
// the pole that stands, its loose end thrown, live on the ground a while.
#define EFFECT_SHAPE_WIRES(X, XI, XD, XV, XVI) \
	XI(points) XI(most) X(damp) XI(iterations) X(slack) X(ground_lift_m) X(ground_keep) X(sparks_min_s) X(sparks_span_s) \
	X(spark_hz) X(spark_near_m) XI(spark_count) X(spark_spread_m) X(spark_size_m) X(spark_life_s) X(life_s) X(thick_m) \
	X(step_s) XV(tint) X(arm_drop_m) XV(primary_m) X(fall_share) X(kick_ms) X(far_share) X(least)
struct WiresShape {
	SHAPE_DECLARE(EFFECT_SHAPE_WIRES)
};

} // namespace city
