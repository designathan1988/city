// CityState: the single source of truth for the whole city. Every system reads and writes only this; mutations from
// the player go through sim/commands, time advances through sim/simulation.
#pragma once

#include "core/job_pool.h"
#include "sim/buildings/building_store.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/edit_journal.h"
#include "sim/lots/lot_cache.h"
#include "sim/nature/nature_state.h"
#include "sim/roads/road_network.h"
#include "sim/state/pose_buffer.h"
#include "sim/simulation/city_growth.h"
#include "sim/state/change_log.h"
#include "sim/terrain/heightfield.h"
#include "sim/traffic/traffic_state.h"
#include "sim/transit/transit_data.h"
#include "sim/transit/transit_sim.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace city {

struct CityClock {
	std::atomic<double> minutes{ 0 }; // city minutes since the map began (never wraps; drives decay)
	std::atomic<double> day_shift{ 0 }; // added to minutes for the time of day (starts at the day start)
	double hour() const;
	void set_hour(double hour); // the time of day moves; `minutes`, the city's age, does not
};

// Per node, the minimum greens of its stages as the traffic last planned them (empty: no signal there). The traffic
// publishes a new one whole after a step that planned a junction again; readers take it with std::atomic_load and never
// wait for a traffic step (the inspector: city_core_query.cpp).
using SignalLeast = std::vector<std::vector<float>>;

struct CityState {
	Catalog catalog;
	ChangeLog log;
	Heightfield terrain;
	NatureState nature; // weather, clouds, fog, painted ground, planted trees, the ecosystem (sim/nature)
	RoadNetwork roads;
	LotCache lots;
	BuildingStore buildings;
	TrafficState traffic; // vehicles and pedestrians (sim/traffic)
	TrafficCounts traffic_counts; // the counts the traffic published: read without traffic_mutex (traffic_state.h)
	std::shared_ptr<const SignalLeast> signal_least; // std::atomic_load / std::atomic_store only (traffic_system.cpp writes)
	TransitData transit; // stops, tracks and lines as the player laid them; replaced whole by each edit
	uint64_t transit_revision = 0; // follows every change of the plan: the simulation and the render read it
	TransitRun transit_run; // what runs on it (sim/transit/transit_sim.h)
	PoseBuffer poses; // published by the simulation, read by the render without a lock
	std::unique_ptr<JobPool> jobs{ std::make_unique<JobPool>() }; // shared worker pool (lot cells, later mesh jobs)
	GrowthQueue growth; // cells whose lots changed after founding: buildings grow there over time
	CityClock clock;
	EditHistory history; // undo / redo of the player's commands (sim/commands/edit_history.cpp)
	double money = 0;
	bool unlimited_money = false;
	uint32_t seed = 0;
	int map_index = 0; // entry of data/catalog/maps.json chosen at creation
	float map_half = 0; // metres from the middle to each edge
	std::atomic<float> camera_x{ 0 }, camera_z{ 0 }; // set by the api each frame (micro window)
	bool ready = false;
	// Owner of "unsaved changes" (tools/gates/owners.json): every change to the map calls touch() (commands, undo/redo,
	// a building that grew); the clock running is not one. Saving stores the revision. Nothing
	// else keeps a dirty flag for the save prompt. `edited`: the player changed this city since it was made or opened
	// (an edit recorded, undone or redone), so its work is theirs (autosave keeps it).
	uint64_t revision = 0, saved_revision = 0;
	uint64_t edit_revision = 0; // the network as the player shapes it: commands, undo/redo and a new or opened map (found_land)
	bool edited = false;
	void touch() { revision++; }
	bool has_unsaved_work() const { return revision != saved_revision; }

	// map_mutex: shared by readers (simulation traffic step, queries, mesh jobs, the render's per-frame sync, which
	// drains the ChangeLog under its own mutex: change_log.h); exclusive for commands, growth, clock, load (the
	// ChangeLog's producers). traffic_mutex: held by the simulation thread while it steps traffic (probes take it too).
	// mutable: const readers (the simulation step and render mesh jobs) take it shared through a const state
	mutable std::shared_mutex map_mutex;
	mutable std::mutex traffic_mutex;
};

} // namespace city
