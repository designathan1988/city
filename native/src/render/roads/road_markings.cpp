// Painted markings as thin lifted geometry: centre and lane lines with their dashes, zebra crossings and stop lines at
// crossing mouths: sized as data/render/streets.json markings says (CONTRAN's horizontal signage manual), where the
// simulation's crosswalk lies (catalog street_furniture.json "crossing"). Material PAINT, lit by the same lamp pools as
// the road.
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"

#include "sim/buildings/lot_access.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

struct Painter {
	const RoadEmit &e;
	int sid;
	LampRow lamp;

	Vector2 pool(float u, float at) const {
		if (!lamp.any) return Vector2(ROAD_POOL_NONE, 0);
		float du, dv;
		lamp.pool(u, at, du, dv);
		return Vector2(du, dv);
	}
	// A rectangle u0..u1 across, va..vb metres along from node a, in pieces of at most one sample step.
	void rect(float u0, float u1, double va, double vb, const Color &c) const {
		bool straight = e.s.roads.segs[sid].q.is_straight(STRAIGHT_TOL);
		int n = straight ? 1 : std::max(1, int(std::ceil((vb - va) / e.L.step)));
		Color col = road_color(c, RoadMat::PAINT);
		for (int k = 0; k < n; k++) {
			double a = va + (vb - va) * k / n, b = va + (vb - va) * (k + 1) / n;
			SegFrame fa = seg_frame(e.s, sid, a), fb = seg_frame(e.s, sid, b);
			Vector3 lift(0, e.L.paint_lift, 0);
			const Vector3 p[4] = { fa.c + fa.left * u0 + lift, fa.c + fa.left * u1 + lift, fb.c + fb.left * u1 + lift, fb.c + fb.left * u0 + lift };
			const Vector2 uv[4] = { Vector2(u0, float(a)), Vector2(u1, float(a)), Vector2(u1, float(b)), Vector2(u0, float(b)) };
			const Vector2 uv2[4] = { pool(u0, float(a)), pool(u1, float(a)), pool(u1, float(b)), pool(u0, float(b)) };
			emit_quad(e.mb, p, uv, uv2, col, Vector3(0, 1, 0));
		}
	}
};

// The lane lines from l0 to l1 metres along; `solid_a` and `solid_b`: how far from l0 and back from l1 the lines between lanes of the
// traffic arriving there run solid (a signalled approach: no change of lane in the queue, MBST vol. IV 5.2.1 LMS-1). The traffic
// arriving at end a keeps to the +u side of the centre; a road one way arrives at the end its lanes run to.
void lines(const Painter &pt, const Profile &p, double l0, double l1, double solid_a, double solid_b) {
	const StreetLook &L = pt.e.L;
	const StreetLook::PaintStyle &st = L.styles[size_t(pt.e.s.marking_style.load())];
	const bool fast = p.speed >= st.fast_kmh;
	const bool many = std::max(p.lanes_fwd(), p.lanes_back()) > 1;
	size_t band = 0; // the dash pattern of the road's speed
	while (band < st.dash_kmh.size() && p.speed >= st.dash_kmh[band]) band++;
	const double dash = st.dash[band], period = dash + st.gap[band];
	const bool one_way = p.lanes_back() == 0 || p.lanes_fwd() == 0;
	float middle = 0.f;
	for (const MarkLine &m : p.lines)
		if (m.centre) middle = m.u;
	for (const MarkLine &m : p.lines) {
		Color c = m.color == LineColor::YELLOW && !st.all_white ? L.yellow : L.white;
		const float w = m.exclusive ? st.exclusive_w : m.centre ? (fast ? st.fast_centre_w : st.centre_w) : (fast ? st.fast_lane_w : st.lane_w);
		const LineKind kind = m.centre && m.kind == LineKind::SOLID && st.multi_lane_double && many ? LineKind::DOUBLE : m.kind;
		// a dashed line between lanes of one direction: solid on the approach its traffic arrives at, when that one is signalled
		const bool to_a = one_way ? p.lanes_fwd() == 0 : m.u > middle;
		const double from = kind == LineKind::DASHED && !m.centre && to_a ? std::min(l1, l0 + solid_a) : l0;
		const double to = kind == LineKind::DASHED && !m.centre && !to_a ? std::max(from, l1 - solid_b) : l1;
		auto line = [&](float u, float hw) {
			if (kind != LineKind::DASHED) {
				if (l1 > l0) pt.rect(u - hw, u + hw, l0, l1, c);
				return;
			}
			if (from > l0) pt.rect(u - hw, u + hw, l0, from, c);
			if (l1 > to) pt.rect(u - hw, u + hw, to, l1, c);
			for (double v = std::floor(from / period) * period; v < to; v += period) {
				double a = std::max(v, from), b = std::min(v + dash, to);
				if (b > a) pt.rect(u - hw, u + hw, a, b, c);
			}
		};
		if (kind == LineKind::DOUBLE) { // two lines double_gap apart
			line(m.u - (st.double_w + st.double_gap) / 2, st.double_w / 2);
			line(m.u + (st.double_w + st.double_gap) / 2, st.double_w / 2);
		} else line(m.u, w / 2);
	}
}

