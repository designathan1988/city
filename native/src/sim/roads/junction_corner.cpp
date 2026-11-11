// A junction corner's outline (junction_corner.h).
#include "sim/roads/junction_corner.h"

#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float SAME_M = 1e-3f; // stations nearer than this are one; a side meets another only this far from their ends
constexpr float PAST_M = 1e-3f; // a station this far past a mouth's line stands over the arm's own section
constexpr float EDGE_M = 1e-4f; // a strip's edge this near a width is at it
constexpr float PARALLEL = 1e-3f; // under this the cross of two units: their lines never meet
constexpr float RETURN_LEAST_M = 0.05f, CONTROL_AHEAD = 0.5f, CONTROL_BEHIND = -60.f; // a return's least radius; a bevel's control
constexpr float HALF = 0.5f;
constexpr int LEAST_ARC_STEPS = 2;

bool raised_walk(const StripEdge &e) { return e.strip.kind == StripKind::SIDEWALK && !e.strip.flush; }

// The stations of a return's arc round `centre` from `from` to `to`, the shorter way, `step` radians apart at most.
void arc(const V2 &centre, const V2 &from, const V2 &to, float step, std::vector<V2> &line, std::vector<float> &share) {
	const float a0 = (from - centre).angle(), radius = std::max(RETURN_LEAST_M, (from - centre).len());
	const float sweep = wrap_angle((to - centre).angle() - a0);
	const int steps = std::max(LEAST_ARC_STEPS, int(std::ceil(std::fabs(sweep) / step)));
	for (int i = 0; i <= steps; i++) {
		const float t = float(i) / float(steps);
		line.push_back(centre + v2_from_angle(a0 + sweep * t) * radius);
		share.push_back(t);
	}
}

// The kerb line from a's kerb at its mouth (pa) to b's (pb): along a's kerb to the return, round it, along b's kerb, when the return's
// arc is drawn (junction.h fillet_drawn); else a curve from mouth to mouth, its control where the two kerbs meet when that lies near.
void kerb_line(const Arm &arm_a, const Arm &arm_b, const CornerSide &a, const CornerSide &b, float arc_step, int bevel_steps, CornerOutline &o) {
	const V2 pa = a.mouth + left_of(a.dir) * a.section.kerb, pb = b.mouth - left_of(b.dir) * b.section.kerb;
	const Corner c = corner_fillet(arm_a, arm_b);
	if (fillet_drawn(c, a.at, b.at)) {
		const V2 da(float(arm_a.dx), float(arm_a.dz)), db(float(arm_b.dx), float(arm_b.dz));
		const V2 ta = da * float(c.trim_a) + left_of(da) * a.section.kerb, tb = db * float(c.trim_b) - left_of(db) * b.section.kerb;
		o.kerb.push_back(pa);
		o.share.push_back(0);
		arc(V2(float(c.cx), float(c.cz)), ta, tb, arc_step, o.kerb, o.share);
		o.kerb.push_back(pb);
		o.share.push_back(1);
		return;
	}
	V2 control = (pa + pb) * HALF;
	float sa = 0, sb = 0;
	if (line_line(pa, a.dir, pb, b.dir, sa, sb) && sa < CONTROL_AHEAD && sb < CONTROL_AHEAD && sa > CONTROL_BEHIND && sb > CONTROL_BEHIND)
		control = pa + a.dir * sa;
	for (int i = 0; i <= bevel_steps; i++) {
		const float t = float(i) / float(bevel_steps), s = 1 - t;
		o.kerb.push_back(pa * (s * s) + control * (2 * s * t) + pb * (t * t));
		o.share.push_back(t);
	}
}

