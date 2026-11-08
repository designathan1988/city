// Strict Dictionary -> EffectParams conversion for project/data/render/effects.json and aftermath.json, through the strict reader
// (render/data_reader.h): every key, type and row checked.
#include "render/effects/effect_params.h"

#include "render/data_reader.h"
#include "render/effects/effect_params_read.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

using namespace effect_read;

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

} // namespace city
