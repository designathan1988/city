// The money's probes for the tests (area probes, api/city_probes.h): a balance set to reach a refusal without spending it, and
// how a road would be built and what it would cost (sim/economy/road_economy.h build_lengths, build_value).
#include "api/city_probes.h"

#include "sim/economy/road_economy.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/dictionary.hpp>

#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

void CityProbes::probe_set_money(double money) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->money = money;
}

Dictionary CityProbes::probe_road_build(int profile, int structure, double height_a, double height_b, double len) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const RoadBuild b{ Structure(structure), height_a, height_b };
	const BuildLengths l = build_lengths(state_->catalog.tune, b, len);
	Dictionary d;
	d["ground"] = l.ground;
	d["embankment"] = l.embankment;
	d["trench"] = l.trench;
	d["bridge"] = l.bridge;
	d["tunnel"] = l.tunnel;
	d["value"] = build_value(*state_, profile, len, b);
	return d;
}

} // namespace city
