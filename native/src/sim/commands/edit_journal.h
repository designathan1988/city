// Undo / redo (Roadcraft core/commands.ts CommandStack + roads/construction/journal.ts RoadJournal): every player
// command runs inside an entry that records its operations with the full records they need to be undone. Undo runs
// the inverse of each operation in reverse order, redo runs them again in order, and both restore the same ids.
// Growth, traffic and the clock are not in the history. A stroke (zone or terrain brush held down) is one entry.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/roads/road_network.h"
#include "sim/terrain/heightfield.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

struct CityState;

enum class OpKind : uint8_t { ADD_NODE, REM_NODE, NODE_PLAIN, ADD_SEG, REM_SEG, PROFILE, ZONE, ADD_BLD, REM_BLD, EDIT_BLD, ADD_MOD,
	MONEY, MARKING };

struct EditOp {
	OpKind kind = OpKind::MONEY;
	int id = -1;
	int side = 0, from = 0, to = 0; // PROFILE / ZONE / NODE_PLAIN
	double money = 0; // MONEY: what the command added to the funds (negative = charged)
	RoadNode node;
	RoadSegment seg;
	Building bld, bld_after;
	TerrainModifier mod;
};

// One undoable step. Terrain samples are kept apart: their values before (while open, by index) and after.
struct EditEntry {
	std::string label; // strings.csv key (cmd.*)
	std::vector<EditOp> ops;
	std::unordered_map<uint32_t, uint16_t> terrain_open;
	std::vector<uint32_t> terrain_idx;
	std::vector<uint16_t> terrain_before, terrain_after;
	double minx = 0, minz = 0, maxx = 0, maxz = 0;
	bool has_box = false;
	size_t bytes = 0;
	bool empty() const { return ops.empty() && terrain_idx.empty() && terrain_open.empty(); }
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
	const std::string &undo_label() const;
	const std::string &redo_label() const;

private:
	size_t begin_ops = 0; // ops the open entry had when the current command began (its rollback stops there)
	void push(CityState &s);
	void close_terrain(const CityState &s);
	void trim();
};

// Applies an entry backwards (undo) or forwards (redo) and settles what it touched (node shapes, lots, revision).
void apply_entry(CityState &s, const EditEntry &e, bool forward);

// Journalled mutations: the only way sim/commands change roads, buildings, terrain and money. Each records its op in
// s.history.open while a command is recording.
int j_add_node(CityState &s, double x, double z, double y);
void j_remove_node_if_orphan(CityState &s, int n);
void j_set_plain(CityState &s, int n, bool plain);
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
// Keeps the base heights of the rectangle as they are now (before a dab changes them).
void j_terrain_touch(CityState &s, const SampleRect &r);

} // namespace city
