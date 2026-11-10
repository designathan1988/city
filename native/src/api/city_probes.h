// CityProbes: what the tests and the captures ask of the city, an area of the core (api/city_core.h): the parked, the kerb, the docks
// and the lots (city_core_park_probe.cpp), the lots' clearance and the amber (city_core_clearance_probe.cpp), who is on foot
// and the clips they are drawn in (city_core_people.cpp), the job queue (city_core_jobs.cpp) and the money (city_core_economy_probe.cpp).
// Made and freed by CityCore; game/store/city_store.gd reaches it through CityCore.area("probes").
#pragma once

#include <godot_cpp/classes/object.hpp>

namespace city {

struct CityState;
class RenderWorld;
class Simulation;

class CityProbes : public godot::Object {
	GDCLASS(CityProbes, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render, Simulation *sim) {
		state_ = state;
		render_ = render;
		sim_ = sim;
	}
	godot::Dictionary park_sample(); // where the capture looks at bays, lots, bicycles and docks
	godot::Array plan_all_yards(); // the yard planner's bench: every yard planned now (city_core_park_probe.cpp)
	bool camera_clear(const godot::Vector3 &at, float radius, bool vehicles = true); // nothing of a lot at the camera's height, no street piece, parked car or, if asked, vehicle where a capture would stand
	godot::Variant kerb_busy(); // where somebody is moving between a door and the footway now (null: nobody)
	int dock_count(); // lorry docks of the whole map
	godot::Dictionary look_twins(float reach_m, float alike); // of each building and its nearest neighbour within reach, the pairs closer in looks than `alike`
	godot::Dictionary lot_clearance(); // how far the cars in lots stay from what is solid along their ways
	godot::Array vehicles_near(const godot::Vector3 &at, float radius); // where each stands on its lane
	// Who is on foot within `radius` of `at` (0: everywhere): ages, parties, modes, doors, and the nearest of each to look at.
	godot::Dictionary people_census(const godot::Vector3 &at, float radius);
	void set_city_hour(float hour); // tests: the city's clock at that hour of the day
	godot::Array people_clips() const; // tests: the clips of people.bin
	bool show_people_clip(const godot::String &name); // captures: everybody at rest in that clip
	// Tests: the amber's decision for a vehicle class in numbers (traffic::amber_stops), the amber a speed in km/h asks.
	bool amber_stops(float v, float stop_m, float top, float left, int type) const;
	float amber_wanted(float kmh) const;
	godot::Dictionary job_pool_probe() const;
	// Tests: the vertices and instances builders took while their thread held the map, by job lane, since the last `reset`
	// (sim/state/map_hold.h); `counted` false in a DLL that does not count them (the export's).
	godot::Dictionary map_hold(bool reset);
	// Tests: a thread of its own takes `mib` MiB of the core's allocator, frees all but one block in `keep_one_in`, gives back,
	// frees the rest and ends: what the allocator held before, at the most, thinned and after (MiB), and its threads.
	godot::Dictionary memory_churn(int mib, int keep_one_in) const;
	godot::Dictionary allocator_probe() const; // what the core's allocator holds of the system now: committed MiB and threads with a heap
	godot::Dictionary flow_replay(); // tests: the translated flow classes alone through a fixed script (city_core_flow_probe.cpp)
	int flow_junction(); // tests: a three-arm junction on automatic with no signal, or -1
	godot::Dictionary flow_push(int n, int seconds); // tests: that much traffic through it, the choice applied; empty while the clock runs
	godot::Dictionary flow_state(int n); // tests: what is measured, and the control of `n`
	godot::Dictionary street_watch(float near_m); // tests: who is on a crossing between junctions, what stopped for them, who is turning back
	void probe_set_money(double money); // tests: a balance to reach the refusal without spending it
	// Tests: how a road would be built and what it would cost (structure: Structure, sim/roads/road_grade.h).
	godot::Dictionary probe_road_build(int profile, int structure, double height_a, double height_b, double len);
	// Tests: the road heights solved on a scene of its own (city_core_grade_probe.cpp), the 4.0's elevation.spec cases.
	godot::Dictionary grade_probe(const godot::Dictionary &scene);
	int undo_steps(); // tests: the steps the player can undo
	godot::String graph_fault(); // tests: what the lane graph broke first, since the city opened (the debug DLL looks at every change)
	godot::PackedStringArray traffic_hashes(int ticks); // tests: a hash of the fleet after each of `ticks` steps; empty while the clock runs
	godot::Dictionary state_hash(); // tests: the city as a number, whole and by part (city_core_state_probe.cpp)
	godot::Dictionary bay_census(); // how many bays the city has and how many different ones (building_renderer_probe.cpp)
	godot::Dictionary far_form() const; // tests: the buildings' far level built on the spot, and what of it its nearest distance cannot show

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
	Simulation *sim_ = nullptr; // the core's clock: a probe that steps the city on this thread asks it to hold still first
};

} // namespace city
