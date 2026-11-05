// The rooms seen through a building's panes: its interior (sim/buildings/interior) and the rooms worked out so far. A pane
// alike on another floor of the same type is looked at once.
#pragma once

#include "render/buildings/shell.h"
#include "sim/buildings/interior/interior.h"

#include <unordered_map>

namespace city::b40 {

struct PaneRooms {
	PaneRooms(const Data &data, const Body &body, Purpose purpose) : inside(data, body, purpose) {}
	Interior inside;
	std::unordered_map<uint64_t, PaneRoom> known;
};

} // namespace city::b40
