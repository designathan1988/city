// Ordered fog edits and their persistent spatial index. Every mutation updates the index before publication.
#pragma once

#include "core/bounds_index.h"

#include <cstdint>
#include <cstddef>
#include <vector>

namespace city {
struct FogDef;

// One dab of painted fog: how far it takes the fog towards full at its centre, how high the bank stands and how fast
// the wind carries it through.
struct FogDab {
	float x = 0, z = 0, radius = 0, strength = 0, height = 0, speed = 0;
	uint8_t erase = 0;
};

struct FogSample {
	float density = 0, height = 0, speed = 0;
};
// The fog the dabs leave at a point: each in turn, a smooth falloff from its centre to its rim, adding towards one (or,
// erasing, taking away); height and speed follow the dabs by what each laid.
void fog_at(const std::vector<const FogDab *> &dabs, float x, float z, const FogDef &d, FogSample &out);

class FogPaint {
public:
	static constexpr float BUCKET_M = 160.f; // same partition used by the rasterizer
	bool empty() const { return dabs_.empty(); }
	size_t size() const { return dabs_.size(); }
	const std::vector<FogDab> &records() const { return dabs_; }
	void push_back(const FogDab &dab);
	void pop_back();
	void clear();
	std::vector<FogDab> take();
	void restore(std::vector<FogDab> dabs);
	std::vector<FogDab> window(float x0, float z0, float x1, float z1) const;
private:
	static Bounds2 bounds(const FogDab &dab);
	static bool indexed(const Bounds2 &box);
	std::vector<FogDab> dabs_;
	BoundsIndex index_;
};
} // namespace city
