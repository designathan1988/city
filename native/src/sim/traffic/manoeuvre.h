// A vehicle's path off the lanes: legs of cubic Hermite curves sampled once by arc length, forward or reversing, out of a
// bay to its lane or from the lane into a bay. Pure geometry; the speeds come from traffic.json (ManoeuvreSpeeds).
#pragma once

#include "core/math.h"

#include <vector>

namespace city {

struct OffPose {
	float x = 0, z = 0, angle = 0; // heading of the body's nose in the XZ plane
};

struct ManoeuvreLeg {
	V2 p0, t0, p1, t1; // ends and unit directions of motion there
	bool reverse = false;
};

struct ManoeuvreSpeeds {
	float forward = 0, backward = 0, ramp = 0, creep = 0;
};

class Manoeuvre {
public:
	Manoeuvre() = default;
	explicit Manoeuvre(const std::vector<ManoeuvreLeg> &legs, float tangent);
	float length() const { return length_; }
	bool done() const;
	float s = 0; // metres driven along it
	float speed(const ManoeuvreSpeeds &v) const; // up to speed and down to rest within each leg
	void advance(float ds);
	OffPose pose_at(float at) const;
	OffPose pose() const { return pose_at(s); }

private:
	struct Sample {
		float x, z, angle, at;
	};
	int leg_now() const;
	std::vector<Sample> samples_;
	std::vector<float> ends_;
	std::vector<bool> reverse_;
	float length_ = 0;
};

// Where a vehicle leaving through a gate whose last inner point is `edge` is once across the footway, `into` metres onto
// the lane (p, t); false when the gate lies ahead of that point of the lane.
bool across_footway(const V2 &edge, const V2 &p, const V2 &t, float into, float least, V2 &out);
// Out of a bay: backing into the aisle, then forward along `via` to the lane point (p, t). `lead` metres of aisle when the
// bay has no way of its own.
Manoeuvre bay_departure(const V2 &at, const V2 &open, const std::vector<V2> &via, const V2 &p, const V2 &t, float lead, float into, float least,
		float tangent);
// From the lane (`from`, heading `angle`) into a bay, nose first.
Manoeuvre bay_arrival(const V2 &from, float angle, const V2 &at, const V2 &open, const std::vector<V2> &via, float lead, float into, float least,
		float tangent);

} // namespace city
