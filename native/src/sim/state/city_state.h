// CityState: the single source of truth for the whole city. Every system reads and writes only this; mutations from
// the player go through sim/commands, time advances through sim/simulation.
// It is two parts: CityData, the city itself (what a save keeps and what is derived from it), and the channels round it
// that outlive any one city (the locks, the change log, the job pool, the traffic and what it publishes). A city opened
// from a file is read into a CityData of its own, aside, and taken in whole (take_city): the old one stays intact until then.
#pragma once

#include "core/job_pool.h"
#include "sim/buildings/building_store.h"
#include "sim/buildings/interior/view.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/edit_journal.h"
#include "sim/demolition/damage_state.h"
#include "sim/lots/lot_book.h"
#include "sim/lots/lot_cache.h"
#include "sim/nature/nature_state.h"
#include "sim/roads/furniture_kept.h"
#include "sim/roads/road_network.h"
#include "sim/state/pose_buffer.h"
#include "sim/simulation/city_growth.h"
#include "sim/state/change_log.h"
#include "sim/state/map_hold.h"
#include "sim/street/pole_routes.h"
#include "sim/street/street_works.h"
#include "sim/terrain/heightfield.h"
#include "sim/traffic/traffic_state.h"
#include "sim/transit/transit_data.h"
#include "sim/transit/transit_sim.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>

namespace city {

// An atomic the city's data carries (read without the map lock by jobs and the simulation): copied and moved with the data
// as its value, when the data is taken in under the exclusive lock.
template <typename T>
struct CarriedAtomic : std::atomic<T> {
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator=;
	CarriedAtomic() : std::atomic<T>(T()) {}
	CarriedAtomic(const CarriedAtomic &o) : std::atomic<T>(o.load()) {}
	CarriedAtomic &operator=(const CarriedAtomic &o) {
		this->store(o.load());
		return *this;
	}
};

struct CityClock {
	CarriedAtomic<double> minutes{ 0 }; // city minutes since the map began (never wraps; drives decay)
	CarriedAtomic<double> day_shift{ 0 }; // added to minutes for the time of day (starts at the day start)
	double hour() const;
	void set_hour(double hour); // the time of day moves; `minutes`, the city's age, does not
};

// The city itself. Its links to the channels and the catalogue (the change log, the job pool, the tables) are set by
// CityState::bind.
struct CityData {
	Catalog catalog;
	Heightfield terrain;
	NatureState nature; // weather, clouds, fog, painted ground, planted trees, the ecosystem (sim/nature)
	RoadNetwork roads;
	StreetWorks street; // what the player built along the streets (sim/street)
	LotCache lots;
	LotBook lot_book; // the lots the player drew (sim/lots/lot_book.h); those of `lots` give way to them
	BuildingStore buildings;
	TransitData transit; // stops, tracks and lines as the player laid them; changed record by record by the edits
	uint64_t transit_revision = 0; // follows every change of the plan: the simulation and the render read it
	DamageState damage; // craters, ruins, furniture knocked out, fires (sim/demolition)
	GrowthQueue growth; // cells whose lots changed after founding: buildings grow there over time
	CityClock clock;
	EditHistory history; // undo / redo of the player's commands (sim/commands/edit_history.cpp)
	float new_road_lift = 0; // the height over the grade line the road tool draws at, metres (Roadcraft 4.0 roadHeightOffset)
	uint16_t new_road_flags = 0; // what the road tool lays new roads with: the furniture set (SEG_FURNITURE_MASK) and SEG_CUT_WALLS
	double money = 0;
	bool unlimited_money = false;
	uint32_t seed = 0;
	int map_index = 0; // entry of data/catalog/maps.json chosen at creation
	int map_tiles = 0; // the land's side as it was founded: a city keeps it when the table changes
	float map_half = 0; // metres from the middle to each edge
	bool ready = false;
	std::string style; // the regional style's id, as the new-city screen chose it (the save block and the single reader come with the mechanism)
	// Owner of "unsaved changes" (tools/gates/owners.json): every change to the map calls touch() (commands, undo/redo,
	// a building that grew); the clock running is not one. Saving stores the revision. Nothing
	// else keeps a dirty flag for the save prompt. `edited`: the player changed this city since it was made or opened
	// (an edit recorded, undone or redone), so its work is theirs (autosave keeps it).
	uint64_t revision = 0, saved_revision = 0;
	CarriedAtomic<int> marking_style{ 0 }; // the map's paint standard (data/render/streets.json markings.styles); mesh jobs read it
	uint64_t edit_revision = 0; // the network as the player shapes it: commands, undo/redo and a new or opened map (found_land)
	bool edited = false;
	// Every change to the map but a building that grew: what is made from the streets alone (their furniture: the network, what
	// the player put up and cut, what a blow knocked out) is kept by it, so the city growing round the streets drops nothing.
	uint64_t street_revision = 0;
	void touch() {
		revision++;
		street_revision++;
	}
	void touch_grown() { revision++; } // a building grew: a change to the map (saved); the streets stand as they stood
	bool has_unsaved_work() const { return revision != saved_revision; }
};

struct CityState : CityData {
	CityState() = default;
	// A city read aside (a file being opened) shares the pool of the live one: no workers of its own.
	explicit CityState(std::shared_ptr<JobPool> pool) : jobs(std::move(pool)) {}

