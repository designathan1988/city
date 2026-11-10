// Where what the player places stands, or why it cannot (Roadcraft landscape.ts snapLandscape, editor/barriers.ts): an item
// on the sidewalk under the pointer, at its kind's depth, clear of the crosswalks' landings and of what stands there; a
// barrier on the footway's back, or at the kerb for a guardrail and a railing.
#include "sim/commands/commands.h"
#include "sim/commands/street_commands.h"

#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double UNIT_LEN_EPS = 1e-6;
const char *const OFF_FOOTWAY = "hint.streetscape.offFootway";
const char *const NARROW = "hint.streetscape.narrow";
const char *const OCCUPIED = "hint.streetscape.occupied";
const char *const CROSSING = "hint.streetscape.crossing";

float radius_of(const StreetFurnitureDef &d, FurnitureKind k) {
	const Footprint &f = d.footprint[int(k)];
	return std::max(f.x_m, f.z_m) * 0.5f;
}

// What stands within `radius` of (x, z): what the player placed and the street's own kit on `seg`.
bool occupied(const CityState &s, double x, double z, float radius, int seg) {
	const StreetFurnitureDef &d = s.catalog.furniture;
	const float reach = radius + d.items.gap_m + radius_of(d, FurnitureKind::BENCH), fx = float(x), fz = float(z);
	bool taken = false;
	s.street.items_in(fx - reach, fz - reach, fx + reach, fz + reach, [&](int id) {
		const StreetItem &o = s.street.items.items[size_t(id)];
		const FurnitureKind k = item_piece(o.kind);
		if (k == FurnitureKind::COUNT) return;
		taken = taken || std::hypot(o.x - fx, o.z - fz) < radius + radius_of(d, k) + d.items.gap_m;
	});
	s.street.poles_in(fx - reach, fz - reach, fx + reach, fz + reach, [&](int id) {
		const UtilityPole &o = s.street.poles.items[size_t(id)];
		taken = taken || std::hypot(o.x - fx, o.z - fz) < radius + radius_of(d, FurnitureKind::POLE) + d.items.gap_m;
	});
	if (taken || seg < 0) return taken;
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, seg, FURNITURE_ALL, spots);
	for (const FurnitureSpot &p : spots) {
		double px, pz, py;
		footway_point(s.roads, p.seg, p.v, p.u, px, pz, py);
		if (std::hypot(px - x, pz - z) < radius + radius_of(d, p.kind) + d.items.gap_m) return true;
	}
	return false;
}
} // namespace

ItemSnap snap_item(const CityState &s, ItemKind kind, double x, double z, double reach) {
	ItemSnap out;
	out.x = x;
	out.z = z;
	const RoadNetwork &net = s.roads;
	const StreetFurnitureDef &d = s.catalog.furniture;
	if (kind == ItemKind::STREETNAME) {
		out.ok = on_road(net, x, z);
		if (!out.ok) out.reason = OFF_FOOTWAY;
		return out;
	}
	if (kind == ItemKind::MEADOW) { // long grass goes on open ground, never on a street
		bool near = false;
		const float fx = float(x), fz = float(z), apart = d.items.meadow_apart_m;
		s.street.items_in(fx - apart, fz - apart, fx + apart, fz + apart, [&](int id) {
			const StreetItem &o = s.street.items.items[size_t(id)];
			near = near || (o.kind == ItemKind::MEADOW && std::hypot(o.x - fx, o.z - fz) < apart);
		});
		out.ok = !near && !on_road(net, x, z);
		if (!out.ok) out.reason = near ? OCCUPIED : OFF_FOOTWAY;
		return out;
	}
	const FurnitureKind piece = item_piece(kind);
	const float radius = radius_of(d, piece);
	FootwayHit hit = footway_at(net, x, z, reach);
	if (!hit.ok && (kind == ItemKind::TREE || kind == ItemKind::SHRUB)) { // a plant in a planted median, on its centre line
		const FootwayHit m = median_at(net, x, z, std::min(reach, double(d.items.median_reach_m)));
		if (m.ok) {
			double y;
			footway_point(net, m.seg, m.v, m.u, out.x, out.z, y);
			out.ok = !occupied(s, out.x, out.z, radius, -1);
			if (!out.ok) out.reason = OCCUPIED;
			return out;
		}
	}
	if (!hit.ok) {
		out.reason = OFF_FOOTWAY;
		return out;
	}
	float u;
	if (!kerbside_place(net, hit.seg, net.profile_of(hit.seg), hit.side, piece, u)) {
		out.reason = NARROW;
		return out;
	}
	hit.u = u;
	double y;
	footway_point(net, hit.seg, hit.v, u, out.x, out.z, y);
	if (on_landing(net, hit, radius, d.poles.landing_clear_m)) out.reason = CROSSING;
	else if (occupied(s, out.x, out.z, radius, hit.seg)) out.reason = OCCUPIED;
	out.ok = out.reason.empty();
	return out;
}

void snap_barrier_point(const CityState &s, BarrierKind kind, double &x, double &z) {
	const RoadNetwork &net = s.roads;
	const BarrierDef &d = s.catalog.furniture.barriers;
	const float half = d.thickness_m[int(kind)] * 0.5f;
	const bool at_kerb = kind == BarrierKind::GUARDRAIL || kind == BarrierKind::RAILING;
	const FootwayHit hit = footway_at(net, x, z, d.footway_reach_m);
	if (!hit.ok) return;
	const Profile &p = net.profile_of(hit.seg);
	const StripEdge &e = hit.side == 0 ? p.edges.front() : p.edges.back();
	float u;
	if (at_kerb) u = sidewalk_u(p, hit.side, half + d.kerb_gap_m);
	else u = hit.side == 0 ? e.u0 - half - d.back_gap_m : e.u1 + half + d.back_gap_m; // past the back of the footway
	double y;
	footway_point(net, hit.seg, hit.v, u, x, z, y);
}

std::string barrier_problem(const CityState &s, BarrierKind kind, const std::vector<float> &pts) {
	const BarrierDef &d = s.catalog.furniture.barriers;
	double length = 0;
	for (size_t i = 0; i + 3 < pts.size(); i += 2) length += std::hypot(pts[i + 2] - pts[i], pts[i + 3] - pts[i + 1]);
	if (pts.size() < 4 || length < d.min_run_m) return "hint.barrier.short";
	const bool at_kerb = kind == BarrierKind::GUARDRAIL || kind == BarrierKind::RAILING;
	for (size_t i = 0; i + 3 < pts.size(); i += 2) {
		const double ax = pts[i], az = pts[i + 1], dx = pts[i + 2] - ax, dz = pts[i + 3] - az, len = std::hypot(dx, dz);
		if (len < UNIT_LEN_EPS) continue;
		const int steps = std::max(1, int(std::ceil(len / d.footway_reach_m)));
		for (int k = 0; k <= steps; k++) {
			const double x = ax + dx * k / steps, z = az + dz * k / steps;
			if (at_kerb ? on_carriageway(s.roads, x, z, 0.f) : on_road(s.roads, x, z)) return "hint.barrier.road";
		}
	}
	return "";
}

} // namespace city
