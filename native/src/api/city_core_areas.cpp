// The areas of the core's script API: objects of their own (the Build drawer, the Landscape drawer, public transport, the test
// probes, the Street tab, the F7 laboratory), each on the core's state and render. The core makes them with itself and frees them before anything they point at;
// game/store/city_store.gd routes a name of game/store/store_api.gd to the area that runs it (CityCore.area).
#include "api/city_builder.h"
#include "api/city_core.h"
#include "api/city_landscape.h"
#include "api/city_probes.h"
#include "api/city_street.h"
#include "api/city_transit.h"
#include "api/lab/city_lab.h"

#include <godot_cpp/core/memory.hpp>

using namespace godot;

namespace city {

void CityCore::make_areas() {
	builder_ = memnew(CityBuilder);
	landscape_ = memnew(CityLandscape);
	transit_ = memnew(CityTransit);
	probes_ = memnew(CityProbes);
	street_ = memnew(CityStreet);
	lab_ = memnew(CityLab);
	attach_areas();
}

void CityCore::attach_areas() {
	builder_->attach(state_.get(), &render_);
	landscape_->attach(state_.get(), &render_);
	transit_->attach(state_.get(), &render_);
	probes_->attach(state_.get(), &render_);
	street_->attach(state_.get(), &render_);
}

// memdelete of an extension object destroys it through the engine (godot-cpp core/memory.hpp: object_destroy of its owner).
void CityCore::free_areas() {
	if (builder_) memdelete(builder_);
	if (landscape_) memdelete(landscape_);
	if (transit_) memdelete(transit_);
	if (probes_) memdelete(probes_);
	if (street_) memdelete(street_);
	if (lab_) memdelete(lab_);
	builder_ = nullptr;
	landscape_ = nullptr;
	transit_ = nullptr;
	probes_ = nullptr;
	street_ = nullptr;
	lab_ = nullptr;
}

// An unknown name is the store's mistake: null, and the store's assert on its table says which.
Object *CityCore::area(const StringName &name) const {
	if (name == StringName("builder")) return builder_;
	if (name == StringName("landscape")) return landscape_;
	if (name == StringName("transit")) return transit_;
	if (name == StringName("probes")) return probes_;
	if (name == StringName("street")) return street_;
	if (name == StringName("lab")) return lab_;
	return nullptr;
}

} // namespace city
