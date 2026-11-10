// The segments a chunk owns: those whose middle point falls in the chunk (a segment crossing two chunks is drawn
// once, by the chunk holding its middle). `fn(segment id)` runs for each.
#pragma once

#include "core/grid.h"
#include "sim/state/city_view.h"

#include <cstdint>

namespace city {

// The middle of a segment lies on its curve, inside its box: the segments whose box meets the chunk's are all that can own it.
template <typename F>
void each_owned_segment(const CityView &s, int64_t key, F &&fn) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int sid) {
		double mx, mz;
		s.roads.segs[size_t(sid)].q.at(0.5, mx, mz);
		if (chunk_of(float(mx), float(mz)) == key) fn(sid);
	});
}

} // namespace city
