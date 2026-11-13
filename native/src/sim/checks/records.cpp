// The invariant runner: Broken (the broken records a pass found, in room reserved once), the two runners over the features'
// checks, and the debug DLL's scoped passes into CityState's BrokenSink: after every command (the ids the ChangeLog holds
// undrained) and after every batch of ticks (what the traffic drained). The whole pass is the probe's (api/city_core_state_probe.cpp).
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *AFTER_TICKS = "ticks"; // the cause a pass after a batch of ticks names
} // namespace

void Broken::reserve(size_t n) {
	records_.clear();
	records_.reserve(n);
	room_ = n;
	dropped_ = 0;
}

void Broken::add(const char *feature, const char *check, int record) {
	if (records_.size() < room_) records_.push_back({ feature, check, record }); // within the capacity reserved: no allocation
	else dropped_++;
}

void Broken::clear() {
	records_.clear();
	dropped_ = 0;
}

const std::vector<BrokenRecord> &Broken::records() const { return records_; }

size_t Broken::dropped() const { return dropped_; }

void BrokenSink::reserve(size_t n) {
	std::lock_guard<std::mutex> lk(mutex);
	broken.reserve(n);
	causes.clear();
	causes.reserve(n);
}

void BrokenSink::clear() {
	broken.clear();
	causes.clear();
}

void BrokenSink::note(const char *cause) { causes.resize(broken.records().size(), cause); } // within the room: no allocation

void run_city_checks(const CityState &s, const CheckScope &scope, Broken &broken) {
	check_roads(s, scope, broken);
	check_transit(s, scope, broken);
	check_buildings(s, scope, broken);
	check_damage(s, scope, broken);
	check_growth(s, scope, broken);
	check_catalog(s, scope, broken);
}

void run_traffic_checks(const CityState &s, const CheckScope &scope, Broken &broken) {
	check_parking(s, scope, broken);
	check_flow(s, scope, broken);
	check_furniture(s, scope, broken);
	check_lane_graph(s, scope, broken);
}

// Every list traffic::apply_network_changes drains. Read without the log's mutex: its producers hold the map exclusively, and
// the callers hold it (shared or exclusive) while they ask.
bool traffic_drained(const CityState &s) {
	const ChangeLog &l = s.log;
	return l.net_nodes.empty() && l.net_nodes_removed.empty() && l.net_segs_removed.empty() && l.net_segs_changed.empty() &&
			l.furniture_segs.empty() && l.lots_changed.empty() && l.lot_fronts.empty() && l.signal_nodes.empty();
}

void after_command(CityState &s, const char *cause, bool refused) {
	BrokenSink &sink = s.broken_sink;
	const ChangeLog &log = s.log;
	const bool drained = traffic_drained(s);
	std::unique_lock<std::mutex> traffic(s.traffic_mutex, std::defer_lock); // map -> traffic -> sink, as the tick takes them
	if (drained) traffic.lock();
	std::lock_guard<std::mutex> lk(sink.mutex);
	sink.segs.assign(log.net_segs_removed.begin(), log.net_segs_removed.end());
	sink.segs.insert(sink.segs.end(), log.net_segs_changed.begin(), log.net_segs_changed.end());
	const CheckScope scope{ false, &log.net_nodes, &sink.segs, &log.lots_changed };
	run_city_checks(s, scope, sink.broken);
	if (refused) check_edit(s, sink.broken);
	if (drained) run_traffic_checks(s, scope, sink.broken); // before a drain the traffic's tables are behind the city: pending
	sink.note(cause);
}

void before_step(CityState &s) {
	const std::vector<int> &nodes = s.log.net_nodes; // the step's drain is on this thread, after this
	s.broken_sink.tick_nodes.insert(s.broken_sink.tick_nodes.end(), nodes.begin(), nodes.end());
}

void after_ticks(CityState &s) {
	BrokenSink &sink = s.broken_sink;
	std::lock_guard<std::mutex> lk(sink.mutex);
	const CheckScope grown{ false, nullptr, nullptr, &s.log.lots_changed };
	check_growth(s, grown, sink.broken);
	if (traffic_drained(s)) { // else pending: tick_nodes waits for the batch that drains
		const CheckScope drained{ false, &sink.tick_nodes, nullptr, nullptr };
		run_traffic_checks(s, drained, sink.broken);
		sink.tick_nodes.clear();
	}
	sink.note(AFTER_TICKS);
}

} // namespace city::checks
