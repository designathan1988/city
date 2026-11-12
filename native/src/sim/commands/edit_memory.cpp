// Deep journal accounting follows payload ownership, never operation-time byte estimates or element counts alone.
#include "sim/commands/edit_memory.h"

#include "sim/commands/edit_journal.h"

#include <type_traits>

namespace city {
namespace {

template <typename T> void account(RetainedMemory &, const T &) {
	static_assert(std::is_trivially_copyable_v<T>, "A journal payload with owned data needs explicit deep accounting");
}
void account(RetainedMemory &m, const RoadNode &v) { m.vector(v.segs); m.vector(v.signal.greens); m.vector(v.links); }
void account(RetainedMemory &m, const SignalChange &v) { m.vector(v.before.greens); m.vector(v.after.greens); }
void account(RetainedMemory &m, const RoadSegment &v) { m.vector(v.table); m.vector(v.lot_table); }
void body_root(RetainedMemory &m, const b40::BodyRef &body) {
	m.shared(b40::BodyTable::authored_of(body), [&](const b40::Body &) { return b40::BodyTable::retained_bytes(body); });
}
void account(RetainedMemory &m, const BuildingChange &v) {
	body_root(m, v.body_before); body_root(m, v.body_after);
}
void account(RetainedMemory &m, const TerrainModifier &v) { m.vector(v.pts); }
void account(RetainedMemory &m, const RailTrack &v) { m.vector(v.points); }
void account(RetainedMemory &m, const TransitLine &v) { m.vector(v.stops); m.string(v.name); }
void account(RetainedMemory &m, const PlayerLot &v) { m.vector(v.corners); }
template <typename T> void delta(RetainedMemory &m, const TableDelta<T> &v) {
	m.vector(v.edits);
	for (const auto &edit : v.edits) {
		if (edit.before) account(m, *edit.before);
		if (edit.after) account(m, *edit.after);
	}
}
void account(RetainedMemory &m, const TransitChange &v) { delta(m, v.stops); delta(m, v.tracks); delta(m, v.lines); }
void account(RetainedMemory &m, const LotsChange &v) { delta(m, v.lots); }
void account(RetainedMemory &m, const LinksChange &v) { m.vector(v.before); m.vector(v.after); }
void account(RetainedMemory &m, const RuinChange &v) { body_root(m, v.body); }
void account(RetainedMemory &m, const StreetItem &v) { m.string(v.text); }
void account(RetainedMemory &m, const UtilityPole &v) { m.vector(v.spans); }
void account(RetainedMemory &m, const Barrier &v) { m.vector(v.pts); }
void account(RetainedMemory &m, const CloudsEdit &v) { m.vector(v.before); m.vector(v.after); }
void account(RetainedMemory &m, const FogCleared &v) { m.vector(v.before); }
void account(RetainedMemory &m, const TreesEdit &v) { m.vector(v.ids); m.vector(v.trees); }
void account(RetainedMemory &m, const TreesCleared &v) {
	m.shared(v.before, [&](const PlantedTrees &) { return v.bytes; });
}
void account(RetainedMemory &m, const ElementsEdit &v) { m.vector(v.items); }
void account(RetainedMemory &m, const WaterEdit &v) { m.vector(v.index); m.vector(v.before); m.vector(v.after); }
void account(RetainedMemory &m, const NatureChange &v) {
	std::visit([&](const auto &edit) { account(m, edit); }, v.edit);
}
void paint_root(RetainedMemory &m, const std::shared_ptr<const PaintTile> &p) {
	m.shared(p, [](const PaintTile &) { return sizeof(PaintTile); });
}

} // namespace

void account_entry(RetainedMemory &m, const EditEntry &e) {
	m.string(e.label);
	m.vector(e.ops);
	for (const EditOp &op : e.ops) std::visit([&](const auto &payload) { account(m, payload); }, op.payload);
	m.hash(e.terrain_open);
	m.vector(e.terrain_idx); m.vector(e.terrain_before); m.vector(e.terrain_after); m.vector(e.clean_tiles);
	m.hash(e.paint_open);
	for (const auto &tile : e.paint_open) paint_root(m, tile.second);
	m.vector(e.paint);
	for (const PaintEdit &tile : e.paint) { paint_root(m, tile.before); paint_root(m, tile.after); }
}

} // namespace city
