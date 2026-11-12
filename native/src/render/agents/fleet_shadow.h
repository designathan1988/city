// The shadow of the far fleet. A camera-wide MultiMesh has one box, so every cascade of the sun draws all of it: thousands of cars
// four times over for a shadow the near cascades never hold. Here one mesh (a car's hull with no wheels, each model scaled to it)
// is drawn shadow-only in sectors of the map, each with a box of its own: the engine's cull leaves a sector out of the cascades
// that do not reach it. Sector size and how far the hull stands inside the car: agents.json far_shadow.
#pragma once

#include "render/agents/people_renderer.h"
#include "render/mesh_builder.h"
#include "render/rs_util.h"
#include "render/static_instance_draw.h"

#include <memory>

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

class FleetShadow {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const MeshBuilder &hull, float sector_m, float inset, int models);
	void share_geometry(const FleetShadow &source); // independent instances, same immutable model resource
	void set_model(int model, const MeshBuilder &body); // the hull fitted to this model's outline
	void add(int model, const Pose &p);
	void draw(); // this frame's sectors, and those left empty go
	void reset(); // discard city instances; keep the model hull and its fits
	void free();

private:
	struct Fit { // the hull on a model: its size, and where its middle goes along the model's sides and length
		godot::Vector3 scale;
		float side = 0, along = 0;
	};
	struct Sector {
		InstanceRows buf{ MM_PLAIN_STRIDE };
		StaticInstanceDraw draw;
		godot::AABB bounds;
	};
	godot::RID scenario_, material_;
	std::shared_ptr<OwnedMesh> hull_;
	godot::AABB hull_box_;
	float sector_m_ = 0, inset_ = 1;
	std::vector<Fit> fit_;
	std::unordered_map<int64_t, Sector> sectors_;
};

} // namespace city
