// Painted markings as thin lifted geometry: centre and lane lines with their dashes, zebra crossings and stop lines at
// crossing mouths: sized as data/render/streets.json markings says (CONTRAN's horizontal signage manual), where the
// simulation's crosswalk lies (catalog street_furniture.json "crossing"). Material PAINT, lit by the same lamp pools as
// the road.
#include "render/roads/road_mesh.h"

#include "sim/buildings/lot_access.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float STRAIGHT_TOL = 0.5f;

struct Painter {
	const RoadEmit &e;
	int sid;
	LampRow lamp;

	Vector2 pool(float u, float at) const {
		if (!lamp.any) return Vector2(POOL_NONE, 0);
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

void lines(const Painter &pt, const Profile &p, double l0, double l1) {
	const StreetLook &L = pt.e.L;
	const float plain_w = p.speed < L.line_fast_kmh ? L.line_w : L.line_fast_w;
	size_t band = 0; // the dash pattern of the road's speed
	while (band < L.dash_kmh.size() && p.speed >= L.dash_kmh[band]) band++;
	const double dash = L.dash[band], period = dash + L.gap[band];
	for (const MarkLine &m : p.lines) {
		Color c = m.color == LineColor::YELLOW ? L.yellow : L.white;
		const float w = m.exclusive ? L.exclusive_w : plain_w, hw = w / 2;
		auto line = [&](float u) {
			if (m.kind != LineKind::DASHED) {
				if (l1 > l0) pt.rect(u - hw, u + hw, l0, l1, c);
				return;
			}
			for (double v = std::floor(l0 / period) * period; v < l1; v += period) {
				double a = std::max(v, l0), b = std::min(v + dash, l1);
				if (b > a) pt.rect(u - hw, u + hw, a, b, c);
			}
		};
		if (m.kind == LineKind::DOUBLE) { // two lines double_gap apart
			line(m.u - (w + L.double_gap) / 2);
			line(m.u + (w + L.double_gap) / 2);
		} else line(m.u);
	}
}

void crossings(const Painter &pt, const Profile &p, double v0, double v1, bool ca, bool cb) {
	const StreetLook &L = pt.e.L;
	const CrossingDef &x = pt.e.s.catalog.furniture.crossing;
	float ar = -p.asph_right, al = p.asph_left;
	for (int end = 0; end < 2; end++) {
		if (!(end == 0 ? ca : cb)) continue;
		auto from = [&](double d) { return end == 0 ? v0 + d : v1 - d; };
		double za = from(x.zebra_from_m), zb = from(x.zebra_to_m);
		// as many bars as fit across the carriageway, the pattern centred on it
		const float gap = L.zebra_period - L.zebra_bar;
		const int bars = int(std::floor((al - ar + gap) / L.zebra_period));
		const float first = ar + ((al - ar) - (bars * L.zebra_period - gap)) / 2;
		for (int k = 0; k < bars; k++) {
			const float u = first + k * L.zebra_period;
			pt.rect(u, u + L.zebra_bar, std::min(za, zb), std::max(za, zb), L.white);
		}
		// stop line on the approach half (end a: traffic arrives on the +u side; one-way roads arrive only at b)
		if (end == 0 && p.one_way) continue;
		float su0 = p.one_way ? ar : (end == 0 ? 0.f : ar), su1 = p.one_way ? al : (end == 0 ? al : 0.f);
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
// along the run (MER, white), the yellow no-parking line by the gutter along the rest of that kerb (LPP).
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
		const float inward = n.u_inner > n.u_kerb ? 1.f : -1.f, u = n.u_kerb + inward * (L.gutter_w + L.no_parking_w / 2);
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
	Painter pt{ e, sid, lamp_row(e.s.roads, sid) };
	lines(pt, p, v0 + (ca ? e.L.line_clear : 0), v1 - (cb ? e.L.line_clear : 0));
	crossings(pt, p, v0, v1, ca, cb);
	parking_paint(pt);
}

} // namespace city
