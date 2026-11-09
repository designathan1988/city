// The small closed loops of road (Roadcraft lab src/sim/intersections/cycles.ts): roundabouts and rings of streets whose summed
// length is under ring_max_storage_m, found as each directed segment's shortest way back to itself without a U-turn. Nobody more is
// let onto a loop from outside it once its vehicles fill ring_meter_share of what it can hold: a ring full to the brim locks itself.
// Pure C++; written only by ring_meter.cpp, made again when CityState::lane_graph_serial moves.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

struct RingLoop {
	std::vector<int> ds; // its directed segments, sorted
	float storage = 0; // metres of lane a queue can stand on (each segment's length times its lanes)
};

struct RingIndex {
	uint64_t built = 0; // CityState::lane_graph_serial + 1 of the graph it was made from (0: never made)
	std::vector<RingLoop> loops;
	std::vector<std::vector<int>> of_ds; // per directed segment: the loops it is on
	// Scratch of a search, kept for its room: best cost and parent per segment (valid at this search's stamp), and the open heap.
	std::vector<float> best;
	std::vector<int> parent;
	std::vector<uint32_t> stamp;
	uint32_t cur = 0;
	std::vector<std::pair<float, int>> open;
};

} // namespace city
