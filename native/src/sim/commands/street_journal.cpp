// Journalled mutations of the street (sim/street) and of a junction's or a segment's street state, with their inverses: the
// street's part of the edit journal, next to it and apart from it.
#include "sim/commands/street_journal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <cmath>

namespace city {

namespace {
void grow_at(CityState &s, double x, double z) { s.history.open.grow(x, z, x, z); }

// What a record of the street changes when it comes or goes: the one rule for a live edit and for its undo and redo.
void invalidate(CityState &s, const StreetItem &r) { mark_street(s, r.x, r.z); }
void invalidate(CityState &s, const UtilityPole &r) { mark_street(s, r.x, r.z); }
void invalidate(CityState &s, const StreetCut &r) { mark_street(s, r.x, r.z); }
void invalidate(CityState &s, const UtilitySpan &r) { // its poles stand while it does
	const UtilityPole &pa = s.street.poles.items[size_t(r.a)], &pb = s.street.poles.items[size_t(r.b)];
	mark_street(s, pa.x, pa.z);
	mark_street(s, pb.x, pb.z);
}
void invalidate(CityState &s, const Barrier &b) { // every chunk its path crosses
	for (size_t i = 0; i + 3 < b.pts.size(); i += 2) {
		const double dx = b.pts[i + 2] - b.pts[i], dz = b.pts[i + 3] - b.pts[i + 1];
		const int steps = int(std::ceil(std::hypot(dx, dz) / CHUNK)) + 1;
		for (int k = 0; k <= steps; k++) mark_street(s, b.pts[i] + dx * k / steps, b.pts[i + 1] + dz * k / steps);
	}
}
} // namespace

int j_add_item(CityState &s, const StreetItem &r) {
	int id = s.street.add_item(r);
	invalidate(s, r);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_ITEM, id, s.street.items.items[size_t(id)]);
		grow_at(s, r.x, r.z);
	}
	return id;
}

void j_remove_item(CityState &s, int id) {
	const StreetItem r = s.street.items.items[size_t(id)];
	if (s.history.recording) {
		j_record(s, OpKind::REM_ITEM, id, r);
		grow_at(s, r.x, r.z);
	}
	s.street.remove_item(id);
	invalidate(s, r);
}

int j_add_pole(CityState &s, const UtilityPole &r) {
	int id = s.street.add_pole(r);
	invalidate(s, r);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_POLE, id, s.street.poles.items[size_t(id)]);
		grow_at(s, r.x, r.z);
	}
	return id;
}

void j_remove_pole(CityState &s, int id) {
	const std::vector<int> spans = s.street.poles.items[size_t(id)].spans;
	for (int span : spans) j_remove_span(s, span);
	const UtilityPole r = s.street.poles.items[size_t(id)];
	if (s.history.recording) {
		j_record(s, OpKind::REM_POLE, id, r);
		grow_at(s, r.x, r.z);
	}
	s.street.remove_pole(id);
	invalidate(s, r);
}

int j_add_span(CityState &s, int a, int b) {
	UtilitySpan r;
	r.a = a;
	r.b = b;
	int id = s.street.add_span(r);
	invalidate(s, r);
	if (s.history.recording) j_record(s, OpKind::ADD_SPAN, id, s.street.spans.items[size_t(id)]);
	return id;
}

void j_remove_span(CityState &s, int id) {
	const UtilitySpan r = s.street.spans.items[size_t(id)];
	if (s.history.recording) j_record(s, OpKind::REM_SPAN, id, r);
	invalidate(s, r);
	s.street.remove_span(id);
}

int j_add_barrier(CityState &s, const Barrier &r) {
	int id = s.street.add_barrier(r);
	invalidate(s, r);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_BARRIER, id, s.street.barriers.items[size_t(id)]);
		grow_at(s, r.pts[0], r.pts[1]);
	}
	return id;
}

void j_remove_barrier(CityState &s, int id) {
	const Barrier r = s.street.barriers.items[size_t(id)];
	if (s.history.recording) {
		j_record(s, OpKind::REM_BARRIER, id, r);
		grow_at(s, r.pts[0], r.pts[1]);
	}
	s.street.remove_barrier(id);
	invalidate(s, r);
}

int j_add_cut(CityState &s, float x, float z) {
	StreetCut r;
	r.x = x;
	r.z = z;
	int id = s.street.add_cut(r);
	invalidate(s, r);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_CUT, id, s.street.cuts.items[size_t(id)]);
		grow_at(s, x, z);
	}
	return id;
}

namespace {
void put_rule(CityState &s, int seg, int end, int rule) {
	RoadSegment &g = s.roads.segs[size_t(seg)];
	g.flags = uint16_t((g.flags & ~seg_rule_mask(end)) | (rule << seg_rule_shift(end)));
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	s.log.net_seg(seg);
	mark_furniture(s, seg);
}
} // namespace

void j_set_rule(CityState &s, int seg, int end, uint16_t rule) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const int was = (g.flags & seg_rule_mask(end)) >> seg_rule_shift(end);
	if (was == rule) return;
	if (s.history.recording) {
		Change c;
		c.side = end;
		c.from = was;
		c.to = rule;
		j_record(s, OpKind::SEG_RULE, seg, c);
		s.history.open.grow(g.minx, g.minz, g.maxx, g.maxz);
	}
	put_rule(s, seg, end, rule);
}

void exec_street(CityState &s, const EditOp &op, bool fwd) {
	switch (op.kind) {
		case OpKind::ADD_ITEM: case OpKind::REM_ITEM:
			if (fwd == (op.kind == OpKind::ADD_ITEM)) s.street.put_item(op.id, op.get<StreetItem>()); else s.street.remove_item(op.id);
			invalidate(s, op.get<StreetItem>());
			break;
		case OpKind::ADD_POLE: case OpKind::REM_POLE:
			if (fwd == (op.kind == OpKind::ADD_POLE)) s.street.put_pole(op.id, op.get<UtilityPole>()); else s.street.remove_pole(op.id);
			invalidate(s, op.get<UtilityPole>());
			break;
		case OpKind::ADD_SPAN: case OpKind::REM_SPAN:
			if (fwd == (op.kind == OpKind::ADD_SPAN)) s.street.put_span(op.id, op.get<UtilitySpan>()); else s.street.remove_span(op.id);
			invalidate(s, op.get<UtilitySpan>());
			break;
		case OpKind::ADD_BARRIER: case OpKind::REM_BARRIER:
			if (fwd == (op.kind == OpKind::ADD_BARRIER)) s.street.put_barrier(op.id, op.get<Barrier>()); else s.street.remove_barrier(op.id);
			invalidate(s, op.get<Barrier>());
			break;
		case OpKind::ADD_CUT: case OpKind::REM_CUT:
			if (fwd == (op.kind == OpKind::ADD_CUT)) s.street.put_cut(op.id, op.get<StreetCut>()); else s.street.remove_cut(op.id);
			invalidate(s, op.get<StreetCut>());
			break;
		case OpKind::SEG_RULE: {
			const Change &c = op.get<Change>();
			put_rule(s, op.id, c.side, fwd ? c.to : c.from);
			break;
		}
		default: break; // not a street op: edit_history.cpp sends only these
	}
}

} // namespace city
