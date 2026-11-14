// Undo / redo (Roadcraft core/commands.ts CommandStack + roads/construction/journal.ts RoadJournal): every player
// command runs inside an entry that records its operations with the records they need to be undone. Undo runs the
// inverse of each operation in reverse order, redo runs them again in order, and both restore the same ids.
// Growth, traffic and the clock are not in the history. A stroke (zone or terrain brush held down) is one entry.
// An operation holds only what its kind needs (EditOp::payload), and what an entry holds is counted by one owner
// (op_bytes, EditEntry::measure) against the history's limits, the entry still open included.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/demolition/damage_state.h"
#include "sim/lots/lot_book.h"
#include "sim/nature/ground_paint.h"
#include "sim/roads/road_network.h"
#include "sim/street/street_works.h"
#include "sim/terrain/heightfield.h"
#include "sim/transit/transit_data.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace city {

struct CityState;

enum class OpKind : uint8_t { ADD_NODE, REM_NODE, NODE_PLAIN, NODE_CTL, NODE_SIGNAL, ADD_SEG, REM_SEG, PROFILE, ZONE, ADD_BLD, REM_BLD, EDIT_BLD, ADD_MOD,
	MONEY, MARKING, NATURE, DMG_CRATER, DMG_RUIN, DMG_GONE, DMG_HURT, ADD_ITEM, REM_ITEM, ADD_POLE, REM_POLE, ADD_SPAN, REM_SPAN,
	ADD_BARRIER, REM_BARRIER, ADD_CUT, REM_CUT, SEG_RULE, NODE_LINKS, NODE_LIFT, NODE_CROSSING, NODE_UTURN,
	SEG_WORTH, // what a standing road is built as, made again (its heights were changed)
	ADD_LOT, REM_LOT, EDIT_LOT, // the player's lots, one record each (LotBook)
	ADD_STOP, REM_STOP, EDIT_STOP, ADD_TRACK, REM_TRACK, EDIT_TRACK, ADD_LINE, REM_LINE, EDIT_LINE, TRANSIT_NEXT }; // the transport plan, one record each

// A change to the landscape (sim/commands/nature_commands.cpp): how to run it again (true) or take it back (false).
using NatureEdit = std::function<void(CityState &, bool forward)>;

// The payload of each kind of op.
struct Change { // NODE_PLAIN, NODE_CTL, NODE_CROSSING, NODE_UTURN, MARKING, ZONE (side, player: the side was the player's before),
	int side = 0, from = 0, to = 0; // SEG_RULE (side: the end), TRANSIT_NEXT (the plan's next id)
	bool player = false;
};
struct ProfileChange { // PROFILE: the segment as it was (what the change is worth: its length, structure and end heights)
	int from = 0, to = 0;
	RoadSegment seg;
};
struct LiftChange { // NODE_LIFT; SEG_WORTH: what a standing road is built as (RoadSegment::worth; from 0: it had none recorded)
	float from = 0, to = 0;
};
struct SignalChange { // NODE_SIGNAL
	SignalSettings from, to;
};
struct BuildingEdit { // EDIT_BLD
	Building before, after;
};
struct LinksChange { // NODE_LINKS: the connections made by hand at node `id`
	std::vector<LaneLink> before, after;
};
struct RuinChange { // DMG_RUIN (`id` its index in the list)
	Ruin ruin;
	bool removed = false;
};
struct HurtChange { // DMG_HURT: the building's damage after (before: 0 means none)
	Hurt after;
	float before = 0;
};
struct NatureChange { // NATURE: the edit and what it keeps (its closure's memory, told by its maker)
	std::shared_ptr<const NatureEdit> edit;
	size_t bytes = 0;
};
// A record of an ordered list (lots, stops, tracks, lines) added or removed at `index`: the list's order comes back with it.
template <typename R>
struct Listed {
	int index = 0;
	R record;
};
template <typename R>
struct Edited { // a record of an ordered list changed in place
	int index = 0;
	R before, after;
};

using OpPayload = std::variant<std::monostate, double, Change, ProfileChange, LiftChange, SignalChange, RoadNode, RoadSegment, Building, BuildingEdit,
	TerrainModifier, LinksChange, Crater, RuinChange, Gone, HurtChange, NatureChange, StreetItem, UtilityPole, UtilitySpan, Barrier, StreetCut,
	Listed<PlayerLot>, Edited<PlayerLot>, Listed<TransitStop>, Edited<TransitStop>, Listed<RailTrack>, Edited<RailTrack>, Listed<TransitLine>,
	Edited<TransitLine>>;

