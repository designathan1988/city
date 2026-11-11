// Where a piece of street furniture stands, see street_furniture.h: a piece's spot and footprint, a plate's post, the
// hydrants' coverage squares and the stretch of a road open to the sky. Shared by the layout (street_furniture.cpp, which owns
// the sidewalk's clearances and the kerbside places), the posts (street_furniture_posts.cpp) and the player's pieces
// (sim/street); every distance comes from data/catalog/street_furniture.json.
#include "sim/roads/street_furniture_internal.h"

#include <cmath>

namespace city {

using namespace furniture;

namespace {
constexpr double SQRT2 = 1.4142135623730951;
constexpr double STRAIGHT_SLACK_M = 0.01; // a segment this much longer than its chord bends

int side_of(const Profile &p, float u) {
	const StripEdge &r = p.edges.front(), &l = p.edges.back();
	if (r.strip.kind == StripKind::SIDEWALK && u >= r.u0 && u <= r.u1) return 0;
	if (l.strip.kind == StripKind::SIDEWALK && u >= l.u0 && u <= l.u1) return 1;
	return -1;
}
} // namespace

FurnitureSpot furniture::spot(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, double v, float u, float face_left, float face_fwd) {
	FurnitureSpot s;
	s.kind = k;
	s.seg = sid;
	s.side = side_of(p, u);
	s.v = v;
	s.u = u;
	s.face_left = face_left;
	s.face_fwd = face_fwd;
	const Footprint &f = defs(net).footprint[int(k)];
	const bool along_traffic = face_fwd != 0; // its z runs along the segment
	s.half_v = (along_traffic ? f.z_m : f.x_m) * 0.5f;
	s.half_u = (along_traffic ? f.x_m : f.z_m) * 0.5f;
	return s;
}

bool sidewalk_on(const Profile &p, int side) { return !p.edges.empty() && walk_on(p, side); }

float sidewalk_u(const Profile &p, int side, float from_kerb) { return kerb_in(p, side, from_kerb); }

float plate_kerb(const RoadNetwork &net, int seg) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const RoadNode &a = net.nodes[size_t(g.a)], &b = net.nodes[size_t(g.b)];
	const SignDef &d = defs(net).signs;
	return g.len > std::hypot(b.x - a.x, b.z - a.z) + STRAIGHT_SLACK_M ? d.plate_kerb_curve_m : d.plate_kerb_m;
}

void hydrant_square(const RoadNetwork &net, double x, double z, double &x0, double &z0, double &x1, double &z1) {
	const double cell = defs(net).hydrants.reach_m / SQRT2;
	x0 = std::floor(x / cell) * cell;
	z0 = std::floor(z / cell) * cell;
	x1 = x0 + cell;
	z1 = z0 + cell;
}

bool open_stretch(const RoadNetwork &net, const RoadSegment &g, float &from, float &to) {
	const float a = net.nodes[size_t(g.a)].lift, b = net.nodes[size_t(g.b)].lift, buried = -net.cat->tune.structure_buried_m;
	from = 0;
	to = 1;
	if (is_sunken(structure_of(g.flags)) || (a < buried && b < buried)) return false;
	if (a >= buried && b >= buried) return true;
	const float t = (buried - a) / (b - a);
	(a < buried ? from : to) = t;
	return true;
}

} // namespace city