void crossings(const Painter &pt, const Profile &p, double v0, double v1, bool ca, bool cb, bool za, bool zb_end) {
	const StreetLook &L = pt.e.L;
	const StreetLook::PaintStyle &st = L.styles[size_t(pt.e.s.marking_style.load())];
	const CrossingDef &x = pt.e.s.catalog.furniture.crossing;
	float ar = -p.asph_right, al = p.asph_left, isle0 = 0, isle1 = 0;
	const bool island = island_span(p, isle0, isle1); // without one both are the centre line
	for (int end = 0; end < 2; end++) {
		if (!(end == 0 ? ca : cb)) continue;
		auto from = [&](double d) { return end == 0 ? v0 + d : v1 - d; };
		const bool zebra = end == 0 ? za : zb_end;
		double z0 = from(x.zebra_from_m), z1 = from(x.zebra_to_m);
		// as many bars as fit across each stretch of carriageway, clear of its kerbs, the pattern centred on it
		auto across = [&](float from, float to) {
			const float wide = to - from, gap = st.zebra_period - st.zebra_bar;
			if (wide < st.zebra_bar) return;
			if (pt.e.far) { // bars under a pixel shimmer: what stands for them is their mean over the carriageway
				pt.rect(from, to, std::min(z0, z1), std::max(z0, z1), L.carriage(p).lerp(L.white, st.zebra_bar / st.zebra_period));
				return;
			}
			const int bars = int(std::floor((wide + gap) / st.zebra_period));
			const float first = from + (wide - (bars * st.zebra_period - gap)) / 2;
			for (int k = 0; k < bars; k++) {
				const float u = first + k * st.zebra_period;
				pt.rect(u, u + st.zebra_bar, std::min(z0, z1), std::max(z0, z1), L.white);
			}
		};
		if (zebra && island) {
			across(ar + st.zebra_edge, isle0 - st.zebra_edge);
			across(isle1 + st.zebra_edge, al - st.zebra_edge);
		} else if (zebra) across(ar + st.zebra_edge, al - st.zebra_edge);
		// stop line on the approach half (end a: traffic arrives on the +u side; one-way roads arrive only at b)
		if (pt.e.far || (end == 0 && p.one_way)) continue;
		float su0 = p.one_way ? ar : (end == 0 ? isle1 : ar), su1 = p.one_way ? al : (end == 0 ? al : isle0);
		double sa = from(x.stop_from_m), sb = from(x.stop_to_m);
		pt.rect(su0, su1, std::min(sa, sb), std::max(sa, sb), L.white);
	}
}
// A line across the parking band at arc `v`.
void bay_line(const Painter &pt, const KerbStretch &k, double v) {
	const double hw = pt.e.L.parking_line_w / 2;
	pt.rect(std::min(k.u_kerb, k.u_inner), std::max(k.u_kerb, k.u_inner), v - hw, v + hw, pt.e.L.white);
}

// The street's parking (CONTRAN MBST vol. IV): each bay's division and the run closed at both ends, the band's edge dashed
// along the run (MER, white), the yellow no-parking line by the kerb along the rest of that kerb (LPP).
void parking_paint(const Painter &pt) {
	const StreetLook &L = pt.e.L;
	if (!parks_along(pt.e.s.roads, pt.sid)) return;
	std::vector<ArcSpan> gates[2];
	street_gates(pt.e.s.roads, pt.e.s.buildings, pt.sid, gates);
	KerbLayout k;
	kerb_layout(pt.e.s.roads, pt.sid, gates, k);
	for (const KerbStretch &b : k.bays) bay_line(pt, b, b.v0);
	const float hw = L.parking_line_w / 2;
	for (const KerbStretch &r : k.runs) {
		bay_line(pt, r, r.v1);
		for (double v = r.v0; v < r.v1; v += L.parking_dash + L.parking_gap) pt.rect(r.u_inner - hw, r.u_inner + hw, v, std::min(v + L.parking_dash, r.v1), L.white);
	}
	for (const KerbStretch &n : k.bans) {
		if (n.v1 - n.v0 < L.no_parking_min) continue;
		const float inward = n.u_inner > n.u_kerb ? 1.f : -1.f, u = n.u_kerb + inward * (L.no_parking_w / 2); // on the kerb's foot: the carriageway reaches the kerb
		pt.rect(u - L.no_parking_w / 2, u + L.no_parking_w / 2, n.v0, n.v1, L.yellow);
	}
}
} // namespace

void emit_markings(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	if (p.rail) return;
	double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
	bool ca = !p.ring && e.s.roads.has_crosswalks(g.a), cb = !p.ring && e.s.roads.has_crosswalks(g.b);
	Painter pt{ e, sid, lamp_row(e.s.roads, sid) };
	const double clear = e.s.catalog.furniture.crossing.stop_to_m; // the lines end at the stop line
	crossings(pt, p, v0, v1, ca, cb, e.s.roads.crosswalk_on(g.a, sid), e.s.roads.crosswalk_on(g.b, sid));
	if (e.far) return; // the lane lines are filtered by the road's shader there
	const double solid = e.L.styles[size_t(e.s.marking_style.load())].signal_solid;
	const bool sa = traffic::control_of(e.s, g.a) == Control::SIGNAL, sb = traffic::control_of(e.s, g.b) == Control::SIGNAL;
	lines(pt, p, v0 + (ca ? clear : 0), v1 - (cb ? clear : 0), sa ? solid : 0.0, sb ? solid : 0.0);
	parking_paint(pt);
}

} // namespace city
