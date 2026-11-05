// The areas of the core's script API: objects of their own (the Build drawer, the Landscape drawer, public transport, the test
// probes), each on the core's state and render. The core makes them with itself and frees them before anything they point at;
// game/store/city_store.gd routes a name of game/store/store_api.gd to the area that runs it (CityCore.area).
#include "api/city_builder.h"
#include "api/city_core.h"
#include "api/city_landscape.h"
#include "api/city_probes.h"
#include "api/city_transit.h"

#include <godot_cpp/core/memory.hpp>

using namespace godot;

namespace city {

void CityCore::make_areas() {
	builder_ = memnew(CityBuilder);
	builder_->attach(state_.get(), &render_);
	landscape_ = memnew(CityLandscape);
	landscape_->attach(state_.get(), &render_);
	transit_ = memnew(CityTransit);
	transit_->attach(state_.get(), &render_);
	probes_ = memnew(CityProbes);
	probes_->attach(state_.get(), &render_);
}

// memdelete of an extension object destroys it through the engine (godot-cpp core/memory.hpp: object_destroy of its owner).
void CityCore::free_areas() {
	if (builder_) memdelete(builder_);
	if (landscape_) memdelete(landscape_);
	if (transit_) memdelete(transit_);
	if (probes_) memdelete(probes_);
	builder_ = nullptr;
	landscape_ = nullptr;
	transit_ = nullptr;
	probes_ = nullptr;
}

// An unknown name is the store's mistake: null, and the store's assert on its table says which.
Object *CityCore::area(const StringName &name) const {
	if (name == StringName("builder")) return builder_;
	if (name == StringName("landscape")) return landscape_;
	if (name == StringName("transit")) return transit_;
	if (name == StringName("probes")) return probes_;
	return nullptr;
}

} // namespace city
