// Journalled mutations of the street (sim/street) and of a junction's or a segment's street state, with their inverses: the
// street's part of the edit journal, next to it and apart from it.
#include "sim/commands/street_journal.h"

#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <cmath>

namespace city {

namespace {
void grow_at(CityState &s, double x, double z) { s.history.open.grow(x, z, x, z); }

void mark_barrier(CityState &s, const Barrier &b) {
	for (size_t i = 0; i + 3 < b.pts.size(); i += 2) {
		const double dx = b.pts[i + 2] - b.pts[i], dz = b.pts[i + 3] - b.pts[i + 1];
		const int steps = int(std::ceil(std::hypot(dx, dz) / CHUNK)) + 1;
		for (int k = 0; k <= steps; k++) mark_street(s, b.pts[i] + dx * k / steps, b.pts[i + 1] + dz * k / steps);
	}
}
} // namespace

int j_add_item(CityState &s, const StreetItem &r) {
	int id = s.street.add_item(r);
	mark_street(s, r.x, r.z);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_ITEM, id).item = s.street.items.items[size_t(id)];
		grow_at(s, r.x, r.z);
	}
	return id;
}

void j_remove_item(CityState &s, int id) {
	const StreetItem r = s.street.items.items[size_t(id)];
	if (s.history.recording) {
		j_record(s, OpKind::REM_ITEM, id).item = r;
		grow_at(s, r.x, r.z);
	}
	s.street.remove_item(id);
	mark_street(s, r.x, r.z);
}

int j_add_pole(CityState &s, const UtilityPole &r) {
	int id = s.street.add_pole(r);
	mark_street(s, r.x, r.z);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_POLE, id).pole = s.street.poles.items[size_t(id)];
		grow_at(s, r.x, r.z);
	}
	return id;
}

void j_remove_pole(CityState &s, int id) {
	const std::vector<int> spans = s.street.poles.items[size_t(id)].spans;
	for (int span : spans) j_remove_span(s, span);
	const UtilityPole r = s.street.poles.items[size_t(id)];
	if (s.history.recording) {
		j_record(s, OpKind::REM_POLE, id).pole = r;
		grow_at(s, r.x, r.z);
	}
	s.street.remove_pole(id);
	mark_street(s, r.x, r.z);
}

int j_add_span(CityState &s, int a, int b) {
	UtilitySpan r;
	r.a = a;
	r.b = b;
	int id = s.street.add_span(r);
	const UtilityPole &pa = s.street.poles.items[size_t(a)], &pb = s.street.poles.items[size_t(b)];
	mark_street(s, pa.x, pa.z);
	mark_street(s, pb.x, pb.z);
	if (s.history.recording) j_record(s, OpKind::ADD_SPAN, id).span = s.street.spans.items[size_t(id)];
	return id;
}

void j_remove_span(CityState &s, int id) {
	const UtilitySpan r = s.street.spans.items[size_t(id)];
	if (s.history.recording) j_record(s, OpKind::REM_SPAN, id).span = r;
	const UtilityPole &pa = s.street.poles.items[size_t(r.a)], &pb = s.street.poles.items[size_t(r.b)];
	mark_street(s, pa.x, pa.z);
	mark_street(s, pb.x, pb.z);
	s.street.remove_span(id);
}

int j_add_barrier(CityState &s, const Barrier &r) {
	int id = s.street.add_barrier(r);
	mark_barrier(s, r);
	if (s.history.recording) {
		EditOp &op = j_record(s, OpKind::ADD_BARRIER, id);
		op.barrier = s.street.barriers.items[size_t(id)];
		s.history.open.bytes += r.pts.size() * sizeof(float);
		grow_at(s, r.pts[0], r.pts[1]);
	}
	return id;
}

void j_remove_barrier(CityState &s, int id) {
	const Barrier r = s.street.barriers.items[size_t(id)];
	if (s.history.recording) {
		EditOp &op = j_record(s, OpKind::REM_BARRIER, id);
		op.barrier = r;
		s.history.open.bytes += r.pts.size() * sizeof(float);
		grow_at(s, r.pts[0], r.pts[1]);
	}
	s.street.remove_barrier(id);
	mark_barrier(s, r);
}