// Leaves out the stations past either mouth's line next to that mouth (a return whose tangent point lies within fillet_drawn's
// slack past a mouth, an arm's straight direction off its curve's): the kerb starts at the mouth, never over the arm's own section.
void keep_behind_mouths(const CornerSide &a, const CornerSide &b, CornerOutline &o) {
	const auto past = [](const CornerSide &s, const V2 &p) { return (p - s.mouth).dot(s.dir) > PAST_M; };
	size_t lo = 1, hi = o.kerb.size() - 1;
	while (lo < hi && past(a, o.kerb[lo])) lo++;
	while (hi > lo && past(b, o.kerb[hi - 1])) hi--;
	std::vector<V2> kerb{ o.kerb.front() };
	std::vector<float> share{ 0 };
	for (size_t k = lo; k < hi; k++) {
		if ((o.kerb[k] - kerb.back()).len() < SAME_M) continue;
		kerb.push_back(o.kerb[k]);
		share.push_back(o.share[k]);
	}
	if ((o.kerb.back() - kerb.back()).len() < SAME_M && kerb.size() > 1) {
		kerb.pop_back();
		share.pop_back();
	}
	kerb.push_back(o.kerb.back());
	share.push_back(1);
	o.kerb = std::move(kerb);
	o.share = std::move(share);
}

// Each station's unit towards the corner's lot: a's left at a's mouth, b's right at b's, square to the line in between, on the side
// the line's first run says.
std::vector<V2> normals(const std::vector<V2> &line, const V2 &na, const V2 &nb) {
	float hand = 1;
	for (size_t k = 1; k < line.size(); k++) {
		const V2 t = line[k] - line[0];
		if (t.len() > SAME_M) {
			hand = left_of(t).dot(na) >= 0 ? 1.f : -1.f;
			break;
		}
	}
	std::vector<V2> out(line.size(), na);
	out.back() = nb;
	for (size_t k = 1; k + 1 < line.size(); k++) {
		const V2 t = line[k + 1] - line[k - 1];
		if (t.len() > SAME_M) out[k] = left_of(t.norm()) * hand;
	}
	return out;
}

// The line moved out along `nn` by wa at a's end to wb at b's.
std::vector<V2> moved(const std::vector<V2> &line, const std::vector<V2> &nn, const std::vector<float> &share, float wa, float wb) {
	std::vector<V2> out;
	for (size_t k = 0; k < line.size(); k++) out.push_back(line[k] + nn[k] * (wa + (wb - wa) * share[k]));
	return out;
}

// Whether two sides of a ring cross, or lie along each other, away from their ends.
bool sides_meet(const V2 &a, const V2 &b, const V2 &c, const V2 &d) {
	const V2 r = b - a, s = d - c, w = c - a;
	const float lr = r.len(), ls = s.len();
	if (lr < SAME_M || ls < SAME_M) return false;
	if (std::fabs(r.cross(s)) <= PARALLEL * lr * ls) {
		if (std::fabs(w.cross(r)) / lr > SAME_M) return false; // two lines side by side
		const float t0 = w.dot(r) / (lr * lr), t1 = (d - a).dot(r) / (lr * lr), m = SAME_M / lr;
		return std::max(t0, t1) > m && std::min(t0, t1) < 1 - m;
	}
	const float den = r.cross(s), t = w.cross(s) / den, u = w.cross(r) / den, mt = SAME_M / lr, mu = SAME_M / ls;
	return t > mt && t < 1 - mt && u > mu && u < 1 - mu;
}

// The footway's polygon: the inner side, then the back the other way round.
std::vector<V2> ring_of(const CornerOutline &o) {
	std::vector<V2> ring;
	const auto add = [&](const V2 &p) {
		if (ring.empty() || (p - ring.back()).len() >= SAME_M) ring.push_back(p);
	};
	for (const V2 &p : o.inner) add(p);
	for (size_t k = o.back.size(); k-- > 0;) add(o.back[k]);
	if (ring.size() > 1 && (ring.front() - ring.back()).len() < SAME_M) ring.pop_back();
	return ring;
}

bool simple_ring(const std::vector<V2> &ring) {
	const size_t n = ring.size();
	if (n < 3) return false;
	for (size_t i = 0; i < n; i++)
		for (size_t j = i + 1; j < n; j++)
			if (sides_meet(ring[i], ring[(i + 1) % n], ring[j], ring[(j + 1) % n])) return false;
	return true;
}

