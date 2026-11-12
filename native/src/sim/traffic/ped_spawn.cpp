// Who comes out on foot: a walker of an age and pace, on the streets the land use has people on at this hour, with a
// destination 150-900 m away and a walking route to it. The crowd of a street just opened stands on its sidewalks; after
// that they come out of the doors (ped_doors.cpp), on a street without one out of nowhere as before. Some come with company
// (ped_party.cpp).
#include "sim/traffic/traffic_system.h"
#include "core/generation.h"
#include "sim/traffic/route_join.h"
#include "sim/traffic/kerb_stops.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

struct PedBirthIntent {
 Pedestrian walker; PeopleNow now; PedDoor door;
 bool ambient = false, from_door = false, out = false, initialized = false;
 int from = -1, attempts = 0, destination = -1, destination_a = -1, goal_side = -1, far = -1;
 uint64_t city = 0, segment = 0, region = 0, destination_generation = 0, destination_a_generation = 0, far_generation = 0;
 Bounds2 origin;
 int vehicle = -1, ds = -1, lane = -1, conn = -1, bus = -1, kerb = -1;
 float bus_stop_at = -1, stop_at = -1;
 uint64_t vehicle_generation = 0;
 std::shared_ptr<const WalkerBirthReservations::Lease> claim;
 RouteKey key() const { return ambient ? RouteKey::demand(RoutePurpose::PED_BIRTH) : RouteKey{RoutePurpose::PED_KERB, vehicle}; }
};

