// A car the camera's window shows never vanishes from it: whether the window shows a vehicle, and what one with no trip to make
// does while it is shown (unseen_exit.cpp). It leaves once out of sight (vehicle_motion.cpp).
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// Whether the camera's window shows the vehicle: within the publication's reach of the camera's point (traffic_system.cpp).
bool window_shows(const CityState &s, const Vehicle &v);
// A car shown with no trip to make drives on by a way its lane has (never back the way it came), to leave where nobody sees it.
// False when its lane leads nowhere its body fits.
bool drive_on(CityState &s, int id);

} // namespace traffic
} // namespace city
