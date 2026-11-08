// The census of who is on foot, for the tests and the captures: how old they are, who walks with whom, where they are, the
// doors the walkers use, and the nearest of each to a point, for a camera to look at.
#include "api/city_probes.h"
#include "core/units.h"
#include "sim/traffic/traffic_system.h"
#include "render/render_world.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <map>

using namespace godot;

namespace city {

namespace {

struct Nearest {
	float d2 = 0;
	Vector3 at, way, road;
	bool any = false;
	void offer(const CityState &s, const Pedestrian &w, const Vector3 &from) {
		const Vector3 p(w.cur.x, w.cur.y, w.cur.z);
		const float d = p.distance_squared_to(from);
		if (any && d >= d2) return;
		any = true;
		d2 = d;
		at = p;
		way = Vector3(w.cur.fx, 0, w.cur.fz);
		Pose middle; // of the street it walks: where a camera looks at it from
		traffic::sidewalk_pose(s, w.seg, 0, w.s, -s.roads.profile_of(w.seg).walk_right, middle);
		road = Vector3(middle.x - p.x, 0, middle.z - p.z).normalized();
	}
};

// Doors whose way to the sidewalk crosses the street: the door on the other side of the middle line from the foot of its way.
int doors_across(const CityState &s) {
	int across = 0;
	for (size_t key = 0; key < s.traffic.doors.size(); key++)
		for (const PedDoor &d : s.traffic.doors[key]) {
			const int seg = int(key / 2), side = int(key % 2);
			Pose foot, middle;
			traffic::sidewalk_pose(s, seg, side, d.s, d.u, foot);
			traffic::sidewalk_pose(s, seg, 0, d.s, -s.roads.profile_of(seg).walk_right, middle);
			const float at_foot = (foot.x - middle.x) * middle.fz - (foot.z - middle.z) * middle.fx;
			const float at_door = (d.x - middle.x) * middle.fz - (d.z - middle.z) * middle.fx;
			across += at_foot * at_door < 0 ? 1 : 0;
		}
	return across;
}

const char *mode_name(PedMode m) {
	switch (m) {
		case PedMode::SIDEWALK: return "sidewalk";
		case PedMode::WAY: return "way";
		case PedMode::WAIT: return "wait";
		case PedMode::LOT: return "lot";
		default: return "follow";
	}
}

// Strangers closer than `touch`, by what the nearer of each pair is doing: alone, leading, following, in a lot.
Dictionary who_touches(const TrafficState &t, float touch) {
	int alone = 0, leading = 0, following = 0, lot = 0;
	for (size_t i = 0; i < t.peds.size(); i++) {
		const Pedestrian &a = t.peds[i];
		if (!a.alive) continue;
		for (size_t j = i + 1; j < t.peds.size(); j++) {
			const Pedestrian &b = t.peds[j];
			if (!b.alive || a.with == int(j) || b.with == int(i)) continue;
			const float dx = a.cur.x - b.cur.x, dz = a.cur.z - b.cur.z;
			if (dx * dx + dz * dz >= touch * touch) continue;
			const auto is = [&](auto f) { return f(a) || f(b); };
			if (is([](const Pedestrian &w) { return w.mode == PedMode::LOT; })) lot++;
			else if (is([](const Pedestrian &w) { return w.with >= 0; })) following++;
			else if (is([](const Pedestrian &w) { return w.mate >= 0; })) leading++;
			else alone++;
		}
	}
	Dictionary d;
	d["alone"] = alone;
	d["leading"] = leading;
	d["following"] = following;
	d["lot"] = lot;
	return d;
}

} // namespace

Array CityProbes::people_clips() const { return render_->people_clips(); }

bool CityProbes::show_people_clip(const String &name) { return render_->show_people_clip(name); }

// The doors the walkers know, and the sidewalks they stand on.
static void census_doors(const CityState &s, Dictionary &d) {
	const TrafficState &t = s.traffic;
	int doors = 0;
	for (const auto &list : t.doors) doors += int(list.size());
	d["doors"] = doors;
	d["doors_across"] = doors_across(s);
	int sidewalks = 0, served = 0;
	for (int seg : t.ped_segs)
		for (int side = 0; side < 2; side++) {
			const Profile &pf = s.roads.profile_of(seg);
			if (!(side == 0 ? pf.has_walk_right : pf.has_walk_left)) continue;
			sidewalks++;
			const size_t key = size_t(seg * 2 + side);
			served += key < t.doors.size() && !t.doors[key].empty() ? 1 : 0;
		}
	d["sidewalks"] = sidewalks;
	d["sidewalks_with_doors"] = served;
	d["doors_to_read"] = int(t.door_todo.size());
	d["came_out"] = t.doors_out;
	d["went_in"] = t.doors_in;
	d["arrived"] = t.ped_arrived;
}

static constexpr const char *ACT_NAMES[int(PedAct::COUNT)] = { "", "talk", "listen", "look", "phone", "headphones", "bag", "crouch", "photo" };

Dictionary CityProbes::people_census(const Vector3 &at, float radius) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	Dictionary d, modes;
	int ages[PED_AGES] = {}, solo = 0, parties = 0, family = 0, child_alone = 0, total = 0, touching = 0;
	float widest = 0, longest = 0;
	int on_foot_line = 0, acting[int(PedAct::COUNT)] = {}, mid_crossing = 0, mid_waiting = 0;
	std::map<std::string, Nearest> spots;
	for (const Pedestrian &w : t.peds) {
		if (!w.alive) continue;
		const float dx = w.cur.x - at.x, dz = w.cur.z - at.z;
		if (radius > 0 && dx * dx + dz * dz > radius * radius) continue;
		total++;
		ages[int(w.age)]++;
		modes[mode_name(w.mode)] = int(modes.get(mode_name(w.mode), 0)) + 1;
		const bool party = w.with >= 0 || w.mate >= 0;
		solo += party ? 0 : 1;
		child_alone += !party && w.age == PedAge::CHILD && !w.company_due && !w.entering ? 1 : 0;
		if (w.age == PedAge::CHILD) spots["child"].offer(s, w, at);
		if (w.age == PedAge::ELDER) spots["elder"].offer(s, w, at);
		if (w.mode == PedMode::WAIT) spots["wait"].offer(s, w, at);
		if (w.way_node >= 0 && s.roads.mid_block(w.way_node)) (w.cross_seg >= 0 ? mid_crossing : mid_waiting)++;
		if (w.act != PedAct::NONE && w.act_share >= 1) {
			acting[int(w.act)]++;
			spots[ACT_NAMES[int(w.act)]].offer(s, w, at);
		}
		if (w.mode == PedMode::LOT) {
			spots[w.entering ? "going_in" : "coming_out"].offer(s, w, at);
			longest = std::max(longest, w.llen);
			on_foot_line += w.entering && w.wi == 0 ? 1 : 0;
		}
		if (w.with < 0) continue;
		const Pedestrian &l = t.peds[size_t(w.with)];
		const bool child = w.age == PedAge::CHILD || l.age == PedAge::CHILD;
		parties++;
		family += child ? 1 : 0;
		const float apart = std::hypot(w.cur.x - l.cur.x, w.cur.z - l.cur.z);
		widest = std::max(widest, apart);
		touching += apart < 2 * s.catalog.traffic.ped_body_m ? 1 : 0;
		spots[child ? "family" : "couple"].offer(s, w, at);
	}
	Dictionary acts;
	for (int a = 1; a < int(PedAct::COUNT); a++) acts[ACT_NAMES[a]] = acting[a];
	d["acts"] = acts;
	d["mid_crossing"] = mid_crossing;
	d["mid_waiting"] = mid_waiting;
	d["total"] = total;
	d["adults"] = ages[int(PedAge::ADULT)];
	d["children"] = ages[int(PedAge::CHILD)];
	d["elders"] = ages[int(PedAge::ELDER)];
	d["solo"] = solo;
	d["parties"] = parties;
	d["families"] = family;
	d["children_alone"] = child_alone;
	d["party_gap_m"] = widest;
	d["party_touching"] = touching;
	d["touching_by"] = who_touches(t, 2 * s.catalog.traffic.ped_body_m);
	d["modes"] = modes;
	d["lot_way_longest_m"] = longest;
	d["going_to_the_foot"] = on_foot_line;
	census_doors(s, d);
	d["wanted"] = traffic::people_wanted(s, traffic::people_now(s.catalog.traffic, float(s.clock.hour())));
	Dictionary where;
	for (const auto &e : spots) {
		Array a;
		a.push_back(e.second.at);
		a.push_back(e.second.way);
		a.push_back(e.second.road);
		where[e.first.c_str()] = a;
	}
	d["spots"] = where;
	return d;
}

void CityProbes::set_city_hour(float hour) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->clock.set_hour(double(hour));
}

} // namespace city
