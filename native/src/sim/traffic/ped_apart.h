// The room between two walkers, whatever their ways: its one owner, for the crowd step (ped_crowd.cpp). A walker's stripe and pace
// are chosen in its own way's frame from where the others stand (ped_stripes.cpp), which keeps apart those who share a way; two
// whose ways converge or cross (round a junction's corners, onto a kerb's queue) are kept apart here, in the world: of the room
// between two bodies (their distance less two bodies) each may take half in a tick, whichever way its step goes, so the two steps
// together never bring them nearer than two bodies. Each answers for its half alone (the reciprocal share of the velocity obstacle
// methods, van den Berg et al. 2011), so every walker's bounds read only where the bodies stand at the step's start. The step a
// walker takes is the nearest one to the step it chose that keeps its bounds: toward each body near it no more than its half,
// ahead no further than it chose and never back, aside no further than its own way lets it this tick. So two whose ways converge
// walk on shoulder to shoulder, each giving way to its own side, and one with no room to give stands.
#pragma once

#include "core/math.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace city {
namespace traffic {

// A walker in one tick of the crowd: where it stands, the axes of its step (its way and its left), the step it chose (`far` ahead,
// `aside` to its left), how far to its left its step may end (`lo`..`hi`: its room, the furniture and its pace aside, from its own
// choice), and the step it takes, in the world.
struct Stride {
	V2 at;
	V2 ahead = V2(1, 0), left = V2(0, -1);
	float far = 0, aside = 0;
	float lo = 0, hi = 0;
	V2 taken;
};

// A bound on a step x: n . x <= most.
struct StepBound {
	V2 n;
	float most = 0;
};

// What keep_apart works with, kept by its caller between steps for its room: the bodies filed by place, one walker's bounds.
struct ApartRoom {
	std::vector<std::pair<int64_t, int>> cells;
	std::vector<StepBound> bounds;
};

// The step each stride takes (Stride::taken). `touch`: two bodies.
void keep_apart(std::vector<Stride> &strides, float touch, ApartRoom &room);

} // namespace traffic
} // namespace city
