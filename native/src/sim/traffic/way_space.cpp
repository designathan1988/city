// The planner's room for its searches (way_space.h).
#include "sim/traffic/way_space.h"

namespace city {
namespace traffic {

WaySpace::WaySpace() : room(std::make_unique<Room>()) {}
WaySpace::~WaySpace() = default;

} // namespace traffic
} // namespace city
