// The chunk that draws a segment, the one owner of that rule: the chunk its curve's middle point falls in (a segment crossing two
// chunks is drawn once, by the chunk holding its middle), and the segments a chunk owns.
#pragma once

#include "core/grid.h"
#include "sim/state/city_view.h"

#include <cstdint>

namespace city {

// The chunk (of side `size`) holding segment g's curve's middle.
inline int64_t segment_chunk(const RoadSegment &g, float size = CHUNK) {
	double mx, mz;
	g.q.at(0.5, mx, mz);
	return chunk_of(float(mx), float(mz), size);
}

// `fn(segment id)` for each segment the chunk owns. The middle of a segment lies on its curve, inside its box: the segments whose
// box meets the chunk's are all that can own it.
template <typename F>
void each_owned_segment(const CityView &s, int64_t key, F &&fn) {
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int sid) {
		if (segment_chunk(s.roads.segs[size_t(sid)]) == key) fn(sid);
	});
}

} // namespace city
