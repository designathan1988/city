// Pending walker room leases: spatial metadata only; physical actors enter walker_index only after a whole route exists.
#pragma once
#include "sim/traffic/actor_index.h"
#include "core/free_slots.h"
#include <memory>
#include <mutex>
#include <vector>
#include <utility>
namespace city {
class WalkerBirthReservations {
 struct Point { float x = 0, z = 0; };
 struct State { std::mutex mutex; ActorIndex index; std::vector<Point> points; FreeSlots free; };
public:
 class Lease {
 public:
  Lease(const Lease &) = delete; Lease &operator=(const Lease &) = delete;
  ~Lease() {
   std::lock_guard<std::mutex> guard(state_->mutex);
   state_->index.erase(id_); state_->free.push_back(id_);
  }
 private:
  friend class WalkerBirthReservations;
  Lease(std::shared_ptr<State> state, int id) : state_(std::move(state)), id_(id) {}
  std::shared_ptr<State> state_; int id_;
 };
 WalkerBirthReservations() : state_(std::make_shared<State>()) {}
 WalkerBirthReservations(WalkerBirthReservations &&) = default;
 WalkerBirthReservations &operator=(WalkerBirthReservations &&) = default;
 WalkerBirthReservations(const WalkerBirthReservations &) = delete;
 WalkerBirthReservations &operator=(const WalkerBirthReservations &) = delete;
 std::shared_ptr<const Lease> reserve(float x, float z, float room) {
  std::lock_guard<std::mutex> guard(state_->mutex);
  const bool blocked = state_->index.any_near(x,z,room,[&](int id) {
   const Point p = state_->points[size_t(id)]; const float dx = p.x-x, dz = p.z-z;
   return dx*dx + dz*dz < room*room;
  });
  if (blocked) return {};
  int id;
  if (state_->free.empty()) { id = int(state_->points.size()); state_->points.push_back({x,z}); }
  else { id = state_->free.back(); state_->free.pop_back(); state_->points[size_t(id)] = {x,z}; }
  auto lease = std::shared_ptr<const Lease>(new Lease(state_, id));
  state_->index.put(id, {x,z,x,z}); return lease;
 }
private:
 std::shared_ptr<State> state_; // outstanding leases can retire their own previous world without touching CityState
};
} // namespace city
