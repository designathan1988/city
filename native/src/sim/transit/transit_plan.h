// The transit's reading of its plan and of the road edits (transit_plan.cpp), and what of the buses' running it shares with that reading
// (transit_sim.cpp). Internal to sim/transit.
#pragma once

#include "sim/transit/transit_sim.h"

namespace city {
namespace transit {

// The run brought to the plan and the roads: made again from the plan when it moved; after an edit of the roads alone, the bus stops on
// the roads it changed read again, only the lines calling at a stop whose lane moved placed and sent on again, and the level crossings
// it reaches made again (Roadcraft 4.0 transit.ts followLanes).
void read_plan(CityState &s);
// A stop of the plan by id and a bus's line, by the places read when the plan last moved (the plan never moves without
// CityState::transit_revision moving); null when gone.
const TransitStop *stop_of(const CityState &s, int id);
const TransitLine *line_of(const CityState &s, const Bus &b);
// A bus sent on along the streets to its next stop that it can reach (transit_sim.cpp).
void send_bus(CityState &s, const TransitLine &l, Bus &b, Vehicle &v);

} // namespace transit
} // namespace city
