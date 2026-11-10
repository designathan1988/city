// The state CityCore keeps for its areas, apart from the facade: the background save, what the inspector asks again and again,
// the snapshot's counts and the pose publication's statistics.
#pragma once

#include "sim/buildings/b40/record.h"
#include "sim/roads/junction.h"
#include "sim/state/pose_buffer.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

// A building's volume on its lot: the lot's frame, the rectangle there and the height of its top over the ground (clear_reach).
struct Solid {
	b40::LotFrame f;
	float x0, y0, x1, y1, top;
};

// The save runs on a thread of its own (city_core_persist.cpp).
struct SaveJob {
	std::thread thread;
	std::atomic<int> state{ 0 }; // 0 idle, 1 writing, 2 ok, 3 error
	std::atomic<int64_t> bytes{ 0 };
	std::string error; // set by the save thread before the state turns 3, read only then
};

// What the Inspect tool asks for each frame, kept until the network changes (city_core_inspect.cpp).
struct InspectCache {
	std::vector<SharpNode> sharp; // impossible_nodes, for this version of the network
	int sharp_version = -1;
};

// What pose_stats reports since its last call (city_core_probe_render.cpp).
struct PoseStats {
	int frames = 0, held = 0, behind = 0, publications = 0, early = 0, late = 0;
	int64_t last_tick_ns = 0, gap_min_ns = 0, gap_max_ns = 0;
};

} // namespace city
