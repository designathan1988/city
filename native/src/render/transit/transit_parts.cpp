// The transport's boxes (transit_parts.h): tables read once; a track in pieces, each on what is under it; a stop from its table, in its frame.
#include "render/transit/transit_parts.h"
#include "core/math.h"

#include "render/data_reader.h"
#include "sim/transit/transit_ground.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, MIN_RUN_M = 1e-3f; // a run this short has no direction

Vector3 v3(DataReader r, const Array &a) {
	r.expect(a, 3);
	return Vector3(float(r.num_at(a, 0)), float(r.num_at(a, 1)), float(r.num_at(a, 2)));
}
Vector3 v3(DataReader &r, const Dictionary &d, const char *key) { return v3(r.in(key), r.list(d, key)); }

Vector2 v2(DataReader &r, const Dictionary &d, const char *key) {
	const Array a = r.list(d, key, 2);
	return Vector2(float(r.in(key).num_at(a, 0)), float(r.in(key).num_at(a, 1)));
}

TrackLayer layer(DataReader r, const Dictionary &d) {
	TrackLayer l;
	l.up = float(r.num(d, "up"));
	l.high = float(r.num(d, "high"));
	l.wide = float(r.num(d, "wide"));
	l.over = float(r.num_or(d, "over", 0.0));
	l.along = float(r.num_or(d, "long", 0.0));
	return l;
}

using PartNames = std::unordered_map<std::string, int>;

int part_named(const PartNames &names, const std::string &name, DataReader &r) {
	const auto it = names.find(name);
	if (it != names.end()) return it->second;
	r.fail(String("unknown part '") + name.c_str() + String("'"));
	return 0;
}

TransitBoxes boxes(const PartNames &names, DataReader r, const Array &list) {
	TransitBoxes out;
	for (int i = 0; i < list.size(); i++) {
		DataReader rb = r.in(i);
		const Dictionary d = r.table_at(list, i);
		const int part = part_named(names, rb.utf8(d, "part"), rb);
		out.push_back({ part, v3(rb, d, "at"), v3(rb, d, "size"), rb.flag_or(d, "tint", false) });
	}
	return out;
}

V2 left_of(const V2 &d) { return V2(d.y, -d.x); } // seen from above: x east, z south

float ground(const CityView &s, const V2 &p) { return s.terrain.height_at(p.x, p.y); }

Vector3 on(const V2 &p, float y) { return Vector3(p.x, y, p.y); }

void place(TransitMeshes &m, const TransitLook &look, const TransitBoxes &list, const V2 &at, float y, const V2 &along, const V2 &out, const Color &tint) {
	for (const TransitBox &b : list)
		transit_box(m, look, b.part, on(at + along * b.at.x + out * b.at.y, y + b.at.z), along, b.size, b.tint ? &tint : nullptr);
}

// Where a metro station with no footway near has its way down: at the station, or the nearest ground off the carriageway round it.
void entrance_spot(const CityView &s, const TransitLook &look, const V2 &at, V2 &spot, V2 &along) {
	spot = at;
	along = V2(1, 0);
	if (!transit_on_road(s, at) && !transit_built(s, at, 0)) return;
	for (float r = look.entrance_step; r <= look.entrance_reach; r += look.entrance_step)
		for (int k = 0; k < look.entrance_turns; k++) {
			const float a = float(k) / float(look.entrance_turns) * TAU_F;
			const V2 d(std::cos(a), std::sin(a));
			const V2 near = at + d * r, spot_at = at + d * (r + look.entrance_clear);
			if (transit_on_road(s, near) || transit_built(s, near, 0) || transit_on_road(s, spot_at) || transit_built(s, spot_at, 0)) continue;
			spot = spot_at;
			along = V2(-d.y, d.x);
			return;
		}
}

