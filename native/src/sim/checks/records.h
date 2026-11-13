// The core's record invariants: one check function per feature, each naming the records it finds broken (a reference to a
// dead or wrong record) into a Broken list. City-data checks run scoped after every command and whole at a test's end;
// traffic-owned checks run after a batch of ticks once the ChangeLog's traffic lists are drained. The passes after a command and
// after a batch of ticks (records.cpp) are called under `if constexpr (CHECKS)` (core/checks.h), so the export's DLL runs none of
// them; the whole pass runs in both DLLs, only when the tests ask (CityProbes::broken_records). Pure C++, no Godot.
#pragma once

#include <cstddef>
#include <mutex>
#include <vector>

namespace city {
struct CityState;
}

namespace city::checks {

// One broken record: the feature and check that found it (static strings) and the record's id in the check's own table.
struct BrokenRecord {
	const char *feature;
	const char *check;
	int record;
};

// The broken records found by one pass. Reserved once (tuning checks_records_kept); add never allocates: past the room it
// counts `dropped` instead of keeping the record.
class Broken {
public:
	void reserve(size_t n);
	void add(const char *feature, const char *check, int record);
	void clear();
	const std::vector<BrokenRecord> &records() const;
	size_t dropped() const;

private:
	std::vector<BrokenRecord> records_;
	size_t room_ = 0;
	size_t dropped_ = 0;
};

// Which records a pass looks at: whole, or only the ids listed (a null list looks at none of that kind).
struct CheckScope {
	bool whole;
	const std::vector<int> *nodes;
	const std::vector<int> *segs;
	const std::vector<int> *buildings;
};

// City-data checks (after a command, scoped; whole at a test's end):
void check_roads(const CityState &, const CheckScope &, Broken &);     // roads.links_on_arms
void check_transit(const CityState &, const CheckScope &, Broken &);   // transit.stop_on_its_road, transit.stop_road_live
void check_buildings(const CityState &, const CheckScope &, Broken &); // interior.target_same_building
void check_damage(const CityState &, const CheckScope &, Broken &);    // damage.fire_on_its_building
void check_growth(const CityState &, const CheckScope &, Broken &);    // growth.nothing_on_rubble
void check_catalog(const CityState &, const CheckScope &, Broken &);   // catalog.variant_room
void check_edit(const CityState &, Broken &);                          // edit.refusal_inert (after a refused command)
// Traffic-owned checks (after a batch of ticks, only when the ChangeLog's traffic lists are drained):
void check_parking(const CityState &, const CheckScope &, Broken &);   // parking.lot_sources_live, yard_docks_live, lorry_target_live
void check_flow(const CityState &, const CheckScope &, Broken &);      // flow.stats_on_live_legs, flow.advisor_on_live_nodes
void check_furniture(const CityState &, const CheckScope &, Broken &); // furniture.kept_matches_control
void check_lane_graph(const CityState &, const CheckScope &, Broken &);// traffic.lane_graph (lane_graph_fault)

// The runners: every city-data check (check_edit excluded: it runs only after a refused command), and every traffic-owned check.
void run_city_checks(const CityState &, const CheckScope &, Broken &);
void run_traffic_checks(const CityState &, const CheckScope &, Broken &);
// The traffic's lists of the ChangeLog (the ids the traffic has still to drain) all empty: its tables have caught up with the city.
bool traffic_drained(const CityState &);

// What the debug DLL's passes found, until the api drains it on the main thread (the probe, CityCore's frame): a channel of
// CityState, filled from the main thread (after a command) or the simulation's (after a batch of ticks), so under its own mutex.
// Reserved once to tuning checks.records_kept (CityCore::load_catalog); nothing in a pass allocates.
struct BrokenSink {
	std::mutex mutex;
	Broken broken;
	std::vector<const char *> causes; // beside broken.records(): what each was found after (a command's label, "ticks")
	std::vector<int> segs; // a pass after a command: its scope's segments, removed and changed (main thread, under the command's hold)
	// The nodes the traffic drained since the last pass after ticks: touched only by the thread running Simulation::tick (one at a
	// time: the simulation's, or a test's step while that one is parked, Simulation::hold_still).
	std::vector<int> tick_nodes;
	void reserve(size_t n); // takes `mutex` itself
	void clear(); // the drain (the caller holds `mutex`): what was found goes, the room stays
	void note(const char *cause); // (the caller holds `mutex`) the records added since the last note were found after `cause`
};

// The debug DLL's passes, each into s.broken_sink (call them under `if constexpr (CHECKS)`):
// after a command, under its exclusive hold: the city-data checks on the ids the ChangeLog holds undrained, check_edit after a
// refused one, and the traffic-owned checks when the traffic has drained everything;
void after_command(CityState &s, const char *cause, bool refused);
// before a traffic step, under the shared map lock: the nodes its drain is about to rebuild join tick_nodes;
void before_step(CityState &s);
// after a batch of ticks, under the shared map lock and traffic_mutex: growth.nothing_on_rubble, and when the traffic has drained
// everything the traffic-owned checks (the lane graph on tick_nodes).
void after_ticks(CityState &s);

} // namespace city::checks
