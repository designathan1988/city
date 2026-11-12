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
	godot::Dictionary vehicle_bodies(const godot::Vector3 &at, float radius); // the drawn bodies: in one another, over a footway
	bool footway_under(const godot::Vector3 &at); // the footway rule vehicle_bodies judges by (sim/street/footway.h on_footway)
	// Who is on foot within `radius` of `at` (0: everywhere): ages, parties, modes, doors, and the nearest of each to look at.
	godot::Dictionary people_census(const godot::Vector3 &at, float radius);
	// The walkers within `radius` of `at` one by one, and those standing in one another, on a carriageway away from a crossing or in a vehicle.
	godot::Dictionary walker_bodies(const godot::Vector3 &at, float radius);
	// Tests: the crowd's rule for the room between two walkers on bodies laid out by the test (each one's place, its step of this tick
	// in the world, how far either side of that step's line it may give way): two bodies' width, and the step each takes
	// (city_core_crowd_probe.cpp).
	godot::Dictionary walkers_apart(const godot::PackedVector2Array &at, const godot::PackedVector2Array &steps, const godot::PackedFloat32Array &aside) const;
	// Tests: every piece of the street of the whole map as a body standing in the world, and every two of them nearer than the hand's
	// gap two pieces keep, by the two kinds, with the first case of each (city_core_bodies_probe.cpp).
	// In volume too: each piece the boxes of its parts (sim/roads/standing_body.h); a pair counts when a part of one passes through a
	// part of the other, or stands nearer it than `clearance_m`.
	godot::Dictionary bodies_audit(float clearance_m);
	// Every way round a corner with no crosswalk (the whole map), walked from each place across its sidewalk: how many, how many
	// touch a carriageway, and the first of them with the run and the node's arms and corners (city_core_people.cpp corner_case).
	godot::Dictionary corner_ways();
	// The lots' bays read near the camera whose way reaches a lane: how many, how many through a gate whose foot lies in a corner or a
	// crosswalk, and the first (bay, building, street, the foot's arc, the lane's point).
	godot::Dictionary lot_gates();
	// Tests and captures: every lot's car gate facing a kerb, those whose kerb the road surface draws above the lip a vehicle access
	// keeps, and where a few of the gates stand (city_core_kerb_probe.cpp).
	godot::Dictionary drive_kerbs();
	// Tests: every lot's front against the footway it faces, the lot's covers there against the footway's top (city_core_lot_probe.cpp).
	godot::Dictionary lot_fronts();
	// Tests: every lot's edges closed once, by its boundary or by a ground block standing on them, never both nor neither (city_core_lot_probe.cpp).
	godot::Dictionary lot_bounds();
	// Tests: every lot's rectangle against the paving as it is drawn round it, none standing on it (city_core_lot_paving_probe.cpp).
	godot::Dictionary lot_paving();
	// Tests: every lot's covers as they are drawn, a later one over an earlier wherever they lie over one another (city_core_lot_cover_probe.cpp).
	godot::Dictionary lot_covers();
	// Tests: the city's road surfaces built on the spot, their top faces at one height over the same ground (city_core_road_probe.cpp).
	godot::Dictionary road_overlaps();
	// Bodies laid on every sidewalk, inside their room and past it, put onto the sidewalk where they stand (pedestrians.cpp
	// onto_sidewalk) and posed again: how many, the farthest one moved and where (street, side, arc, place across, metres).
	godot::Dictionary onto_sidewalk_moves();
	void set_city_hour(float hour); // tests: the city's clock at that hour of the day
	godot::Array people_clips() const; // tests: the clips of people.bin
	bool show_people_clip(const godot::String &name); // captures: everybody at rest in that clip
	// Tests: the decision of a light gone against a vehicle of a class in numbers (traffic::amber_stops): going `v`, `line_m` before
	// its line, which it may take at `top`, with `left` seconds of amber (none: the light is red). The amber a speed in km/h asks.
	bool light_holds(float v, float line_m, float top, float left, int type) const;
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
	godot::Dictionary flow_push(int n, int seconds); // tests: that much traffic through it, the choice applied; empty while the clock runs
	godot::Dictionary flow_state(int n); // tests: what is measured, and the control of `n`
	godot::Dictionary street_watch(float near_m); // tests: who is on a crossing between junctions, what stopped for them, who is turning back
	// Tests: a scene by construction at the crossing between junctions nearest `at` (city_core_scene_probe.cpp): somebody stood on
	// the footway before it whose trip ends on the other sidewalk past it (id -1 with no room; its look, the street and sidewalk it
	// is bound for); a car stood on the lane into the crossing beside the kerb that walker (on the crosswalk) is crossing to, `metres`
	// before its line at `speed` (-1 with no room); and what the two are doing now.
	godot::Dictionary scene_walker(const godot::Vector3 &at);
	int scene_car(int walker, float metres, float speed);
	godot::Dictionary scene_watch(int walker, int64_t look, int car);
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
	// Tests: every lot's plants as they are drawn (each instance's model measured point by point) against the room its element is:
	// how many, how many past it in plan or over its height, the first (render/trees/garden_census.h).
	godot::Dictionary garden_rooms() const;
	// Tests and captures: what stands on a record's lot, a row an element (kind, x, z, width, depth, height, base, angle).
	godot::Array lot_elements(int id) const;
	// Tests: one plant element of a kind ("tree", "shrub", "hedge", "flowers") `w` by `d` and `h` tall, laid `turn` radians to its
	// lot, stood alone with the draws of `seed`: the box its instances draw, their top and how far past the room they stand.
	godot::Dictionary garden_fit(const godot::String &kind, float w, float d, float h, float turn, int seed) const;
	// Tests: `count` bodies of `zone` (storeys capped at `cap`, 0: none) made on the spot on a lot `w` by `d`, from `seed` on, and what
	// their lots' layout laid: the planters, those with nothing growing in them, the plants that do not stand on their planter's soil
	// (city_core_lot_parts_probe.cpp). No city is read.
	godot::Dictionary lot_parts(int zone, int cap, float w, float d, int seed, int count) const;
	// Tests: `count` bodies of an open lot (a park, a square, a sports court: BuildingFn `fn`) `w` by `d` made on the spot, from `seed`
	// on: their benches, those laid with their long side along the way they look, those that do not look at the cover they stand by;
	// and their grove: each tree's room against the lot's lines, the other parts, the other trees' rooms, and its trunk's ground.
	godot::Dictionary open_lot_parts(int fn, float w, float d, int seed, int count) const;
	// Tests: a square of the city's plan laid on a block of lots given by hand (rows of x, z, width, depth, facing), no city read, and
	// every square of the city: each tree's room against the square's ground, the other trees' rooms, the plan's step and edge
	// clearance, and its walks (city_core_square_probe.cpp).
	godot::Dictionary square_fit(const godot::Array &lots, int seed) const;
	godot::Dictionary square_rooms() const;
	godot::Dictionary ground_drawn(const godot::Vector3 &at) const; // tests: the ground's height as the terrain's shader reads it there (render/terrain_bridge.h drawn)
	// Tests: the street lamps' pools within `radius` of (x, z): drawn on the GPU, wanted by each lit lamp, and the lamps' lenses (RenderWorld::lamp_pools).
	godot::Dictionary lamp_pools(float x, float z, float radius) const;
	// Tests: every furniture chunk drawn, its spans and which level of its wires and of its poles' shadows the engine is told to draw
	// (PropRenderer::wire_draws): exactly one wire level where there are spans, never two.
	godot::Array wire_draws() const;
	godot::Dictionary surface_probe(const godot::Array &arrays, const godot::Dictionary &lods, int64_t format) const; // tests: render/surface_data.h's surface of these arrays

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
	Simulation *sim_ = nullptr; // the core's clock: a probe that steps the city on this thread asks it to hold still first
};

} // namespace city
