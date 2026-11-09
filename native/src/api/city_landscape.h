// CityLandscape: the Landscape drawer's script API, an area of the core (api/city_core.h): weather, clouds, fog, painted ground,
// trees, elements, biome and the fields the screen passes read (city_core_nature.cpp). Made and freed by CityCore; game/store/city_store.gd
// reaches it through CityCore.area("landscape").
#pragma once

#include <godot_cpp/classes/object.hpp>
#include "sim/commands/nature_commands.h"

namespace city {

struct CityState;
class RenderWorld;

class CityLandscape : public godot::Object {
	GDCLASS(CityLandscape, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render) {
		state_ = state;
		render_ = render;
	}
	godot::Dictionary set_weather(const godot::Dictionary &change);
	godot::Dictionary add_cloud(const godot::Dictionary &c);
	godot::Dictionary edit_cloud(int id, const godot::Dictionary &c);
	godot::Dictionary remove_cloud(int id);
	godot::Dictionary scatter_clouds(int count, const godot::Dictionary &like, float variation);
	godot::Dictionary clear_clouds(), clear_fog(), clear_gullies(), clear_trees(), clear_elements(), clear_relief();
	int cloud_at(const godot::Vector3 &origin, const godot::Vector3 &direction, float drift_x, float drift_z) const;
	godot::Dictionary fog_brush(const godot::Vector3 &p, float radius, float strength, float height, float speed, bool erase);
	godot::Dictionary paint_brush(const godot::Vector3 &p, float radius, float strength, float hardness, int kind);
	godot::Dictionary tree_brush(const godot::Vector3 &p, float radius, int mode, int kind, const godot::Dictionary &brush);
	godot::Dictionary element_brush(const godot::Vector3 &p, float radius, int kind, const godot::Dictionary &brush, bool erase);
	// The dabs a held brush lays along one pointer move, as one command (sim/commands/commands.h lay_dabs): the same dab
	// as the single-dab method at every point, derived state settled once when the command ends.
	godot::Dictionary fog_path(const godot::PackedVector3Array &points, float radius, float strength, float height, float speed, bool erase);
	godot::Dictionary paint_path(const godot::PackedVector3Array &points, float radius, float strength, float hardness, int kind);
	godot::Dictionary tree_path(const godot::PackedVector3Array &points, float radius, int mode, int kind, const godot::Dictionary &brush);
	godot::Dictionary element_path(const godot::PackedVector3Array &points, float radius, int kind, const godot::Dictionary &brush, bool erase);
	godot::Array effects_near(const godot::Vector3 &p, float reach) const;
	godot::Dictionary set_biome(int region);
	godot::Dictionary landscape() const;
	godot::Dictionary cloud(int id) const; // one cloud as landscape() lists it; empty when there is none by that id
	int64_t sky_revision() const;
	// The render's fields as RenderWorld::sync last took them in: read-only, with the revision that says a new one is in.
	godot::Dictionary fog_field() const;
	int64_t fog_revision() const;
	godot::Dictionary canyon_field() const;
	int64_t canyon_revision() const;
	godot::Dictionary shore_field() const;
	int64_t shore_revision() const;
	godot::Dictionary ecology_field() const;
	int64_t ecology_revision() const;

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
};

} // namespace city
