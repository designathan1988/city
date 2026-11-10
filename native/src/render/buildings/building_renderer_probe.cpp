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

BuildingViewNeed BuildingRenderer::view_need() const {
	BuildingViewNeed n = need_;
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
		MapRead lk(s); // for the copy of the records alone
		for (const Building &b : s.buildings.items)
			if (b.alive && !(b.floors == 0 && b.body == 0 && !b40::is_open_lot(b.fn))) records.push_back(b);
	}
	const auto key = [](float grain, const b40::BayCtx &c, const b40::Opening &o) {
		char text[BAY_KEY_CHARS];
		const auto q = [grain](float v) { return int(std::round(v * grain)); };
		std::snprintf(text, sizeof(text), "%d/%d/%d/%d/%d/%d/%d", int(c.comp), q(c.W), q(c.H), q(o.w), q(o.h0), q(o.h1), q(o.depth));
		return std::string(text);
	};
	for (const Building &rec : records) {
		const float reach = rec.w + rec.d; // past the lot, whatever its turn
		const HeightPatch ground = s.terrain.patch(rec.x - reach, rec.z - reach, rec.x + reach, rec.z + reach);
		b40::Placed pl;
		bodies_.place(designs(), look_, ground, rec, pl);
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

// Every superchunk with a building, built as its far job builds it. A face is four corners (two triangles from one first corner) or
// three, as Shell::face writes it, with one normal.
Dictionary BuildingRenderer::far_form() const {
	constexpr float LOOKS_DOWN = -0.5f; // a normal this far under the level: the face is seen only from below
	constexpr size_t FACE3 = 3, FACE4 = 2 * FACE3, LAST_CORNER = 3; // the indices of a three-cornered and of a four-cornered face
	if (!state_) return Dictionary(); // no map was drawn yet
	const CityState &s = *state_;
	std::unordered_set<int64_t> supers;
	{
		MapRead lk(s);
		for (const Building &b : s.buildings.items)
			if (b.alive) supers.insert(chunk_of(b.x, b.z, SUPERCHUNK));
	}
	const float error = far_error_m();
	int64_t buildings = 0, triangles = 0, bytes = 0, sent = 0, thin_quads = 0, down_faces = 0, roof_boxes = 0, boxes_left = 0, cast_triangles = 0;
	for (const int64_t key : supers) {
		const SuperChunks chunks = chunks_of(key);
		Inputs in;
		{
			MapRead lk(s);
			in = gather(s, chunks.data(), int(chunks.size()));
		}
		const SuperBuilt built = build_super_cpu(in, key);
		const b40::Shell &m = built.far_shell;
		buildings += int64_t(in.records.size());
		triangles += int64_t(m.triangles());
		bytes += built.bytes();
		sent += int64_t(m.pos.size() * (2 * sizeof(Vector3) + sizeof(Color) + sizeof(Vector2) + size_t(m.custom_used) * sizeof(uint32_t)) + m.idx.size() * sizeof(int32_t));
		sent += built.casts.bytes(); // the casters' positions and indices, as laid
		for (size_t i = 0; i + FACE3 <= m.idx.size();) {
			const size_t first = size_t(m.idx[i]);
			const bool quad = i + FACE4 <= m.idx.size() && size_t(m.idx[i + FACE3]) == first;
			if (m.nrm[first].y < LOOKS_DOWN) down_faces++;
			if (quad && std::min(m.pos[first].distance_to(m.pos[first + 1]), m.pos[first].distance_to(m.pos[first + LAST_CORNER])) < error) thin_quads++;
			i += quad ? FACE4 : FACE3;
		}
		roof_boxes += m.boxes_kept;
		boxes_left += m.boxes_left;
		cast_triangles += built.casts.emitted();
	}
	Dictionary d;
	d["buildings"] = buildings;
	d["triangles"] = triangles;
	d["bytes"] = bytes; // what the frame's upload budget is charged
	d["sent_bytes"] = sent; // what the arrays handed to the engine hold
	d["error_m"] = error;
	d["thin_quads"] = thin_quads;
	d["down_faces"] = down_faces;
	d["part_least_m"] = far_part_least();
	d["roof_boxes"] = roof_boxes; // what stands on the roofs, a box each, that the far level's distance shows
	d["boxes_left"] = boxes_left; // and what it does not show, left out
	d["cast_triangles"] = cast_triangles; // what it throws in the sun, as laid (far_casts.h): the blocks' sides and roofs alone
	return d;
}

Dictionary BuildingRenderer::building_cost() const {
	static const char *const NAMES[HELD_KINDS] = { "near_walls", "near_panes", "near_guards", "near_cards", "near_pieces", "mid_walls", "mid_panes", "mid_cards" };
	std::array<Held, HELD_KINDS> sum;
	Held far;
	int64_t far_cast_triangles = 0;
	int64_t near_chunks = 0, mid_chunks = 0, bay_chunks = 0, bay_rows = 0, mid_rows = 0, instances = 0, props = 0;
	for (const auto &[key, ch] : chunks_) {
		bay_chunks += ch.built[LEVEL_BAYS] ? 1 : 0;
		for (const auto &sub : ch.mid_draws) {
			bay_rows += sub[MID_BAYS].count;
			mid_rows += sub[MID_BOXES].count + sub[MID_SOLIDS].count;
		}
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
		far_cast_triangles += sp.casts.triangles;
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
	d["far_cast_triangles"] = far_cast_triangles; // what the far level's casters hold, welded and simplified
	d["near_chunks"] = near_chunks;
	d["mid_chunks"] = mid_chunks;
	d["bay_chunks"] = bay_chunks; // chunks whose openings stand as instances (within their own reach)
	d["bay_rows"] = bay_rows; // the instances of the mid level's openings held
	d["mid_rows"] = mid_rows; // and of what stands out of its walls
	d["instances"] = instances;
	d["props"] = props;
	return d;
}

} // namespace city
