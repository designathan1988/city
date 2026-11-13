// Strict Dictionary -> EffectParams conversion for project/data/render/effects.json.
#include "render/effects/effect_params.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
struct Reader {
	PackedStringArray &errors;
	const char *where;
	Dictionary d;

	Reader(PackedStringArray &e, const Dictionary &root, const char *group) : errors(e), where(group) {
		if (root.has(group)) d = root[group];
		else errors.push_back(String("effects: missing '") + group + "'");
	}
	float num(const char *key) {
		if (d.has(key)) return float(double(d[key]));
		errors.push_back(String("effects.") + where + ": missing '" + key + "'");
		return 0.f;
	}
	int integer(const char *key) { return int(num(key)); }
	Color color(const char *key) { // "#rrggbb", sRGB
		if (d.has(key)) return Color::html(String(d[key]));
		errors.push_back(String("effects.") + where + ": missing '" + key + "'");
		return Color();
	}
};
} // namespace

bool EffectParams::load(const Dictionary &t, PackedStringArray &errors) {
	const int before = int(errors.size());
	Reader puffs(errors, t, "puffs");
	pool = puffs.integer("pool");
	wind_x = puffs.num("wind_x");
	wind_z = puffs.num("wind_z");
	Reader fireball(errors, t, "fireball");
	fireball_base = fireball.num("base_m");
	fireball_per_force = fireball.num("per_force_m");
	Array list = t.has("bursts") ? Array(t["bursts"]) : Array();
	for (int i = 0; i < list.size(); i++) {
		Dictionary row = list[i];
		BurstDef b;
		b.kind = int(double(row.get("kind", 0)));
		b.count = int(double(row.get("count", 0)));
		b.height = float(double(row.get("height_m", 0)));
		b.spread_m = float(double(row.get("spread_m", 0)));
		b.spread_r = float(double(row.get("spread_r", 0)));
		b.size_m = float(double(row.get("size_m", 0)));
		b.size_r = float(double(row.get("size_r", 0)));
		b.life = float(double(row.get("life_s", 0)));
		bursts.push_back(b);
	}
	Array srcs = t.has("sources") ? Array(t["sources"]) : Array();
	for (int i = 0; i < srcs.size(); i++) {
		Dictionary row = srcs[i];
		SourceDef d;
		d.for_s = float(double(row.get("for_s", 0)));
		d.for_k = float(double(row.get("for_k", 0)));
		d.for_cap_s = float(double(row.get("for_cap_s", 0)));
		d.rate_hz = float(double(row.get("rate_hz", 0)));
		d.size_k = float(double(row.get("size_k", 0)));
		d.size_cap_m = float(double(row.get("size_cap_m", 0)));
		d.height_m = float(double(row.get("height_m", 0)));
		sources.push_back(d);
	}
	Reader emit(errors, t, "emitter");
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
	return int(errors.size()) == before && !bursts.empty();
}

void EffectParams::load_look(const Dictionary &t, PackedStringArray &errors) {
	Reader burn(errors, t, "burn");
	burn_fire_rate = burn.num("puff_rate_hz");
	burn_building_rate = burn.num("building_rate_hz");
	flame_kind = burn.integer("flame_kind");
	smoke_kind = burn.integer("smoke_kind");
	flame_size = burn.num("flame_size_m");
	flame_life = burn.num("flame_life_s");
	smoke_size = burn.num("smoke_size_m");
	smoke_life = burn.num("smoke_life_s");
	column_chance = burn.num("column_chance");
	Reader ring(errors, t, "ring");
	ring_life = ring.num("life_s");
	ring_height = ring.num("height_m");
	Reader flash(errors, t, "flash");
	flash_color = Color::html(String(flash.d.get("color", "#ffffff")));
	flash_energy = flash.num("energy");
	flash_range_r = flash.num("reach_k");
	flash_range_min = flash.num("range_min_m");
	flash_life = flash.num("life_s");
	flash_decay = flash.num("decay");
	flash_height = flash.num("height_m");
	Reader shake(errors, t, "shake");
	shake_base = shake.num("base_m");
	shake_per_r = shake.num("radius_k_m");
	shake_decay = shake.num("decay_hz");
	Reader shrap(errors, t, "shrapnel");
	shrap_base = shrap.integer("base");
	shrap_per_r = shrap.num("count_k");
	shrap_speed_min = shrap.num("speed_min_m");
	shrap_speed_span = shrap.num("speed_span_m");
	shrap_lift_min = shrap.num("lift_min_m");
	shrap_lift_span = shrap.num("lift_span_m");
	shrap_size_min = shrap.num("size_min_m");
	shrap_size_span = shrap.num("size_span_m");
}

void EffectParams::load_things(const Dictionary &t, PackedStringArray &errors) {
	Reader soot(errors, t, "soot");
	soot_base = soot.integer("base");
	soot_per_step = soot.num("count_k");
	soot_max = soot.integer("max");
	soot_reach_r = soot.num("reach_r");
	soot_size_min = soot.num("size_min_m");
	soot_size_span = soot.num("size_span_m");
	soot_size_r = soot.num("size_r");
	soot_kept = soot.integer("kept");
	Reader debris(errors, t, "debris");
	debris_max = debris.integer("max_pieces");
	gravity = debris.num("gravity_ms2");
	restitution = debris.num("restitution");
	friction = debris.num("friction");
	rest_s = debris.num("rest_s");
	damp_ground = debris.num("damp_ground");
	Reader geyser(errors, t, "geyser");
	geyser_s = geyser.num("seconds");
	geyser_fade = geyser.num("fade_s");
	geyser_rate = geyser.num("jet_hz");
	geyser_size = geyser.num("size_m");
	Reader blood(errors, t, "blood");
	blood_max = blood.integer("max_decals");
	pool_spread_s = blood.num("pool_spread_s");
	pool_size = blood.num("pool_size_m");
	splat_m = blood.num("splat_m");
	drop_m = blood.num("drop_m");
	pool_torn = blood.num("pool_torn");
	splash_ms = blood.num("splash_ms");
	splashes_most = blood.integer("splashes_most");
	Reader paper(errors, t, "paper");
	paper_count = paper.integer("count");
	paper_size = paper.num("size_m");
	paper_life = paper.num("life_s");
	paper_spread = paper.num("spread_m");
	Reader down(errors, t, "down");
	down_dust_count = down.integer("dust_count");
	down_dust_size = down.num("dust_size_m");
	down_dust_life = down.num("dust_life_s");
	down_dust_spread = down.num("dust_spread_m");
	push_ms = down.num("push_ms");
	lift_ms = down.num("lift_ms");
	limb_push_ms = down.num("limb_push_ms");
	tracer_life = down.num("tracer_life_s");
	tracer_m = down.num("tracer_m");
	flash_m = down.num("flash_m");
	body_lead = down.num("body_lead_s");
	brace_ms = down.num("brace_ms");
	brace_down = down.num("brace_down");
	Reader tints(errors, t, "tints");
	tint_wood = tints.color("wood");
	tint_metal = tints.color("metal");
	tint_leaf = tints.color("leaf");
	tint_char = tints.color("char");
	tint_flesh = tints.color("flesh");
	tint_paint = tints.color("paint");
}

} // namespace city
