// The street's operations of the edit journal (street_journal.cpp): each changes the state and, while a command records,
// keeps what undoes it.
#pragma once

#include "sim/commands/edit_journal.h"

namespace city {

int j_add_item(CityState &s, const StreetItem &r);
void j_remove_item(CityState &s, int id);
int j_add_pole(CityState &s, const UtilityPole &r);
void j_remove_pole(CityState &s, int id); // and its spans
int j_add_span(CityState &s, int a, int b);
void j_remove_span(CityState &s, int id);
int j_add_barrier(CityState &s, const Barrier &r);
void j_remove_barrier(CityState &s, int id);
int j_add_cut(CityState &s, float x, float z);
// The plate the player put on the approach of `seg` at its end `end` (0 a, 1 b), a SegRule; the junction's control is
// j_set_control's.
void j_set_rule(CityState &s, int seg, int end, uint16_t rule);
void exec_street(CityState &s, const EditOp &op, bool fwd); // one op of these, forward or undone

} // namespace city
