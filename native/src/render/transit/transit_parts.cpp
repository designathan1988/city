// The transport's boxes (transit_parts.h): tables read once; a track in pieces, each on what is under it; a stop from its table, in its frame.
#include "render/transit/transit_parts.h"

#include "core/color.h"
#include "sim/transit/transit_ground.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, FAR_M = 1e9f, MIN_RUN_M = 1e-3f;
constexpr float FULL_TURN = 6.28318530718f;

Color hex(const Variant &v) {
	return Color::hex(parse_hex_color(String(v).utf8().get_data()));
}

Vector3 v3(const Variant &v) {
	const Array a = v;
	return Vector3(float(a[0]), float(a[1]), float(a[2]));
}

Vector2 v2(const Variant &v) {
	const Array a = v;
	return Vector2(float(a[0]), float(a[1]));
}

TrackLayer layer(const Dictionary &d) {
	TrackLayer l;
	l.up = d["up"];
	l.high = d["high"];
	l.wide = d["wide"];
	l.over = d.get("over", 0.f);
	l.along = d.get("long", 0.f);
	return l;
}

TransitBoxes boxes(const TransitLook &look, const Array &list, PackedStringArray &errors) {
	TransitBoxes out;
	for (int i = 0; i < list.size(); i++) {
		const Dictionary d = list[i];
		const std::string name(String(d["part"]).utf8().get_data());
		const auto it = look.part_of.find(name);
		if (it == look.part_of.end()) {
			errors.push_back(String("transit: parte desconhecida ") + name.c_str());
			continue;
		}
		out.push_back({ it->second, v3(d["at"]), v3(d["size"]), bool(d.get("tint", false)) });
	}
	return out;
}

V2 left_of(const V2 &d) { return V2(d.y, -d.x); } // seen from above: x east, z south

float ground(const CityState &s, const V2 &p) { return s.terrain.height_at(p.x, p.y); }

Vector3 on(const V2 &p, float y) { return Vector3(p.x, y, p.y); }

void place(TransitMeshes &m, const TransitLook &look, const TransitBoxes &list, const V2 &at, float y, const V2 &along, const V2 &out, const Color &tint) {
	for (const TransitBox &b : list)
		transit_box(m, look, b.part, on(at + along * b.at.x + out * b.at.y, y + b.at.z), along, b.size, b.tint ? &tint : nullptr);
}

Color line_colour(const CityState &s, const TransitLook &look, int stop) {
	for (const TransitLine &l : s.transit.lines)
		if (std::find(l.stops.begin(), l.stops.end(), stop) != l.stops.end()) return rgb8_to_color(l.colour);
	return look.default_line;
}

// Where a metro station with no footway near has its way down: at the station, or the nearest ground off the carriageway round it.
void entrance_spot(const CityState &s, const TransitLook &look, const V2 &at, V2 &spot, V2 &along) {
	spot = at;
	along = V2(1, 0);
	if (!transit_on_road(s, at) && !transit_built(s, at, 0)) return;
	for (float r = look.entrance_step; r <= look.entrance_reach; r += look.entrance_step)
		for (int k = 0; k < look.entrance_turns; k++) {
			const float a = float(k) / float(look.entrance_turns) * FULL_TURN;
			const V2 d(std::cos(a), std::sin(a));
			const V2 near = at + d * r, spot_at = at + d * (r + look.entrance_clear);
			if (transit_on_road(s, near) || transit_built(s, near, 0) || transit_on_road(s, spot_at) || transit_built(s, spot_at, 0)) continue;
			spot = spot_at;
			along = V2(-d.y, d.x);
			return;
		}
}

void metro_parts(const CityState &s, const TransitLook &look, const TransitStop &q, const Color &tint, TransitMeshes &m) {
	V2 dir, side;
	transit_stop_frame(s, q, dir, side);
	V2 at = q.entrance, along;
	if (q.has_entrance && (q.entrance - q.at).len() > 0) {
		const V2 from = (q.entrance - q.at).norm();
		along = V2(-from.y, from.x);
	} else entrance_spot(s, look, q.at, at, along);
	const float up = ground(s, at), deep = ground(s, q.at) - look.depth;
	place(m, look, look.entrance, at, up, along, left_of(along), tint);
	place(m, look, look.below, q.at, ground(s, q.at), dir, side, tint);
	const int bore = look.part("x_bore");
	transit_box(m, look, bore, on(at, deep), along, Vector3(look.shaft.x, up - deep, look.shaft.y));
	const V2 across = q.at - at;
	if (across.len() > 0) transit_box(m, look, bore, on((at + q.at) * HALF, deep), across.norm(), Vector3(across.len(), look.passage.y, look.passage.x));
}

