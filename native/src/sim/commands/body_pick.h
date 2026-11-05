// What a ray from the camera meets of the buildings: the record, the block, the face, the storey and the bay. The Build tools'
// clicks and hovers; a hit tells them what to edit. World axes: x and z on the map, y up.
#pragma once

#include <cstdint>

namespace city {

struct CityState;

struct Ray3 {
	float ox = 0, oy = 0, oz = 0, dx = 0, dy = 0, dz = 0; // `d` of length 1
};

struct BodyHit {
	int id = -1; // the record, -1 for nothing
	int volume = -1;
	int side = -1; // the face hit, -1 for the block's top
	int storey = 0, level = 0, bay = 0;
	float t = 0; // along the ray, metres
};

// The nearest building face within `reach` metres along the ray.
BodyHit pick_body(const CityState &s, const Ray3 &ray, float reach);

} // namespace city
