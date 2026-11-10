// What people hold (heldProps.ts): one small mesh a thing, a MultiMesh a thing, drawn with the instance of the body that holds it. The bone that
// poses it is the hand's, read from the same palette at the same phase (project/render/shaders/people_held.gdshader), so nothing is worked out
// on this side but who holds what. Data: agents.json "held".
#pragma once

#include "render/render_context.h"
#include "render/row_keeper.h"
#include "render/rs_util.h"
#include "sim/state/pose_buffer.h"

#include <vector>

namespace city {

class HeldThings {
public:
	// `act_names`: each PedAct's name, as the things are called in the data (a name no thing has holds nothing).
	bool init(const RenderContext &ctx, const godot::Dictionary &held, const char *const *act_names, godot::PackedStringArray &errors);
	void reset();
	godot::RID material() const { return material_; }
	void begin();
	// The row of what somebody doing `act` holds, nullptr when it is nothing; its instance is the body's.
	float *row(uint64_t key, int act);
	void draw(const godot::RID &scenario, const godot::AABB &box, RenderLayer layer);

private:
	struct Thing {
		OwnedMesh mesh;
		MultiMeshDraw draw;
		InstanceRows rows;
	};
	std::vector<Thing> things_;
	int thing_of_[int(PedAct::COUNT)] = {};
	godot::RID material_;
	RowKeeper keeper_;
};

} // namespace city
