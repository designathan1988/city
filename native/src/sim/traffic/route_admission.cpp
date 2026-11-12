// Shared road-query admission, cooperative path assembly and identity-checked whole-result delivery.
#include "sim/traffic/route_admission.h"
#include "sim/traffic/route_search.h"
#include "sim/state/city_state.h"
#include <list>
#include <map>
#include <optional>
#include <utility>

namespace city::traffic {
struct RouteAdmission::Impl {
	struct Job {
		RouteKey key; RoadRequest request; std::unique_ptr<RouteRecipient> recipient; bool walking = false;
		std::optional<RouteDraft> prepared;
		SearchStatus result = SearchStatus::PENDING;
		uint64_t city = 0, graph = 0, roads = 0, deferred = 0;
		bool request_ready = false;
		int endpoint_from = -1, endpoint_goal = -1; uint64_t from_generation = 0, goal_generation = 0;
	};
	std::list<Job> jobs;
	std::map<RouteKey, std::list<Job>::iterator> indexed;
	RoadRouteSearch search;
	RouteDraft prepared;
	size_t copied = 0;
	bool started = false, allocated = false, pumping = false, walking = false;
	SearchStatus result = SearchStatus::PENDING;
	static RoutePool &pool(CityState &s, bool walking) { return walking ? s.traffic.ped_pool : s.traffic.route_pool; }
	void discard(CityState &s) { pool(s, walking).release(prepared); search.discard(); started = allocated = false; copied = 0; result = SearchStatus::PENDING; walking = false; }
	static uint64_t endpoint(const CityState &s, int id, bool walking) { return walking ? s.roads.node_generation(id) : s.roads.segment_generation(id < 0 ? -1 : id / 2); }
	static void capture_endpoints(const CityState &s, Job &job, bool initial = false) {
		if (initial || job.endpoint_from != job.request.from) { job.endpoint_from = job.request.from; job.from_generation = endpoint(s, job.request.from, job.walking); }
		if (initial || job.endpoint_goal != job.request.goal) { job.endpoint_goal = job.request.goal; job.goal_generation = endpoint(s, job.request.goal, job.walking); }
	}
	static SearchStatus endpoint_status(const CityState &s, const Job &job) {
		if (endpoint(s, job.request.goal, job.walking) != job.goal_generation) return SearchStatus::TARGET_RETIRED;
		if (endpoint(s, job.request.from, job.walking) != job.from_generation) return SearchStatus::SOURCE_RETIRED;
		return SearchStatus::PENDING;
	}
	void defer(std::list<Job>::iterator job, uint64_t cycle) { job->deferred = cycle; jobs.splice(jobs.end(), jobs, job); }
};
RouteAdmission::RouteAdmission() : impl_(std::make_unique<Impl>()) {}
RouteAdmission::~RouteAdmission() = default; // city reset destroys the matching RoutePool pages, not callbacks into a dead pool
RouteAdmission::RouteAdmission(RouteAdmission &&) noexcept = default;
RouteAdmission &RouteAdmission::operator=(RouteAdmission &&) noexcept = default;
void RouteAdmission::cancel(CityState &s, RouteKey key) {
	const auto found = impl_->indexed.find(key); if (found == impl_->indexed.end()) return;
	if (found->second == impl_->jobs.begin()) impl_->discard(s);
	found->second->recipient->retire_result(s);
	if (found->second->prepared) Impl::pool(s, found->second->walking).release(*found->second->prepared);
	impl_->jobs.erase(found->second); impl_->indexed.erase(found);
}
void RouteAdmission::cancel_actor(CityState &s, int actor, RouteActorKind kind) {
	for (int purpose = 0; purpose < int(RoutePurpose::COUNT); ++purpose) cancel(s, {RoutePurpose(purpose), actor, kind});
}
void RouteAdmission::submit(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient) { submit_impl(s, key, request, std::move(recipient), false); }
void RouteAdmission::submit_walk(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient) { submit_impl(s, key, request, std::move(recipient), true); }
void RouteAdmission::submit_impl(CityState &s, RouteKey key, RoadRequest request, std::unique_ptr<RouteRecipient> recipient, bool walking) {
	cancel(s, key);
	impl_->jobs.push_back({key, request, std::move(recipient), walking});
	Impl::capture_endpoints(s, impl_->jobs.back(), true);
	impl_->indexed.emplace(key, std::prev(impl_->jobs.end()));
}
void RouteAdmission::cancel_kind(CityState &s, RouteActorKind kind) {
	for (auto i = impl_->indexed.begin(); i != impl_->indexed.end();) { const RouteKey key = i->first; ++i; if (key.kind == kind) cancel(s, key); }
}
void RouteAdmission::cancel_walk(CityState &s) {
	for (auto i = impl_->indexed.begin(); i != impl_->indexed.end();) {
		const RouteKey key = i->first; const bool walking = i->second->walking; ++i; if (walking) cancel(s, key);
	}
}
bool RouteAdmission::pending() const { return !impl_->jobs.empty(); }
void RouteAdmission::advance(CityState &s, WorkBudget &work) {
	Impl &q = *impl_; if (q.pumping) return; q.pumping = true;
	const uint64_t cycle = next_generation();
	while (!q.jobs.empty() && work.left > 0) {
		auto job = q.jobs.begin(); auto &front = *job;
		RoutePool &pool = Impl::pool(s, front.walking);
		if (front.deferred == cycle) break; // each deferred entry is visited at most once in this safe-point cycle
		const RecipientState recipient = front.recipient->state(s);
		if (recipient == RecipientState::WAIT_SOURCE) { work.take(); q.discard(s); q.defer(job, cycle); continue; }
		const SearchStatus retired = Impl::endpoint_status(s, front);
		if (recipient == RecipientState::OBSOLETE || retired != SearchStatus::PENDING) {
			q.discard(s); front.recipient->retire_result(s);
			if (front.prepared) pool.release(*front.prepared); else front.prepared.emplace();
			front.result = retired != SearchStatus::PENDING ? retired : SearchStatus::INVALIDATED;
		} else if (front.prepared && (front.city != s.generation || front.graph != s.traffic.g.routing_revision || (front.walking && front.roads != uint64_t(s.roads.version)))) {
			front.recipient->retire_result(s); pool.release(*front.prepared); front.prepared.reset(); front.result = SearchStatus::PENDING;
		}
		if (!front.prepared && !front.request_ready) {
			const auto preparation = front.recipient->prepare_request(s, front.request, work);
			if (preparation == RequestPreparation::PENDING) { q.defer(job, cycle); continue; }
			if (preparation == RequestPreparation::SKIPPED) { front.prepared.emplace(); front.result = SearchStatus::SKIPPED; front.city = s.generation; front.graph = s.traffic.g.routing_revision; front.roads = uint64_t(s.roads.version); }
			else { Impl::capture_endpoints(s, front); front.request_ready = true; }
		}
		if (!front.prepared) {
			if (!q.started) {
				const auto &r = front.request;
				q.walking = front.walking;
				if (front.walking) q.search.begin_walk(s, r.from, r.goal, r.limit); else q.search.begin(s, r.from, r.goal, r.limit, r.banned, r.bike);
				q.started = true;
				front.city = s.generation; front.graph = s.traffic.g.routing_revision; front.roads = uint64_t(s.roads.version);
			} else if (!q.search.source_current(s)) { work.take(); q.discard(s); continue; }
			if (q.result == SearchStatus::PENDING) q.result = q.search.advance(s, work);
			if (q.result == SearchStatus::PENDING) break;
			if (q.result == SearchStatus::UNPREPARED) { work.take(); q.discard(s); q.defer(job, cycle); continue; }
			if (q.result == SearchStatus::FOUND) {
				if (!q.allocated) {
					if (!work.take()) break;
					pool.prepare(q.prepared, int(q.search.length())); pool.set_goal(q.prepared, front.goal_generation); q.allocated = true;
				}
				while (q.copied < q.search.length() && work.take()) { pool.write(q.prepared, int(q.copied), q.search.at(q.copied)); ++q.copied; }
				if (q.copied != q.search.length()) break;
			}
			front.result = q.result; front.prepared.emplace(std::move(q.prepared)); q.discard(s);
		}
		if (!front.recipient->prepare_result(s, front.result, *front.prepared, work)) { q.defer(job, cycle); continue; }
		if (!work.take()) break;
		// Detach before callback: submitting the next attempt cannot invalidate a borrowed list/actor reference.
		std::list<Impl::Job> delivered; delivered.splice(delivered.end(), q.jobs, job); q.indexed.erase(front.key);
		if (front.recipient->complete(s, front.result, *front.prepared)) { front.recipient->retire_result(s); pool.release(*front.prepared); }
		else if (q.indexed.find(front.key) == q.indexed.end()) {
			front.deferred = cycle; q.jobs.splice(q.jobs.end(), delivered, job); q.indexed.emplace(front.key, job);
		} else { front.recipient->retire_result(s); pool.release(*front.prepared); } // a newer desired request already replaced this result
	}
	q.pumping = false;
}
} // namespace city::traffic
