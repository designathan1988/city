// What the building renderer's files share of the stream (building_renderer.cpp, building_renderer_view.cpp): the levels a chunk's
// request asks, what the view asks for a chunk, the chunks a superchunk holds a side, and the clock the far meshes' rebuilds are paced by.
#pragma once

#include "core/grid.h"

#include <chrono>
#include <cstdint>

namespace city::building_stream {

constexpr int BAND_BIT_NEAR = 2, BAND_BIT_MID = 4, BAND_BIT_BAYS = 8; // the levels a request asks (requested_mask_)
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // per axis

// What the view asks of the stream for a chunk: the levels (`mask`, the bits above), nearest first by `d`.
struct Want {
	float d;
	int64_t key;
	uint8_t mask;
};

// The steady clock in seconds.
inline double now_s() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

} // namespace city::building_stream
