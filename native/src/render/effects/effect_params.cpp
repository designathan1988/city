// Strict Dictionary -> EffectParams conversion for project/data/render/effects.json and aftermath.json, through the strict reader
// (render/data_reader.h): every key, type and row checked.
#include "render/effects/effect_params.h"

#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
constexpr size_t FEWEST_CORNERS = 3;
constexpr int MIN_ROPE_POINTS = 2;
constexpr int VEC3 = 3;

// A list of three numbers (already checked for its length by the reader) as a vector; a zero one when the reader failed it.
Vector3 vec3_of(const Array &a) {
	return a.size() == VEC3 ? Vector3(float(double(a[0])), float(double(a[1])), float(double(a[2]))) : Vector3();
}

// One table of effects.json, read through the strict reader.
struct Group {
	DataReader r;
	Dictionary d;
	Group(DataReader &top, const Dictionary &root, const char *key) : r(top.in(key)), d(top.table(root, key)) {}
	float num(const char *key) { return float(r.num(d, key)); }
	int integer(const char *key) { return r.integer(d, key); }
};
} // namespace

bool EffectParams::load(const Dictionary &t, const Dictionary &aftermath, const Dictionary &gore_table, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "effects.json" };
	Group puffs(r, t, "puffs");
	pool = puffs.integer("pool");
	wind_x = puffs.num("wind_x");
	wind_z = puffs.num("wind_z");
	Group fireball(r, t, "fireball");
	fireball_base = fireball.num("base_m");
	fireball_per_force = fireball.num("per_force_m");
	const Array list = r.list(t, "bursts");
	for (int i = 0; i < list.size(); i++) {
		DataReader rb = r.in("bursts").in(i);
		const Dictionary row = r.in("bursts").table_at(list, i);
		BurstDef b;
		b.kind = rb.integer(row, "kind");
		b.count = rb.integer(row, "count");
		b.height = float(rb.num(row, "height_m"));
		b.spread_m = float(rb.num(row, "spread_m"));
		b.spread_r = float(rb.num(row, "spread_r"));
		b.size_m = float(rb.num(row, "size_m"));
		b.size_r = float(rb.num(row, "size_r"));
		b.life = float(rb.num(row, "life_s"));
		bursts.push_back(b);
	}
	const Array srcs = r.list(t, "sources");
	for (int i = 0; i < srcs.size(); i++) {
		DataReader rs = r.in("sources").in(i);
		const Dictionary row = r.in("sources").table_at(srcs, i);
		SourceDef d;
		d.for_s = float(rs.num(row, "for_s"));
		d.for_k = float(rs.num(row, "for_k"));
		d.for_cap_s = float(rs.num(row, "for_cap_s"));
		d.rate_hz = float(rs.num(row, "rate_hz"));
		d.size_k = float(rs.num(row, "size_k"));
		d.size_cap_m = float(rs.num(row, "size_cap_m"));
		d.height_m = float(rs.num(row, "height_m"));
		sources.push_back(d);
	}
	Group emit(r, t, "emitter");
	flame_base = emit.num("flame_base_m");
	flame_k = emit.num("flame_k");
	dying_s = emit.num("dying_s");
	dying_floor = emit.num("dying_floor");
	puff_height = emit.num("puff_height_m");
	chimney_height = emit.num("chimney_height_m");
	chimney_spread = emit.num("chimney_spread_m");
	light_chance = emit.num("light_chance");
	light_height = emit.num("light_height_m");
	light_spread_k = emit.num("light_spread_k");
	light_size = emit.num("light_size_m");
	light_life = emit.num("light_life_s");
	load_look(t, errors);
	load_things(t, errors);
	load_heave(aftermath, errors);
	load_wires(aftermath, errors);
	load_gore(gore_table, errors);
	if (bursts.empty()) r.in("bursts").fail("no burst");
	return errors.size() == before;
}

