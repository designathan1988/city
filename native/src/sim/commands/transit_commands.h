// The player's public transport (Roadcraft editor/transitTools.ts): stops, terminals, tracks, stations and lines. Each command checks
// what it is given and replaces the plan whole, as one undo step. Callers hold map_mutex exclusively, inside run_edit.
#pragma once

#include "sim/commands/commands.h"
#include "sim/transit/transit_data.h"

namespace city {

EditResult transit_place_stop(CityState &s, float x, float z); // on the footway nearest the point
EditResult transit_toggle_terminal(CityState &s, float x, float z); // the bus stop at the point
EditResult transit_place_station(CityState &s, float x, float z, TransitMode mode); // on the track at the point
EditResult transit_lay_track(CityState &s, const std::vector<V2> &points, TransitMode mode);
EditResult transit_make_line(CityState &s, const std::vector<int> &stops);
EditResult transit_remove_at(CityState &s, float x, float z); // the stop, else the track, at the point
EditResult transit_remove_line(CityState &s, int id);
EditResult transit_set_line_vehicles(CityState &s, int id, int vehicles);
// The end or corner of a track of `mode` near a point, else the nearest point on one, else the point itself.
V2 transit_snap_track(const CityState &s, const V2 &p, TransitMode mode);

} // namespace city
