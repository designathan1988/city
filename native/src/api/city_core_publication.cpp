// Candidate city publication: prepare without mutating the live city, then rebind owners and retire old source roots.
#include "api/city_core.h"

#include <algorithm>

namespace city {

std::unique_ptr<CityState> CityCore::make_candidate() const {
	auto candidate = std::make_unique<CityState>();
	std::shared_lock<std::shared_mutex> map(state_->map_mutex);
	candidate->catalog = state_->catalog;
	candidate->buildings.designs = state_->buildings.designs;
	// The immutable catalog is shared/copied; authored city versions belong to a fresh table and generation.
	candidate->buildings.designs.edited = std::make_shared<b40::BodyTable>();
	candidate->style = state_->style;
	candidate->revision = state_->revision;
	candidate->saved_revision = state_->saved_revision;
	candidate->edit_revision = state_->edit_revision;
	candidate->lane_graph_serial = state_->lane_graph_serial;
	candidate->camera_x.store(state_->camera_x.load());
	candidate->camera_z.store(state_->camera_z.load());
	candidate->marking_style.store(state_->marking_style.load());
	const TrafficLevels &from = state_->traffic_levels;
	TrafficLevels &to = candidate->traffic_levels;
	to.traffic.store(from.traffic.load()); to.people.store(from.people.load());
	to.demand.store(from.demand.load()); to.population.store(from.population.load());
	to.vehicles_on.store(from.vehicles_on.load()); to.people_on.store(from.people_on.load());
	to.growth_on.store(from.growth_on.load()); to.crowd_on.store(from.crowd_on.load());
	return candidate;
}

void CityCore::publish_city(std::unique_ptr<CityState> candidate, bool saved, bool paused) {
	// All decode/generation/traffic preparation succeeded. The old simulation and its graph keep running until here.
	sim_.stop();
	state_->jobs->cancel_all();
	candidate->revision = std::max(candidate->revision, state_->revision + 1);
	candidate->edit_revision = std::max(candidate->edit_revision, state_->edit_revision + 1);
	if (saved) candidate->saved_revision = candidate->revision;
	render_.reset(); // old instances and inboxes retire with their city, before any new poses are drawn
	retired_states_.push_back(std::move(state_));
	state_ = std::move(candidate);
	attach_areas(); // preserve the script objects held by CityStore; replace only their state bindings
	inspect_ = InspectCache();
	junction_marks_ = JunctionMarkCache();
	wear_stamp_ns_ = 0;
	pose_stats_ = PoseStats();
	need_full_rebuild_ = true;
	sim_.start(*state_, paused);
	collect_retired();
}

void CityCore::collect_retired() {
	retired_states_.erase(std::remove_if(retired_states_.begin(), retired_states_.end(), [](const auto &s) {
		return s->jobs->drained(); // no active capture can still read this city's catalog/body/terrain
	}), retired_states_.end());
}

} // namespace city
