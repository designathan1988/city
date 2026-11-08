// Strict Dictionary -> BlastParams conversion for project/data/catalog/blast.json through the strict reader
// (api/data_reader.h): a missing or mistyped key or a list of the wrong length is reported and loading fails, so no default value
// can silently replace the data.
#include "api/catalog_loader.h"
#include "render/data_reader.h"

#include "sim/demolition/blast_model.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

// Bands of a scaled distance, in order: [{z0, z1, c: [BLAST_COEFFS]}...].
void curves(DataReader r, const Array &rows, std::vector<BlastCurve> &out) {
	for (int i = 0; i < rows.size(); i++) {
		DataReader rb = r.in(i);
		const Dictionary row = r.table_at(rows, i);
		BlastCurve c;
		c.z0 = float(rb.num(row, "z0"));
		c.z1 = float(rb.num(row, "z1"));
		const Array coeffs = rb.list(row, "c", BLAST_COEFFS);
		for (int k = 0; k < BLAST_COEFFS && k < coeffs.size(); k++) c.c[k] = float(rb.in("c").num_at(coeffs, k));
		if (!out.empty() && c.z0 < out.back().z1) rb.fail("bands out of order");
		out.push_back(c);
	}
	if (out.empty()) r.fail("no band");
}

} // namespace

