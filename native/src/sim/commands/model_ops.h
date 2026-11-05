// Build > Models: a model made into a body, a body set down in the city, where the pointer's ghost goes, and what a new building does
// to the ones it stands on. `model_body` knows nothing of the city; `place_body` is one undo step.
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/data.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/building_store.h"
#include "sim/commands/commands.h"

#include <string>

namespace city {

struct CityState;

// The model as a body in the lot frame, the same bytes each time.
void model_body(const b40::Data &data, const b40::ModelSpec &m, b40::Body &b);
// The lot made to fit what stands on the ground: moved to the lot frame, `plan.W` and `plan.D` set. Answers where the new front's
// middle lay in the old frame.
V2 fit_plan(b40::Body &b);

// Where a footprint goes with the pointer at (x, z): to a road first, then to a neighbour's bearing, then to the grid.
enum PlaceSnapKind : int { SNAP_GRID, SNAP_BUILDING, SNAP_ROAD };
struct PlaceSnap {
	double x = 0, z = 0, rot = 0; // the middle of the front, and the record's turn
	int kind = SNAP_GRID;
};
PlaceSnap snap_place(const CityState &s, float w, float d, double x, double z, double rot);

// Why the body cannot stand with the middle of its front at (ax, az): a strings.csv key, or "".
std::string place_problem(const CityState &s, const b40::Body &body, double ax, double az, double rot);
EditResult place_body(CityState &s, const b40::Body &body, double ax, double az, double rot, int fn, int zone);

// `other` (its lot framed by `of`) joins `into` (framed by `at`): its blocks keep their walls and roofs and its parts stay where they
// stand; nothing is cut.
void weld_body(b40::Body &into, const b40::LotFrame &at, const b40::Body &other, const b40::LotFrame &of, float turn);

} // namespace city