// A back of three points, a's outer corner, the point, b's: one point where the corners are it.
void squared(CornerOutline &o, const V2 &oa, const V2 &p, const V2 &ob) {
	o.back.clear();
	o.back_share.clear();
	const V2 at[3] = { oa, p, ob };
	const float share[3] = { 0, HALF, 1 };
	for (int k = 0; k < 3; k++) {
		if (!o.back.empty() && (at[k] - o.back.back()).len() < SAME_M) continue;
		o.back.push_back(at[k]);
		o.back_share.push_back(share[k]);
	}
}
} // namespace

SideSection side_section(const Profile &p, bool plus_u) {
	SideSection s;
	s.kerb = plus_u ? p.asph_left : p.asph_right;
	s.outer = plus_u ? p.left : p.right;
	s.back = s.kerb;
	const int n = int(p.edges.size()), step = plus_u ? 1 : -1;
	int first = -1; // the strip at the carriageway's edge on that side
	for (int k = plus_u ? 0 : n - 1; k >= 0 && k < n; k += step) {
		const float from = plus_u ? p.edges[size_t(k)].u0 : -p.edges[size_t(k)].u1;
		if (from >= s.kerb - EDGE_M) {
			first = k;
			break;
		}
	}
	for (int k = first; k >= 0 && k < n; k += step) s.raised = s.raised || raised_walk(p.edges[size_t(k)]);
	const int beyond = first + step;
	if (first >= 0 && p.edges[size_t(first)].strip.kind == StripKind::CURB && beyond >= 0 && beyond < n && raised_walk(p.edges[size_t(beyond)])) {
		s.stone = true;
		s.back = plus_u ? p.edges[size_t(first)].u1 : -p.edges[size_t(first)].u0;
	}
	return s;
}

CornerOutline corner_outline(const Arm &arm_a, const Arm &arm_b, const CornerSide &a, const CornerSide &b, float arc_step, int bevel_steps) {
	CornerOutline o;
	const V2 na = left_of(a.dir), nb = -left_of(b.dir);
	const SideSection &sa = a.section, &sb = b.section;
	kerb_line(arm_a, arm_b, a, b, arc_step, bevel_steps, o);
	keep_behind_mouths(a, b, o);
	const std::vector<V2> nn = normals(o.kerb, na, nb);
	o.stone = moved(o.kerb, nn, o.share, sa.back - sa.kerb, sb.back - sb.kerb);
	o.inner = o.stone;
	const V2 oa = a.mouth + na * sa.outer, ob = b.mouth + nb * sb.outer;
	const CornerMode mode = corner_fillet(arm_a, arm_b).mode;
	float s = 0, t = 0;
	const bool point = (mode == CornerMode::FILLET || mode == CornerMode::BEVEL) && line_line(oa, a.dir, ob, b.dir, s, t) && std::fabs(a.dir.cross(b.dir)) >= PARALLEL;
	const V2 p = oa + a.dir * s;
	if (point && s <= PAST_M && t <= PAST_M) {
		o.kind = CornerBack::SQUARE;
		squared(o, oa, p, ob);
		if (simple_ring(ring_of(o))) return o;
	}
	o.kind = CornerBack::OFFSET;
	o.back = moved(o.kerb, nn, o.share, sa.outer - sa.kerb, sb.outer - sb.kerb);
	o.back_share = o.share;
	o.simple = simple_ring(ring_of(o));
	if (o.simple || !point) return o;
	o.kind = CornerBack::RUN;
	o.run_a = std::max(0.f, s);
	o.run_b = std::max(0.f, t);
	o.inner.clear();
	if (o.run_a > SAME_M) o.inner.push_back(a.mouth + a.dir * o.run_a + na * sa.back);
	o.inner.insert(o.inner.end(), o.stone.begin(), o.stone.end());
	if (o.run_b > SAME_M) o.inner.push_back(b.mouth + b.dir * o.run_b + nb * sb.back);
	squared(o, oa + a.dir * o.run_a, p, ob + b.dir * o.run_b);
	o.simple = simple_ring(ring_of(o));
	return o;
}

} // namespace city
