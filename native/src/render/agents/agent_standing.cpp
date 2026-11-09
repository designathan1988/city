// The cars outside the near traffic (agent_renderer.cpp): those standing in bays, by the cells round the camera, at the level their
// distance gives; and the far traffic's cars (PoseFrame::far) past the near reach, each at the far level in the view, casting nothing.
#include "render/agents/agent_renderer.h"

#include "core/grid.h"
#include "core/random.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

void AgentRenderer::place_parked(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const LevelRule &rule, int &placed) {
	auto &bufs = seen_rows_;
	auto &casts = cast_rows_;
	if (f.parked != parked_indexed_) index_parked(f.parked);
	if (f.parked) for (uint32_t index : parked_near(cam)) {
		const ParkedRecord &rec = (*f.parked)[index];
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		const float dx = rec.at.x - cam.x, dy = rec.at.y - cam.y, dz = rec.at.z - cam.z, flat2 = dx * dx + dz * dz;
		if (flat2 > rule.r2 || type < 0 || type >= VEHICLE_TYPES) continue;
		const float d2 = flat2 + dy * dy;
		const int model = model_of(type, paint);
		const bool shown = in_view(view, model, rec.at, d2);
		const uint64_t key = uint64_t(rec.id) * ROW_KINDS + PARKED_ROW;
		const int level = level_of(rule, key, d2);
		if (shown) count_for_cap(rule, level, d2);
		if (level == FAR_LEVEL) fleet_shadow_.add(model, rec.at);
		if (!shown && level == FAR_LEVEL) continue;
		placed += shown ? 1 : 0;
		agent_row(shown ? keeper_.place(key, model * DRAWS + level, bufs[model][level]) : casts[model][level].add(), rec.at, 1.f, float(paint % paints_), 0.f, NOBODY_ABOARD, AGENT_STEER_MID, hash_float(rec.id * SEED_SALT)); // no indicator, wheels straight
		drawn_vehicles_++;
	}
}

void AgentRenderer::place_far(const PoseFrame &f, const Vector3 &cam, const ViewCull &view) {
	for (InstanceRows &rows : far_rows_) rows.restart();
	drawn_far_ = 0;
	const float near2 = vehicle_r_ * vehicle_r_;
	for (const ParkedRecord &rec : f.far) {
		const int type = int(rec.look & LOOK_TYPE_MASK), paint = int((rec.look >> LOOK_PAINT_SHIFT) & LOOK_TYPE_MASK);
		if (type < 0 || type >= VEHICLE_TYPES) continue;
		const float dx = rec.at.x - cam.x, dy = rec.at.y - cam.y, dz = rec.at.z - cam.z, d2 = dx * dx + dy * dy + dz * dz;
		const int model = model_of(type, paint);
		if (dx * dx + dz * dz <= near2 || !in_view(view, model, rec.at, d2)) continue; // within the near reach the near model draws
		agent_row(far_rows_[model].add(), rec.at, 1.f, float(paint % paints_), 0.f, 0.f, AGENT_STEER_MID, hash_float(rec.id * SEED_SALT));
		drawn_far_++;
	}
	const AABB box(Vector3(cam.x - far_traffic_r_, cam.y - AABB_HALF_H, cam.z - far_traffic_r_), Vector3(far_traffic_r_ * 2, AABB_HALF_H * 2, far_traffic_r_ * 2));
	for (int i = 0; i < AGENT_MODELS; i++) far_draws_[i].set(scenario_, meshes_[i][FAR_LEVEL].rid, RID(), far_rows_[i], box, false, LAYER_VEHICLES);
}

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
const std::vector<uint32_t> &AgentRenderer::parked_near(const Vector3 &cam) {
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