void metro_parts(const CityView &s, const TransitLook &look, const TransitStop &q, const RailTrack *track, const Color &tint, TransitMeshes &m) {
	V2 dir, side;
	transit_stop_frame(s, q, track, dir, side);
	V2 at = q.entrance, along;
	if (q.has_entrance && (q.entrance - q.at).len() > 0) {
		const V2 from = (q.entrance - q.at).norm();
		along = V2(-from.y, from.x);
	} else entrance_spot(s, look, q.at, at, along);
	const float up = ground(s, at), deep = ground(s, q.at) - look.depth;
	place(m, look, look.entrance, at, up, along, left_of(along), tint);
	place(m, look, look.below, q.at, ground(s, q.at), dir, side, tint);
	const int bore = look.bore_part;
	transit_box(m, look, bore, on(at, deep), along, Vector3(look.shaft.x, up - deep, look.shaft.y));
	const V2 across = q.at - at;
	if (across.len() > 0) transit_box(m, look, bore, on((at + q.at) * HALF, deep), across.norm(), Vector3(across.len(), look.passage.y, look.passage.x));
}

// One piece of track, in the parts of its mode, on what is under it.
void track_piece(const CityView &s, const TransitLook &look, bool metro, const V2 &a, const V2 &d, float d0, float piece, TransitMeshes &m) {
	const V2 mid = a + d * (d0 + piece * HALF), n = left_of(d);
	float y = ground(s, mid) - (metro ? look.depth : 0);
	bool on_road = false;
	const bool paved = !metro && transit_paved(s, mid, look.kerb, y, on_road); // on a street, flush with it
	const int ballast = look.ballast_part[metro], sleeper = look.sleeper_part[metro], rail = look.rail_part[metro];
	if (metro) {
		const int bore = look.bore_part, roof = look.roof_part;
		const float base = y + look.floor_up, along = piece + look.bore_over, side = look.bore_wide * HALF + look.wall_off;
		transit_box(m, look, bore, on(mid, base), d, Vector3(along, look.slab, look.bore_wide));
		for (int k : { -1, 1 }) transit_box(m, look, bore, on(mid + n * (float(k) * side), base), d, Vector3(along, look.bore_high, look.slab));
		transit_box(m, look, roof, on(mid, base + look.bore_high), d, Vector3(along, look.slab, look.roof_wide));
	}
	if (!paved) {
		transit_box(m, look, ballast, on(mid, y + look.ballast.up), d, Vector3(piece + look.ballast.over, look.ballast.high, look.ballast.wide));
		for (float at = d0 + look.sleeper_step * HALF; at < d0 + piece; at += look.sleeper_step)
			transit_box(m, look, sleeper, on(a + d * at, y + look.sleeper.up), n, Vector3(look.sleeper.along, look.sleeper.high, look.sleeper.wide));
	}
	for (int k : { -1, 1 })
		transit_box(m, look, rail, on(mid + n * (float(k) * look.gauge * HALF), y + (paved ? look.rail_paved_up : look.rail.up)), d,
				Vector3(piece + look.rail.over, paved ? look.rail_paved_high : look.rail.high, look.rail.wide));
}

// What a train track has under a point: 0 open ground, 1 a footway, 2 a carriageway.
int under_track(const CityView &s, const TransitLook &look, const V2 &p) {
	float y = 0;
	bool road = false;
	return transit_paved(s, p, look.kerb, y, road) ? (road ? 2 : 1) : 0;
}

// A train track's piece cut where what is under it changes, to a sleeper's step: whole, its bed would run on over the footway at the
// height of the ground beyond, with the rails left in the air.
void track_cut(const CityView &s, const TransitLook &look, const V2 &a, const V2 &d, float d0, float piece, TransitMeshes &m) {
	const float step = look.sleeper_step, end = d0 + piece;
	float from = d0;
	int was = under_track(s, look, a + d * (d0 + step * HALF));
	for (float at = d0 + step; at < end; at += step) {
		const int now = under_track(s, look, a + d * (at + step * HALF));
		if (now == was) continue;
		track_piece(s, look, false, a, d, from, at - from, m);
		from = at;
		was = now;
	}
	track_piece(s, look, false, a, d, from, end - from, m);
}
} // namespace

