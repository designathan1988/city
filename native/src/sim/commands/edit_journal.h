// The journal of the player's edits (its records: sim/state/edit_history.h): every player command runs inside an entry, and
// each journalled mutation below records its operation there with what it needs to be undone. Undo runs the inverse of each
// operation in reverse order, redo runs them again in order, and both restore the same ids.
#pragma once

#include "sim/state/edit_history.h"

#include <cstdint>
#include <string>
#include <vector>

namespace city {

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
int j_add_segment(CityState &s, int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags);
void j_remove_segment(CityState &s, int seg);
void j_set_profile(CityState &s, int seg, int profile);
void j_set_marking(CityState &s, int style);
int j_add_building(CityState &s, const Building &b);
void j_remove_building(CityState &s, int id);
void j_edit_building(CityState &s, int id, const Building &after);
int j_add_modifier(CityState &s, const std::vector<float> &pts, float flat, float max_run);
void j_money(CityState &s, double delta);
// The public transport plan replaced by `next`: what differs, record by record, is what the journal keeps.
void j_transit(CityState &s, TransitData next);
// The player's lots, one record at a time (sim/commands/lot_commands.cpp). The lot cells they cover are the caller's to mark.
int j_add_lot(CityState &s, PlayerLot lot); // its id, the book's next
void j_remove_lot(CityState &s, int id);
void j_edit_lot(CityState &s, int id, const PlayerLot &after);
// The lot cells under a ring's box queued for the lot cache (lot_commands.cpp): what a command and its undo read again.
void mark_lot_cells(CityState &s, const std::vector<XZ> &ring);
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
