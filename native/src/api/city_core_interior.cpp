// The Interior layer as the script API: set the view (a command, under the map lock) and read it back with the floors of
// the building and the lamps of the floor seen.
#include "api/city_core.h"

#include "render/buildings/interior_lamps.h"
#include "sim/commands/interior_commands.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

Dictionary CityCore::set_interior(bool on, int target, int level, int dir) {
	Dictionary d;
	map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::COMMAND);
	if (!state_->ready) return d;
	InteriorRequest r;
	r.on = on;
	r.target = target;
	r.level = level;
	r.dir = dir;
	const char *problem = set_interior_view(*state_, r);
	d["ok"] = problem[0] == 0;
	d["error"] = String(problem);
	d["on"] = state_->interior.on;
	d["level"] = state_->interior.cut.level;
	return d;
}

Dictionary CityCore::interior() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	d["on"] = s.interior.on;
	d["target"] = s.interior.target;
	d["level"] = s.interior.cut.level;
	d["dir"] = s.interior.cut.dir;
	d["revision"] = int(s.interior.revision);
	d["floors"] = s.interior.on ? interior_floors(s, s.interior.target) : 0;
	PackedVector3Array lamps;
	if (s.interior.on && s.interior.target >= 0)
		for (const Vector3 &p : interior_lamps(s, s.interior.target, s.interior.cut.level)) lamps.push_back(p);
	d["lamps"] = lamps;
	return d;
}

int CityCore::interior_revision() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return int(state_->interior.revision);
}

Dictionary CityCore::interior_cost(int id, int level, int dir) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const BuildingInteriorCost c = render_.interior_cost(*state_, id, level, dir);
	Dictionary d;
	d["shell_triangles"] = c.shell_triangles;
	d["shell_boxes"] = c.shell_boxes;
	d["inside_triangles"] = c.inside_triangles;
	d["inside_boxes"] = c.inside_boxes;
	d["inside_bytes"] = c.inside_bytes;
	return d;
}

} // namespace city
