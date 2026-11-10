// The shadow of the far fleet. A camera-wide MultiMesh has one box, so every cascade of the sun draws all of it: thousands of cars
// four times over for a shadow the near cascades never hold. Here one mesh (a car's hull with no wheels, each model scaled to it)
// is drawn shadow-only in sectors of the map, each with a box of its own: the engine's cull leaves a sector out of the cascades
// that do not reach it. Sector size and how far the hull stands inside the car: agents.json far_shadow.
#pragma once

#include "render/agents/people_renderer.h"
#include "render/mesh_builder.h"
#include "render/rs_util.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

godot::AABB mesh_outline(const MeshBuilder &b);

class FleetShadow {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const MeshBuilder &hull, float sector_m, float inset, int models);
	void set_model(int model, const MeshBuilder &body); // the hull fitted to this model's outline
	void add(int model, const Pose &p);
	void draw(); // this frame's sectors, and those left empty go
	void free();

private:
	struct Fit { // the hull on a model: its size, and where its middle goes along the model's sides and length
		godot::Vector3 scale;
		float side = 0, along = 0;
	};
	struct Sector {
		InstanceRows buf{ MM_PLAIN_STRIDE };
		MultiMeshDraw draw;
	};
	godot::RID scenario_, material_;
	OwnedMesh hull_;
	godot::AABB hull_box_;
	float sector_m_ = 0, inset_ = 1;
	std::vector<Fit> fit_;
	std::unordered_map<int64_t, Sector> sectors_;
};

} // namespace city
