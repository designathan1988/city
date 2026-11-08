// The parts of a frame's render CPU that render_cost gives in milliseconds (city_core_probe_render.cpp), in the order the panels
// list them: the one list the lab's CPU tab, the health log and the camera probe read (CityCore::constants "frame_sections").
#pragma once

namespace city {

constexpr const char *FRAME_SECTIONS[] = { "agents", "lock", "stream", "levels", "flush", "roads", "props", "buildings", "trees", "overlays" };

} // namespace city
