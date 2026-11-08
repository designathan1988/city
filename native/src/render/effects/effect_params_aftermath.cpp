// Strict Dictionary -> EffectParams for what a blow leaves on the street (aftermath.json: heave, rope, arc, leak, pole, tree) and what a
// body opened or cut shows (gore.json), through the strict reader (render/data_reader.h).
#include "render/effects/effect_params.h"

#include "render/data_reader.h"
#include "render/effects/effect_params_read.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

using namespace effect_read;

void EffectParams::load_heave(const Dictionary &t, PackedStringArray &errors) {
	DataReader r{ errors, "aftermath.json" };
	Group h(r, t, "heave");
	HeaveLook &v = heave;
	v.plates_base = h.integer("plates_base");
	v.plates_per_m = h.num("plates_per_m");
	v.plates_most = h.integer("plates_most");
	v.crowded = h.integer("crowded");
	v.crowd_r = h.num("crowd_r");
	v.plate_in = h.num("plate_in");
	v.plate_span = h.num("plate_span");
	v.plate_w_min = h.num("plate_w_min_m");
	v.plate_w_span = h.num("plate_w_span_m");
	v.scale_base = h.num("scale_base");
	v.scale_grow_m = h.num("scale_grow_m");
	v.scale_most = h.num("scale_most");
	v.plate_depth_min = h.num("plate_depth_min");
	v.plate_depth_span = h.num("plate_depth_span");
	v.plate_thick_hard = h.num("plate_thick_hard_m");
	v.plate_thick_earth = h.num("plate_thick_earth_m");
	v.plate_lift = h.num("plate_lift");
	v.tilt_min = h.num("tilt_min_rad");
	v.tilt_span = h.num("tilt_span_rad");
	v.dirt_min = h.num("dirt_min");
	v.dirt_span = h.num("dirt_span");
	v.shade_min = h.num("shade_min");
	v.shade_span = h.num("shade_span");
	v.concrete_share = h.num("concrete_share");
	v.slabs_base = h.integer("slabs_base");
	v.slabs_per_m = h.num("slabs_per_m");
	v.slab_in = h.num("slab_in");
	v.slab_span = h.num("slab_span");
	v.slab_jitter = h.num("slab_jitter_rad");
	v.slab_min_m = h.num("slab_min_m");
	v.slab_span_m = h.num("slab_span_m");
	v.slab_earth_scale = h.num("slab_earth_scale");
	v.slab_thick_hard = h.num("slab_thick_hard_m");
	v.slab_thick_earth = h.num("slab_thick_earth_m");
	v.slab_depth_min = h.num("slab_depth_min");
	v.slab_depth_span = h.num("slab_depth_span");
	v.slab_tilt_min = h.num("slab_tilt_min_rad");
	v.slab_tilt_span = h.num("slab_tilt_span_rad");
	v.slab_embed = h.num("slab_embed");
	v.slab_concrete_every = h.integer("slab_concrete_every");
	if (v.slab_concrete_every < 1) h.r.in("slab_concrete_every").fail("one slab in one or more");
	v.clod_every = h.integer("clod_every");
	v.clod_min_m = h.num("clod_min_m");
	v.clod_span_m = h.num("clod_span_m");
	v.clod_h_m = h.num("clod_h_m");
	v.bevel_m = h.num("bevel_m");
	const Array outline = h.r.list(h.d, "outline_r");
	for (int i = 0; i < outline.size(); i++) v.outline_r.push_back(float(double(outline[i])));
	if (v.outline_r.size() < FEWEST_CORNERS) h.r.in("outline_r").fail("a plate needs three corners or more");
	if (v.clod_every < 1) h.r.in("clod_every").fail("a clod every one plate or more");
	v.asphalt = tints.asphalt;
	v.concrete = tints.concrete;
	v.earth = tints.earth;
	v.dirt = tints.dirt;
}