bool TransitLook::read(const Dictionary &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "transit.json" };
	parts.clear();
	PartNames names;
	const Dictionary list = r.table(t, "parts");
	const Array keys = list.keys();
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		DataReader rp = r.in("parts").in(key);
		const Dictionary d = r.in("parts").table(list, key.utf8().get_data());
		TransitPart p;
		p.colour = Color::hex(rp.colour(d, "colour"));
		p.roughness = float(rp.num(d, "roughness"));
		p.metal = float(rp.num(d, "metal"));
		p.xray = rp.integer_or(d, "xray", -1);
		if (p.xray >= TRANSIT_XRAY_LAYERS) rp.fail("xray: past the last layer");
		p.glass = rp.flag_or(d, "glass", false);
		names[key.utf8().get_data()] = int(parts.size());
		parts.push_back(p);
	}
	DataReader rn = r.in("parts");
	const char *const surface[2] = { "", "x_" };
	for (int m = 0; m < 2; m++) {
		ballast_part[m] = part_named(names, std::string(surface[m]) + "ballast", rn);
		sleeper_part[m] = part_named(names, std::string(surface[m]) + "sleeper", rn);
		rail_part[m] = part_named(names, std::string(surface[m]) + "rail", rn);
	}
	bore_part = part_named(names, "x_bore", rn);
	roof_part = part_named(names, "x_roof", rn);
	car_part = part_named(names, "car", rn);
	default_line = Color::hex(r.colour(t, "default_line"));
	run = float(r.num(t, "run_m"));
	const Dictionary k = r.table(t, "track"), m = r.table(t, "metro");
	DataReader rk = r.in("track"), rm = r.in("metro");
	gauge = float(rk.num(k, "gauge_m"));
	piece = float(rk.num(k, "piece_m"));
	sleeper_step = float(rk.num(k, "sleeper_step_m"));
	ballast = layer(rk.in("ballast"), rk.table(k, "ballast"));
	sleeper = layer(rk.in("sleeper"), rk.table(k, "sleeper"));
	const Dictionary rails = rk.table(k, "rail");
	rail = layer(rk.in("rail"), rails);
	rail_paved_up = float(rk.in("rail").num(rails, "paved_up"));
	rail_paved_high = float(rk.in("rail").num(rails, "paved_high"));
	depth = float(rm.num(m, "rails_below_m"));
	bore_wide = float(rm.num(m, "wide_m"));
	bore_high = float(rm.num(m, "high_m"));
	floor_up = float(rm.num(m, "floor_up_m"));
	slab = float(rm.num(m, "shell_m"));
	wall_off = float(rm.num(m, "side_gap_m"));
	roof_wide = float(rm.num(m, "roof_wide_m"));
	bore_over = float(rm.num(m, "over_m"));
	shaft = v2(rm, m, "shaft_m");
	passage = v2(rm, m, "passage_m");
	entrance_step = float(rm.num(m, "entrance_step_m"));
	entrance_reach = float(rm.num(m, "entrance_reach_m"));
	entrance_clear = float(rm.num(m, "entrance_clear_m"));
	entrance_turns = rm.integer(m, "entrance_turns");
	station = boxes(names, r.in("station"), r.list(t, "station"));
	stop = boxes(names, r.in("stop"), r.list(t, "stop"));
	terminal = boxes(names, r.in("terminal"), r.list(t, "terminal"));
	entrance = boxes(names, r.in("entrance"), r.list(t, "entrance"));
	below = boxes(names, r.in("below"), r.list(t, "below"));
	car = v3(r, t, "car_m");
	car_up = float(r.num(t, "car_up_m"));
	preview.clear();
	const Dictionary seen = r.table(t, "preview");
	const Array seen_keys = seen.keys();
	DataReader rv = r.in("preview");
	for (int i = 0; i < seen_keys.size(); i++) {
		const String key = seen_keys[i];
		const float opacity = float(rv.num(seen, key.utf8().get_data()));
		const int part = part_named(names, key.utf8().get_data(), rv);
		if (opacity <= 0 || opacity > 1) rv.fail(key + String(": an opacity is over 0 and at most 1"));
		if (parts[size_t(part)].xray >= 0) rv.fail(key + String(": a part drawn through the ground keeps its own"));
		preview.push_back({ part, opacity });
	}
	preview_step = float(r.num(t, "preview_step_m"));
	if (preview_step <= 0) r.fail("preview_step_m: must be positive");
	return errors.size() == before;
}

