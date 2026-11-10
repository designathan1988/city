// Node shape and the streets through it: arm setbacks, dead-end bulbs and straight joints settled after an edit, and
// the street runs drawn along joints (Roadcraft roads/junction.ts bulbAt).
#include "sim/roads/road_network.h"

#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double STRAIGHT_ANGLE_COS = -0.9986; // two arms this close to opposite are one street going on
constexpr double BULB_REACH_PAD = 3, BULB_SEARCH_PAD = 30; // past a bulb's round, the room it keeps; past that, how far it looks
// A float bound that never lies inside the double box it stands for (the hash finds its cells from floats).
float below(double v) { return std::nextafter(float(v), -INFINITY); }
float above(double v) { return std::nextafter(float(v), INFINITY); }
} // namespace

bool RoadNetwork::bulb_clear(int n) const {
	const RoadNode &nd = nodes[n];
	int s = nd.segs[0];
	const RoadSegment &g = segs[s];
	if (cat->profile(g.profile).rail) return false;
	if (bulb_retreat(cat->tune, arm_of(g.q, cat->profile(g.profile), g.a == n ? 0 : 1), g.len) <= 0) return false;
	// a road closer than its round and the room it keeps leaves no room for a bulb; the roads it looks at lie within its reach and more
	const double reach = bulb_round(cat->tune, arm_of(g.q, cat->profile(g.profile), g.a == n ? 0 : 1)) + BULB_REACH_PAD;
	const double pad = bulb_reach(cat->tune) + BULB_REACH_PAD + BULB_SEARCH_PAD;
	bool clear = true;
	for_segments_near(nd.x - pad, nd.z - pad, nd.x + pad, nd.z + pad, [&](int o) {
		if (!clear || o == s) return;
		double t, d;
		closest_t(segs[o].q, nd.x, nd.z, t, d);
		if (d - cat->profile(segs[o].profile).right < reach) clear = false;
	});
	return clear;
}

bool RoadNetwork::refresh_node(int n, bool arms_changed) {
	RoadNode &nd = nodes[n];
	if (!nd.alive) return false;
	size_t k = nd.segs.size();
	const bool was_joint = nd.joint, was_bulb = nd.bulb;
	nd.bulb = k == 1 && !nd.plain && bulb_clear(n);
	nd.joint = false;
	std::vector<Arm> arms;
	std::vector<double> lens, out;
	std::vector<int> order;
	for (int s : nd.segs) {
		arms.push_back(arm_of(segs[s].q, profile_of(s), segs[s].a == n ? 0 : 1));
		lens.push_back(segs[s].len);
	}
	if (k == 2) nd.joint = arms[0].dx * arms[1].dx + arms[0].dz * arms[1].dz < STRAIGHT_ANGLE_COS;
	if (nd.joint != was_joint) grade.node_changed(n); // a plate there, or none
	// the node itself is drawn and driven otherwise: a bulb or a plate, or arms come or gone (crosswalks, the junction's
	// outline and connectors read every arm)
	const bool whole = arms_changed || nd.joint != was_joint || nd.bulb != was_bulb;
	if (arms_changed || nd.joint != was_joint) run_dirty_.insert(run_dirty_.end(), nd.segs.begin(), nd.segs.end()); // the street goes on here, or no more
	// one road that only changes structure here keeps its outline through a seam (build.ts structureSeam)
	const bool seam = k == 2 && segs[size_t(nd.segs[0])].profile == segs[size_t(nd.segs[1])].profile &&
			structure_of(segs[size_t(nd.segs[0])].flags) != structure_of(segs[size_t(nd.segs[1])].flags);
	junction_retreats(cat->tune, arms, lens, out, order, nd.bulb, seam);
	if (arms_changed || nd.bulb != was_bulb) // a dead end now, or no more, or its bulb come or gone: the ground under its roads is shaped again
		for (int s : nd.segs) grade.seg_changed(s);
	bool changed = whole;
	for (size_t i = 0; i < k; i++) {
		RoadSegment &g = segs[nd.segs[i]];
		// what this junction wants of the road, then both its mouths against its length (trim.ts clampSegmentTrims): the other
		// end's mouth may move with this one, and its node is drawn and driven again
		(g.a == n ? g.want_a : g.want_b) = out[i];
		const EndTrims t = clamp_segment_trims(cat->tune, g.want_a, g.want_b, g.len);
		double &mouth = g.a == n ? g.retreat_a : g.retreat_b, &far_mouth = g.a == n ? g.retreat_b : g.retreat_a;
		const double here = g.a == n ? t.a : t.b, there = g.a == n ? t.b : t.a;
		const bool moved = mouth != here, far_moved = far_mouth != there;
		if (moved || far_moved) grade.seg_changed(nd.segs[i]); // its plate there moved
		mouth = here;
		far_mouth = there;
		if (far_moved && log) log->net_node(g.a == n ? g.b : g.a);
		if (!moved && !far_moved && !whole) continue;
		changed = true;
		edited_segs_.push_back(nd.segs[i]);
		mark_segment(nd.segs[i]);
		// the mouths moved: the lane graph rebuilds this arm and its node on the next traffic step
		if (log) {
			log->net_seg(nd.segs[i]);
			log->net_node(n);
		}
	}
	return changed;
}

void RoadNetwork::settle_shapes() {
	std::vector<int> own, near;
	own.swap(shape_nodes_);
	const double pad = bulb_reach(cat->tune) + BULB_REACH_PAD + BULB_SEARCH_PAD;
	for (const ChangedBox &b : shape_boxes_)
		node_hash.query(below(b.minx - pad), below(b.minz - pad), above(b.maxx + pad), above(b.maxz + pad), [&](int n) { near.push_back(n); });
	shape_boxes_.clear();
	ChangeLog::dedup(own);
	ChangeLog::dedup(near);
	for (int n : own)
		if (n >= 0 && n < int(nodes.size())) refresh_node(n, true);
	// the nodes near a changed road whose own arms stand as they were: only a dead end's bulb (and its mouth) can change
	for (int n : near)
		if (!std::binary_search(own.begin(), own.end(), n)) refresh_node(n, false);
	settle_runs();
}

