// What people wear, not as an act (agents.json "worn"): a backpack on the back, an umbrella opened over the head while it rains. One mesh a
// thing, one MultiMesh a thing, an instance with the body's own numbers. Each thing is posed by the bone the data names, one the cooked
// skeleton has (the hand, the head, or the bone a ragdoll joint sits on), from that joint's rest place on each body
// (project/render/shaders/people_worn.gdshader): an open canopy rides on the chest above the head, never in the walk's swinging hand.
// Who wears what is a share of each age, from the walker's look hash, while the rain is at least the thing's own.
#pragma once

#include "render/agents/people_asset.h"
#include "render/data_reader.h"
#include "render/render_context.h"
#include "render/row_keeper.h"
#include "render/rs_util.h"
#include "sim/state/pose_buffer.h"

#include <godot_cpp/variant/dictionary.hpp>

#include <string>
#include <vector>

namespace city {

class WornThings {
public:
	bool init(const RenderContext &ctx, const DataValue &worn, const PeopleAsset &asset, godot::PackedStringArray &errors);
	void reset();
	godot::RID material() const { return material_; }
	void set_rain(float rain) { rain_ = rain; } // the map's weather, 0..1 (sim/nature/sky.h Weather::rain)
	void begin();
	int count() const { return int(things_.size()); }
	// Whether somebody of this look and age (PedAge) wears thing `k` in the weather of now.
	bool wears(int k, uint32_t look, int age) const;
	// The row of thing `k` on the walker `key`; its instance is the body's.
	float *row(uint64_t key, int k);
	void draw(const godot::RID &scenario, const godot::AABB &box, RenderLayer layer);
	// Each thing's instances drawn on the last frame, by its name (the tests).
	godot::Dictionary shown() const;

private:
	struct Thing {
		std::string name;
		float share[PED_AGES] = {};
		float rain_from = 0;
		OwnedMesh mesh;
		MultiMeshDraw draw;
		InstanceRows rows;
		RowKeeper keeper; // a walker wears several things: each its own rows
	};
	std::vector<Thing> things_;
	godot::RID material_;
	float rain_ = 0;
};

} // namespace city
