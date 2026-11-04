// The bomb and its probe as script API (sim/commands/strike_commands.cpp): the blow is one undoable edit; the probe
// counts what blows have left on the map, so a test can tell the explosion changed the city.
#include "api/city_core.h"

#include "api/edit_result.h"

using namespace godot;

namespace city {

Dictionary CityCore::strike(const Vector3 &p, int strength) {
	return run_edit(*state_, "cmd.strike", [&](CityState &s) { return strike_at(s, p.x, p.z, double(strength), double(p.y)); });
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
	d["fires"] = int(s.damage.fires.size());
	d["strikes"] = int(s.damage.strikes);
	d["buildings"] = s.buildings.count;
	d["pedestrians"] = s.traffic.ped_count;
	d["vehicles"] = s.traffic.vehicle_count;
	int segs = 0;
	for (const RoadSegment &g : s.roads.segs) segs += g.alive ? 1 : 0;
	d["segments"] = segs;
	return d;
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
