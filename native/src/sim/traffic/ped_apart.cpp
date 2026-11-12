// The room between two walkers, whatever their ways (ped_apart.h): the bodies filed by place in cells as wide as two bodies and
// the two longest steps of the tick, so whoever a walker could meet this tick stands in its cell or one beside it. A walker's
// step is a point of the plane and each of its bounds a half of it (toward a body near it no more than half the room between the
// two; ahead no further than it chose and not back; aside within its reach): the step it takes is the point of all the halves
// nearest the step it chose, which is that step itself, its foot on one bound's line, or the point where two bounds' lines meet.
#include "sim/traffic/ped_apart.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float HALF = 0.5f; // of the room between two bodies, what each may take in a tick: the other half is the other's
constexpr int CELL_ROW_BITS = 32;
constexpr int BESIDE = 1; // a cell and the cells beside it: every body within a cell's width of a place
constexpr float KEPT_M = 1e-5f; // a bound is kept within this: the rounding of a point worked out on its line
constexpr float PARALLEL = 1e-6f; // two bounds whose lines turn less than this from each other meet nowhere

int64_t band_of(float v, float cell) { return int64_t(std::floor(v / cell)); } // which band of that width a coordinate falls in
int64_t cell_key(int64_t i, int64_t j) { return (i << CELL_ROW_BITS) ^ int64_t(uint32_t(int32_t(j))); }

V2 chosen(const Stride &s) { return s.ahead * s.far + s.left * s.aside; }

bool keeps(const std::vector<StepBound> &bounds, const V2 &x) {
	for (const StepBound &b : bounds)
		if (b.n.dot(x) > b.most + KEPT_M) return false;
	return true;
}

// The point that keeps every bound nearest `want`; false when no point keeps them all. Every bound's `n` is a unit vector.
bool nearest_kept(const std::vector<StepBound> &bounds, const V2 &want, V2 &out) {
	if (keeps(bounds, want)) {
		out = want;
		return true;
	}
	float best = BIG;
	const auto offer = [&](const V2 &x) {
		const float d = (x - want).len2();
		if (d < best && keeps(bounds, x)) {
			best = d;
			out = x;
		}
	};
	for (size_t i = 0; i < bounds.size(); i++) {
		const StepBound &a = bounds[i];
		if (a.n.dot(want) > a.most) offer(want - a.n * (a.n.dot(want) - a.most)); // its foot on a bound it breaks
		for (size_t j = i + 1; j < bounds.size(); j++) {
			const StepBound &b = bounds[j];
			const float turn = a.n.cross(b.n);
			if (std::fabs(turn) < PARALLEL) continue;
			offer(V2(a.most * b.n.y - a.n.y * b.most, a.n.x * b.most - a.most * b.n.x) / turn); // where the two lines meet
		}
	}
	return best < BIG;
}

} // namespace

void keep_apart(std::vector<Stride> &strides, float touch, ApartRoom &room) {
	float longest = 0;
	for (Stride &s : strides) {
		s.taken = chosen(s);
		// what a step may be at the most: the step chosen, or as far aside as its reach goes
		longest = std::max(longest, std::max(s.taken.len(), std::max(std::fabs(s.lo), std::fabs(s.hi))));
	}
	if (longest <= 0) return; // nobody steps
	const float meet = touch + 2 * longest; // two bodies farther apart than this cannot meet within the tick
	std::vector<std::pair<int64_t, int>> &cells = room.cells;
	std::vector<StepBound> &bounds = room.bounds;
	cells.clear();
	for (int i = 0; i < int(strides.size()); i++)
		cells.emplace_back(cell_key(band_of(strides[size_t(i)].at.x, meet), band_of(strides[size_t(i)].at.y, meet)), i);
	std::sort(cells.begin(), cells.end());
	for (int i = 0; i < int(strides.size()); i++) {
		Stride &s = strides[size_t(i)];
		const V2 want = chosen(s);
		if (want.x == 0 && want.y == 0) continue; // it stands: nothing of its own to hold
		bounds.clear();
		const int64_t ci = band_of(s.at.x, meet), cj = band_of(s.at.y, meet);
		bool held = false;
		for (int64_t a = ci - BESIDE; a <= ci + BESIDE; a++)
			for (int64_t b = cj - BESIDE; b <= cj + BESIDE; b++) {
				const int64_t key = cell_key(a, b);
				for (auto it = std::lower_bound(cells.begin(), cells.end(), std::make_pair(key, -1)); it != cells.end() && it->first == key; ++it) {
					if (it->second == i) continue;
					const V2 to = strides[size_t(it->second)].at - s.at;
					const float apart = to.len();
					if (apart <= 0 || apart >= meet) continue; // on the same spot no direction leads toward it; too far to meet
					// toward it, half of the room between the two at most; none when they stand in one another already
					bounds.push_back(StepBound{ to / apart, std::max(0.f, (apart - touch) * HALF) });
					held = held || bounds.back().n.dot(want) > bounds.back().most;
				}
			}
		if (!held) continue; // the step it chose keeps its half toward everybody
		// its own way's bounds: ahead no further than it chose and not back, aside within its reach
		bounds.push_back(StepBound{ s.ahead, s.far });
		bounds.push_back(StepBound{ -s.ahead, 0.f });
		bounds.push_back(StepBound{ s.left, s.hi });
		bounds.push_back(StepBound{ -s.left, -s.lo });
		// no step keeps them all (its way makes it give way where a body leaves no room): it stays on its way's side of things,
		// as near standing as its reach aside lets it
		if (!nearest_kept(bounds, want, s.taken)) s.taken = s.left * clampf(0.f, s.lo, s.hi);
	}
}

} // namespace traffic
} // namespace city
