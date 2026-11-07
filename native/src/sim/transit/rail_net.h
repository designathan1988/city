// One mode's tracks as a graph and the legs found on it (rail_net.cpp), kept while the tracks of that mode are the same: the same
// tracks give the same graph and the same leg between two points. The legs are kept by their two end points (their bits); those the
// last making of the loops did not use are let go, so it holds the plan's legs, not their history.
#pragma once

#include "sim/transit/transit_data.h"

#include <array>
#include <cstdint>
#include <map>
#include <vector>

namespace city {

struct RailLeg {
	std::vector<V2> points;
	uint64_t used = 0; // the making of the loops that last asked for it
};

struct RailNet {
	bool made = false;
	float join = 0;
	std::vector<std::vector<V2>> tracks; // the points of the mode's tracks it was made from, in order
	RailGraph graph;
	std::map<std::array<uint32_t, 4>, RailLeg> legs;
	uint64_t pass = 0; // makings of the loops so far
	RailSearch search;
};

namespace transit {

// A making of the loops begins: the net made again only when the mode's tracks (or the joining reach) are not those it was made from.
void ready_net(const TransitData &t, TransitMode mode, float join, RailNet &net);
// The points along the tracks between two points of the net (rail_path), found once while the net stands.
const std::vector<V2> &leg_of(RailNet &net, const V2 &from, const V2 &to);
// The making of the loops is over: the legs it did not ask for go.
void drop_unused_legs(RailNet &net);

} // namespace transit
} // namespace city
