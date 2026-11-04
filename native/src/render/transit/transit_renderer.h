// Public transport on the map: the plan's tracks, stations, bus stops and metro entrances as one static mesh, made again when the plan
// changes (it is small), and the cars of the trains as a MultiMesh, each tinted with its line's colour. Tables from data/render/transit.json.
#pragma once

#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

namespace city {

class TransitRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void sync(const CityState &s); // the plan's mesh, when it changed; caller holds the map shared
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	int drawn_cars() const { return drawn_cars_; }

private:
	godot::RID scenario_, material_, instance_;
	godot::Dictionary look_;
	OwnedMesh plan_, car_;
	MultiMeshDraw cars_;
	uint64_t plan_seen_ = ~uint64_t(0);
	float radius_ = 0;
	int drawn_cars_ = 0;
};

} // namespace city
