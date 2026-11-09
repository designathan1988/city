// The names streets are given (Roadcraft world/roads/streetNames.ts nameFor): a street without a name plate takes one from
// the style's list, the prefix by its kind of road, none repeated. The plates stand at its corners (street_spots.h). What the
// inspector reads of a street: its name and a segment's house numbers (streetNameOf, streetNumbers).
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
	std::string text;
};

// The segment a name item names, or -1: the nearest within the reach of a name (street_names.reach_m).
int name_segment(const RoadNetwork &net, const StreetItem &item);
// Names for the streets of `segs` that have none, each once, in the order of their lowest segment. Reads the streets the
// names already given hold from works.names: settled (settle_name_plates) since the roads and the names last changed.
std::vector<StreetName> names_for(const RoadNetwork &net, const StreetWorks &works, std::vector<int> segs);
// Brings works.names up to the roads and the name items: made again are the names added, put back or removed, those
// whose street holds a segment the roads changed (net.take_edits), and those near a road that came, went or changed casing
// (the segment a name names may be another); the prop chunks of their plates before and after are marked in `log`.
// Runs after the roads' shapes and heights are settled (the plates stand on them).
void settle_name_plates(StreetWorks &works, RoadNetwork &net, ChangeLog &log);
// The name of the street segment `seg` runs in (the lab's streetNameOf): the first name item, by id, whose street holds it, as
// works.names settled them; "" when none does.
std::string street_name_of(const StreetWorks &works, int seg);
// A segment's place in its street's numbering (the lab's streetNumbers): the metres from the street's start where it begins and
// ends, and whether the even numbers are on its own right going a to b. The street starts at its end nearest the map's origin
// (the lower node id on a tie); a loop starts at its first segment's node a. False: the segment is not a live one.
struct StreetNumbers {
	int from = 0, to = 0;
	bool even_on_right = false;
};
bool street_numbers(const RoadNetwork &net, int seg, StreetNumbers &out);

} // namespace city
