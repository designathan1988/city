// The names streets are given (Roadcraft world/roads/streetNames.ts nameFor): a street without a name plate takes one from
// the style's list, the form of the name by its kind of road, none repeated. A given name is kept as keys of the player's
// translations (GivenName), so a plate reads in the player's language. The plates stand at its corners (street_spots.h).
#pragma once

#include "sim/street/street_works.h"

#include <string>
#include <vector>

namespace city {

class RoadNetwork;
struct ChangeLog;

// Where a street's name goes and what it says, as a STREETNAME item.
struct StreetName {
	float x = 0, z = 0;
	GivenName given;
};

// The segment a name item names, or -1: the nearest within the reach of a name (street_names.reach_m).
int name_segment(const RoadNetwork &net, const StreetItem &item);
// Names for the streets of `segs` that have none, each once, in the order of their lowest segment. Reads the streets the
// names already given hold from works.names: settled (settle_name_plates) since the roads and the names last changed.
std::vector<StreetName> names_for(const RoadNetwork &net, const StreetWorks &works, std::vector<int> segs);
// The name item of the street segment `seg` is a piece of (the first made, by id), or none: its player's words or its given
// name. Reads works.names as settle_name_plates left it.
const StreetItem *name_item_of(const StreetWorks &works, int seg);
// The door numbers along the road of `seg` (its pieces through its cuts): metres from the start of its street, the end of
// the street nearer the map's middle, and whether the even numbers lie on the right of `seg`'s own way (a to b). The
// practice of Brazilian towns: the city of Sao Paulo's decree 49.346/2008, art. 34, numbers a street from its starting point, the even
// numbers on the right and the odd ones on the left of who walks from that point.
struct DoorNumbers {
	int from = 0, to = 0;
	bool even_right = true;
};
DoorNumbers door_numbers(const RoadNetwork &net, int seg);
// Brings works.names up to the roads and the name items: made again are the names added, put back or removed, those
// whose street holds a segment the roads changed (net.take_edits), and those near a road that came, went or changed casing
// (the segment a name names may be another); the prop chunks of their plates before and after are marked in `log`.
// Runs after the roads' shapes and heights are settled (the plates stand on them).
void settle_name_plates(StreetWorks &works, RoadNetwork &net, ChangeLog &log);

} // namespace city
