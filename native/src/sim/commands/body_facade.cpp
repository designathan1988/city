// The Facade tab's edits: what a bay holds at each scope, a zone of bays, a whole pattern, a face's rhythm, relief (body_ops.h).
#include "sim/buildings/b40/gen.h"
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_TARGET = "tool.failed";
constexpr float NO_DEPTH = 1e-4f, FREE_RELIEF_STEP_M = 0.01f;
constexpr int SIDE_MASK = (1 << KEY_SIDE_SHIFT) - 1;

Volume *volume_at(Body &b, int vol) { return vol >= 0 && vol < int(b.volumes.size()) ? &b.volumes[size_t(vol)] : nullptr; }

int side_of_key(uint32_t key) { return int((key >> KEY_SIDE_SHIFT) & uint32_t(SIDE_MASK)); }
int storey_of_key(uint32_t key) { return int(key >> KEY_STOREY_SHIFT); }

void set_bay(Volume &v, int storey, int side, int bay, uint8_t what) {
	uint32_t key = bay_key(storey, side, bay);
	v.bays.erase(key);
	if (fill_at(v, storey, side) != what) v.bays[key] = what;
}

void clear_side(Volume &v, int storey, int side) {
	for (auto it = v.bays.begin(); it != v.bays.end();) {
		if (storey_of_key(it->first) == storey && side_of_key(it->first) == side) it = v.bays.erase(it);
		else ++it;
	}
}

void clear_storey(Volume &v, int storey) {
	for (int side = 0; side < v.sides(); side++) {
		clear_side(v, storey, side);
		v.side_fill.erase(side_key(storey, side));
	}
}

// The piece of a relief that a region leaves: under and over it at full width, then left and right beside it.
void cut_relief(const Relief &r, const Relief &region, std::vector<Relief> &out) {
	bool apart = r.side != region.side || r.bay0 > region.bay1 || region.bay0 > r.bay1 || r.storey0 > region.storey1 || region.storey0 > r.storey1;
	if (apart) {
		out.push_back(r);
		return;
	}
	auto piece = [&](int s0, int s1, int b0, int b1) {
		Relief p = r;
		p.storey0 = s0;
		p.storey1 = s1;
		p.bay0 = b0;
		p.bay1 = b1;
		out.push_back(p);
	};
	if (r.storey0 < region.storey0) piece(r.storey0, region.storey0 - 1, r.bay0, r.bay1);
	if (r.storey1 > region.storey1) piece(region.storey1 + 1, r.storey1, r.bay0, r.bay1);
	int s0 = std::max(r.storey0, region.storey0), s1 = std::min(r.storey1, region.storey1);
	if (r.bay0 < region.bay0) piece(s0, s1, r.bay0, region.bay0 - 1);
	if (r.bay1 > region.bay1) piece(s0, s1, region.bay1 + 1, r.bay1);
}

// A pattern's fill for a storey of a face: `first` the volume's first face, the one the street sees.
uint8_t pattern_fill(FacadeKind p, bool ground, bool first) {
	switch (p) {
		case PATTERN_STOREFRONT: return ground ? BAY_SHOPFRONT : BAY_WIDE;
		case PATTERN_OFFICE: return ground && first ? BAY_SHOPFRONT : BAY_WIDE;
		case PATTERN_INDUSTRIAL: return BAY_WALL;
		case PATTERN_ARCADE: return ground ? BAY_PILLAR : BAY_WINDOW;
		case PATTERN_GALLERY: return BAY_PILLAR;
		case PATTERN_ART_DECO: case PATTERN_ART_DECO_CROWN: return BAY_SASH;
		case PATTERN_OBSERVATION: return BAY_WIDE;
		default: return BAY_WINDOW;
	}
}

// The bays a pattern puts over its fill on one storey of one face.
void pattern_bays(Volume &v, FacadeKind p, int storey, int side, bool ground, bool first) {
	int count = bays_of(v, side);
	if (ground && first) {
		if (p == PATTERN_INDUSTRIAL)
			for (int i = 0; i < count; i += 2) v.bays[bay_key(storey, side, i)] = BAY_LOADING_DOOR;
		v.bays[bay_key(storey, side, count / 2)] = BAY_DOOR;
	} else if (p == PATTERN_RESIDENTIAL && storey > 0 && first) {
		for (int i = 1; i < count; i += 2) v.bays[bay_key(storey, side, i)] = BAY_BALCONY;
	} else if (p == PATTERN_INDUSTRIAL && storey > 0) {
		for (int i = 0; i < count; i += 3) v.bays[bay_key(storey, side, i)] = BAY_WINDOW;
	} else if ((p == PATTERN_ART_DECO || p == PATTERN_ART_DECO_CROWN) && !ground) {
		for (int i = 0; i < count; i += 3) v.bays[bay_key(storey, side, i)] = BAY_WALL;
	}
}

void apply_pattern(Volume &v, FacadeKind p, int only_storey, int only_side) {
	for (int k = 0; k < v.storeys; k++) {
		if (only_storey >= 0 && k != only_storey) continue;
		bool ground = v.base + k == 0;
		for (int side = 0; side < v.sides(); side++) {
			if (only_side >= 0 && side != only_side) continue;
			bool first = side == (only_side >= 0 ? only_side : 0);
			clear_side(v, k, side);
			v.side_fill[side_key(k, side)] = pattern_fill(p, ground, first);
			pattern_bays(v, p, k, side, ground, first);
		}
	}
}
} // namespace

