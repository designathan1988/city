// Undo / redo (Roadcraft core/commands.ts CommandStack + roads/construction/journal.ts RoadJournal): every player
// command runs inside an entry that records its operations with the full records they need to be undone. Undo runs
// the inverse of each operation in reverse order, redo runs them again in order, and both restore the same ids.
// Growth, traffic and the clock are not in the history. A stroke (zone or terrain brush held down) is one entry.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/demolition/damage_state.h"
#include "sim/nature/ground_paint.h"
#include "sim/roads/road_network.h"
#include "sim/street/street_works.h"
#include "sim/terrain/heightfield.h"
#include "sim/transit/transit_data.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

struct CityState;

enum class OpKind : uint8_t { ADD_NODE, REM_NODE, NODE_PLAIN, NODE_CTL, NODE_SIGNAL, ADD_SEG, REM_SEG, PROFILE, ZONE, ADD_BLD, REM_BLD, EDIT_BLD, ADD_MOD,
	MONEY, MARKING, NATURE, TRANSIT, DMG_CRATER, DMG_RUIN, DMG_GONE, DMG_HURT, ADD_ITEM, REM_ITEM, ADD_POLE, REM_POLE, ADD_SPAN, REM_SPAN,
	ADD_BARRIER, REM_BARRIER, ADD_CUT, REM_CUT, SEG_RULE };

// A change to the landscape (sim/commands/nature_commands.cpp): how to run it again (true) or take it back (false).
using NatureEdit = std::function<void(CityState &, bool forward)>;

struct EditOp {
	OpKind kind = OpKind::MONEY;
	int id = -1;
	int side = 0, from = 0, to = 0; // PROFILE / ZONE / NODE_PLAIN / NODE_CTL / SEG_RULE
	bool player = false; // ZONE: the side was the player's before
	double money = 0; // MONEY: what the command added to the funds (negative = charged)
	RoadNode node;
	SignalSettings signal_from, signal_to; // NODE_SIGNAL
	RoadSegment seg;
	Building bld, bld_after;
	TerrainModifier mod;
	std::shared_ptr<const TransitData> transit_before, transit_after; // TRANSIT: the plan replaced whole
	Crater crater; // DMG_CRATER
	Ruin ruin; // DMG_RUIN (side 0 added, 1 removed; id its index)
	Gone gone; // DMG_GONE
	Hurt hurt; // DMG_HURT: the building's damage after (hurt_before: before; 0 means none)
	float hurt_before = 0;
	StreetItem item; // the street's records (street_journal.cpp)
	UtilityPole pole;
	UtilitySpan span;
	Barrier barrier;
	StreetCut cut;
	std::shared_ptr<const NatureEdit> nature; // NATURE
};

// A tile of painted ground before and after an entry (null: it did not exist).
struct PaintEdit {
	int key = 0;
	std::shared_ptr<const PaintTile> before, after;
};

// One undoable step. Terrain samples are kept apart: their values before (while open, by index) and after.
struct EditEntry {
	std::string label; // strings.csv key (cmd.*)
	std::vector<EditOp> ops;
	std::unordered_map<uint32_t, uint16_t> terrain_open;
	std::vector<uint32_t> terrain_idx;
	std::vector<uint16_t> terrain_before, terrain_after;
	std::unordered_map<int, std::shared_ptr<const PaintTile>> paint_open; // tiles as they were (while open)
	std::vector<PaintEdit> paint;
	double minx = 0, minz = 0, maxx = 0, maxz = 0;
	bool has_box = false;
	size_t bytes = 0;
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

	void reset(const Tuning &t); // a new or loaded map: empty, with the limits of tuning "history"
	void clear();
	void begin(const std::string &label);
	// Ends a command: kept (pushed when no stroke is open) when ok, else everything it did is rolled back.
	void end(CityState &s, bool ok);
	void begin_group(const std::string &label);
	void end_group(CityState &s);
	bool undo(CityState &s);
	bool redo(CityState &s);
	// Where the command in progress starts in the open entry (a stroke holds several commands).
	size_t command_start() const { return begin_ops; }
	const std::string &undo_label() const;
	const std::string &redo_label() const;

private:
	size_t begin_ops = 0; // ops the open entry had when the current command began (its rollback stops there)
	void push(CityState &s);
	void close_terrain(const CityState &s);
	void close_paint(const CityState &s);
	void trim();
};

// Applies an entry backwards (undo) or forwards (redo) and settles what it touched (node shapes, lots, revision).
void apply_entry(CityState &s, const EditEntry &e, bool forward);

// Journalled mutations: the only way sim/commands change roads, buildings, terrain and money. Each records its op in
// s.history.open while a command is recording.
int j_add_node(CityState &s, double x, double z, double y);
void j_remove_node_if_orphan(CityState &s, int n);
void j_set_plain(CityState &s, int n, bool plain);
void j_set_control(CityState &s, int n, uint8_t ctl);
void j_set_signal(CityState &s, int n, const SignalSettings &to);
int j_add_segment(CityState &s, int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags);
void j_remove_segment(CityState &s, int seg);
void j_set_profile(CityState &s, int seg, int profile);
void j_set_marking(CityState &s, int style);
void j_set_zone(CityState &s, int seg, int side, int zone);
int j_add_building(CityState &s, const Building &b);
void j_remove_building(CityState &s, int id);
void j_edit_building(CityState &s, int id, const Building &after);
int j_add_modifier(CityState &s, const std::vector<float> &pts, float flat, float max_run);
void j_money(CityState &s, double delta);
void j_transit(CityState &s, TransitData next); // the public transport plan replaced
// Damage (journal_damage.cpp): a crater dug, a ruin left or cleared, a piece of street furniture knocked out.
void j_add_crater(CityState &s, const Crater &c);
void j_add_ruin(CityState &s, const Ruin &r);
void j_remove_ruin(CityState &s, int index);
void j_add_gone(CityState &s, const Gone &g);
// Sets what a building has taken (0 clears it).
void j_set_hurt(CityState &s, uint32_t seed, float x, float z, float amount);
void apply_damage_op(CityState &s, const EditOp &op, bool forward);
EditOp &j_record(CityState &s, OpKind k, int id); // appends an op to the open step, for the journals of other owners
// Keeps the base heights of the rectangle as they are now (before a dab changes them).
void j_terrain_touch(CityState &s, const SampleRect &r);
// A landscape change already made, with what undoes and redoes it; `bytes` what it keeps.
void j_nature(CityState &s, size_t bytes, NatureEdit edit);
// Keeps the painted tiles as they are now (before a dab changes them).
void j_paint_touch(CityState &s, const std::vector<int> &tiles);

} // namespace city
