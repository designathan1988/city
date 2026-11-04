// The Street tab as the script API: previews under the shared lock, commands through run_edit (sim/commands). A position is a
// ground point; results are Dictionaries with `ok`, `error` and `note` (strings.csv keys), so the tools show the core's words.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

namespace {
Vector3 on_ground(const CityState &s, double x, double z) { return Vector3(float(x), s.terrain.height_at(float(x), float(z)), float(z)); }

std::vector<float> flat(const PackedVector3Array &pts) {
	std::vector<float> out;
	for (int i = 0; i < pts.size(); i++) {
		out.push_back(pts[i].x);
		out.push_back(pts[i].z);
	}
	return out;
}
} // namespace

Dictionary CityCore::preview_item(const Vector3 &p, int kind, float reach) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready || kind < 0 || kind >= ITEM_KINDS) return d;
	const ItemSnap snap = snap_item(*state_, ItemKind(kind), p.x, p.z, reach);
	d["ok"] = snap.ok;
	d["error"] = String(snap.reason.c_str());
	d["position"] = on_ground(*state_, snap.x, snap.z);
	return d;
}

Dictionary CityCore::place_item(const Vector3 &p, int kind, int sign, const String &text, float reach) {
	return run_edit(*state_, "cmd.place.streetItem", [&](CityState &s) {
		return city::place_item(s, ItemKind(kind), SignType(sign), text.utf8().get_data(), p.x, p.z, reach);
	});
}

Dictionary CityCore::place_item_row(const Vector3 &a, const Vector3 &b, int kind, float every_m) {
	return run_edit(*state_, "cmd.place.streetItem", [&](CityState &s) { return city::place_item_row(s, ItemKind(kind), a.x, a.z, b.x, b.z, every_m); });
}

Dictionary CityCore::preview_barrier(const PackedVector3Array &pts, const Vector3 &cursor, int kind) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready) return d;
	double x = cursor.x, z = cursor.z;
	snap_barrier_point(*state_, BarrierKind(kind), x, z);
	std::vector<float> run = flat(pts);
	run.push_back(float(x));
	run.push_back(float(z));
	const std::string why = barrier_problem(*state_, BarrierKind(kind), run);
	d["position"] = on_ground(*state_, x, z);
	d["ok"] = why != "hint.barrier.road";
	d["error"] = String(why.c_str());
	return d;
}

Dictionary CityCore::build_barrier(const PackedVector3Array &pts, int kind) {
	return run_edit(*state_, "cmd.build.wall", [&](CityState &s) { return city::build_barrier(s, BarrierKind(kind), flat(pts)); });
}

Dictionary CityCore::remove_street(const Vector3 &p, float reach) {
	return run_edit(*state_, "cmd.demolish.item", [&](CityState &s) {
		EditResult r;
		r.ok = remove_street_at(s, p.x, p.z, reach);
		if (!r.ok) r.error = "hint.barrier.nothing";
		return r;
	});
}

Dictionary CityCore::preview_pole_run(const Vector3 &a, const Vector3 &b, float reach, int lamps) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	if (!state_->ready) return d;
	const PoleRunPlan plan = plan_pole_run(*state_, a.x, a.z, b.x, b.z, reach, LampMode(lamps));
	Array poles;
	for (const PlannedPole &p : plan.poles) {
		Dictionary one;
		one["position"] = on_ground(*state_, p.x, p.z);
		one["standing"] = p.existing >= 0;
		one["lamp"] = p.lamp;
		poles.push_back(one);
	}
	d["poles"] = poles;
	d["error"] = String(plan.refused.c_str());
	d["ok"] = plan.refused.empty() && !plan.poles.empty();
	d["from"] = on_ground(*state_, plan.from_x, plan.from_z);
	d["to"] = on_ground(*state_, plan.to_x, plan.to_z);
	return d;
}

Dictionary CityCore::build_pole_run(const Vector3 &a, const Vector3 &b, float reach, int lamps) {
	return run_edit(*state_, "cmd.place.poleRun", [&](CityState &s) { return city::build_pole_run(s, a.x, a.z, b.x, b.z, reach, LampMode(lamps)); });
}

void CityCore::show_pole_preview(const Array &poles) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::vector<Vector3> at;
	std::vector<bool> lamp, standing;
	for (int i = 0; i < poles.size(); i++) {
		const Dictionary p = poles[i];
		at.push_back(p["position"]);
		lamp.push_back(p["lamp"]);
		standing.push_back(p["standing"]);
	}
	render_.set_pole_preview(*state_, at, lamp, standing);
}

void CityCore::set_road_furniture(int level) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->new_road_flags = uint16_t(level << SEG_FURNITURE_SHIFT) & SEG_FURNITURE_MASK;
}

int CityCore::road_furniture() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return (state_->new_road_flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT;
}

Array CityCore::sign_cells() { return render_.take_sign_cells(); }

// What stands along the streets, kit and player's alike, for a comparison per kilometre: the pieces of each FurnitureKind (`kinds`,
// in its order), the street length and the poles that carry a light.
Dictionary CityCore::furniture_census() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	PackedInt32Array kinds;
	kinds.resize(FURNITURE_KINDS);
	kinds.fill(0);
	double metres = 0;
	int lit = 0;
	std::vector<FurnitureSpot> spots;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		metres += s.roads.segs[size_t(sid)].len;
		spots.clear();
		traffic::street_furniture_of(s, sid, FURNITURE_ALL, spots);
		for (const FurnitureSpot &p : spots) {
			kinds[int(p.kind)] += 1;
			lit += p.kind == FurnitureKind::LAMP && p.lit ? 1 : 0;
		}
	}
	Dictionary d;
	d["kinds"] = kinds;
	d["street_m"] = metres;
	d["lit"] = lit;
	return d;
}

Dictionary CityCore::street_counts() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const StreetWorks &w = state_->street;
	Dictionary d;
	d["items"] = w.items.alive;
	d["poles"] = w.poles.alive;
	d["spans"] = w.spans.alive;
	d["barriers"] = w.barriers.alive;
	d["cuts"] = w.cuts.alive;
	d["instances"] = render_.street_instances();
	d["sign_cells"] = render_.sign_cells();
	return d;
}

} // namespace city
