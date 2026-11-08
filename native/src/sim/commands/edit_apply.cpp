// The inverse of each recorded op (Roadcraft journal.ts exec), run backwards (undo) or forwards (redo), and an entry
// applied whole: its ops in order or in reverse, its terrain samples and painted tiles, the transaction finished.
#include "sim/commands/edit_journal.h"

#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
// The lane graph follows an undo/redo the same way it follows a journal op: mark what the traffic must rebuild.
void mark_seg(CityState &s, int id, bool removed) {
	const RoadSegment &g = s.roads.segs[id];
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	(removed ? s.log.net_seg_removed(id) : s.log.net_seg(id));
}

// A record of an ordered list put back at its place, or taken from it.
template <typename R>
void put_listed(std::vector<R> &list, const Listed<R> &l, bool in) {
	if (in) list.insert(list.begin() + std::min<std::ptrdiff_t>(l.index, std::ptrdiff_t(list.size())), l.record);
	else if (l.index >= 0 && l.index < int(list.size())) list.erase(list.begin() + l.index);
}
template <typename R>
void put_edited(std::vector<R> &list, const Edited<R> &e, bool fwd) {
	if (e.index >= 0 && e.index < int(list.size())) list[size_t(e.index)] = fwd ? e.after : e.before;
}

void exec_lot(CityState &s, const EditOp &op, bool fwd) {
	LotBook &book = s.lot_book;
	if (op.kind == OpKind::EDIT_LOT) {
		const Edited<PlayerLot> &e = op.get<Edited<PlayerLot>>();
		book.set(e.index, fwd ? e.after : e.before);
		mark_lot_cells(s, e.before.corners);
		mark_lot_cells(s, e.after.corners);
		return;
	}
	const Listed<PlayerLot> &l = op.get<Listed<PlayerLot>>();
	const bool in = (op.kind == OpKind::ADD_LOT) == fwd;
	if (in) book.insert(l.index, l.record);
	else book.erase(l.index);
	if (op.kind == OpKind::ADD_LOT) book.next_id = fwd ? l.record.id + 1 : l.record.id; // j_add_lot gave it the book's next id
	mark_lot_cells(s, l.record.corners);
}

void exec_transit(CityState &s, const EditOp &op, bool fwd) {
	TransitData &t = s.transit;
	switch (op.kind) {
		case OpKind::ADD_STOP: case OpKind::REM_STOP: put_listed(t.stops, op.get<Listed<TransitStop>>(), (op.kind == OpKind::ADD_STOP) == fwd); break;
		case OpKind::EDIT_STOP: put_edited(t.stops, op.get<Edited<TransitStop>>(), fwd); break;
		case OpKind::ADD_TRACK: case OpKind::REM_TRACK: put_listed(t.tracks, op.get<Listed<RailTrack>>(), (op.kind == OpKind::ADD_TRACK) == fwd); break;
		case OpKind::EDIT_TRACK: put_edited(t.tracks, op.get<Edited<RailTrack>>(), fwd); break;
		case OpKind::ADD_LINE: case OpKind::REM_LINE: put_listed(t.lines, op.get<Listed<TransitLine>>(), (op.kind == OpKind::ADD_LINE) == fwd); break;
		case OpKind::EDIT_LINE: put_edited(t.lines, op.get<Edited<TransitLine>>(), fwd); break;
		case OpKind::TRANSIT_NEXT: t.next_id = fwd ? op.get<Change>().to : op.get<Change>().from; break;
		default: return;
	}
	s.transit_revision++;
}

// A node's op: laid or taken, its plain end, crossing, U-turn, height, control, signal or connections made by hand.
void exec_node(CityState &s, const EditOp &op, bool fwd) {
	RoadNetwork &net = s.roads;
	switch (op.kind) {
		case OpKind::ADD_NODE:
			if (fwd) net.put_node(op.id, op.get<RoadNode>()); else net.remove_node_if_orphan(op.id);
			s.log.net_node(op.id);
			break;
		case OpKind::REM_NODE:
			if (fwd) net.remove_node_if_orphan(op.id); else net.put_node(op.id, op.get<RoadNode>());
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_PLAIN: {
			const Change &c = op.get<Change>();
			net.set_plain(op.id, (fwd ? c.to : c.from) != 0);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_CROSSING: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].crossing = uint8_t(fwd ? c.to : c.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_UTURN: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].uturn = uint8_t(fwd ? c.to : c.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_LIFT: {
			const LiftChange &c = op.get<LiftChange>();
			net.set_lift(op.id, fwd ? c.to : c.from);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_CTL: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].ctl = uint8_t(fwd ? c.to : c.from);
			mark_control(s, op.id);
			break;
		}
		case OpKind::NODE_SIGNAL: {
			const SignalChange &c = op.get<SignalChange>();
			net.nodes[op.id].signal = fwd ? c.to : c.from;
			s.log.signal_nodes.push_back(op.id);
			break;
		}
		case OpKind::NODE_LINKS: {
			const LinksChange &c = op.get<LinksChange>();
			s.roads.nodes[size_t(op.id)].links = fwd ? c.after : c.before;
			s.log.net_node(op.id);
			break;
		}
		default: break;
	}
}