namespace {

constexpr int PICK_TRIES = 12, DEST_TRIES = 12;
constexpr float SCALE_MIN = 0.9f, SCALE_MAX = 1.1f;

bool has_side(const Profile &p, int side) { return side == 0 ? p.has_walk_right : p.has_walk_left; }

// Only the walkable segments inside ped_window_m, by the people their zones have on the street now.
int pick_seg(CityState &s, const PeopleNow &now) {
	TrafficState &t = s.traffic;
	const int n = int(t.ped_segs.size());
	for (int k = 0; k < PICK_TRIES && n > 0 && now.peak > 0; k++) {
		const int seg = t.ped_segs[size_t(t.rng.next() % uint32_t(n))];
		if (!walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const TrafficParams &p = s.catalog.traffic;
		const float here = std::max(zone_people(p, now, s.catalog.zone(g.zl).base % BASE_ZONES), zone_people(p, now, s.catalog.zone(g.zr).base % BASE_ZONES));
		if (t.rng.f() * now.peak < here) return seg;
	}
	return -1;
}

// Whether a walker standing at `at` would be closer to another than the room walkers keep (two bodies and the gap):
// against every walker, those born this step too (a few are born a step).
bool crowded(const TrafficState &t, const TrafficParams &p, const Pose &at) {
	const float room = 2 * p.ped_body_m + p.ped_gap_m;
	return t.walker_index.any_near(at.x, at.z, room, [&](int id) {
		const Pedestrian &o = t.peds[size_t(id)];
		const float dx = o.cur.x - at.x, dz = o.cur.z - at.z;
		return o.alive && dx * dx + dz * dz < room * room;
	});
}

// Standing at the door, its way out to the sidewalk at `d`.
void at_door(const CityState &s, Pedestrian &w, const PedDoor &d) {
	Pose foot;
	sidewalk_pose(s, w.seg, w.side, d.s, d.u, foot);
	w.s = d.s;
	w.cur.x = d.x;
	w.cur.y = d.y;
	w.cur.z = d.z;
	w.ways[0] = Waypoint{ foot.x, foot.y, foot.z, -1 };
	w.wn = 1;
	w.wi = 0;
	w.mode = PedMode::LOT;
	w.lx = d.x;
	w.ly = d.y;
	w.lz = d.z;
	w.llen = std::hypot(foot.x - d.x, foot.z - d.z);
	w.ls = 0;
	const V2 way = V2(foot.x - d.x, foot.z - d.z).norm();
	w.cur.fx = way.x;
	w.cur.fz = way.y;
	onto_line(w);
}

} // namespace

PedAge roll_age(TrafficState &t, const TrafficParams &p) {
	const float r = t.rng.f();
	if (r < p.ped_child_share) return PedAge::CHILD;
	return r < p.ped_child_share + p.ped_elder_share ? PedAge::ELDER : PedAge::ADULT;
}

// The age of somebody who goes on foot with no one: a child never does (it walks with a grown-up, the party), so the share of
// children is left out and the others keep their proportion.
PedAge roll_age_alone(TrafficState &t, const TrafficParams &p) {
	const float r = t.rng.f() * (1.f - p.ped_child_share);
	return r < p.ped_elder_share ? PedAge::ELDER : PedAge::ADULT;
}

void dress_walker(TrafficState &t, const TrafficParams &p, Pedestrian &w, PedAge age, float hp) {
	w.alive = true;
	w.hp = hp;
	w.age = age;
	w.look = t.rng.next();
	const float pace = age == PedAge::CHILD ? p.ped_child_pace : age == PedAge::ELDER ? p.ped_elder_pace : 1.f;
	w.speed = t.rng.range(p.walk_speed_min, p.walk_speed_max) * pace;
	w.track = t.rng.range(p.walk_track_min, p.walk_track_max);
	w.scale = t.rng.range(SCALE_MIN, SCALE_MAX);
	w.walked = t.rng.range(0.f, w.speed);
}

int alloc_walker(TrafficState &t) {
	if (!t.free_peds.empty()) {
		int id = t.free_peds.back();
		t.free_peds.pop_back();
		t.live_peds.insert(id);
		return id;
	}
	t.peds.emplace_back();
	const int id = int(t.peds.size()) - 1;
	t.live_peds.insert(id);
	return id;
}

void commit_walker(TrafficState &t, int id, Pedestrian &&w) {
	w.generation = next_generation(); w.escape_route_request = 0; w.escape_goal = -1; w.escape_goal_generation = 0;
	t.peds[size_t(id)] = std::move(w); w.route = {}; // raw route ownership moved with this actual actor, never duplicated in the proposal
}

namespace {

void capture_origin(const CityState &s, PedBirthIntent &intent) {
 const auto &g = s.roads.segs[size_t(intent.walker.seg)];
 intent.segment = s.roads.segment_generation(intent.walker.seg);
 intent.origin = {g.minx,g.minz,g.maxx,g.maxz};
 intent.region = s.roads.ground_regions.stamp(g.minx,g.minz,g.maxx,g.maxz);
}

bool initialize_birth(CityState &s, PedBirthIntent &intent) {
 TrafficState &t = s.traffic; const TrafficParams &p = s.catalog.traffic; Pedestrian &w = intent.walker;
 if (intent.ambient) {
  const int seg = pick_seg(s, intent.now); if (seg < 0) return false;
  const auto &g = s.roads.segs[size_t(seg)]; const auto &pf = s.roads.profile_of(seg);
  dress_walker(t,p,w,roll_age(t,p),s.catalog.blast.walker_hp);
  w.seg = seg; w.dir = int(t.rng.next() & 1);
  w.side = has_side(pf,0) && (!has_side(pf,1) || (t.rng.next() & 1)) ? 0 : 1;
  w.s = t.rng.range(arc_lo(g),arc_hi(g)); on_sidewalk(w);
  intent.out = intent.from_door && pick_door(s,intent.now,seg,w.side,true,arc_lo(g),arc_hi(g),intent.door);
  if (intent.out) at_door(s,w,intent.door);
  else {
   w.s = clear_arc(s,seg,w.side,w.s,w.u); if (w.s > arc_hi(g)) return false;
   sidewalk_pose(s,seg,w.side,w.s,w.u,w.cur);
  }
  capture_origin(s,intent);
 } else {
  const auto &g = s.roads.segs[size_t(w.seg)];
  dress_walker(t,p,w,roll_age_alone(t,p),s.catalog.blast.walker_hp);
  w.dir = int(t.rng.next() & 1); w.s = clampf(w.s,arc_lo(g),arc_hi(g));
  on_sidewalk(w); sidewalk_pose(s,w.seg,w.side,w.s,w.u,w.cur);
 }
 if (crowded(t,p,w.cur)) return false;
 intent.claim = t.walker_birth_reservations.reserve(w.cur.x,w.cur.z,2*p.ped_body_m+p.ped_gap_m);
 if (!intent.claim) return false;
 const auto &g = s.roads.segs[size_t(w.seg)]; intent.from = w.dir == 0 ? g.b : g.a;
 intent.initialized = true; return true;
}

void queue_birth(CityState &s, const std::shared_ptr<PedBirthTicket> &ticket);
void queue_ambient(CityState &s, const PeopleNow &now, bool from_door);

class WalkerBirthRecipient final : public RouteRecipient {
public:
 explicit WalkerBirthRecipient(std::shared_ptr<PedBirthTicket> ticket) : ticket_(std::move(ticket)), request_(ticket_->request) {}
 ~WalkerBirthRecipient() override {
  if (ticket_->request == request_ && ticket_->state == PedBirthState::PENDING) {
   ticket_->state = PedBirthState::CANCELLED; ticket_->intent->claim.reset();
  }
 }
 RecipientState state(const CityState &s) const override {
  const auto &i = *ticket_->intent; const auto &w = i.walker;
  if (ticket_->request != request_ || ticket_->state != PedBirthState::PENDING || s.generation != i.city) return RecipientState::OBSOLETE;
  if (!i.ambient) {
   if (i.vehicle < 0 || size_t(i.vehicle) >= s.traffic.vehicles.size()) return RecipientState::OBSOLETE;
   const auto &v = s.traffic.vehicles[size_t(i.vehicle)];
   if (!v.alive || v.generation != i.vehicle_generation || v.ds != i.ds || v.lane != i.lane || v.conn != i.conn || v.off || v.bus != i.bus || v.kerb != i.kerb || v.bus_stop_at != i.bus_stop_at || v.stop_at != i.stop_at ||
    i.ds < 0 || size_t(i.ds) >= s.traffic.g.ds.size() || s.traffic.g.ds[size_t(i.ds)].seg != w.seg) return RecipientState::OBSOLETE;
  }
  if (i.initialized || !i.ambient) {
   if (s.roads.segment_generation(w.seg) != i.segment || !walkable(s,w.seg) ||
    s.roads.ground_regions.stamp(float(i.origin.minx),float(i.origin.minz),float(i.origin.maxx),float(i.origin.maxz)) != i.region ||
    (i.out && !door_current(s,i.door))) return RecipientState::OBSOLETE;
  }
  if (i.initialized && i.ambient) {
   const float dx = w.cur.x-s.camera_x.load(), dz = w.cur.z-s.camera_z.load();
   const float reach = s.catalog.traffic.ped_window_m;
   if (dx*dx+dz*dz > reach*reach) return RecipientState::OBSOLETE;
  }
  if (i.destination >= 0 && (s.roads.segment_generation(i.destination) != i.destination_generation || !walkable(s,i.destination) ||
   s.roads.segs[size_t(i.destination)].a != i.destination_a || s.roads.node_generation(i.destination_a) != i.destination_a_generation ||
   s.roads.segs[size_t(i.destination)].b != i.far || s.roads.node_generation(i.far) != i.far_generation)) return RecipientState::OBSOLETE;
  return s.traffic_levels.people_on ? RecipientState::READY : RecipientState::WAIT_SOURCE;
 }
 RequestPreparation prepare_request(CityState &s, RoadRequest &request, WorkBudget &work) override {
  auto &i = *ticket_->intent;
  if (!i.initialized) {
   if (!work.take()) return RequestPreparation::PENDING;
   if (!initialize_birth(s,i)) return RequestPreparation::SKIPPED;
  }
  const int n = int(s.roads.segs.size()); const auto &p = s.catalog.traffic;
  while (i.attempts < 2*DEST_TRIES) {
   if (!work.take()) return RequestPreparation::PENDING;
   const bool door = i.attempts++ < DEST_TRIES;
   const int key = door ? door_sidewalk(s) : -1;
   const int seg = door ? (key < 0 ? -1 : key/2) : n > 0 ? int(s.traffic.rng.next()%uint32_t(n)) : -1;
   if (seg < 0 || !walkable(s,seg)) continue;
   const auto &g = s.roads.segs[size_t(seg)]; const auto &a = s.roads.nodes[size_t(i.from)], &b = s.roads.nodes[size_t(g.a)];
   const float d = float(std::hypot(b.x-a.x,b.z-a.z));
   if (d < p.ped_trip_min_m || d > p.ped_trip_max_m) continue;
   i.destination = seg; i.destination_generation = s.roads.segment_generation(seg);
   i.destination_a = g.a; i.destination_a_generation = s.roads.node_generation(g.a);
   i.goal_side = door ? key%2 : -1; i.far = g.b; i.far_generation = s.roads.node_generation(g.b);
   request = {i.from,g.a}; return RequestPreparation::READY;
  }
  return RequestPreparation::SKIPPED;
 }
 bool prepare_result(CityState &s, SearchStatus status, const RouteDraft &path, WorkBudget &work) override {
  if (state(s) != RecipientState::READY || status != SearchStatus::FOUND || path.size() < 2) return true;
  const auto &i = *ticket_->intent; const auto &pool = s.traffic.ped_pool;
  const bool append = i.goal_side >= 0 && pool.at(path,path.size()-2) != i.far;
  return output_.prepare(s.traffic.ped_pool,RouteRef{},-1,path,0,false,0,work,append ? i.far : -1,i.far_generation);
 }
 void retire_result(CityState &s) override { output_.retire(s.traffic.ped_pool); }
 bool complete(CityState &s, SearchStatus status, RouteDraft &path) override {
  auto &i = *ticket_->intent;
  if (state(s) != RecipientState::READY || status == SearchStatus::SOURCE_RETIRED || status == SearchStatus::TARGET_RETIRED || status == SearchStatus::INVALIDATED) {
   finish(s,PedBirthState::CANCELLED); return true;
  }
  if (status != SearchStatus::FOUND || path.size() < 2) {
   if (status == SearchStatus::SKIPPED) finish(s,PedBirthState::FAILED);
   else { i.destination = -1; queue_birth(s,ticket_); } // an actual failed route advances to the next saved candidate ordinal
   return true;
  }
  auto &t = s.traffic; auto &w = i.walker;
  if (i.out) {
   w.cur.y = w.ly = s.terrain.height_at(i.door.x,i.door.z);
   Pose foot; sidewalk_pose(s,w.seg,w.side,i.door.s,i.door.u,foot); w.ways[0].y = foot.y;
  } else {
   if (!i.ambient) {
    const auto &v = t.vehicles[size_t(i.vehicle)]; const auto &p = s.catalog.traffic;
    const auto at = kerb_door_at(s,v,p.classes[int(v.type)].length*p.kerb_door_along);
    const auto &g = s.roads.segs[size_t(w.seg)]; w.s = clampf(kerb_footway_arc(g,at.foot),arc_lo(g),arc_hi(g));
   }
   sidewalk_pose(s,w.seg,w.side,w.s,w.u,w.cur);
  }
  if (crowded(t,s.catalog.traffic,w.cur)) { finish(s,PedBirthState::FAILED); return true; }
  if (!i.ambient) {
   i.claim.reset(); i.claim = t.walker_birth_reservations.reserve(w.cur.x,w.cur.z,2*s.catalog.traffic.ped_body_m+s.catalog.traffic.ped_gap_m);
   if (!i.claim) { finish(s,PedBirthState::FAILED); return true; }
  }
  output_.adopt(t.ped_pool,w.route); w.goal_side = i.goal_side;
  bool company = false;
  if (i.ambient) { company = w.age == PedAge::CHILD || t.rng.f() < s.catalog.traffic.ped_company_share; w.company_due = company && i.out; }
  else w.mode = PedMode::KERB;
  const int id = alloc_walker(t); commit_walker(t,id,std::move(w)); index_walker(s,id); t.ped_count++;
  if (i.ambient) { t.doors_out += i.out ? 1 : 0; if (company && !i.out) add_company(s,id); }
  ticket_->ped = id; ticket_->generation = t.peds[size_t(id)].generation; ticket_->claimed = i.ambient;
  finish(s,PedBirthState::READY); return true;
 }
private:
 void finish(CityState &s, PedBirthState state) {
  const auto &i = *ticket_->intent; ticket_->state = state; ticket_->intent->claim.reset();
  if (i.ambient && s.traffic.pedestrian_birth == ticket_) {
   s.traffic.pedestrian_birth.reset(); --s.traffic.pedestrian_birth_remaining;
   if (s.traffic.pedestrian_birth_remaining > 0) queue_ambient(s,i.now,i.from_door);
  }
 }
 std::shared_ptr<PedBirthTicket> ticket_; uint64_t request_; RouteJoin output_;
};

void queue_birth(CityState &s, const std::shared_ptr<PedBirthTicket> &ticket) {
 ticket->request = next_generation();
 s.traffic.route_admission.submit_walk(s,ticket->intent->key(),{},std::make_unique<WalkerBirthRecipient>(ticket));
}
void queue_ambient(CityState &s, const PeopleNow &now, bool from_door) {
 auto ticket = std::make_shared<PedBirthTicket>(); ticket->intent = std::make_shared<PedBirthIntent>();
 auto &i = *ticket->intent; i.ambient = true; i.from_door = from_door; i.now = now; i.city = s.generation;
 s.traffic.pedestrian_birth = ticket; queue_birth(s,ticket);
}
} // namespace

void spawn_walkers(CityState &s, const PeopleNow &now, bool from_door, int attempts) {
 auto &t = s.traffic;
 if (t.pedestrian_birth && t.pedestrian_birth->state == PedBirthState::PENDING) return;
 if (t.pedestrian_birth) { t.pedestrian_birth.reset(); --t.pedestrian_birth_remaining; } // cancelled by a source/domain owner
 if (t.pedestrian_birth_remaining <= 0) t.pedestrian_birth_remaining = attempts;
 if (t.pedestrian_birth_remaining > 0) queue_ambient(s,now,from_door);
}

std::shared_ptr<PedBirthTicket> request_footway_birth(CityState &s, int vehicle, int seg, int side, float arc) {
 auto ticket = std::make_shared<PedBirthTicket>(); ticket->intent = std::make_shared<PedBirthIntent>(); auto &i = *ticket->intent;
 i.city = s.generation; i.walker.seg = seg; i.walker.side = side; i.walker.s = arc; i.vehicle = vehicle;
 if (vehicle < 0 || size_t(vehicle) >= s.traffic.vehicles.size() || seg < 0 || size_t(seg) >= s.roads.segs.size() || !walkable(s,seg)) { ticket->state = PedBirthState::CANCELLED; return ticket; }
 const auto &v = s.traffic.vehicles[size_t(vehicle)];
 i.vehicle_generation = v.generation; i.ds = v.ds; i.lane = v.lane; i.conn = v.conn; i.bus = v.bus; i.kerb = v.kerb; i.bus_stop_at = v.bus_stop_at; i.stop_at = v.stop_at;
 capture_origin(s,i); queue_birth(s,ticket); return ticket;
}
PedBirthState take_walker_birth(CityState &s, std::shared_ptr<PedBirthTicket> &ticket, int &ped, uint64_t &generation) {
 if (!ticket) return PedBirthState::CANCELLED;
 if (ticket->state == PedBirthState::PENDING) return ticket->state;
 PedBirthState state = ticket->state;
 if (state == PedBirthState::READY) {
  const auto &t = s.traffic;
  if (ticket->claimed || ticket->ped < 0 || size_t(ticket->ped) >= t.peds.size() || !t.peds[size_t(ticket->ped)].alive ||
   t.peds[size_t(ticket->ped)].generation != ticket->generation || t.peds[size_t(ticket->ped)].mode != PedMode::KERB) state = PedBirthState::CANCELLED;
  else { ped = ticket->ped; generation = ticket->generation; ticket->claimed = true; }
 }
 ticket.reset(); return state;
}
void discard_walker_birth(CityState &s, std::shared_ptr<PedBirthTicket> &ticket) {
 if (!ticket) return;
 if (ticket->state == PedBirthState::PENDING) s.traffic.route_admission.cancel(s,ticket->intent->key());
 if (ticket->state == PedBirthState::READY && !ticket->claimed && ticket->ped >= 0 && size_t(ticket->ped) < s.traffic.peds.size()) {
  const auto &w = s.traffic.peds[size_t(ticket->ped)]; if (w.alive && w.generation == ticket->generation) despawn(s,ticket->ped,false);
 }
 ticket.reset();
}

} // namespace traffic
} // namespace city