void transit_box(TransitMeshes &m, const TransitLook &look, int part, const Vector3 &base, const V2 &dir, const Vector3 &size, const Color *tint) {
	const TransitPart &p = look.parts[size_t(part)];
	MeshBuilder &mb = p.xray >= 0 ? m.xray[p.xray] : (p.glass ? m.glass : m.solid);
	const Color c = tint ? *tint : p.colour;
	const Vector2 t(p.roughness, p.metal);
	const Vector2 uvs[4] = { t, t, t, t };
	const V2 d = dir * (size.x * HALF), n = V2(-dir.y, dir.x) * (size.z * HALF), at(base.x, base.z);
	const V2 q[4] = { at - d - n, at + d - n, at + d + n, at - d + n };
	const float y0 = base.y, y1 = base.y + size.y;
	for (int i = 0; i < 4; i++) {
		const V2 &a = q[i], &b = q[(i + 1) % 4], e = b - a, out(e.y, -e.x);
		mb.quad(on(a, y0), on(b, y0), on(b, y1), on(a, y1), c, Vector3(out.x, 0, out.y), uvs);
	}
	mb.quad(on(q[0], y1), on(q[1], y1), on(q[2], y1), on(q[3], y1), c, Vector3(0, 1, 0), uvs);
}

namespace {
// Every piece of a track, in order: fn(walked, a, d, d0, piece) with the metres walked to it, its run's start and direction, where
// it starts along the run and its length. A stretch is look.run metres of that walk.
template <typename F>
void for_pieces(const TransitLook &look, const RailTrack &track, F &&fn) {
	float walked = 0;
	for (size_t i = 1; i < track.points.size(); i++) {
		const V2 &a = track.points[i - 1], &b = track.points[i];
		const float len = (b - a).len();
		if (len < MIN_RUN_M) continue;
		const V2 d = (b - a).norm();
		const int pieces = std::max(1, int(std::ceil(len / look.piece)));
		const float piece = len / float(pieces);
		for (int k = 0; k < pieces; k++, walked += piece) fn(walked, a, d, float(k) * piece, piece);
	}
}

// One piece in its track's mode.
void lay_piece(const CityView &s, const TransitLook &look, bool metro, const V2 &a, const V2 &d, float d0, float piece, TransitMeshes &m) {
	if (metro) track_piece(s, look, true, a, d, d0, piece, m);
	else track_cut(s, look, a, d, d0, piece, m);
}
} // namespace

Color transit_line_colour(const TransitData &plan, const TransitLook &look, int stop) {
	for (const TransitLine &l : plan.lines)
		if (std::find(l.stops.begin(), l.stops.end(), stop) != l.stops.end()) return rgb8_to_color(l.colour);
	return look.default_line;
}

std::vector<CutBox> transit_track_stretches(const TransitLook &look, const RailTrack &track) {
	std::vector<CutBox> out;
	for_pieces(look, track, [&](float walked, const V2 &a, const V2 &d, float d0, float piece) {
		const size_t stretch = size_t(walked / look.run);
		const V2 p0 = a + d * d0, p1 = a + d * (d0 + piece);
		const CutBox box{ std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::max(p0.x, p1.x), std::max(p0.y, p1.y) };
		if (out.size() <= stretch) out.resize(stretch + 1, box);
		out[stretch].take(box.minx, box.minz, box.maxx, box.maxz);
	});
	return out;
}

void transit_track_stretch(const CityView &s, const TransitLook &look, const RailTrack &track, size_t stretch, TransitMeshes &out) {
	const bool metro = track.mode == TransitMode::METRO;
	for_pieces(look, track, [&](float walked, const V2 &a, const V2 &d, float d0, float piece) {
		if (size_t(walked / look.run) == stretch) lay_piece(s, look, metro, a, d, d0, piece, out);
	});
}

void transit_track_whole(const CityView &s, const TransitLook &look, const RailTrack &track, TransitMeshes &out) {
	const bool metro = track.mode == TransitMode::METRO;
	for_pieces(look, track, [&](float, const V2 &a, const V2 &d, float d0, float piece) { lay_piece(s, look, metro, a, d, d0, piece, out); });
}

void transit_stop_parts(const CityView &s, const TransitLook &look, const TransitStop &q, const RailTrack *track, const Color &tint, TransitMeshes &out) {
	if (q.mode == TransitMode::METRO) {
		metro_parts(s, look, q, track, tint, out);
		return;
	}
	V2 along, side;
	transit_stop_frame(s, q, track, along, side);
	place(out, look, q.mode == TransitMode::TRAIN ? look.station : (q.terminal ? look.terminal : look.stop), q.at, ground(s, q.at), along, side, tint);
}

} // namespace city