std::string op_set_bay(Body &b, int vol, int storey, int side, int bay, int what, int scope) {
	Volume *v = volume_at(b, vol);
	if (!v || storey < 0 || storey >= v->storeys || side < 0 || side >= v->sides() || what < 0 || what >= BAY_COUNT) return NO_TARGET;
	uint8_t c = uint8_t(what);
	switch (scope) {
		case SCOPE_BAY: set_bay(*v, storey, side, bay, c); break;
		case SCOPE_ROW:
			clear_side(*v, storey, side);
			v->side_fill[side_key(storey, side)] = c;
			break;
		case SCOPE_COLUMN:
			for (int k = 0; k < v->storeys; k++) v->bays[bay_key(k, side, bay)] = c;
			break;
		case SCOPE_STOREY:
			clear_storey(*v, storey);
			v->fill[size_t(storey)] = c;
			break;
		case SCOPE_SIDE:
			for (int k = 0; k < v->storeys; k++) {
				clear_side(*v, k, side);
				v->side_fill[side_key(k, side)] = c;
			}
			break;
		case SCOPE_VOLUME:
			for (int k = 0; k < v->storeys; k++) {
				clear_storey(*v, k);
				v->fill[size_t(k)] = c;
			}
			break;
		default: return NO_TARGET;
	}
	return "";
}

std::string op_set_zone(Body &b, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what) {
	Volume *v = volume_at(b, vol);
	if (!v || side < 0 || side >= v->sides() || what < 0 || what >= BAY_COUNT) return NO_TARGET;
	int s0 = std::max(0, std::min(storey0, storey1)), s1 = std::min(v->storeys - 1, std::max(storey0, storey1));
	int i0 = std::min(bay0, bay1), i1 = std::max(bay0, bay1);
	for (int k = s0; k <= s1; k++)
		for (int i = i0; i <= i1; i++) set_bay(*v, k, side, i, uint8_t(what));
	return "";
}

std::string op_set_pattern(Body &b, int vol, int storey, int side, int pattern) {
	if (pattern < 0 || pattern >= PATTERN_COUNT) return NO_TARGET;
	FacadeKind p = FacadeKind(pattern);
	if (vol < 0) {
		for (Volume &v : b.volumes) apply_pattern(v, p, -1, -1);
		return "";
	}
	Volume *v = volume_at(b, vol);
	if (!v) return NO_TARGET;
	apply_pattern(*v, p, storey, side);
	return "";
}

std::string op_set_face(Body &b, const Tuning &t, int vol, int side, const FaceGeometry &patch) {
	Volume *v = volume_at(b, vol);
	if (!v || side < 0 || side >= v->sides()) return NO_TARGET;
	int was = bays_of(*v, side);
	FaceGeometry &f = v->faces[side];
	float lo = float(t.build_least_opening), hi = float(t.build_most_opening), out = float(t.build_max_projection_m);
	if (patch.bays >= 0) f.bays = std::clamp(patch.bays, 1, int(t.build_max_bays));
	if (patch.window_w > 0) f.window_w = std::clamp(patch.window_w, lo, hi);
	if (patch.window_h > 0) f.window_h = std::clamp(patch.window_h, lo, hi);
	if (patch.sill >= 0) f.sill = std::clamp(patch.sill, 0.f, out);
	if (patch.pier_w >= 0) f.pier_w = std::clamp(patch.pier_w, 0.f, out);
	if (patch.pier_d >= 0) f.pier_d = std::clamp(patch.pier_d, 0.f, out);
	if (patch.pier_every >= 0) f.pier_every = std::clamp(patch.pier_every, 1, int(t.build_max_pier_every));
	int now = bays_of(*v, side);
	if (now == was) return "";
	// Fewer or more bays: what was set on one bay goes to the bay at the same place along the face.
	auto index = [&](int i) { return std::min(now - 1, i * now / was); };
	std::unordered_map<uint32_t, uint8_t> bays;
	for (const auto &e : v->bays) {
		int bay = int(e.first & uint32_t(SIDE_MASK));
		bays[side_of_key(e.first) == side ? bay_key(storey_of_key(e.first), side, index(bay)) : e.first] = e.second;
	}
	v->bays = std::move(bays);
	for (Relief &r : v->reliefs) {
		if (r.side != side) continue;
		r.bay0 = index(r.bay0);
		r.bay1 = std::min(now - 1, ((r.bay1 + 1) * now + was - 1) / was - 1);
	}
	return "";
}

std::string op_set_relief(Body &b, const Tuning &t, int vol, const Relief &region, bool snap) {
	Volume *v = volume_at(b, vol);
	if (!v || region.side < 0 || region.side >= v->sides()) return NO_TARGET;
	float step = snap ? float(t.build_relief_step_m) : FREE_RELIEF_STEP_M;
	float d = std::clamp(std::round(region.depth / step) * step, -float(t.build_max_recess_m), float(t.build_max_projection_m));
	std::vector<Relief> kept;
	for (const Relief &r : v->reliefs) cut_relief(r, region, kept);
	if (std::fabs(d) > NO_DEPTH) {
		kept.push_back(region);
		kept.back().depth = d;
	}
	v->reliefs = std::move(kept);
	return "";
}

} // namespace city
