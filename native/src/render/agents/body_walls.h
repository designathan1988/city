// The walls round somebody thrown: the volumes of the buildings near them (the bodies of the records, kept until the city changes).
#pragma once

#include "render/agents/ragdoll.h"
#include "sim/state/city_state.h"

#include <unordered_map>
#include <vector>

namespace city {

class BodyWalls {
public:
	std::vector<BodyWall> near(const CityState &s, float x, float z, float reach);

private:
	std::unordered_map<int, std::vector<BodyWall>> known_;
	uint64_t revision_ = ~uint64_t(0);
};

} // namespace city
