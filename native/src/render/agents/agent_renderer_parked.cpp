// The cars standing in their bays filed by cell for the agent renderer (agent_renderer.h): the list the simulation published, laid
// out in cells vehicle_r_ wide once per publication, and every car near the camera read off the cells round it, not the whole list.
#include "render/agents/agent_renderer.h"

#include "core/grid.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
int parked_cell(float v, float cell) { return int(std::floor(v / cell)); }
} // namespace

void AgentRenderer::index_parked(const std::shared_ptr<const std::vector<ParkedRecord>> &parked) {
	parked_indexed_ = parked;
	parked_cells_.clear();
	if (!parked) return;
	for (size_t i = 0; i < parked->size(); i++) {
		const Pose &at = (*parked)[i].at;
		parked_cells_[cell_key(parked_cell(at.x, vehicle_r_), parked_cell(at.z, vehicle_r_))].push_back(uint32_t(i));
	}
}

// Every car within vehicle_r_ of the camera on the ground lies in a cell the square round it touches; a cell more on each side
// takes in what rounding may put across its edge. The rest are farther: the loop over them would only skip them.
const std::vector<uint32_t> &AgentRenderer::parked_near(const godot::Vector3 &cam) {
	parked_near_.clear();
	const int x0 = parked_cell(cam.x - vehicle_r_, vehicle_r_) - 1, x1 = parked_cell(cam.x + vehicle_r_, vehicle_r_) + 1;
	const int z0 = parked_cell(cam.z - vehicle_r_, vehicle_r_) - 1, z1 = parked_cell(cam.z + vehicle_r_, vehicle_r_) + 1;
	for (int j = z0; j <= z1; j++)
		for (int i = x0; i <= x1; i++) {
			const auto it = parked_cells_.find(cell_key(i, j));
			if (it != parked_cells_.end()) parked_near_.insert(parked_near_.end(), it->second.begin(), it->second.end());
		}
	std::sort(parked_near_.begin(), parked_near_.end()); // the list's order: the rows are placed as before
	return parked_near_;
}

} // namespace city
