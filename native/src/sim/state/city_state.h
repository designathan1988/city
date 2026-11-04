// CityState: the single source of truth for the whole city. Every system reads and writes only this; mutations from
// the player go through sim/commands, time advances through sim/simulation.
#pragma once

#include "core/job_pool.h"
#include "sim/buildings/building_store.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/edit_journal.h"
#include "sim/lots/lot_cache.h"
#include "sim/roads/road_network.h"
#include "sim/state/pose_buffer.h"
#include "sim/simulation/city_growth.h"
#include "sim/state/change_log.h"
#include "sim/terrain/heightfield.h"
#include "sim/traffic/traffic_state.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace city {

struct CityClock {
	std::atomic<double> minutes{ 0 }; // city minutes since the map began (never wraps; drives decay)
	std::atomic<double> day_shift{ 0 }; // added to minutes for the time of day (starts at the day start)
	double hour() const;
	void set_hour(double hour); // the time of day moves; `minutes`, the city's age, does not
};

struct CityState {
	Catalog catalog;
	ChangeLog log;
	Heightfield terrain;
	RoadNetwork roads;
	LotCache lots;
	BuildingStore buildings;
	TrafficState traffic; // vehicles and pedestrians (sim/traffic)
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
	uint64_t edit_revision = 0; // commands and undo/redo only: the snapshot's expensive counts follow this
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
