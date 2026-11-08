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
	X(pool_min) X(pool_span) X(pool_torn) XI(torn_pools) X(drip_scatter_m) X(drip_min_m) X(drip_span_m)
struct ThingsShape {
	SHAPE_DECLARE(EFFECT_SHAPE_EFFECT_THINGS)
};

} // namespace city