struct EditOp {
	OpKind kind = OpKind::MONEY;
	int id = -1;
	OpPayload payload; // what the kind needs (above); MONEY: what the command added to the funds (negative = charged)
	template <typename P>
	const P &get() const { return std::get<P>(payload); }
};

// What an op holds, its payload's own memory included: the one count of the history's bytes.
size_t op_bytes(const EditOp &op);

// A tile of painted ground before and after an entry (null: it did not exist).
struct PaintEdit {
	int key = 0;
	std::shared_ptr<const PaintTile> before, after;
};

// A terrain tile (Heightfield region) an open entry touched: its base heights as they were, and whether it matched the
// generated land then (undoing the entry makes it so again).
struct TerrainTouch {
	std::vector<uint16_t> before;
	bool clean = false;
};

// One undoable step. Terrain samples are kept apart: while the entry is open, each tile it touched as it was (by tile, in
// tile order); once closed, the samples that changed with their values before and after.
struct EditEntry {
	std::string label; // strings.csv key (cmd.*)
	std::vector<EditOp> ops;
	std::map<int, TerrainTouch> terrain_open;
	std::vector<uint32_t> terrain_idx;
	std::vector<int> clean_tiles; // tiles that matched the generated land before this edit: undoing it makes them so again
	std::vector<uint16_t> terrain_before, terrain_after;
	std::unordered_map<int, std::shared_ptr<const PaintTile>> paint_open; // tiles as they were (while open)
	std::vector<PaintEdit> paint;
	double minx = 0, minz = 0, maxx = 0, maxz = 0;
	bool has_box = false;
	size_t bytes = 0; // what it holds: kept up while it is open, measured again when it closes
	void reduce(); // edit_reduce.cpp: to its net effect, when it closes
	void measure(); // `bytes` counted again from what it holds (edit_history.cpp)
	bool empty() const { return ops.empty() && terrain_idx.empty() && terrain_open.empty() && paint.empty() && paint_open.empty(); }
	void grow(double x0, double z0, double x1, double z1);
};

class EditHistory {
public:
	std::vector<EditEntry> undo_stack, redo_stack;
	EditEntry open;
	bool recording = false; // inside a command: the journal functions below record
	bool group = false; // a stroke is open: consecutive commands go into the same entry
	bool stroke_has_h0 = false; // terrain stroke: the height under its first dab
	float stroke_h0 = 0;
	size_t max_steps = 0, max_bytes = 0; // from tuning "history"

	// The city's revisions (CityData revision, street_revision, edit_revision): what a refused command must leave as it found.
	struct Revisions {
		uint64_t map = 0, streets = 0, edits = 0;
	};

	void reset(const Tuning &t); // a new or loaded map: empty, with the limits of tuning "history"
	void clear();
	// Starts a command, keeping the city's revisions as they are: a refusal gives them back.
	void begin(const CityState &s, const std::string &label);
	// Ends a command: kept (pushed when no stroke is open) when ok, else everything it did is rolled back and the revisions it
	// moved go back to their values at begin. Either way the transaction is finished once (road_edit.h finish_command).
	void end(CityState &s, bool ok);
	// What the last command found (edit.refusal_inert, sim/checks/edit.cpp): whether it was refused, and the revisions at its begin.
	bool refused() const { return refused_; }
	const Revisions &at_begin() const { return at_begin_; }
	void begin_group(const std::string &label);
	void end_group(CityState &s);
	bool undo(CityState &s);
	bool redo(CityState &s);
	// Where the command in progress starts in the open entry (a stroke holds several commands).
	size_t command_start() const { return begin_ops; }
	const std::string &undo_label() const;
	const std::string &redo_label() const;
	// The open entry grew by `bytes`: what the history holds is retired against its limits, the open entry counted.
	void hold(size_t bytes);
	void changed() { touched_ = true; } // the command in progress recorded something (an op, a tile touched)

private:
	size_t begin_ops = 0; // ops the open entry had when the current command began (its rollback stops there)
	bool touched_ = false; // the command in progress recorded something
	// A refused command's rollback is the same city: nothing made for the old revisions is stale, and raising them would mark the
	// city unsaved and drop the kept furniture of every street (furniture_kept.h). No reader saw the raised values: the command
	// holds the map exclusively. It holds because a refusal rolls back all a command changed: only ops are rolled back, so no command
	// refuses after touching terrain or paint (j_terrain_touch, j_paint_touch).
	bool refused_ = false;
	Revisions at_begin_;
	void push(CityState &s);
	void close_terrain(const CityState &s);
	void close_paint(const CityState &s);
	void trim();
};

