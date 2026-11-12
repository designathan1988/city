// Instance data whose source changes independently of the render clock. Compare the active rows, not spare capacity:
// a view/bounds change may update culling metadata without resending an identical instance buffer.
#pragma once

#include "render/rs_util.h"

#include <cstring>
#include <vector>

namespace city {

struct StaticInstanceDraw {
	MultiMeshDraw draw;
	void set(const godot::RID &scenario, const godot::RID &mesh, const godot::RID &material, InstanceRows &rows,
			const godot::AABB &bounds, bool shadows, RenderLayer layer) {
		const size_t floats = size_t(rows.count()) * size_t(rows.stride());
		const float *data = rows.data();
		if (!prepared_ || sent_.size() != floats || (floats && std::memcmp(sent_.data(), data, floats * sizeof(float)) != 0)) {
			sent_.resize(floats);
			if (floats) std::memcpy(sent_.data(), data, floats * sizeof(float));
			draw.set(scenario, mesh, material, rows, bounds, shadows, layer);
			prepared_ = true;
			settle_motion_ = !draw.shadow_only && rows.count() > 0;
		} else if (settle_motion_) {
			// RD retains the previous transform page for two frames after a change. One identical publication on the
			// following frame gives a stopped row zero velocity; subsequent frames need no buffer traffic.
			draw.set(scenario, mesh, material, rows, bounds, shadows, layer);
			settle_motion_ = false;
		} else if (bounds != bounds_ && draw.inst.is_valid()) {
			auto *rs = godot::RenderingServer::get_singleton();
			rs->multimesh_set_custom_aabb(draw.mm, bounds);
			rs->instance_set_custom_aabb(draw.inst, bounds);
		}
		bounds_ = bounds;
	}
	bool needs_settle() const { return settle_motion_; }
	void free() {
		draw.free();
		std::vector<float>().swap(sent_);
		prepared_ = settle_motion_ = false;
	}

private:
	std::vector<float> sent_;
	godot::AABB bounds_;
	bool prepared_ = false, settle_motion_ = false;
};

} // namespace city
