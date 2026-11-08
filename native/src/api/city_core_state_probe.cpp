// The city as one number (the state hash of a deterministic game, as Factorio's map CRC): the save's own bytes hashed whole and by
// part, so whatever a save keeps is covered without a list to keep up, and a difference says which part it is in. `derived` is
// what a save does not keep because it is made from the rest (final heights, lots, lane graph, solved grades): where an
// optimisation works, it must leave those equal.
#include "api/city_probes.h"

#include "sim/citygen/lot_gen.h"
#include "sim/persistence/save_format.h"
#include "sim/roads/road_grade.h"
#include "sim/simulation/simulation.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr uint64_t FNV_BASIS = 14695981039346656037ULL, FNV_PRIME = 1099511628211ULL; // FNV-1a, 64 bits
constexpr int HEX = 16; // the base a hash is written in

String hex_of(const uint8_t *from, const uint8_t *to) {
	uint64_t h = FNV_BASIS;
	for (const uint8_t *p = from; p != to; p++) h = (h ^ *p) * FNV_PRIME;
	return String::num_uint64(h, HEX);
}

constexpr uint64_t WORD_MUL = 0xff51afd7ed558ccdull; // eight bytes at a time: a whole map's heights in milliseconds
constexpr int WORD_SHIFT = 33;

uint64_t fold(uint64_t h, const void *data, size_t bytes) {
	const uint8_t *p = static_cast<const uint8_t *>(data);
	for (; bytes >= sizeof(uint64_t); bytes -= sizeof(uint64_t), p += sizeof(uint64_t)) {
		uint64_t w;
		std::memcpy(&w, p, sizeof(w));
		h = (h ^ w) * WORD_MUL;
		h ^= h >> WORD_SHIFT;
	}
	for (; bytes > 0; bytes--, p++) h = (h ^ *p) * WORD_MUL;
	return h;
}

// The fleet and the walkers as a number: what each one is and where, in slot order.
uint64_t fleet_hash(const TrafficState &t) {
	uint64_t h = FNV_BASIS;
	for (const Vehicle &v : t.vehicles) {
		if (!v.alive) continue;
		const float f[] = { float(v.ds), float(v.lane), float(v.conn), v.s, v.v, v.cs, float(v.micro), float(v.type) };
		h = fold(h, f, sizeof(f));
	}
	for (const Pedestrian &p : t.peds) {
		if (!p.alive) continue;
		const float f[] = { float(p.seg), float(p.dir), float(p.side), p.s, p.speed, p.track, float(p.mode) };
		h = fold(h, f, sizeof(f));
	}
	return h;
}

Dictionary derived_of(const CityState &s) {
	Dictionary d;
	d["heights"] = String::num_uint64(fold(FNV_BASIS, s.terrain.final_h.data(), s.terrain.final_h.size() * sizeof(float)), HEX);
	std::vector<int64_t> cells;
	for (const auto &kv : s.lots.by_cell) cells.push_back(kv.first);
	std::sort(cells.begin(), cells.end());
	uint64_t lots = FNV_BASIS;
	for (int64_t k : cells)
		for (const Lot &l : s.lots.by_cell.at(k)) {
			const double f[] = { l.x, l.z, l.w, l.d, l.rot, double(l.zone), double(l.profile), double(l.seed) };
			lots = fold(lots, f, sizeof(f));
		}
	d["lots"] = String::num_uint64(lots, HEX);
	uint64_t graph = FNV_BASIS;
	for (const Conn &c : s.traffic.g.conns) {
		const float f[] = { c.x0, c.z0, c.x1, c.z1, c.len, float(c.alive), float(c.node) };
		graph = fold(graph, f, sizeof(f));
		graph = fold(graph, c.conflicts.data(), c.conflicts.size() * sizeof(int));
		graph = fold(graph, c.conflict_at.data(), c.conflict_at.size() * sizeof(float));
	}
	d["graph"] = String::num_uint64(graph, HEX);
	uint64_t grades = FNV_BASIS;
	for (const GradeProfile &p : s.roads.grade.prof) grades = fold(grades, p.h.data(), p.h.size() * sizeof(float));
	d["grades"] = String::num_uint64(grades, HEX);
	return d;
}
} // namespace

int CityProbes::undo_steps() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return int(state_->history.undo_stack.size());
}

String CityProbes::graph_fault() {
	std::lock_guard<std::mutex> traffic(state_->traffic_mutex);
	return String(state_->traffic.graph_fault.c_str());
}

// The determinism check of a lockstep game (Factorio's map CRC): two runs from the same map must give the same series, and the
// first tick that differs says where the cause is. Steps run here, only while the clock holds still (Simulation::hold_still).
PackedStringArray CityProbes::traffic_hashes(int ticks) {
	if (!sim_->hold_still()) return PackedStringArray(); // beside a running clock its ticks would land among these steps
	std::shared_lock<std::shared_mutex> map(state_->map_mutex);
	std::lock_guard<std::mutex> traffic(state_->traffic_mutex);
	const float dt = float(1.0 / state_->catalog.tune.sim_tick_hz);
	PackedStringArray out;
	for (int k = 0; k < ticks; k++) {
		traffic::step(*state_, dt);
		out.push_back(String::num_uint64(fleet_hash(state_->traffic), HEX));
	}
	return out;
}

Dictionary CityProbes::state_hash() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::vector<SavePart> marks;
	const std::vector<uint8_t> bytes = encode_city(*state_, &marks);
	Dictionary parts;
	size_t from = 0;
	for (const SavePart &m : marks) {
		parts[String(m.name)] = hex_of(bytes.data() + from, bytes.data() + m.end);
		from = m.end;
	}
	Dictionary d;
	d["whole"] = hex_of(bytes.data(), bytes.data() + bytes.size());
	d["bytes"] = int64_t(bytes.size());
	d["parts"] = parts;
	std::lock_guard<std::mutex> traffic(state_->traffic_mutex);
	d["derived"] = derived_of(*state_);
	return d;
}

} // namespace city
