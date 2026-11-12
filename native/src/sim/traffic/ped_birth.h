// Explicit pedestrian birth outcomes: parents own pending recipes and claim only complete, generation-checked actors.
#pragma once
#include <cstdint>
#include <memory>
namespace city {
struct CityState;
namespace traffic {
struct PedBirthIntent;
enum class PedBirthState : uint8_t { PENDING, READY, FAILED, CANCELLED };
struct PedBirthTicket {
 PedBirthState state = PedBirthState::PENDING;
 int ped = -1; uint64_t generation = 0, request = 0;
 bool claimed = false;
 std::shared_ptr<PedBirthIntent> intent;
};
std::shared_ptr<PedBirthTicket> request_footway_birth(CityState &s, int vehicle, int seg, int side, float arc);
PedBirthState take_walker_birth(CityState &s, std::shared_ptr<PedBirthTicket> &ticket, int &ped, uint64_t &generation);
void discard_walker_birth(CityState &s, std::shared_ptr<PedBirthTicket> &ticket);
} // namespace traffic
} // namespace city
