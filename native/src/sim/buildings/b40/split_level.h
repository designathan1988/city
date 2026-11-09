// The split level of a grown body on a slope (internal to sim/buildings/b40): the step its back takes (split_level.cpp, Roadcraft
// world/buildings/splitLevel.ts stepToSlope), the garage under it (garageUnder) and the drives' ramps that bound both
// (world/growth/lotPlan.ts yardRamp, garageRamp). The lot's answer to the step is lot_slope.cpp's (follow_slope, lot_site.h).
#pragma once

#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/lot_layout.h"
#include "sim/buildings/b40/lot_site.h"

namespace city::b40 {

constexpr int SLOPE_WHOLE = 2; // the steps of a whole storey (half storeys counted)

struct SplitStep {
	float y = 0, lift = 0; // the line (lot y) behind which the back stands at its own level, and that level over the floor
	int steps = 0; // half storeys up (+) or down (-)
};

// The drive's ramp to a yard or a garage at a level of its own: where it starts and ends (lot y), its grade.
struct DriveRamp {
	float start = 0, end = 0, grade = 0;
	float reach() const { return grade * (end - start); }
};
bool yard_ramp(const LotSlopeShape &K, const LotPlan &p, DriveRamp &out); // a car park's or a loading yard's; false: none
bool garage_ramp(const LotSlopeShape &K, const LotPlan &p, DriveRamp &out); // a house's, down to a garage under its back

// Steps the back of the body (lot frame, a lot W x D) to the ground of `site`; `yard` and `max_lift` when a car park or a loading
// yard behind goes to the back's level with it. False: the body stays level.
bool step_to_slope(const Data &data, Body &b, const LotSite &site, float W, float D, const Rect *yard, float max_lift, SplitStep &out);
// A garage door in the lower ground floor of a house stepped a whole storey down, on the drive's side: its middle on the back face
// (lot frame) and its bay's width. False: no bay takes one.
bool garage_under(const Data &data, Body &b, bool left, V2 &door, float &width);

} // namespace city::b40