void RoadNetwork::refresh_all() {
	shape_nodes_.clear();
	shape_boxes_.clear();
	for (int n = 0; n < int(nodes.size()); n++) refresh_node(n, true);
	settle_runs();
}

int RoadNetwork::going_on(int n, int from) const {
	const std::vector<int> &arms = nodes[size_t(n)].segs;
	if (int(arms.size()) != STREET_JOINT_ARMS) return -1;
	const int next = arms[0] == from ? arms[1] : arms[0];
	const RoadSegment &g = segs[size_t(next)];
	return next != from && g.alive && g.a != g.b ? next : -1;
}

namespace {
// The segments a settle already wrote, kept sorted: a run or a chain is walked once, and nothing is sized by the map.
bool settled_has(const std::vector<int> &settled, int s) { return std::binary_search(settled.begin(), settled.end(), s); }
void settled_add(std::vector<int> &settled, std::vector<int> more) {
	std::sort(more.begin(), more.end());
	const size_t held = settled.size();
	settled.insert(settled.end(), more.begin(), more.end());
	std::inplace_merge(settled.begin(), settled.begin() + std::ptrdiff_t(held), settled.end());
}
} // namespace

// The face of each chain a dirty segment is on (StreetRun::face): the chain walked once both ways from the segment through every
// node of two arms, each piece with whether it runs the other way from it; the chain's lowest-numbered piece gives the face, its
// east or south one, turned for every piece that runs the other way from that one.
void RoadNetwork::settle_faces(const std::vector<int> &dirty) {
	struct Piece {
		int seg;
		bool flip;
	};
	std::vector<Piece> chain;
	std::vector<int> settled;
	for (int s : dirty) {
		if (s < 0 || s >= int(segs.size()) || !segs[size_t(s)].alive || settled_has(settled, s)) continue;
		chain.clear();
		chain.push_back({ s, false });
		for (const bool toward_b : { true, false }) {
			int at = s, n = toward_b ? segs[size_t(s)].b : segs[size_t(s)].a;
			bool flip = false;
			for (int next = going_on(n, at); next >= 0 && next != s; next = going_on(n, at)) {
				const RoadSegment &here = segs[size_t(at)], &on = segs[size_t(next)];
				flip = flip != ((here.b == n) != (on.a == n)); // the same way: it leaves the node the other arrives at
				chain.push_back({ next, flip });
				n = on.a == n ? on.b : on.a;
				at = next;
			}
		}
		Piece lowest = chain.front();
		for (const Piece &p : chain)
			if (p.seg < lowest.seg) lowest = p;
		const RoadSegment &g = segs[size_t(lowest.seg)];
		const RoadNode &a = nodes[size_t(g.a)], &b = nodes[size_t(g.b)];
		const int8_t face = (b.x - a.x) - (b.z - a.z) >= 0 ? 0 : 1; // the right face looks to (-dz, dx): east is +x, south +z
		std::vector<int> written;
		for (const Piece &p : chain) {
			runs_[size_t(p.seg)].face = p.flip != lowest.flip ? int8_t(1 - face) : face;
			written.push_back(p.seg);
		}
		settled_add(settled, std::move(written));
	}
}

void RoadNetwork::settle_runs() {
	runs_.resize(segs.size());
	std::vector<int> dirty;
	dirty.swap(run_dirty_);
	ChangeLog::dedup(dirty);
	// The segment before `at` on its street, -1 where the street begins (street_start of the render, the one definition).
	const auto before = [&](int at) {
		const RoadNode &n = nodes[size_t(segs[size_t(at)].a)];
		if (!n.joint || n.segs.size() != 2) return -1;
		const int back = n.segs[0] == at ? n.segs[1] : n.segs[0];
		return segs[size_t(back)].b == segs[size_t(at)].a ? back : -1; // a piece laid the other way starts its own
	};
	const auto after = [&](int at) {
		const RoadNode &n = nodes[size_t(segs[size_t(at)].b)];
		if (!n.joint || n.segs.size() != 2) return -1;
		const int next = n.segs[0] == at ? n.segs[1] : n.segs[0];
		return segs[size_t(next)].a == segs[size_t(at)].b ? next : -1;
	};
	std::vector<int> settled; // the segments whose run this settle wrote
	std::vector<int> run;
	for (int s : dirty) {
		if (s < 0 || s >= int(segs.size()) || settled_has(settled, s)) continue;
		if (!segs[size_t(s)].alive) {
			runs_[size_t(s)] = StreetRun();
			continue;
		}
		int head = s;
		bool ring = false;
		for (int back = before(head); back >= 0; back = before(head)) {
			if (back == s) {
				ring = true;
				break;
			}
			head = back;
		}
		run.clear();
		for (int at = head; at >= 0; at = after(at)) {
			run.push_back(at);
			const int next = after(at);
			if (next == head) break;
		}
		double total = 0;
		for (int at : run) total += segs[size_t(at)].len;
		const int id = ring ? *std::min_element(run.begin(), run.end()) : head;
		double start = 0;
		for (int at : run) {
			StreetRun &r = runs_[size_t(at)]; // its face is the chain's, settled below
			r.run = id;
			r.start = ring ? total - segs[size_t(at)].len : start;
			r.total = total;
			start += segs[size_t(at)].len;
		}
		settled_add(settled, run);
	}
	settle_faces(dirty);
}

} // namespace city
