// What a building job reads from the map, copied while it holds the map lock shared for a moment (BuildingRenderer::gather): the
// records of its chunks, in query order, and the final ground under their lots. The build itself runs without the lock.
#pragma once

#include "render/buildings/interior_emit.h"
#include "sim/state/city_state.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace city {

struct BuildingInputs {
	std::vector<Building> records;
	std::shared_ptr<const std::unordered_set<uint32_t>> broken; // standing in pieces (RuinPieces): only their lots are drawn here
	std::vector<int> ids; // the id of each record
	InteriorView view; // the Interior layer as the job started
	HeightPatch ground;
};

} // namespace city
