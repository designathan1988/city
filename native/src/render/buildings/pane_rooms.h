// The rooms seen through a building's panes: its interior (sim/buildings/interior) and the rooms worked out so far. A pane
// alike on another floor of the same type is looked at once.
#pragma once

#include "render/buildings/shell.h"
#include "sim/buildings/interior/interior.h"

#include <array>
#include <functional>
#include <map>
#include <utility>

namespace city::b40 {

// Exact inputs to room_at: seed, level, volume, position, both basis axes, sill and storey height.
using PaneRoomKey = std::array<uint32_t, 11>;
using PaneMemo = std::function<PaneRoom(const std::string &, const PaneRoomKey &, const std::function<PaneRoom()> &)>;
struct PaneRooms {
	PaneRooms(const Data &data, const Body &body, Purpose purpose, FloorMemo floor = {}, PaneMemo pane = {})
		: inside(data, body, purpose, std::move(floor)), memo(std::move(pane)) {}
	BodyRef source_lease; // configured shared-cache workspaces pin the source separately from retained pure values
	Interior inside;
	PaneMemo memo;
	std::map<PaneRoomKey, PaneRoom> known;
};

} // namespace city::b40
