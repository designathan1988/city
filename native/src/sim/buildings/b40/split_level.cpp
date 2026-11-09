// A building that follows its hillside (Roadcraft world/buildings/splitLevel.ts stepToSlope, garageUnder; world/growth/lotPlan.ts
// yardRamp, garageRamp): the back of a body moves half a storey or a whole one from the street floor where the ground under it calls
// for it, as CityEngine moves each footprint to the terrain before the ground is graded round it (split_level.h).
#include "sim/buildings/b40/split_level.h"

#include "sim/buildings/b40/record.h" // LOT_PLINTH_M: the plinth the floor stands on

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <vector>

namespace city::b40 {

namespace {
constexpr int SIDE_FRONT = 0, SIDE_RIGHT = 1, SIDE_BACK = 2, SIDE_LEFT = 3; // a block's faces (types.h Facing)
constexpr uint32_t KEY_BYTE = 0xff; // a bay key's side and bay bytes (types.h bay_key)
constexpr float MIDDLE = 0.5f; // a bay's middle, half its width along

bool is_access(uint8_t c) { return c == BAY_DOOR || c == BAY_DOUBLE_DOOR || c == BAY_GARAGE_DOOR || c == BAY_LOADING_DOOR; }
int key_storey(uint32_t key) { return int(key >> KEY_STOREY_SHIFT); }
int bay_side(uint32_t key) { return int((key >> KEY_SIDE_SHIFT) & KEY_BYTE); }
bool solid_on_ground(const Volume &v) { return v.mode == BLOCK_SOLID && v.base == 0; }

// Whether storey 0 of a block has a way in on `side` (splitLevel.ts doorOn).
bool door_on(const Volume &v, int side) {
	if (v.storeys < 1 || v.fill.empty()) return false;
	for (const auto &e : v.bays)
		if (key_storey(e.first) == 0 && bay_side(e.first) == side && is_access(e.second)) return true;
	return is_access(fill_at(v, 0, side));
}

// The bay overrides of storey `storey` (any storey when < 0) on the `drop` sides taken out.
void drop_bays(Volume &v, std::initializer_list<int> drop, int storey = -1) {
	for (auto it = v.bays.begin(); it != v.bays.end();) {
		const bool hit = (storey < 0 || key_storey(it->first) == storey) && std::find(drop.begin(), drop.end(), bay_side(it->first)) != drop.end();
		it = hit ? v.bays.erase(it) : std::next(it);
	}
}

// Block `v` split across its depth at `y`: `v` keeps the front, the back part is returned (splitLevel.ts splitBlock).
Volume split_block(Volume &v, float y) {
	const float end = v.y + v.d;
	if ((v.roof == ROOF_GABLE || v.roof == ROOF_HIP) && v.ridge < 0) v.ridge = v.w >= v.d ? 0 : 1; // the ridge as it ran on the whole
	Volume back = v;
	back.y = y;
	back.d = end - y;
	v.d = y - v.y;
	drop_bays(v, { SIDE_RIGHT, SIDE_BACK, SIDE_LEFT }); // their bay counts change with the block (withoutBays)
	drop_bays(back, { SIDE_FRONT, SIDE_RIGHT, SIDE_LEFT });
	auto keep = [](Volume &part, int side) {
		part.reliefs.erase(std::remove_if(part.reliefs.begin(), part.reliefs.end(), [&](const Relief &r) { return r.side != side; }), part.reliefs.end());
		for (int face : { SIDE_RIGHT, SIDE_LEFT }) part.faces[face].bays = -1;
		part.gear.erase(std::remove_if(part.gear.begin(), part.gear.end(), [&](const RoofPart &g) { return g.y < part.y || g.y > part.y + part.d; }), part.gear.end());
	};
	keep(v, SIDE_FRONT);
	keep(back, SIDE_BACK);
	return back;
}

// A storey under the block's storeys like the one above, the others numbered one up (a lower ground floor).
void storey_under(Volume &v) {
	move_storeys(v.bays, KEY_STOREY_SHIFT, 1, v.storeys + 1);
	move_storeys(v.side_fill, KEY_SIDE_SHIFT, 1, v.storeys + 1);
	move_storeys(v.storey_paint, KEY_SIDE_SHIFT, 1, v.storeys + 1);
	v.fill.insert(v.fill.begin(), v.fill.empty() ? uint8_t(BAY_WALL) : v.fill.front());
	v.storeys++;
	std::vector<std::pair<uint32_t, uint8_t>> above;
	for (const auto &e : v.bays)
		if (key_storey(e.first) == 1) above.push_back({ e.first & ((1u << KEY_STOREY_SHIFT) - 1), e.second });
	for (const auto &e : above) v.bays[e.first] = e.second;
	auto sf = v.side_fill.find(side_key(1, SIDE_BACK));
	if (sf != v.side_fill.end()) v.side_fill[side_key(0, SIDE_BACK)] = sf->second;
}

// The storey 0 of a block taken out, the others numbered one down.
void storey_off(Volume &v) {
	move_storeys(v.bays, KEY_STOREY_SHIFT, -1, v.storeys - 1);
	move_storeys(v.side_fill, KEY_SIDE_SHIFT, -1, v.storeys - 1);
	move_storeys(v.storey_paint, KEY_SIDE_SHIFT, -1, v.storeys - 1);
	if (!v.fill.empty()) v.fill.erase(v.fill.begin());
	v.storeys--;
}

// The bay of storey `k`'s back face that holds a way in, or -1.
int back_door(const Volume &v, int k) {
	for (const auto &e : v.bays)
		if (key_storey(e.first) == k && bay_side(e.first) == SIDE_BACK && is_access(e.second)) return int(e.first & KEY_BYTE);
	return -1;
}

bool has_drive(const LotPlan &p, bool path_too) {
	for (const SidePlan *s : { &p.left, &p.right })
		if (s->use == SIDE_DRIVE || (path_too && s->use == SIDE_DRIVE_PATH)) return true;
	return false;
}

// Where the back is cut off the front, and the ground blocks behind the line and across it.
struct StepLine {
	float y = 0;
	std::vector<size_t> behind, crossing;
};

// The line: the back of the door block, or across its middle - and behind every shaft and every upper block across it.
bool step_line(const LotSlopeShape &K, const Body &b, StepLine &out) {
	std::vector<size_t> solids;
	for (size_t i = 0; i < b.volumes.size(); i++)
		if (solid_on_ground(b.volumes[i])) solids.push_back(i);
	if (solids.empty()) return false;
	for (const Volume &v : b.volumes)
		if (!v.outline.empty() || v.mode != BLOCK_SOLID || v.lift != 0) return false;
	size_t front = solids[0]; // the one with the door, the nearest the street
	for (size_t i : solids) {
		const Volume &p = b.volumes[i], &q = b.volumes[front];
		const bool dp = door_on(p, SIDE_FRONT), dq = door_on(q, SIDE_FRONT);
		if (dp != dq ? dp : p.y < q.y) front = i;
	}
	const Volume &fv = b.volumes[front];
	const float front_back = fv.y + fv.d;
	auto snap_up = [&](float v) { return std::ceil((v - EPS) / K.grid) * K.grid; };
	bool behind_any = false;
	for (size_t i : solids) behind_any = behind_any || b.volumes[i].y >= front_back - EPS;
	float y = behind_any ? front_back : std::round((fv.y + fv.d / 2) / K.grid) * K.grid;
	for (const Core &c : b.cores) y = std::max(y, snap_up(c.y + CORE_SIDE_M));
	for (const Volume &v : b.volumes)
		if (v.base > 0 && v.y < y - EPS && v.y + v.d > y + EPS) y = snap_up(v.y + v.d);
	out.y = y;
	for (size_t i : solids) {
		const Volume &v = b.volumes[i];
		if (v.y >= y - EPS) out.behind.push_back(i);
		else if (v.y + v.d > y + EPS) out.crossing.push_back(i);
	}
	if (out.behind.empty() && out.crossing.empty()) return false;
	for (size_t i : out.crossing)
		if (y - b.volumes[i].y < K.min_part || b.volumes[i].y + b.volumes[i].d - y < K.min_part) return false;
	for (const Volume &v : b.volumes) // nothing above the ground may straddle the line now
		if (v.base > 0 && v.y < y - EPS && v.y + v.d > y + EPS) return false;
	return true;
}

// The level the back takes: of a half storey or a whole one either way (and whole risers for a yard's ramp), the one that cuts and
// fills the ground under the back (and the yard) least, a fill costing more than a cut; false when none saves enough.
bool choose_lift(const LotSlopeShape &K, const Body &b, const LotSite &site, float W, float D, const StepLine &line, const Rect *yard,
		float max_lift, SplitStep &out) {
	std::vector<float> samples;
	const int n = K.samples;
	auto read = [&](float x0, float y0, float x1, float y1) {
		for (int i = 0; i <= n; i++)
			for (int j = 0; j <= n; j++) samples.push_back(site.ground_at(x0 + (x1 - x0) * float(i) / float(n), y0 + (y1 - y0) * float(j) / float(n), W, D));
	};
	bool tall = true;
	for (const std::vector<size_t> *set : { &line.behind, &line.crossing })
		for (size_t i : *set) {
			const Volume &v = b.volumes[i];
			read(v.x, std::max(v.y, line.y), v.x + v.w, v.y + v.d);
			tall = tall && v.storeys >= 2;
		}
	if (yard) read(yard->x0, yard->y0, yard->x1, yard->y1);
	auto cost_at = [&](float lift) {
		const float pad = lift - LOT_PLINTH_M;
		float sum = 0;
		for (float g : samples) sum += pad > g ? (pad - g) * K.fill_weight : g - pad;
		return sum / float(samples.size());
	};
	const float H = b.level_h(0);
	std::vector<std::pair<int, float>> options = { { -SLOPE_WHOLE, -H }, { -1, -H / 2 }, { 1, H / 2 } };
	if (tall) options.push_back({ SLOPE_WHOLE, H });
	const float most = std::min(max_lift, K.max_lift);
	if (yard)
		for (int k = 1; k <= int(std::floor(most / RISER_M + EPS)); k++) {
			options.push_back({ -1, -float(k) * RISER_M });
			options.push_back({ 1, float(k) * RISER_M });
		}
	float best = cost_at(0);
	out.steps = 0;
	for (const auto &o : options) {
		if (std::fabs(o.second) > most + EPS) continue;
		const float c = cost_at(o.second);
		if (c < best - EPS) {
			best = c;
			out.steps = o.first;
			out.lift = o.second;
		}
	}
	out.y = line.y;
	return out.steps != 0 && cost_at(0) - best >= K.gain;
}

// The step made: the blocks across the line split, the back's blocks at their level (a lower ground floor under them, walled where
// earth stands against it, or their ground floor taken by the hill), the blocks on their roofs with them, the parts on their faces.
void make_step(const LotSlopeShape &K, Body &b, float W, const StepLine &line, const SplitStep &s) {
	std::vector<size_t> parts = line.behind;
	for (size_t i : line.crossing) {
		Volume back = split_block(b.volumes[i], line.y);
		b.volumes.push_back(std::move(back));
		parts.push_back(b.volumes.size() - 1);
	}
	const float env_x0 = b.plan.x0 - W / 2, env_x1 = b.plan.x1 - W / 2;
	auto blind = [&](Volume &v, int storey) { // no openings on a face that looks onto a side strip laid higher than it
		for (int side : { SIDE_RIGHT, SIDE_LEFT }) {
			const bool edge = side == SIDE_LEFT ? std::fabs(v.x - env_x0) < K.on_edge : std::fabs(v.x + v.w - env_x1) < K.on_edge;
			if (!edge) continue;
			v.side_fill[side_key(storey, side)] = BAY_WALL;
			drop_bays(v, { side }, storey);
		}
	};
	for (size_t i : parts) {
		Volume &v = b.volumes[i];
		const int door = back_door(v, 0);
		if (s.steps == -SLOPE_WHOLE) {
			storey_under(v);
			v.side_fill[side_key(0, SIDE_FRONT)] = BAY_WALL;
			drop_bays(v, { SIDE_FRONT }, 0);
			blind(v, 0);
			if (door >= 0) { // the back door goes down to the garden's level
				const uint8_t fill = fill_at(v, 1, SIDE_BACK);
				v.bays[bay_key(1, SIDE_BACK, door)] = is_access(fill) ? uint8_t(BAY_WINDOW) : fill;
				v.bays[bay_key(0, SIDE_BACK, door)] = BAY_DOOR;
			}
		} else if (s.steps == SLOPE_WHOLE) {
			storey_off(v);
			if (door >= 0) v.bays[bay_key(0, SIDE_BACK, door)] = BAY_DOOR;
		} else if (s.steps < 0) {
			blind(v, 0);
		}
		v.lift = s.lift;
	}
	for (Volume &v : b.volumes) {
		if (v.base <= 0 || v.y < line.y - EPS) continue;
		v.base += s.steps == -SLOPE_WHOLE ? 1 : s.steps == SLOPE_WHOLE ? -1 : 0;
		v.lift = s.lift;
	}
	const float dz = std::abs(s.steps) == SLOPE_WHOLE ? 0 : s.lift; // a whole storey keeps every floor where it was
	for (LotElement &e : b.elements)
		if ((e.kind == EL_CANOPY || e.kind == EL_AC || e.kind == EL_AWNING || e.kind == EL_CLOCK) && e.y >= line.y - EPS) e.z += dz;
}
} // namespace

bool yard_ramp(const LotSlopeShape &K, const LotPlan &p, DriveRamp &out) {
	if ((p.back != BACK_PARKING && p.back != BACK_LOADING) || !has_drive(p, true)) return false;
	out = { std::max(p.front_depth, K.ramp_landing), p.D - p.back_depth, p.kind == KIND_WORKS ? K.works_grade : K.yard_grade };
	return out.end - out.start >= K.ramp_min;
}

bool garage_ramp(const LotSlopeShape &K, const LotPlan &p, DriveRamp &out) {
	if (p.kind != KIND_HOUSE || !has_drive(p, false)) return false;
	out = { std::max(p.front_depth, K.house_ramp_start), p.D - p.back_depth, K.garage_grade };
	return out.end - out.start >= K.ramp_min;
}

bool step_to_slope(const Data &data, Body &b, const LotSite &site, float W, float D, const Rect *yard, float max_lift, SplitStep &out) {
	const LotSlopeShape &K = data.shapes.slope;
	StepLine line;
	if (!step_line(K, b, line) || !choose_lift(K, b, site, W, D, line, yard, max_lift, out)) return false;
	make_step(K, b, W, line, out);
	return true;
}

bool garage_under(const Data &data, Body &b, bool left, V2 &door, float &width) {
	const float H = b.level_h(0);
	float back = -std::numeric_limits<float>::infinity();
	for (const Volume &v : b.volumes)
		if (solid_on_ground(v)) back = std::max(back, v.y + v.d);
	Volume *pick = nullptr; // the lower block at the back nearest the drive's side
	for (Volume &v : b.volumes) {
		if (!solid_on_ground(v) || !v.outline.empty() || v.storeys < 2 || std::fabs(v.lift + H) > EPS || v.y + v.d < back - EPS) continue;
		if (!pick || (left ? v.x < pick->x : v.x + v.w > pick->x + pick->w)) pick = &v;
	}
	if (!pick) return false;
	Volume &v = *pick;
	const int n = bays_of(v, SIDE_BACK);
	if (n < 1) return false;
	const float w = side_length(v, SIDE_BACK) / float(n);
	if (w < data.shapes.slope.garage_bay - EPS) return false;
	// The back face runs from the block's right to its left (the outline counter-clockwise): the drive's side's two bays first.
	for (int t = 0; t < std::min(n, SLOPE_WHOLE); t++) {
		const int i = left ? n - 1 - t : t;
		if (is_access(bay_at(v, 0, SIDE_BACK, i))) continue;
		v.bays[bay_key(0, SIDE_BACK, i)] = BAY_GARAGE_DOOR;
		door = V2(v.x + v.w - (float(i) + MIDDLE) * w, v.y + v.d);
		width = w;
		return true;
	}
	return false;
}

} // namespace city::b40
