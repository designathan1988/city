// What the tests read of the building levels: what the view still asks and how a chunk stands.
#include "render/buildings/building_renderer.h"

#include "core/grid.h"
#include "render/buildings/facade_bay.h"
#include "sim/buildings/b40/civic.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace godot;

namespace city {

int BuildingRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return need_.unmet + int(requested_.size() + done_.size() + super_requested_.size() + super_done_.size());
}

BuildingRenderer::ViewNeed BuildingRenderer::view_need() const {
	ViewNeed n = need_;
	std::lock_guard<std::mutex> lk(queue_mutex_);
	n.requested = int(requested_.size() + super_requested_.size());
	n.done = int(done_.size() + super_done_.size());
	return n;
}

int BuildingRenderer::chunk_flags(float x, float z) const {
	const int64_t key = chunk_of(x, z);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return 0;
	const Chunk &ch = it->second;
	constexpr int NEAR_BIT = 1, MID_BIT = 2, SHOWN_BIT = 4, DETAIL_BIT = 8, KNOWN_BIT = 16, FAR_BIT = 32;
	int flags = KNOWN_BIT | (ch.built[LEVEL_NEAR] ? NEAR_BIT : 0) | (ch.built[LEVEL_MID] ? MID_BIT : 0) | (ch.shown ? SHOWN_BIT : 0) | (ch.detail ? DETAIL_BIT : 0);
	auto sp = supers_.find(chunk_of(ch.centre.x, ch.centre.z, SUPERCHUNK));
	if (sp != supers_.end() && sp->second.far.inst.is_valid()) flags |= FAR_BIT;
	return flags;
}

Dictionary BuildingRenderer::bay_census() {
	constexpr int BAY_KEY_CHARS = 96;
	constexpr float FINE = 100.f, COARSE = 4.f; // bays per metre told apart: by the centimetre, by the 25 cm
	if (!state_) return Dictionary(); // no chunk was asked yet
	const CityState &s = *state_;
	std::unordered_map<std::string, int64_t> fine, coarse;
	std::array<int64_t, b40::BAY_COUNT> by_comp{};
	int64_t bays = 0, buildings = 0;
	std::vector<Building> records;
	{
		std::shared_lock<std::shared_mutex> lk(s.map_mutex);
		for (const Building &b : s.buildings.items)
			if (b.alive && !(b.floors == 0 && b.body == 0 && !b40::is_open_lot(b.fn))) records.push_back(b);
	}
	const auto key = [](float grain, const b40::BayCtx &c, const b40::Opening &o) {
		char text[BAY_KEY_CHARS];
		const auto q = [grain](float v) { return int(std::round(v * grain)); };
		std::snprintf(text, sizeof(text), "%d/%d/%d/%d/%d/%d/%d", int(c.comp), q(c.W), q(c.H), q(o.w), q(o.h0), q(o.h1), q(o.depth));
		return std::string(text);
	};
	BuildingBodies bodies;
	for (const Building &rec : records) {
		const float reach = rec.w + rec.d; // past the lot, whatever its turn
		const HeightPatch ground = s.terrain.patch(rec.x - reach, rec.z - reach, rec.x + reach, rec.z + reach);
		b40::Placed pl;
		bodies.place(designs(), look_, ground, rec, pl);
		buildings++;
		b40::for_each_bay(pl, [&](const b40::BayCtx &c, const b40::Opening &o) {
			bays++;
			if (c.comp < b40::BAY_COUNT) by_comp[c.comp]++;
			fine[key(FINE, c, o)]++;
			coarse[key(COARSE, c, o)]++;
		});
	}
	const auto cover = [](const std::unordered_map<std::string, int64_t> &m, int64_t total) {
		std::vector<int64_t> counts;
		for (const auto &kv : m) counts.push_back(kv.second);
		std::sort(counts.rbegin(), counts.rend());
		Array out;
		int64_t sum = 0;
		size_t next = 1;
		for (size_t i = 0; i < counts.size(); i++) {
			sum += counts[i];
			if (i + 1 == next || i + 1 == counts.size()) {
				Array row;
				row.push_back(int64_t(i + 1));
				row.push_back(double(sum) / double(std::max<int64_t>(total, 1)));
				out.push_back(row);
				next *= 4;
			}
		}
		return out;
	};
	Dictionary d;
	d["buildings"] = buildings;
	d["bays"] = bays;
	d["distinct_cm"] = int64_t(fine.size());
	d["distinct_25cm"] = int64_t(coarse.size());
	d["cover_cm"] = cover(fine, bays);
	d["cover_25cm"] = cover(coarse, bays);
	Array comps;
	for (int64_t n : by_comp) comps.push_back(n);
	d["by_component"] = comps;
	return d;
}

Dictionary BuildingRenderer::building_cost() const {
	static const char *const NAMES[HELD_KINDS] = { "near_walls", "near_panes", "near_guards", "near_cards", "near_pieces", "mid_walls", "mid_panes", "mid_cards" };
	std::array<Held, HELD_KINDS> sum;
	Held far;
	int64_t near_chunks = 0, mid_chunks = 0, instances = 0, props = 0;
	for (const auto &[key, ch] : chunks_) {
		near_chunks += ch.built[LEVEL_NEAR] ? 1 : 0;
		mid_chunks += ch.built[LEVEL_MID] ? 1 : 0;
		instances += ch.built[LEVEL_NEAR] ? ch.instances : 0;
		props += ch.built[LEVEL_NEAR] ? ch.props : 0;
		for (int k = 0; k < HELD_KINDS; k++) {
			if (!ch.built[k < HELD_MID_WALLS ? LEVEL_NEAR : LEVEL_MID]) continue;
			sum[size_t(k)].triangles += ch.held[size_t(k)].triangles;
			sum[size_t(k)].bytes += ch.held[size_t(k)].bytes;
		}
	}
	for (const auto &[key, sp] : supers_) {
		far.triangles += sp.held.triangles;
		far.bytes += sp.held.bytes;
	}
	Dictionary d;
	auto put_held = [&d](const char *name, const Held &h) {
		Dictionary one;
		one["triangles"] = h.triangles;
		one["bytes"] = h.bytes;
		d[name] = one;
	};
	for (int k = 0; k < HELD_KINDS; k++) put_held(NAMES[k], sum[size_t(k)]);
	put_held("far", far);
	d["near_chunks"] = near_chunks;
	d["mid_chunks"] = mid_chunks;
	d["instances"] = instances;
	d["props"] = props;
	return d;
}

} // namespace city
