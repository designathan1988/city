// CityStreet: the Street tab's script API, an area of the core (api/city_core.h): items and rows of them, barriers, pole runs
// and their preview, what is taken away, the furniture set and the lighting new roads are laid with, the plates to paint and the counts the
// tests read (city_core_street.cpp). Made and freed by CityCore; game/store/city_store.gd reaches it through
// CityCore.area("street").
#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace city {

struct CityState;
class RenderWorld;

class CityStreet : public godot::Object {
	GDCLASS(CityStreet, godot::Object)

public:
	void attach(CityState *state, RenderWorld *render) {
		state_ = state;
		render_ = render;
	}
	godot::Dictionary preview_item(const godot::Vector3 &p, int kind, float reach);
	godot::Dictionary place_item(const godot::Vector3 &p, int kind, int sign, const godot::String &text, float reach);
	godot::Dictionary place_item_row(const godot::Vector3 &a, const godot::Vector3 &b, int kind, float every_m);
	godot::Dictionary preview_barrier(const godot::PackedVector3Array &pts, const godot::Vector3 &cursor, int kind);
	godot::Dictionary build_barrier(const godot::PackedVector3Array &pts, int kind);
	godot::Dictionary remove_street(const godot::Vector3 &p, float reach);
	godot::Dictionary preview_pole_run(const godot::Vector3 &a, const godot::Vector3 &b, float reach, int lamps, int kind);
	godot::Dictionary build_pole_run(const godot::Vector3 &a, const godot::Vector3 &b, float reach, int lamps, int kind);
	void show_pole_preview(const godot::Array &poles);
	void set_road_furniture(int level); // what new roads are laid with: 0 complete, 1 basic, 2 none
	int road_furniture();
	void set_road_height(float metres); // what the road tool draws at, over the grade line (Roadcraft roadHeightOffset)
	float road_height();
	void set_road_walls(bool on); // the sides of the cuttings of new roads: retaining walls, or a batter
	bool road_walls();
	void set_road_power(bool on); // how new roads are lit: the power line's poles carry the lamps (on), or lighting columns
	bool road_power();
	godot::Array sign_cells(); // the plates to paint (project/render/sign_sheet.gd)
	godot::Array sign_cells_laid() const; // tests: every plate of the city with its cell (SignAtlas::laid)
	godot::Dictionary street_counts();
	godot::PackedVector3Array street_poles(); // where the player's poles stand (tests)
	godot::Dictionary furniture_census();
	godot::Array furniture_spots(int seg);

protected:
	static void _bind_methods();

private:
	CityState *state_ = nullptr; // the core's: it outlives this area (CityCore frees its areas first)
	RenderWorld *render_ = nullptr;
};

} // namespace city
