// Driving/walking road-network route input/source lifetime and cooperative search; no CityState pointer or callback survives an advance.
#pragma once
#include "core/work_budget.h"
#include "sim/traffic/astar.h"
#include "sim/traffic/lane_graph_types.h"
#include "sim/traffic/route_counts.h"
namespace city {
struct CityState;
namespace traffic {
class RoadRouteSearch {
public:
	void begin(const CityState &s, int from, int goal, int max_expanded = -1, int banned = -1, bool bike = false);
	void begin_walk(const CityState &s, int from, int goal, int max_expanded = -1);
	void discard();
	bool source_current(const CityState &s) const;
	bool resumable() const { return walking_ || result_ != SearchStatus::PENDING || counts_.ready || !micro_; }
	SearchStatus advance(const CityState &s, WorkBudget &work);
	size_t length() const { return result_ == SearchStatus::FOUND ? search_.length() : 0; }
	int at(size_t i) const { return search_.at(i); }
private:
	template<class Take> SearchStatus step(const CityState &s, Take &&take);
	SearchStatus step_walk(const CityState &s, WorkBudget &work);
	AStarScratch<RouteConnCursor> search_;
	RouteCounts::Input counts_;
	uint64_t city_ = 0, graph_ = 0, roads_ = 0;
	int goal_ = -1, banned_ = -1;
	bool bike_ = false, micro_ = false, walking_ = false;
	SearchStatus result_ = SearchStatus::INVALIDATED;
};
} // namespace traffic
} // namespace city
