// CityTransit: public transport's script API, an area of the core (api/city_core.h): stops, stations, tracks, lines, the plan, the
// footway and the pickers (city_core_transit.cpp). Made and freed by CityCore; game/store/city_store.gd reaches it through
// CityCore.area("transit").
#pragma once

#include <godot_cpp/classes/object.hpp>

namespace city {

struct CityState;
class RenderWorld;

class CityTransit : public godot::Object {
	GDCLASS(CityTransit, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render) {
		state_ = state;
		render_ = render;
	}
	godot::Dictionary transit_stop(const godot::Vector3 &p), transit_terminal(const godot::Vector3 &p), transit_remove(const godot::Vector3 &p);
	godot::Dictionary transit_station(const godot::Vector3 &p, int mode);
	godot::Dictionary transit_track(const godot::PackedVector3Array &points, int mode);
	godot::Dictionary transit_line(const godot::PackedInt32Array &stops);
	godot::Dictionary transit_drop_line(int id);
	godot::Dictionary transit_vehicles(int id, int vehicles);
	godot::Dictionary transit_plan(), transit_run(); // transit_run: what is running, for the tests
	godot::Dictionary transit_footway(const godot::Vector3 &p, float reach); // for the captures
	void transit_xray(bool on);
	void transit_preview(const godot::PackedVector3Array &points, int mode);
	int transit_pick(const godot::Vector3 &p, int mode, bool any);
	godot::Vector3 transit_snap(const godot::Vector3 &p, int mode);

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
};

} // namespace city