// A segment's op, or the network's marking style: laid or taken, its profile, a side's zone.
void exec_segment(CityState &s, const EditOp &op, bool fwd) {
	RoadNetwork &net = s.roads;
	switch (op.kind) {
		case OpKind::ADD_SEG:
			if (fwd) { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			else { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			break;
		case OpKind::REM_SEG:
			if (fwd) { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			else { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			break;
		case OpKind::MARKING: {
			const Change &c = op.get<Change>();
			s.marking_style.store(fwd ? c.to : c.from);
			for (int seg = 0; seg < int(net.segs.size()); seg++)
				if (net.segs[size_t(seg)].alive) net.mark_segment(seg);
			break;
		}
		case OpKind::PROFILE: {
			const ProfileChange &c = op.get<ProfileChange>();
			net.set_profile(op.id, fwd ? c.to : c.from);
			mark_seg(s, op.id, false);
			break;
		}
		default: break;
	}
}

void exec(CityState &s, const EditOp &op, bool fwd) {
	switch (op.kind) {
		case OpKind::ADD_NODE: case OpKind::REM_NODE: case OpKind::NODE_PLAIN: case OpKind::NODE_CROSSING: case OpKind::NODE_UTURN:
		case OpKind::NODE_LIFT: case OpKind::NODE_CTL: case OpKind::NODE_SIGNAL: case OpKind::NODE_LINKS: exec_node(s, op, fwd); break;
		case OpKind::ADD_SEG: case OpKind::REM_SEG: case OpKind::MARKING: case OpKind::PROFILE: exec_segment(s, op, fwd); break;
		case OpKind::ADD_BLD: if (fwd) s.buildings.put(op.id, op.get<Building>(), true); else s.buildings.remove(op.id); break;
		case OpKind::REM_BLD: if (fwd) s.buildings.remove(op.id); else s.buildings.put(op.id, op.get<Building>(), true); break;
		case OpKind::EDIT_BLD: {
			const BuildingEdit &e = op.get<BuildingEdit>();
			s.buildings.remove(op.id);
			s.buildings.put(op.id, fwd ? e.after : e.before, false);
			break;
		}
		case OpKind::ADD_MOD: if (fwd) s.terrain.put_modifier(op.id, op.get<TerrainModifier>()); else s.terrain.remove_modifier(op.id); break;
		case OpKind::MONEY: s.money += fwd ? op.get<double>() : -op.get<double>(); break;
		case OpKind::ADD_LOT: case OpKind::REM_LOT: case OpKind::EDIT_LOT: exec_lot(s, op, fwd); break;
		case OpKind::ADD_STOP: case OpKind::REM_STOP: case OpKind::EDIT_STOP: case OpKind::ADD_TRACK: case OpKind::REM_TRACK: case OpKind::EDIT_TRACK:
		case OpKind::ADD_LINE: case OpKind::REM_LINE: case OpKind::EDIT_LINE: case OpKind::TRANSIT_NEXT: exec_transit(s, op, fwd); break;
		case OpKind::DMG_CRATER:
		case OpKind::DMG_RUIN:
		case OpKind::DMG_GONE:
		case OpKind::DMG_HURT: apply_damage_op(s, op, fwd); break;
		case OpKind::NATURE: (*op.get<NatureChange>().edit)(s, fwd); break;
		case OpKind::ADD_ITEM: case OpKind::REM_ITEM: case OpKind::ADD_POLE: case OpKind::REM_POLE: case OpKind::ADD_SPAN:
		case OpKind::REM_SPAN: case OpKind::ADD_BARRIER: case OpKind::REM_BARRIER: case OpKind::ADD_CUT: case OpKind::REM_CUT:
		case OpKind::SEG_RULE: exec_street(s, op, fwd); break;
	}
}

} // namespace

void apply_entry(CityState &s, const EditEntry &e, bool forward) {
	if (forward) {
		for (const EditOp &op : e.ops) exec(s, op, true);
	} else {
		for (size_t i = e.ops.size(); i-- > 0;) exec(s, e.ops[i], false);
	}
	if (!e.terrain_idx.empty()) {
		s.terrain.write_samples(e.terrain_idx, forward ? e.terrain_after : e.terrain_before);
		if (!forward)
			for (int tile : e.clean_tiles) s.terrain.tile_dirty[size_t(tile)] = 0;
		mark_ground(s, e.minx, e.minz, e.maxx, e.maxz);
	}
	for (const PaintEdit &p : e.paint) {
		s.nature.ground.put_tile(p.key, (forward ? p.after : p.before).get());
		int r[4];
		s.nature.ground.tile_rect(p.key, r);
		mark_paint(s, r);
	}
	finish_command(s, true); // an entry applied is a change: settled, derived, the revisions moved
}

} // namespace city