// One piece of track, in the parts of its mode, on what is under it.
void track_piece(const CityState &s, const TransitLook &look, bool metro, const V2 &a, const V2 &d, float d0, float piece, TransitMeshes &m) {
	const V2 mid = a + d * (d0 + piece * HALF), n = left_of(d);
	float y = ground(s, mid) - (metro ? look.depth : 0);
	bool on_road = false;
	const bool paved = !metro && transit_paved(s, mid, look.kerb, y, on_road); // on a street, flush with it
	const int ballast = look.part(metro ? "x_ballast" : "ballast"), sleeper = look.part(metro ? "x_sleeper" : "sleeper"), rail = look.part(metro ? "x_rail" : "rail");
	if (metro) {
		const int bore = look.part("x_bore"), roof = look.part("x_roof");
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
} // namespace

bool TransitLook::read(const Dictionary &t, PackedStringArray &errors) {
	parts.clear();
	part_of.clear();
	const Dictionary list = t["parts"];
	const Array names = list.keys();
	for (int i = 0; i < names.size(); i++) {
		const Dictionary d = list[names[i]];
		TransitPart p;
		p.colour = hex(d["colour"]);
		p.roughness = d["roughness"];
		p.metal = d["metal"];
		p.xray = d.get("xray", -1);
		p.glass = d.get("glass", false);
		part_of[std::string(String(names[i]).utf8().get_data())] = int(parts.size());
		parts.push_back(p);
	}
	default_line = hex(t["default_line"]);
	run = t["run_m"];
	const Dictionary k = t["track"], m = t["metro"];
	gauge = k["gauge_m"];
	piece = k["piece_m"];
	sleeper_step = k["sleeper_step_m"];
	ballast = layer(k["ballast"]);
	sleeper = layer(k["sleeper"]);
	rail = layer(k["rail"]);
	rail_paved_up = Dictionary(k["rail"])["paved_up"];
	rail_paved_high = Dictionary(k["rail"])["paved_high"];
	depth = m["rails_below_m"];
	bore_wide = m["wide_m"];
	bore_high = m["high_m"];
	floor_up = m["floor_up_m"];
	slab = m["shell_m"];
	wall_off = m["side_gap_m"];
	roof_wide = m["roof_wide_m"];
	bore_over = m["over_m"];
	shaft = v2(m["shaft_m"]);
	passage = v2(m["passage_m"]);
	entrance_step = m["entrance_step_m"];
	entrance_reach = m["entrance_reach_m"];
	entrance_clear = m["entrance_clear_m"];
	entrance_turns = m["entrance_turns"];
	station = boxes(*this, t["station"], errors);
	stop = boxes(*this, t["stop"], errors);
	terminal = boxes(*this, t["terminal"], errors);
	entrance = boxes(*this, t["entrance"], errors);
	below = boxes(*this, t["below"], errors);
	car = v3(t["car_m"]);
	car_up = t["car_up_m"];
	return errors.is_empty();
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

void transit_track_parts(const CityState &s, const TransitLook &look, const RailTrack &track, std::vector<TransitMeshes> &out) {
	const bool metro = track.mode == TransitMode::METRO;
	float walked = 0;
	for (size_t i = 1; i < track.points.size(); i++) {
		const V2 &a = track.points[i - 1], &b = track.points[i];
		const float len = (b - a).len();
		if (len < MIN_RUN_M) continue;
		const V2 d = (b - a).norm();
		const int pieces = std::max(1, int(std::ceil(len / look.piece)));
		const float piece = len / float(pieces);
		for (int k = 0; k < pieces; k++, walked += piece) {
			const size_t stretch = size_t(walked / look.run);
			if (out.size() <= stretch) out.resize(stretch + 1);
			track_piece(s, look, metro, a, d, float(k) * piece, piece, out[stretch]);
		}
	}
}

void transit_stop_parts(const CityState &s, const TransitLook &look, const TransitStop &q, TransitMeshes &out) {
	const Color tint = line_colour(s, look, q.id);
	if (q.mode == TransitMode::METRO) {
		metro_parts(s, look, q, tint, out);
		return;
	}
	V2 along, side;
	transit_stop_frame(s, q, along, side);
	place(out, look, q.mode == TransitMode::TRAIN ? look.station : (q.terminal ? look.terminal : look.stop), q.at, ground(s, q.at), along, side, tint);
}

} // namespace city
