// Ordered route requests: one shared tick work allowance and one reusable search, whole results only at safe points.
#pragma once
#include "core/work_budget.h"
#include "sim/traffic/astar.h"
#include "sim/traffic/route_arena.h"
#include <memory>
namespace city {
struct CityState;
namespace traffic {
enum class RoutePurpose : uint8_t { BUS, MESO, TRIP, LOT, LORRY, REPAIR, RECONSIDER, DETOUR, PED_ESCAPE, PED_BIRTH, PED_KERB, VEHICLE_BIRTH, COUNT }; // subsequent producers join this owner, never another allowance
enum class RouteActorKind : uint8_t { VEHICLE, PEDESTRIAN, DEMAND };
struct RouteKey {
	RoutePurpose purpose = RoutePurpose::BUS; int actor = -1; RouteActorKind kind = RouteActorKind::VEHICLE;
	bool operator<(const RouteKey &b) const { if (purpose != b.purpose) return purpose < b.purpose; if (actor != b.actor) return actor < b.actor; return kind < b.kind; }
	static RouteKey pedestrian(RoutePurpose purpose, int actor) { return {purpose, actor, RouteActorKind::PEDESTRIAN}; }
	static RouteKey demand(RoutePurpose purpose) { return {purpose, 0, RouteActorKind::DEMAND}; }
};
struct RoadRequest { int from = -1, goal = -1, limit = -1, banned = -1; bool bike = false; };
enum class RecipientState : uint8_t { READY, WAIT_SOURCE, OBSOLETE };
enum class RequestPreparation : uint8_t { PENDING, READY, SKIPPED };
struct RouteRecipient {
	virtual ~RouteRecipient() = default;
	virtual RecipientState state(const CityState &s) const = 0; // bounded identity/dependency checks, no geometry
	virtual RequestPreparation prepare_request(CityState &, RoadRequest &, WorkBudget &) { return RequestPreparation::READY; } // captured intent selection, no actor publication
	virtual bool prepare_result(CityState &, SearchStatus, const RouteDraft &, WorkBudget &) { return true; } // owned preparation only, no actor publication/callbacks
	virtual void retire_result(CityState &) {} // release owned preparation on cancellation, source restart or terminal delivery
	virtual bool complete(CityState &s, SearchStatus status, RouteDraft &prepared) = 0; // false defers unchanged result; true consumes it, unused handle is released
};
class RouteAdmission {
public:
	RouteAdmission(); ~RouteAdmission();
	RouteAdmission(RouteAdmission &&) noexcept;
	RouteAdmission &operator=(RouteAdmission &&) noexcept;
	RouteAdmission(const RouteAdmission &) = delete;
	RouteAdmission &operator=(const RouteAdmission &) = delete;
	void submit(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient);
	void submit_walk(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient); // the same queue/query scratch, output belongs to ped_pool
	void cancel(CityState &s, RouteKey key);
	void cancel_actor(CityState &s, int actor, RouteActorKind kind = RouteActorKind::VEHICLE);
	void cancel_kind(CityState &s, RouteActorKind kind); // domain reset, no callback into actors being destroyed
	void cancel_walk(CityState &s); // all walking jobs, including vehicle-owned kerb births, before ped_pool reset
	bool pending() const;
	void advance(CityState &s, WorkBudget &work); // caller safe point; submit never invokes another actor's callback
private:
	void submit_impl(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient, bool walking);
	struct Impl; std::unique_ptr<Impl> impl_;
};
} // namespace traffic
} // namespace city
