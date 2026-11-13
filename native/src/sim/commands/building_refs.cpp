// What names a building by id, dropped or carried where the building goes (building_refs.h).
#include "sim/commands/building_refs.h"

#include "sim/commands/interior_commands.h"

#include <algorithm>

namespace city {

void building_gone(CityState &s, int id) {
	if (s.interior.on && s.interior.target == id) set_interior_view(s, InteriorRequest{}); // off: the chunks it showed are marked
	std::vector<Fire> &fires = s.damage.fires;
	fires.erase(std::remove_if(fires.begin(), fires.end(), [id](const Fire &f) { return f.building == id; }), fires.end());
}

void building_moved(CityState &s, int from, int to) {
	if (s.interior.on && s.interior.target == from) {
		s.interior.target = to; // the same building (its seed goes with it): the record's chunks are marked by its placing
		s.interior.revision++;
	}
	for (Fire &f : s.damage.fires)
		if (f.building == from) f.building = to;
}

} // namespace city
