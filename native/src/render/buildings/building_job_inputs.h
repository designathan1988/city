// What a building mesh job reads from the map (BuildingJobInputs): the records, the buildings standing in pieces, the Interior
// layer and the ground, copied under a short shared lock (BuildingRenderer::gather) so the build runs without the lock.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/buildings/interior/view.h"
#include "sim/terrain/height_patch.h"

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>

namespace city {

// What a job reads from the map, copied while it holds the map lock shared for a moment (gather): the records
// of its chunks, in query order, and the final ground under their lots. The build itself runs without the lock.
struct BuildingJobInputs {
	std::vector<Building> records;
	std::shared_ptr<const std::unordered_set<uint32_t>> broken; // standing in pieces (RuinPieces): only their lots are drawn here
	std::vector<int> ids; // the id of each record
	InteriorView view; // the Interior layer as the job started
	HeightPatch ground;
};

} // namespace city
