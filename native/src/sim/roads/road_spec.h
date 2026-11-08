// A road the player composes (Roadcraft world/roads/profile.ts): its elements from the left edge to the right looking from a to b, what stops
// it from being built, and the strips it is drawn with. A road of the catalogue reads back as elements too, so any road can be the start.
#pragma once

#include "sim/catalog/catalog.h"

#include <string>
#include <vector>

namespace city {

// How wide an element may be, metres (the editor's limits too).
constexpr float LANE_LEAST_M = 2, LANE_MOST_M = 6, WALK_LEAST_M = 1, WALK_MOST_M = 12, MEDIAN_LEAST_M = 1, MEDIAN_MOST_M = 8;

// A problem and the elements it is about, by index (none: the road as a whole). `problem` names its text (profile.problem.<problem>).
struct SpecIssue {
	std::string problem;
	std::vector<int> elements;
};

std::vector<SpecIssue> spec_issues(const RoadSpec &spec, RoadClass road_class);
RoadSpec spec_of(const Profile &p);
// Right edge to left edge, as the catalogue writes them: a footway and a median at their whole width, kerbs taken out of them.
std::vector<Strip> spec_strips(const RoadSpec &spec, float kerb);
// The catalogue's strip a composed element draws as: the editor's footway is a sidewalk, its cycle track a bike strip.
StripKind strip_of(SpecKind kind);
// What the line between the two directions is, on a road that does not say.
std::string spec_centre(const RoadSpec &spec);

} // namespace city