bool load_blast_params(const Dictionary &t, BlastParams &p, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "blast.json" };
	p.charge_kg_per_s2 = float(r.num(t, "charge_kg_per_s2"));
	p.strength_min = r.integer(t, "strength_min");
	p.strength_max = r.integer(t, "strength_max");
	p.atmosphere_kpa = float(r.num(t, "atmosphere_kpa"));
	curves(r.in("pressure_curves"), r.list(t, "pressure_curves"), p.pressure);
	curves(r.in("impulse_curves"), r.list(t, "impulse_curves"), p.impulse);
	curves(r.in("duration_curves"), r.list(t, "duration_curves"), p.duration);
	p.response_ms = float(r.in("structure").num(r.table(t, "structure"), "response_ms"));
	const Dictionary lim = r.table(t, "limits_kpa");
	p.light_kpa = float(r.in("limits_kpa").num(lim, "light"));
	p.structural_kpa = float(r.in("limits_kpa").num(lim, "structural"));
	p.collapse_kpa = float(r.in("limits_kpa").num(lim, "collapse"));
	p.vehicle_kpa = float(r.in("limits_kpa").num(lim, "vehicle"));
	p.pole_kpa = float(r.in("limits_kpa").num(lim, "pole"));
	const Dictionary lethal = r.table(t, "lethality");
	p.lethal_threshold_kpa = float(r.in("lethality").num(lethal, "threshold_kpa"));
	p.lethal_half_kpa = float(r.in("lethality").num(lethal, "half_kpa"));
	p.lethal_threshold_prob = float(r.in("lethality").num(lethal, "threshold_probability"));
	const Dictionary ear = r.table(t, "eardrum");
	p.ear_threshold_kpa = float(r.in("eardrum").num(ear, "threshold_kpa"));
	p.ear_half_kpa = float(r.in("eardrum").num(ear, "half_kpa"));
	p.ear_threshold_prob = float(r.in("eardrum").num(ear, "threshold_probability"));
	p.lethal_sigma = blast::probit_sigma(p.lethal_threshold_kpa, p.lethal_half_kpa, p.lethal_threshold_prob);
	p.ear_sigma = blast::probit_sigma(p.ear_threshold_kpa, p.ear_half_kpa, p.ear_threshold_prob);
	const Dictionary noise = r.table(t, "noise");
	p.startle_pa = float(r.in("noise").num(noise, "startle_pa"));
	p.reference_pa = float(r.in("noise").num(noise, "reference_pa"));
	p.startle_cap_m = float(r.in("noise").num(noise, "startle_cap_m"));
	const Dictionary people = r.table(t, "people");
	p.knock_min_s = float(r.in("people").num(people, "knock_min_s"));
	p.knock_span_s = float(r.in("people").num(people, "knock_span_s"));
	p.panic_s = float(r.in("people").num(people, "panic_s"));
	p.panic_gain = float(r.in("people").num(people, "panic_gain"));
	p.torn_kpa = float(r.in("people").num(people, "torn_kpa"));
	p.sever_chance = float(r.in("people").num(people, "sever_chance"));
	p.maim_chance = float(r.in("people").num(people, "maim_chance"));
	const Dictionary strike = r.table(t, "strike");
	p.hold_growth_s = float(r.in("strike").num(strike, "hold_growth_s"));
	p.fire_max = r.in("strike").integer(strike, "fire_max");
	p.fire_min_s = float(r.in("strike").num(strike, "fire_min_s"));
	p.fire_span_s = float(r.in("strike").num(strike, "fire_span_s"));
	p.wear_gain = float(r.in("strike").num(strike, "wear_gain"));
	const Dictionary shot = r.table(t, "shot");
	p.walker_hp = float(r.in("shot").num(shot, "walker_hp"));
	p.damage_head = float(r.in("shot").num(shot, "damage_head"));
	p.damage_torso = float(r.in("shot").num(shot, "damage_torso"));
	p.damage_limb = float(r.in("shot").num(shot, "damage_limb"));
	p.sever_hits_hp = float(r.in("shot").num(shot, "sever_hits_hp"));
	p.sever_hp_cost = float(r.in("shot").num(shot, "sever_hp_cost"));
	p.body_reach_m = float(r.in("shot").num(shot, "body_reach_m"));
	p.body_height_m = float(r.in("shot").num(shot, "body_height_m"));
	p.head_above_m = float(r.in("shot").num(shot, "head_above_m"));
	p.torso_above_m = float(r.in("shot").num(shot, "torso_above_m"));
	p.arm_aside_m = float(r.in("shot").num(shot, "arm_aside_m"));
	p.grave_hp = float(r.in("shot").num(shot, "grave_hp"));
	p.bleed_limb = float(r.in("shot").num(shot, "bleed_limb"));
	p.bleed_leg = float(r.in("shot").num(shot, "bleed_leg"));
	p.bleed_hurt = float(r.in("shot").num(shot, "bleed_hurt"));
	p.glass_above = float(r.in("shot").num(shot, "glass_above"));
	p.two_wheeler_rider_m = float(r.in("shot").num(shot, "two_wheeler_rider_m"));
	p.flinch_s = float(r.in("shot").num(shot, "flinch_s"));
	p.startle_m = float(r.in("shot").num(shot, "startle_m"));
	p.pace_light = float(r.in("shot").num(shot, "pace_light"));
	p.pace_leg = float(r.in("shot").num(shot, "pace_leg"));
	p.drips_per_s = float(r.in("shot").num(shot, "drips_per_s"));
	p.pace_grave = float(r.in("shot").num(shot, "pace_grave"));
	p.max_range_m = float(r.in("shot").num(shot, "max_range_m"));
	const Array boxes = r.in("shot").list(shot, "vehicle_box_m", VEHICLE_TYPES); // one height per vehicle type
	for (int k = 0; k < int(boxes.size()) && k < VEHICLE_TYPES; k++) p.vehicle_box_m[k] = float(r.in("shot").in("vehicle_box_m").num_at(boxes, k));
	p.open_hits = r.in("shot").integer(shot, "open_hits");
	p.vehicle_shots_to_burst = r.in("shot").integer(shot, "vehicle_shots_to_burst");
	p.burst_force = r.in("shot").integer(shot, "burst_force");
	const Dictionary crater = r.table(t, "crater");
	p.crater_k_earth = float(r.in("crater").num(crater, "k_earth"));
	p.crater_k_hard = float(r.in("crater").num(crater, "k_hard"));
	p.crater_ratio = float(r.in("crater").num(crater, "diameter_to_depth"));
	p.pass_width_m = float(r.in("crater").num(crater, "pass_width_m"));
	p.crater_min_m = float(r.in("crater").num(crater, "min_radius_m"));
	return errors.size() == before;
}

} // namespace city
