// The Roof and Paint tabs' edits: a roof's kind and shape, and what a building, a block, a side or a floor is finished in (body_ops.h).
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_TARGET = "tool.failed";
constexpr int RIDGE_ALONG_Y = 1;

Volume *volume_at(Body &b, int vol) { return vol >= 0 && vol < int(b.volumes.size()) ? &b.volumes[size_t(vol)] : nullptr; }

// The paint for this finish and colour: the one the body has, or a new one. Bodies carry few paints, so a walk is enough.
int paint_of(Body &b, int finish, uint32_t rgb) {
	for (size_t i = 0; i < b.paints.size(); i++)
		if (b.paints[i].finish == finish && b.paints[i].rgb == rgb) return int(i);
	return b.add_paint(Paint{ int8_t(finish), rgb });
}

void clear_floor_paints(Volume &v) { v.storey_paint.clear(); }

void clear_side_paints(Volume &v, int side) {
	for (int k = 0; k < v.storeys; k++) v.storey_paint.erase(side_key(k, side));
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
