// What the building renderer's files share of the stream (building_renderer.cpp, building_renderer_view.cpp): the levels a chunk's
// request asks, the chunks a superchunk holds a side, and the clock the far meshes' rebuilds are paced by.
#pragma once

#include "core/grid.h"

#include <chrono>

namespace city::building_stream {

constexpr int BAND_BIT_NEAR = 2, BAND_BIT_MID = 4, BAND_BIT_BAYS = 8; // the levels a request asks (requested_mask_)
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // per axis

// The steady clock in seconds.
inline double now_s() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

} // namespace city::building_stream