void EffectParams::load_wires(const Dictionary &t, PackedStringArray &errors) {
	DataReader r{ errors, "aftermath.json" };
	Group w(r, t, "rope");
	rope.points = w.integer("points");
	rope.most = w.integer("most");
	rope.passes = w.integer("passes");
	rope.slack = w.num("slack");
	rope.damp = w.num("damp");
	rope.lift_m = w.num("lift_m");
	rope.friction = w.num("friction");
	rope.crackle_min_s = w.num("crackle_min_s");
	rope.crackle_span_s = w.num("crackle_span_s");
	rope.crackle_hz = w.num("crackle_hz");
	rope.crackle_near_m = w.num("crackle_near_m");
	rope.crackle_count = w.integer("crackle_count");
	rope.crackle_spread_m = w.num("crackle_spread_m");
	rope.crackle_size_m = w.num("crackle_size_m");
	rope.crackle_life_s = w.num("crackle_life_s");
	rope.flash_chance = w.num("flash_chance");
	rope.flash_energy = w.num("flash_energy");
	rope.flash_s = w.num("flash_s");
	rope.kick_m = w.num("kick_m");
	rope.kick_lift = w.num("kick_lift");
	rope.drop_share = w.num("drop_share");
	rope.settle_s = w.num("settle_s");
	if (rope.points < MIN_ROPE_POINTS) w.r.in("points").fail("a wire needs two points or more");
	Group a(r, t, "arc");
	arc.sparks = a.integer("sparks");
	arc.gap_min_s = a.num("gap_min_s");
	arc.gap_span_s = a.num("gap_span_s");
	arc.spread_m = a.num("spread_m");
	arc.size_m = a.num("size_m");
	arc.life_s = a.num("life_s");
	arc.color = Color::hex(a.r.colour(a.d, "color"));
	arc.energy = a.num("energy");
	arc.range_m = a.num("range_m");
	arc.flash_s = a.num("flash_s");
	arc.pole_arc_min_s = a.num("pole_arc_min_s");
	arc.pole_arc_span_s = a.num("pole_arc_span_s");
	arc.signal_arc_min_s = a.num("signal_arc_min_s");
	arc.signal_arc_span_s = a.num("signal_arc_span_s");
	Group l(r, t, "leak");
	leak_rate = l.num("jet_hz");
	leak_size = l.num("size_m");
	leak_life = l.num("life_s");
	Group tr(r, t, "tree");
	const struct {
		const char *key;
		float *to;
	} tree_nums[] = { { "stump_min", &tree.stump_min }, { "stump_span", &tree.stump_span }, { "trunk_r_share", &tree.trunk_r_share },
		{ "throw_share", &tree.throw_share }, { "spin_min", &tree.spin_min }, { "spin_span", &tree.spin_span }, { "burn_min_s", &tree.burn_min_s },
		{ "burn_span_s", &tree.burn_span_s }, { "chip_r_m", &tree.chip_r_m }, { "chip_len_min_m", &tree.chip_len_min_m },
		{ "chip_len_span_m", &tree.chip_len_span_m }, { "leaves_per_m", &tree.leaves_per_m }, { "leaf_min_m", &tree.leaf_min_m },
		{ "leaf_span_m", &tree.leaf_span_m }, { "leaf_thick_m", &tree.leaf_thick_m }, { "crown_at", &tree.crown_at },
		{ "leaf_scatter_ms", &tree.leaf_scatter_ms }, { "street_height_m", &tree.street_height_m }, { "street_width_m", &tree.street_width_m } };
	for (const auto &n : tree_nums) *n.to = tr.num(n.key);
	tree.chips = tr.integer("chips");
	Group p(r, t, "pole");
	pole.crown_m = p.num("crown_m");
	pole.fall_spin_min = p.num("fall_spin_min");
	pole.fall_spin_span = p.num("fall_spin_span");
	pole.snap_spin_min = p.num("snap_spin_min");
	pole.snap_spin_span = p.num("snap_spin_span");
	pole.cut_min = p.num("cut_min");
	pole.cut_span = p.num("cut_span");
	pole.arm_section_m = p.num("arm_section_m");
	pole.arm_over_m = p.num("arm_over_m");
	pole.lamp_reach_share = p.num("lamp_reach_share");
	pole.lamp_head_m = vec3_of(p.r.list(p.d, "lamp_head_m", VEC3));
	pole.post_head_m = vec3_of(p.r.list(p.d, "post_head_m", VEC3));
}

