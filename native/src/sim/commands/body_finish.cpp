// The Roof and Paint tabs' edits: a roof's kind and shape, and what a building, a block, a side or a floor is finished in (body_ops.h).
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_TARGET = "tool.failed";
constexpr int RIDGE_ALONG_Y = 1;
constexpr const char *NO_FIT = "building.problem.roofPart";
constexpr const char *NO_MAST = "building.problem.noMast";

Volume *volume_at(Body &b, int vol) { return vol >= 0 && vol < int(b.volumes.size()) ? &b.volumes[size_t(vol)] : nullptr; }

// The paint for this finish and colour: the one the body has, or a new one. Bodies carry few paints, so a walk is enough.
int paint_of(Body &b, int finish, uint32_t rgb) {
	for (size_t i = 0; i < b.paints.size(); i++)
		if (b.paints[i].finish == finish && b.paints[i].rgb == rgb) return int(i);
	return b.add_paint(Paint{ int8_t(finish), rgb });
}

void clear_floor_paints(Volume &v) { v.storey_paint.clear(); }

// Every corner of the part on the block's plan.
bool fits(const Volume &v, const RoofPart &p) {
	const Ring ring = ring_of(v);
	float c = std::cos(p.turn), s = std::sin(p.turn);
	for (float sx : { -1.f, 1.f })
		for (float sy : { -1.f, 1.f }) {
			float dx = sx * p.w / 2, dy = sy * p.d / 2;
			if (!contains_point(ring, { p.x + dx * c - dy * s, p.y + dx * s + dy * c })) return false;
		}
	return true;
}

void clear_side_paints(Volume &v, int side) {
	for (int k = 0; k < v.storeys; k++) v.storey_paint.erase(side_key(k, side));
}

// The mast the point of the plan is at: the nearest spire or lantern within its own size, or none.
RoofPart *mast_at(Volume &v, float x, float y) {
	RoofPart *best = nullptr;
	float best_d = 0;
	for (RoofPart &p : v.gear) {
		if (p.kind != GEAR_SPIRE && p.kind != GEAR_LANTERN) continue;
		const float d = std::hypot(p.x - x, p.y - y);
		if (d <= std::max(p.w, p.d) && (!best || d < best_d)) {
			best = &p;
			best_d = d;
		}
	}
	return best;
}
} // namespace

std::string op_set_roof(Body &b, int vol, int roof) {
	Volume *v = volume_at(b, vol);
	if (!v || roof < ROOF_FLAT || roof > ROOF_SAWTOOTH) return NO_TARGET;
	v->roof = Roof(roof);
	return "";
}

std::string op_roof_shape(Body &b, const Tuning &t, int vol, float pitch, int ridge, int fall) {
	Volume *v = volume_at(b, vol);
	if (!v || ridge > RIDGE_ALONG_Y || fall > 3) return NO_TARGET;
	v->pitch = pitch < 0 ? -1 : std::clamp(std::round(pitch), float(t.build_min_pitch_deg), float(t.build_max_pitch_deg));
	v->ridge = ridge;
	v->fall = fall;
	return "";
}

std::string op_add_gear(Body &b, const Tuning &t, int vol, int kind, float x, float y, float w, float d, float h) {
	Volume *v = volume_at(b, vol);
	if (!v || kind < 0 || kind >= GEAR_COUNT) return NO_TARGET;
	if (int(v->gear.size()) >= t.build_max_gear) return "building.problem.size";
	RoofPart p;
	p.kind = uint8_t(kind);
	p.x = x;
	p.y = y;
	p.w = w;
	p.d = d;
	p.h = h;
	if (kind == GEAR_LANTERN) p.flag = DEFAULT_FLAG;
	if (!fits(*v, p)) return NO_FIT;
	v->gear.push_back(p);
	return "";
}

std::string op_set_flag(Body &b, int vol, float x, float y, int pattern) {
	Volume *v = volume_at(b, vol);
	if (!v || pattern < FLAG_NONE || pattern >= FLAG_COUNT) return NO_TARGET;
	RoofPart *mast = mast_at(*v, x, y);
	if (!mast) return NO_MAST;
	mast->flag.pattern = int8_t(pattern);
	return "";
}

std::string op_flag_colour(Body &b, int vol, float x, float y, int slot, uint32_t rgb) {
	Volume *v = volume_at(b, vol);
	if (!v || slot < 0 || slot >= FLAG_COLOURS) return NO_TARGET;
	RoofPart *mast = mast_at(*v, x, y);
	if (!mast) return NO_MAST;
	mast->flag.rgb[slot] = rgb;
	if (mast->flag.pattern == FLAG_NONE) mast->flag.pattern = DEFAULT_FLAG.pattern;
	return "";
}

std::string op_remove_gear(Body &b, int vol, int gear) {
	Volume *v = volume_at(b, vol);
	if (!v || gear < 0 || gear >= int(v->gear.size())) return NO_TARGET;
	v->gear.erase(v->gear.begin() + gear);
	return "";
}

std::string op_turn_gear(Body &b, const Tuning &t, int vol, int gear) {
	Volume *v = volume_at(b, vol);
	if (!v || gear < 0 || gear >= int(v->gear.size())) return NO_TARGET;
	RoofPart p = v->gear[size_t(gear)];
	p.turn += float(t.build_gear_turn_deg) * float(DEG_TO_RAD);
	if (!fits(*v, p)) return NO_FIT;
	v->gear[size_t(gear)] = p;
	return "";
}

std::string op_paint(Body &b, int scope, int vol, int side, int storey, int finish, uint32_t rgb) {
	if (finish < FIN_PLASTER || finish >= FIN_COUNT) return NO_TARGET;
	int paint = paint_of(b, finish, rgb);
	Volume *v = volume_at(b, vol);
	switch (scope) {
		case PAINT_BUILDING_WALL:
			b.wall = paint;
			for (Volume &each : b.volumes) {
				each.wall = -1;
				clear_floor_paints(each);
			}
			return "";
		case PAINT_BUILDING_ROOF:
			b.roof = paint;
			for (Volume &each : b.volumes) each.roof_paint = -1;
			return "";
		case PAINT_BUILDING_TRIM:
			b.trim = paint;
			return "";
		case PAINT_VOLUME_WALL:
			if (!v) return NO_TARGET;
			v->wall = paint;
			clear_floor_paints(*v);
			return "";
		case PAINT_VOLUME_ROOF:
			if (!v) return NO_TARGET;
			v->roof_paint = paint;
			return "";
		case PAINT_SIDE:
			if (!v || side < 0 || side >= v->sides()) return NO_TARGET;
			clear_side_paints(*v, side);
			for (int k = 0; k < v->storeys; k++) v->storey_paint[side_key(k, side)] = paint;
			return "";
		case PAINT_FLOOR:
			if (!v || storey < 0 || storey >= v->storeys) return NO_TARGET;
			for (int s = 0; s < v->sides(); s++) v->storey_paint[side_key(storey, s)] = paint;
			return "";
		default: return NO_TARGET;
	}
}

} // namespace city
