// Which directed segments can lead to which (Roadcraft lab src/sim/routing/laneReach.ts): the strongly connected components of the
// segment graph, for cars and for bicycles, and the components each one asked from reaches, so a search for a goal that cannot be
// reached is answered before it walks the map, and a trip is drawn only toward a goal it can reach. Pure C++; written only by
// route_reach.cpp, made again when CityState::lane_graph_serial moves.
#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

constexpr int REACH_MODES = 2; // cars, then bicycles (which enter only the segments a bicycle may ride)

struct RouteReach {
	uint64_t built = 0; // CityState::lane_graph_serial + 1 of the graph it was made from (0: never made)
	std::vector<int> comp[REACH_MODES]; // per directed segment: its component, -1 off the graph
	// The condensation: per component, the components its segments lead into (no repeats).
	std::vector<std::vector<int>> next[REACH_MODES];
	// Per component asked from: the components it reaches, itself included, as bits (made on the first question after a build).
	std::unordered_map<int, std::vector<uint64_t>> ahead[REACH_MODES];
	// Scratch of a build, kept for its room: the segment graph as compressed rows, and the component search's stacks.
	std::vector<int> row, col, index, low, stack, call, at;
	std::vector<uint8_t> on;
};

} // namespace city