void EffectParams::load_gore(const Dictionary &t, PackedStringArray &errors) {
	DataReader r{ errors, "gore.json" };
	Group g(r, t, "gore");
	GoreLook &v = gore;
	v.links = g.integer("links");
	v.passes = g.integer("passes");
	v.opened_organs_min = g.integer("opened_organs_min");
	v.opened_organs_span = g.integer("opened_organs_span");
	v.torn_organs_min = g.integer("torn_organs_min");
	v.torn_organs_span = g.integer("torn_organs_span");
	const struct {
		const char *key;
		float *to;
	} nums[] = { { "link_min_m", &v.link_min_m }, { "link_span_m", &v.link_span_m }, { "coil_m", &v.coil_m }, { "coil_turn", &v.coil_turn },
		{ "drop_share", &v.drop_share }, { "gut_r_min_m", &v.gut_r_min_m }, { "gut_r_span_m", &v.gut_r_span_m }, { "hold_chance", &v.hold_chance },
		{ "hold_min_s", &v.hold_min_s }, { "hold_span_s", &v.hold_span_s }, { "damp", &v.damp }, { "friction", &v.friction }, { "still_mm", &v.still_mm },
		{ "still_s", &v.still_s }, { "organ_min_m", &v.organ_min_m }, { "organ_span_m", &v.organ_span_m }, { "organ_long_span", &v.organ_long_span },
		{ "organ_flat_min", &v.organ_flat_min }, { "organ_flat_span", &v.organ_flat_span }, { "organ_scatter_ms", &v.organ_scatter_ms },
		{ "organ_lift_min_ms", &v.organ_lift_min_ms }, { "organ_lift_span_ms", &v.organ_lift_span_ms }, { "bone_r_m", &v.bone_r_m },
		{ "bone_len_m", &v.bone_len_m }, { "head_bone_len_m", &v.head_bone_len_m }, { "piece_bone_len_m", &v.piece_bone_len_m },
		{ "head_piece_bone_len_m", &v.head_piece_bone_len_m }, { "cuff_r_m", &v.cuff_r_m }, { "cuff_len_m", &v.cuff_len_m },
		{ "stump_bone_chance", &v.stump_bone_chance }, { "piece_bone_from", &v.piece_bone_from }, { "piece_bone_to", &v.piece_bone_to },
		{ "open_torn_chance", &v.open_torn_chance }, { "open_dead_chance", &v.open_dead_chance }, { "pelvis_chance", &v.pelvis_chance },
		{ "pelvis_kick_share", &v.pelvis_kick_share }, { "belly_kick_ms", &v.belly_kick_ms }, { "piece_speed_min_ms", &v.piece_speed_min_ms },
		{ "piece_speed_power_ms", &v.piece_speed_power_ms }, { "power_least", &v.power_least }, { "speed_jitter_min", &v.speed_jitter_min },
		{ "speed_jitter_span", &v.speed_jitter_span }, { "piece_spread_ms", &v.piece_spread_ms }, { "piece_lift_min_ms", &v.piece_lift_min_ms },
		{ "piece_lift_span_ms", &v.piece_lift_span_ms }, { "lift_base", &v.lift_base }, { "shot_speed_min_ms", &v.shot_speed_min_ms },
		{ "shot_speed_span_ms", &v.shot_speed_span_ms }, { "shot_spread_ms", &v.shot_spread_ms }, { "shot_lift_min_ms", &v.shot_lift_min_ms },
		{ "shot_lift_span_ms", &v.shot_lift_span_ms } };
	for (const auto &n : nums) *n.to = g.num(n.key);
	v.bone_tint = Color::hex(g.r.colour(g.d, "bone_tint"));
	v.cuff_tint = Color::hex(g.r.colour(g.d, "cuff_tint"));
	const DataValue all = g.r.at(g.d);
	const auto tints_of = [&](const char *key, std::vector<Color> &into) {
		const DataValue list = all[key];
		for (int i = 0; i < list.size(); i++) into.push_back(Color::hex(list[i].colour()));
		if (into.empty()) list.fail("one tint or more");
	};
	tints_of("gut_tints", v.gut_tints);
	tints_of("organ_tints", v.organ_tints);
	if (v.links < MIN_ROPE_POINTS) g.r.in("links").fail("a chain needs two links or more");
}

} // namespace city
