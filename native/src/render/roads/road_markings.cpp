// Painted markings as thin lifted geometry: centre and lane lines with their dashes, zebra crossings and stop lines at
// crossing mouths: sized as data/render/streets.json markings says (CONTRAN's horizontal signage manual), where the
// simulation's crosswalk lies (catalog street_furniture.json "crossing"). Material PAINT; at night the lamps' pools light it with
// the road under it (render/props/lamp_pools.h).
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"

#include "sim/buildings/lot_access.h"
#include "sim/roads/profiles.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

using namespace godot;

namespace city {

namespace {

struct Painter {
	const RoadEmit &e;
	int sid;

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
			emit_quad(e.mb, p, uv, col, Vector3(0, 1, 0));
		}
	}
};

// `open_a`, `open_b`: how far from each end an exclusive lane's line lets a driver in (a junction there: the entry stretch).
void lines(const Painter &pt, const Profile &p, double l0, double l1, double open_a, double open_b) {
	const StreetLook &L = pt.e.L;
	const StreetLook::PaintStyle &st = L.styles[size_t(pt.e.s.marking_style.load())];
	const bool fast = p.speed >= st.fast_kmh;
	const bool many = std::max(p.lanes_fwd(), p.lanes_back()) > 1;
	size_t band = 0; // the dash pattern of the road's speed
	while (band < st.dash_kmh.size() && p.speed >= st.dash_kmh[band]) band++;
	const double dash = st.dash[band], period = dash + st.gap[band];
	for (const MarkLine &m : p.lines) {
		Color c = m.color == LineColor::YELLOW && !st.all_white ? L.yellow : L.white;
		const float w = m.exclusive ? st.exclusive_w : m.centre ? (fast ? st.fast_centre_w : st.centre_w) : (fast ? st.fast_lane_w : st.lane_w);
		const LineKind kind = m.centre && m.kind == LineKind::SOLID && st.multi_lane_double && many ? LineKind::DOUBLE : m.kind;
		// An exclusive lane's line is continuous but on the stretch before a junction, where it is the continuity line a
		// driver crosses to turn (MBST vol. IV 5.5.1; the stretch: traffic.json lane_entry_m).
		const double whole0 = m.exclusive ? std::min(l1, l0 + open_a) : l0, whole1 = m.exclusive ? std::max(whole0, l1 - open_b) : l1;
		auto line = [&](float u, float hw) {
			const auto dashes = [&](double from, double to) {
				for (double v = std::floor(from / period) * period; v < to; v += period) {
					double a = std::max(v, from), b = std::min(v + dash, to);
					if (b > a) pt.rect(u - hw, u + hw, a, b, c);
				}
			};
			if (kind == LineKind::DASHED) return dashes(l0, l1);
			dashes(l0, whole0);
			if (whole1 > whole0) pt.rect(u - hw, u + hw, whole0, whole1, c);
			dashes(whole1, l1);
		};
		if (kind == LineKind::DOUBLE) { // two lines double_gap apart
			line(m.u - (st.double_w + st.double_gap) / 2, st.double_w / 2);
			line(m.u + (st.double_w + st.double_gap) / 2, st.double_w / 2);
		} else line(m.u, w / 2);
	}
}

// The stretches across the road the traffic of one direction drives on (towards node a, or towards b): each run of its lanes with
// the asphalt beside it that is no lane (a cycle lane, a parking band, a shoulder), read from the profile's own strips, wherever its
// two directions meet. None for a direction the road has no lanes of.
std::vector<std::pair<float, float>> arriving_spans(const Profile &p, bool towards_a) {
	std::vector<std::pair<float, float>> out;
	const int n = int(p.edges.size());
	const auto lane = [&](int i) {
		return i >= 0 && i < n && p.edges[size_t(i)].strip.kind == StripKind::LANE && (p.edges[size_t(i)].strip.dir == -1) == towards_a;
	};
	const auto beside = [&](int i) {
		return i >= 0 && i < n && p.edges[size_t(i)].strip.kind != StripKind::LANE && is_asphalt(p.edges[size_t(i)].strip.kind);
	};
	for (int i = 0; i < n; i++) {
		if (!lane(i)) continue;
		int from = i, to = i;
		while (lane(to + 1)) to++;
		const int last_lane = to;
		while (beside(from - 1)) from--;
		while (beside(to + 1)) to++;
		out.emplace_back(p.edges[size_t(from)].u0, p.edges[size_t(to)].u1);
		i = last_lane;
	}
	return out;
}

void crossings(const Painter &pt, const Profile &p, double v0, double v1, bool ca, bool cb, bool za, bool zb_end) {
	const StreetLook &L = pt.e.L;
	const StreetLook::PaintStyle &st = L.styles[size_t(pt.e.s.marking_style.load())];
	const CrossingDef &x = pt.e.s.catalog.furniture.crossing;
	const RoadSegment &g = pt.e.s.roads.segs[pt.sid];
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
		// the stop line, where the road network says this arm's lies (the place the traffic stops at), across the lanes that arrive here
		if (pt.e.far) continue;
		const double back = pt.e.s.roads.stop_line_back(end == 0 ? g.a : g.b, pt.sid);
		const double sa = from(std::max(0.0, back - double(x.stop_width()))), sb = from(back);
		for (const auto &[u0, u1] : arriving_spans(p, end == 0)) pt.rect(u0, u1, std::min(sa, sb), std::max(sa, sb), L.white);
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
	street_gates(pt.e.s.roads, pt.e.s.buildings, pt.e.s.designs, pt.sid, pt.e.s.catalog.traffic.kerb_gate_margin_m, gates);
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
	bool ca = !p.ring && crossing_node(e.s, g.a), cb = !p.ring && crossing_node(e.s, g.b);
	Painter pt{ e, sid };
	crossings(pt, p, v0, v1, ca, cb, crosswalk_at(e.s, g.a, sid), crosswalk_at(e.s, g.b, sid));
	if (e.far) return; // the lane lines are filtered by the road's shader there
	const double entry = e.s.catalog.traffic.lane_entry_m;
	lines(pt, p, v0 + e.s.roads.stop_line_back(g.a, sid), v1 - e.s.roads.stop_line_back(g.b, sid), ca ? entry : 0, cb ? entry : 0); // the lines end at the stop lines
	parking_paint(pt);
	emit_legends(e, sid);
}

} // namespace city
