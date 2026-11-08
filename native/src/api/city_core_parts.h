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

// What a building's volumes are made from (city_core_sight.cpp solids_key): its body, by the edited version's stamp or by what a body
// is made from, and its lot's frame. Equal keys, equal volumes.
struct SolidsKey {
	uint64_t stamp = 0;
	uint32_t seed = 0;
	float x = 0, z = 0, w = 0, d = 0, rot = 0;
	uint16_t floors = 0;
	uint8_t zone = 0, fn = 0;
	bool operator==(const SolidsKey &o) const {
		return stamp == o.stamp && seed == o.seed && x == o.x && z == o.z && w == o.w && d == o.d && rot == o.rot && floors == o.floors &&
				zone == o.zone && fn == o.fn;
	}
};

// A building's volumes and the key they were made under.
struct CachedSolids {
	SolidsKey key;
	std::vector<Solid> solids;
	bool made = false;
};

// What the Inspect tool and the camera ask for each frame, kept until the network or the city changes (city_core_inspect.cpp).
struct InspectCache {
	std::unordered_map<int, CachedSolids> solids; // clear_reach, sight_pull, eye_free: each building asked, kept while its key stays
	std::vector<SharpNode> sharp; // impossible_nodes, for this version of the network
	int sharp_version = -1;
};

// What pose_stats reports since its last call (city_core_probe_render.cpp).
struct PoseStats {
	int frames = 0, held = 0, behind = 0, publications = 0, early = 0, late = 0;
	int64_t last_tick_ns = 0, gap_min_ns = 0, gap_max_ns = 0;
};

} // namespace city
