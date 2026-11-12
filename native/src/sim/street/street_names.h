// The names streets are given (Roadcraft world/roads/streetNames.ts nameFor): a street without a name plate takes one from
// the style's list, the prefix by its kind of road, none repeated. The plates stand at its corners (street_spots.h).
#pragma once

#include "sim/street/street_works.h"

#include <string>
#include <vector>

namespace city {

class RoadNetwork;
class NamedPlates;

// Where a street's name goes and what it says, as a STREETNAME item.
struct StreetName {
	float x = 0, z = 0;
	std::string text;
};

// The segment a name item names, or -1: the nearest within the reach of a name (street_names.reach_m).
int name_segment(const RoadNetwork &net, const StreetItem &item);
// Names for the streets of `segs` that have none, each once, in the order of their lowest segment.
std::vector<StreetName> names_for(const RoadNetwork &net, const StreetWorks &works, const NamedPlates &base, std::vector<int> segs);

} // namespace city