void EffectParams::load_look(const Dictionary &t, PackedStringArray &errors) {
	DataReader r{ errors, "effects.json" };
	Group burn(r, t, "burn");
	burn_fire_rate = burn.num("puff_rate_hz");
	burn_building_rate = burn.num("building_rate_hz");
	flame_kind = burn.integer("flame_kind");
	smoke_kind = burn.integer("smoke_kind");
	flame_size = burn.num("flame_size_m");
	flame_life = burn.num("flame_life_s");
	smoke_size = burn.num("smoke_size_m");
	smoke_life = burn.num("smoke_life_s");
	column_chance = burn.num("column_chance");
	Group ring(r, t, "ring");
	ring_life = ring.num("life_s");
	ring_height = ring.num("height_m");
	ring_reach_r = ring.num("reach_r");
	Group flash(r, t, "flash");
	flash_color = Color::hex(flash.r.colour(flash.d, "color"));
	flash_energy = flash.num("energy");
	flash_range_r = flash.num("reach_k");
	flash_range_min = flash.num("range_min_m");
	flash_life = flash.num("life_s");
	flash_decay = flash.num("decay");
	flash_height = flash.num("height_m");
	Group shake(r, t, "shake");
	shake_base = shake.num("base_m");
	shake_per_r = shake.num("radius_k_m");
	shake_decay = shake.num("decay_hz");
	Group shrap(r, t, "shrapnel");
	shrap_base = shrap.integer("base");
	shrap_per_r = shrap.num("count_k");
	shrap_speed_min = shrap.num("speed_min_m");
	shrap_speed_span = shrap.num("speed_span_m");
	shrap_lift_min = shrap.num("lift_min_m");
	shrap_lift_span = shrap.num("lift_span_m");
	shrap_size_min = shrap.num("size_min_m");
	shrap_size_span = shrap.num("size_span_m");
	Group tint(r, t, "tints");
	const auto hue = [&](const char *key) { return Color::hex(tint.r.colour(tint.d, key)); };
	tints = Tints{ hue("asphalt"), hue("concrete"), hue("earth"), hue("brick"), hue("glass"), hue("wood"), hue("metal"), hue("char"), hue("leaf"),
		hue("paint"), hue("dirt") };
}

void EffectParams::load_things(const Dictionary &t, PackedStringArray &errors) {
	DataReader r{ errors, "effects.json" };
	Group soot(r, t, "soot");
	soot_base = soot.integer("base");
	soot_per_step = soot.num("count_k");
	soot_max = soot.integer("max");
	soot_reach_r = soot.num("reach_r");
	soot_size_min = soot.num("size_min_m");
	soot_size_span = soot.num("size_span_m");
	soot_size_r = soot.num("size_r");
	soot_kept = soot.integer("kept");
	Group debris(r, t, "debris");
	debris_max = debris.integer("max_pieces");
	gravity = debris.num("gravity_ms2");
	restitution = debris.num("restitution");
	friction = debris.num("friction");
	rest_s = debris.num("rest_s");
	damp_ground = debris.num("damp_ground");
	Group geyser(r, t, "geyser");
	geyser_s = geyser.num("seconds");
	geyser_fade = geyser.num("fade_s");
	geyser_rate = geyser.num("jet_hz");
	geyser_size = geyser.num("size_m");
	Group blood(r, t, "blood");
	blood_max = blood.integer("max_decals");
	pool_spread_s = blood.num("pool_spread_s");
	pool_size = blood.num("pool_size_m");
	splat_m = blood.num("splat_m");
	drop_m = blood.num("drop_m");
	Group paper(r, t, "paper");
	paper_count = paper.integer("count");
	paper_size = paper.num("size_m");
	paper_life = paper.num("life_s");
	paper_spread = paper.num("spread_m");
	Group down(r, t, "down");
	down_dust_count = down.integer("dust_count");
	down_dust_size = down.num("dust_size_m");
	down_dust_life = down.num("dust_life_s");
	down_dust_spread = down.num("dust_spread_m");
	body_life = down.num("body_life_s");
	push_ms = down.num("push_ms");
	lift_ms = down.num("lift_ms");
	tracer_life = down.num("tracer_life_s");
	tracer_m = down.num("tracer_m");
	flash_m = down.num("flash_m");
	body_lead = down.num("body_lead_s");
}

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