int j_add_cut(CityState &s, float x, float z) {
	StreetCut r;
	r.x = x;
	r.z = z;
	int id = s.street.add_cut(r);
	mark_street(s, x, z);
	if (s.history.recording) {
		j_record(s, OpKind::ADD_CUT, id).cut = s.street.cuts.items[size_t(id)];
		grow_at(s, x, z);
	}
	return id;
}

void j_set_ctl(CityState &s, int n, uint8_t ctl) {
	RoadNode &nd = s.roads.nodes[size_t(n)];
	if (nd.ctl == ctl) return;
	if (s.history.recording) {
		EditOp &op = j_record(s, OpKind::NODE_CTL, n);
		op.from = nd.ctl;
		op.to = ctl;
		grow_at(s, nd.x, nd.z);
	}
	nd.ctl = ctl;
	s.log.net_node(n);
}

void j_set_flags(CityState &s, int seg, uint16_t flags) {
	RoadSegment &g = s.roads.segs[size_t(seg)];
	if (g.flags == flags) return;
	if (s.history.recording) {
		EditOp &op = j_record(s, OpKind::SEG_FLAGS, seg);
		op.from = g.flags;
		op.to = flags;
		s.history.open.grow(g.minx, g.minz, g.maxx, g.maxz);
	}
	g.flags = flags;
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	mark_furniture(s, seg);
}

void exec_street(CityState &s, const EditOp &op, bool fwd) {
	RoadNetwork &net = s.roads;
	switch (op.kind) {
		case OpKind::ADD_ITEM: case OpKind::REM_ITEM:
			if (fwd == (op.kind == OpKind::ADD_ITEM)) s.street.put_item(op.id, op.item); else s.street.remove_item(op.id);
			mark_street(s, op.item.x, op.item.z);
			break;
		case OpKind::ADD_POLE: case OpKind::REM_POLE:
			if (fwd == (op.kind == OpKind::ADD_POLE)) s.street.put_pole(op.id, op.pole); else s.street.remove_pole(op.id);
			mark_street(s, op.pole.x, op.pole.z);
			break;
		case OpKind::ADD_SPAN: case OpKind::REM_SPAN: {
			if (fwd == (op.kind == OpKind::ADD_SPAN)) s.street.put_span(op.id, op.span); else s.street.remove_span(op.id);
			const UtilityPole &pa = s.street.poles.items[size_t(op.span.a)], &pb = s.street.poles.items[size_t(op.span.b)];
			mark_street(s, pa.x, pa.z);
			mark_street(s, pb.x, pb.z);
			break;
		}
		case OpKind::ADD_BARRIER: case OpKind::REM_BARRIER:
			if (fwd == (op.kind == OpKind::ADD_BARRIER)) s.street.put_barrier(op.id, op.barrier); else s.street.remove_barrier(op.id);
			for (size_t i = 0; i + 1 < op.barrier.pts.size(); i += 2) mark_street(s, op.barrier.pts[i], op.barrier.pts[i + 1]);
			break;
		case OpKind::ADD_CUT: case OpKind::REM_CUT:
			if (fwd == (op.kind == OpKind::ADD_CUT)) s.street.put_cut(op.id, op.cut); else s.street.remove_cut(op.id);
			mark_street(s, op.cut.x, op.cut.z);
			break;
		case OpKind::NODE_CTL:
			net.nodes[size_t(op.id)].ctl = uint8_t(fwd ? op.to : op.from);
			s.log.net_node(op.id);
			break;
		case OpKind::SEG_FLAGS: {
			RoadSegment &g = net.segs[size_t(op.id)];
			g.flags = uint16_t(fwd ? op.to : op.from);
			s.log.net_node(g.a);
			s.log.net_node(g.b);
			s.log.net_seg(op.id);
			mark_furniture(s, op.id);
			break;
		}
		default: break;
	}
}

} // namespace city
