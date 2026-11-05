// CityProbes: what the tests and the captures ask of the city, an area of the core (api/city_core.h): the parked, the kerb, the docks
// and the lots (city_core_park_probe.cpp), the lots' clearance and the amber (city_core_clearance_probe.cpp), who is on foot
// and the clips they are drawn in (city_core_people.cpp) and the job queue (city_core_jobs.cpp). Made and freed by CityCore; game/store/city_store.gd reaches it through
// CityCore.area("probes").
#pragma once

#include <godot_cpp/classes/object.hpp>

namespace city {

struct CityState;
class RenderWorld;

class CityProbes : public godot::Object {
	GDCLASS(CityProbes, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render) {
		state_ = state;
		render_ = render;
	}
	godot::Dictionary park_sample(); // where the capture looks at bays, lots, bicycles and docks
	bool camera_clear(const godot::Vector3 &at, float radius, bool vehicles = true); // nothing of a lot at the camera's height, no street piece, parked car or, if asked, vehicle where a capture would stand
	godot::Variant kerb_busy(); // where somebody is moving between a door and the footway now (null: nobody)
	int dock_count(); // lorry docks of the whole map
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

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
};

} // namespace city