// Applies an entry backwards (undo) or forwards (redo) and finishes the transaction (road_edit.h finish_command).
void apply_entry(CityState &s, const EditEntry &e, bool forward);

// Journalled mutations: the only way sim/commands change roads, buildings, terrain and money. Each records its op in
// s.history.open while a command is recording.
int j_add_node(CityState &s, double x, double z, double y, float lift = 0); // lift: its authored height (RoadNode.lift)
void j_remove_node_if_orphan(CityState &s, int n);
void j_set_plain(CityState &s, int n, bool plain);
void j_set_lift(CityState &s, int n, float lift);
void j_set_crossing(CityState &s, int n, uint8_t kind);
void j_set_uturn(CityState &s, int n, uint8_t kind);
void j_set_control(CityState &s, int n, uint8_t ctl);
void j_set_signal(CityState &s, int n, const SignalSettings &to);
// `worth`: what the segment is built as (RoadSegment::worth), handed on from the road it comes of; WORTH_AS_LAID: priced as
// it is laid by itself now (sim/economy/road_economy.h built_alone).
constexpr float WORTH_AS_LAID = -1.f;
void j_set_worth(CityState &s, int seg, float worth); // a standing road built otherwise now (its heights were changed)
int j_add_segment(CityState &s, int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags, float worth = WORTH_AS_LAID);
void j_remove_segment(CityState &s, int seg); // the links and bans naming it dropped, its bus stops left on no road
void j_set_profile(CityState &s, int seg, int profile);
void j_set_marking(CityState &s, int style);
void j_set_zone(CityState &s, int seg, int side, int zone);
int j_add_building(CityState &s, const Building &b);
void j_remove_building(CityState &s, int id);
void j_edit_building(CityState &s, int id, const Building &after);
int j_add_modifier(CityState &s, const std::vector<float> &pts, float flat, float max_run);
void j_money(CityState &s, double delta);
// The public transport plan replaced by `next`: what differs, record by record, is what the journal keeps.
void j_transit(CityState &s, TransitData next);
// The road of the bus stop at `index` of the plan set to `seg` (-1: none, the stop inert): one EDIT_STOP, not the whole plan.
void j_stop_road(CityState &s, int index, int seg);
// The player's lots, one record at a time (sim/commands/lot_commands.cpp). The lot cells they cover are the caller's to mark.
int j_add_lot(CityState &s, PlayerLot lot); // its id, the book's next
void j_remove_lot(CityState &s, int id);
void j_edit_lot(CityState &s, int id, const PlayerLot &after);
// The lot cells under a ring's box queued for the lot cache (lot_commands.cpp): what a command and its undo read again.
void mark_lot_cells(CityState &s, const std::vector<LotPoint> &ring);
void j_links(CityState &s, int node, std::vector<LaneLink> next); // the connections made by hand at a node replaced
// Damage (journal_damage.cpp): a crater dug, a ruin left or cleared, a piece of street furniture knocked out.
void j_add_crater(CityState &s, const Crater &c);
void j_add_ruin(CityState &s, const Ruin &r);
void j_remove_ruin(CityState &s, int index);
void j_add_gone(CityState &s, const Gone &g);
// Sets what a building has taken (0 clears it).
void j_set_hurt(CityState &s, uint32_t seed, float x, float z, float amount);
void apply_damage_op(CityState &s, const EditOp &op, bool forward);
// Appends an op with its payload to the open step, for the journals of other owners (street, damage); its bytes are counted.
EditOp &j_record(CityState &s, OpKind k, int id, OpPayload payload);
// Keeps the base heights of the rectangle as they are now (before a dab changes them).
void j_terrain_touch(CityState &s, const SampleRect &r);
// A landscape change already made, with what undoes and redoes it; `bytes` what it keeps.
void j_nature(CityState &s, size_t bytes, NatureEdit edit);
void j_add_scar(CityState &s, const Scar &c); // journal_damage.cpp
// Keeps the painted tiles as they are now (before a dab changes them).
void j_paint_touch(CityState &s, const std::vector<int> &tiles);

} // namespace city