	ChangeLog log;
	InteriorView interior; // the Interior layer: which buildings are drawn cut open (a view, not saved)
	mutable FurnitureKept furniture_kept; // each segment's street furniture placed once for the streets' revision (furniture_kept.h)
	TrafficState traffic; // vehicles and pedestrians (sim/traffic)
	TrafficLevels traffic_levels; // the panels' choices: written by the main thread, read by the traffic (traffic_view.h)
	uint64_t lane_graph_serial = 0; // moves when the lane graph changes; outlives TrafficState, as the views do
	TripleBuffer<TrafficView> traffic_views; // what the traffic published for the main thread (traffic_view.h)
	TransitRun transit_run; // what runs on it (sim/transit/transit_sim.h)
	PoseBuffer poses; // published by the simulation, read by the render without a lock
	std::shared_ptr<JobPool> jobs{ std::make_shared<JobPool>() }; // shared worker pool (lot cells, later mesh jobs)
	std::atomic<float> camera_x{ 0 }, camera_z{ 0 }; // the ground the player is looking at, set by the api each frame: the traffic's and walkers' windows

	// The data's links to the channels and the catalogue (change log, job pool, tables), set again after the data moved in.
	void bind();

	// map_mutex: shared by readers (simulation traffic step, queries, mesh jobs, the render's per-frame sync, which
	// drains the ChangeLog under its own mutex: change_log.h); exclusive for commands, growth, clock, load (the
	// ChangeLog's producers). traffic_mutex: held by the simulation thread while it steps traffic (probes take it too).
	// mutable: const readers (the simulation step and render mesh jobs) take it shared through a const state
	mutable std::shared_mutex map_mutex;
	mutable std::mutex traffic_mutex;
};

// The render's one way to read the map: map_mutex shared for the guard's scope, and how deep its thread holds the map counted
// (map_hold.h), so what a builder takes under it is seen. `release` lets the map go before the scope ends.
class MapRead {
public:
	explicit MapRead(const CityState &s) : lock_(s.map_mutex) { map_hold::enter(); }
	~MapRead() { release(); }
	MapRead(const MapRead &) = delete;
	MapRead &operator=(const MapRead &) = delete;
	void release() {
		if (!lock_.owns_lock()) return;
		lock_.unlock();
		map_hold::leave();
	}

private:
	std::shared_lock<std::shared_mutex> lock_;
};

// Takes in the city read aside into `read` (caller holds live.map_mutex exclusively, the simulation stopped): the live state
// ends as decoding in place would have left it, its revisions going on from where they were, its change log keeping what
// the reading marked. `read` is left with the old city, to be freed after the lock is let go. The generator's tables
// (BuildingStore::designs, set at boot) stay the live ones.
void take_city(CityState &live, CityState &read);

} // namespace city
