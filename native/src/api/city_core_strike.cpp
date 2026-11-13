// The bomb and its probe as script API (sim/commands/strike_commands.cpp): the blow is one undoable edit; the probe
// counts what blows have left on the map, so a test can tell the explosion changed the city.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/traffic/traffic_system.h"

using namespace godot;

namespace city {

Dictionary CityCore::strike(const Vector3 &p, int strength) {
	return run_edit(*state_, "cmd.strike", [&](CityState &s) { return strike_at(s, p.x, p.z, double(strength), double(p.y)); });
}

Dictionary CityCore::shoot(const Vector3 &origin, const Vector3 &direction) {
	Dictionary d;
	ShotResult r;
	{
		map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::COMMAND);
		if (!state_->ready) return d;
		r = shoot_at(*state_, double(origin.x), double(origin.y), double(origin.z), double(direction.x), double(direction.y), double(direction.z));
	}
	d["ok"] = true;
	d["hit"] = int(r.hit);
	d["killed"] = r.killed;
	d["burst"] = r.burst;
	if (r.burst) strike(Vector3(r.burst_x, r.burst_y, r.burst_z), state_->catalog.blast.burst_force);
	return d;
}

PackedInt64Array CityCore::ruin_seeds() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	PackedInt64Array out;
	for (const Ruin &r : state_->damage.ruins) out.push_back(int64_t(r.seed));
	return out;
}

Dictionary CityCore::damage_probe() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (!s.ready) return d;
	d["craters"] = int(s.damage.craters.size());
	d["ruins"] = int(s.damage.ruins.size());
	d["gone"] = int(s.damage.gone.size());
	d["hurt"] = int(s.damage.hurt.size());
	d["scars"] = int(s.damage.scars.size());
	d["broken"] = render_.broken_buildings();
	d["fires"] = int(s.damage.fires.size());
	d["strikes"] = int(s.damage.strikes);
	d["pieces"] = render_.effect_pieces();
	d["blood_marks"] = render_.blood_marks();
	d["events"] = int64_t(state_->log.damage_events.size()); // not yet played: no render, as in a test with no view, leaves them here
	d["effects_busy"] = render_.effects_busy();
	d["buildings"] = s.buildings.count;
	d["pedestrians"] = s.traffic.ped_count;
	d["vehicles"] = s.traffic.vehicle_count;
	int segs = 0;
	for (const RoadSegment &g : s.roads.segs) segs += g.alive ? 1 : 0;
	d["segments"] = segs;
	return d;
}

Dictionary CityCore::shot_targets(int limit) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const TrafficState &t = state_->traffic;
	Array walkers, vehicles;
	for (int id = 0; id < int(t.peds.size()) && int(walkers.size()) < limit; id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		Dictionary d;
		d["id"] = id;
		// on its feet: where it stands and the way it walks; lying: its body's middle and the way it lies, its head that way (ped_system.h
		// lying, the capsule a round meets and the crowd goes round)
		const bool down = traffic::lying(t, w);
		const V2 mid = down ? traffic::lying_middle(state_->catalog.blast, w) : V2(w.cur.x, w.cur.z);
		d["position"] = Vector3(mid.x, w.cur.y, mid.y);
		d["facing"] = down ? Vector2(w.fall_x, w.fall_z) : Vector2(w.cur.fx, w.cur.fz);
		d["hp"] = w.hp;
		d["fallen"] = down;
		d["lost"] = int(w.lost);
		d["bleed"] = w.bleed;
		d["scale"] = w.scale;
		walkers.push_back(d);
	}
	for (int id = 0; id < int(t.vehicles.size()) && int(vehicles.size()) < limit; id++) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive) continue;
		Dictionary d;
		d["id"] = id;
		d["position"] = Vector3(v.cur.x, v.cur.y, v.cur.z);
		d["type"] = int(v.type);
		d["shots"] = int(v.shots);
		d["stopped"] = v.vf <= 0.f;
		vehicles.push_back(d);
	}
	Array dead; // the dead bodies lying (ped_dead.cpp), as the walkers lying: the body's middle, the way it lies, its id the render's
	for (const DeadBody &b : t.dead) {
		if (int(dead.size()) >= limit) break;
		const V2 mid = traffic::dead_middle(state_->catalog.blast, b);
		Dictionary d;
		d["id"] = b.id;
		d["position"] = Vector3(mid.x, b.y, mid.y);
		d["facing"] = Vector2(b.fall_x, b.fall_z);
		d["lost"] = int(b.lost);
		d["scale"] = b.scale;
		d["sidewalk"] = b.side >= 0 ? b.seg * 2 + b.side : -1;
		dead.push_back(d);
	}
	Dictionary out;
	out["walkers"] = walkers;
	out["vehicles"] = vehicles;
	out["dead"] = dead;
	out["count_walkers"] = t.ped_count;
	return out;
}

// What a charge of `strength` does at a distance: the model's numbers, for the tests (kg, overpressure kPa, impulse
// kPa-ms, duration ms, the pressure a structure feels, the probabilities of death and of a burst eardrum).
Dictionary CityCore::blast_probe(float strength, float distance) const {
	const BlastParams &p = state_->catalog.blast;
	const float kg = blast::charge_kg(p, strength);
	const blast::Airblast a = blast::airblast(p, kg, distance);
	Dictionary d;
	d["kg"] = kg;
	d["pressure_kpa"] = a.pressure_kpa;
	d["impulse_kpa_ms"] = a.impulse_kpa_ms;
	d["duration_ms"] = a.duration_ms;
	d["felt_kpa"] = blast::felt_pressure(p, a);
	d["lethality"] = blast::lethality(p, a.pressure_kpa);
	d["eardrum"] = blast::eardrum(p, a.pressure_kpa);
	d["collapse_range_m"] = blast::range_at_pressure(p, kg, p.collapse_kpa);
	d["light_range_m"] = blast::range_at_pressure(p, kg, p.light_kpa);
	d["crater_radius_m"] = blast::crater_radius(p, kg, blast::Surface::EARTH);
	return d;
}

} // namespace city
