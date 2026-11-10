// The bays near the camera (parking_state.h): the kerbs of the streets that park and the lots, read a few a step as the
// camera moves and dropped behind it, with the scenery cars that stand in them (seeded by where the bay is, so a bay
// holds the same car whenever it comes back into view). The way out of a lot is asked only of a bay a car is sent to.
#include "sim/traffic/traffic_system.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/lot_access.h"
#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float ROW_CELL_M = 0.4f; // a bay's seed: where it is in steps of this (the same car whatever the float rounding)
constexpr uint32_t ROW_SALT = 0x9e3779b9u; // 2^32 / the golden ratio (Knuth's multiplicative hash): spreads a cell's row
constexpr uint8_t FILED_NONE = 0, FILED_FREE = 1, FILED_ROW = 2;

int take_bay(ParkingState &p) {
	int b;
	if (!p.free_bays.empty()) {
		b = p.free_bays.back();
		p.free_bays.pop_back();
	} else {
		b = int(p.bays.size());
		p.bays.emplace_back();
	}
	p.bays[size_t(b)] = Bay();
	p.bays[size_t(b)].alive = true;
	return b;
}

// A scenery car in the bay, by its seed: the share of bays that hold one, of a body the stall holds whole.
void stand(CityState &s, int b) {
	const TrafficParams &tp = s.catalog.traffic;
	ParkingState &p = s.traffic.park;
	Bay &bay = p.bays[size_t(b)];
	if (bay.dock || bay.vacated || bay.holder >= 0 || bay.has_row) return;
	const int cx = int(std::lround(bay.at.x / ROW_CELL_M)), cz = int(std::lround(bay.at.y / ROW_CELL_M));
	Rng r(hash_combine(uint32_t(cx) ^ ROW_SALT, uint32_t(cz)));
	if (r.f() >= tp.kerb_parked_share) return;
	int fits[int(VehicleType::MOTO)], n = 0;
	for (int t = 0; t < int(VehicleType::MOTO); t++)
		if (tp.classes[t].length * 0.5f <= bay.room) fits[n++] = t;
	if (n == 0) return;
	bay.row_type = uint8_t(fits[int(r.next() % uint32_t(n))]);
	bay.row_paint = uint8_t(r.next() % PAINTS);
	bay.row_id = p.next_row++;
	add_row(p, b);
}

PlaceKind place_of(const CityState &s, const Building &rec) {
	if (rec.fn != FN_NONE) return rec.fn == FN_SUPERMARKET || rec.fn == FN_MALL || rec.fn == FN_GAS_STATION ? PlaceKind::SHOP : PlaceKind::CIVIC;
	const int base = s.catalog.zone(rec.zone).base;
	return base == BASE_COMMERCIAL ? PlaceKind::SHOP : base == BASE_INDUSTRIAL ? PlaceKind::WORK : PlaceKind::HOME;
}

void read_lot(CityState &s, int id) {
	ParkingState &p = s.traffic.park;
	ParkSource &src = p.lots[id];
	const Building &rec = s.buildings.items[size_t(id)];
	if (!rec.alive) return;
	LotAccess acc;
	lot_bays(*b40::record_elements(s.buildings.designs, rec), b40::lot_frame(rec), acc);
	const PlaceKind kind = place_of(s, rec);
	for (const LotBay &lb : acc.bays) {
		const int b = take_bay(p);
		Bay &bay = p.bays[size_t(b)];
		bay.source = id;
		bay.dock = lb.dock;
		bay.kind = kind;
		bay.at = lb.at;
		bay.open = lb.open;
		bay.room = lb.d * 0.5f;
		bay.y = s.terrain.height_at(lb.at.x, lb.at.y);
		src.bays.push_back(b);
		stand(s, b);
		refile_bay(s, b, false);
	}
}

void read_kerb(CityState &s, int seg) {
	ParkingState &p = s.traffic.park;
	ParkSource &src = p.kerbs[seg];
	if (!s.roads.segs[size_t(seg)].alive || !parks_along(s.roads, seg)) return;
	std::vector<ArcSpan> gates[2];
	street_gates(s.roads, s.buildings, seg, gates);
	KerbLayout k;
	kerb_layout(s.roads, seg, gates, k);
	const LaneGraph &g = s.traffic.g;
	for (const KerbStretch &st : k.bays) {
		// a parked car faces the way the traffic beside it goes (CTB art. 48)
		const int dir = st.side == s.catalog.traffic.drive_side ? 0 : 1, ds = seg * 2 + (g.ds[size_t(seg) * 2 + dir].ok ? dir : 1 - dir);
		Pose at;
		seg_point(s, seg, float((st.v0 + st.v1) * 0.5), (st.u_kerb + st.u_inner) * 0.5f, at);
		const float way = ds == seg * 2 ? 1.f : -1.f;
		const int b = take_bay(p);
		Bay &bay = p.bays[size_t(b)];
		bay.kerb = true;
		bay.source = seg;
		bay.at = V2(at.x, at.z);
		bay.open = V2(-at.fx * way, -at.fz * way);
		bay.y = at.y;
		bay.room = float(st.v1 - st.v0) * 0.5f;
		src.bays.push_back(b);
		stand(s, b);
	}
}

// Takes a source's bays away, unless (`force` off) one holds a car on its way in or out. True when they went.
bool drop_source(CityState &s, ParkSource &src, bool force) {
	ParkingState &p = s.traffic.park;
	if (!force)
		for (int b : src.bays) {
			const int holder = p.bays[size_t(b)].holder;
			if (holder >= 0 && !lot_car_at_rest(s, holder)) return false;
		}
	for (int b : src.bays) {
		if (p.bays[size_t(b)].holder >= 0) lot_car_gone(s, p.bays[size_t(b)].holder);
		remove_row(p, b);
		refile_bay(s, b, true);
		p.bays[size_t(b)].alive = false;
		p.bays[size_t(b)].via = {};
		p.free_bays.push_back(b);
	}
	src.bays.clear();
	return true;
}

V2 seg_mid(const CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	return V2((g.minx + g.maxx) * 0.5f, (g.minz + g.maxz) * 0.5f);
}

V2 lot_mid(const CityState &s, int id) {
	const Building &b = s.buildings.items[size_t(id)];
	return V2(b.x, b.z);
}

template <typename W>
void drop_far(CityState &s, std::unordered_map<int, ParkSource> &sources, const V2 &eye, float out, W &&where) {
	for (auto it = sources.begin(); it != sources.end();) {
		if ((where(it->first) - eye).len2() > out * out && drop_source(s, it->second, false)) it = sources.erase(it);
		else ++it;
	}
}

// What is near the camera and not read yet, the nearest last (taken from the back); what fell behind goes.
void look_round(CityState &s, float cx, float cz) {
	const TrafficParams &tp = s.catalog.traffic;
	ParkingState &p = s.traffic.park;
	const float r = tp.park_window_m;
	const V2 eye(cx, cz);
	drop_far(s, p.lots, eye, tp.park_window_out_m, [&](int id) { return lot_mid(s, id); });
	drop_far(s, p.kerbs, eye, tp.park_window_out_m, [&](int seg) { return seg_mid(s, seg); });
	p.lots_wanted.clear();
	p.kerbs_wanted.clear();
	s.buildings.query(cx - r, cz - r, cx + r, cz + r, [&](int id) {
		if (p.lots.count(id) == 0 && (lot_mid(s, id) - eye).len2() < r * r) p.lots_wanted.push_back(id);
	});
	s.roads.for_segments_near(cx - r, cz - r, cx + r, cz + r, [&](int seg) {
		if (p.kerbs.count(seg) == 0 && parks_along(s.roads, seg) && (seg_mid(s, seg) - eye).len2() < r * r) p.kerbs_wanted.push_back(seg);
	});
	std::sort(p.lots_wanted.begin(), p.lots_wanted.end(), [&](int a, int b) {
		const float da = (lot_mid(s, a) - eye).len2(), db = (lot_mid(s, b) - eye).len2();
		return da != db ? da > db : a < b;
	});
	std::sort(p.kerbs_wanted.begin(), p.kerbs_wanted.end(), [&](int a, int b) {
		const float da = (seg_mid(s, a) - eye).len2(), db = (seg_mid(s, b) - eye).len2();
		return da != db ? da > db : a < b;
	});
}

} // namespace

void read_lots_with_docks(CityState &s) {
	ParkingState &p = s.traffic.park;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive || p.lots.count(id)) continue;
		const std::shared_ptr<const std::vector<b40::LotElement>> elements = b40::record_elements(s.buildings.designs, rec);
		for (const b40::LotElement &e : *elements)
			if (e.kind == b40::EL_DOCK) {
				read_lot(s, id);
				break;
			}
	}
}

void add_row(ParkingState &p, int b) {
	Bay &bay = p.bays[size_t(b)];
	bay.has_row = true;
	bay.row_at = int(p.rows.size());
	p.rows.push_back(b);
	p.rows_dirty = true;
}

void remove_row(ParkingState &p, int b) {
	Bay &bay = p.bays[size_t(b)];
	if (!bay.has_row) return;
	bay.has_row = false;
	const int last = p.rows.back();
	p.rows[size_t(bay.row_at)] = last;
	p.bays[size_t(last)].row_at = bay.row_at;
	p.rows.pop_back();
	bay.row_at = -1;
	p.rows_dirty = true;
}

void refile_bay(CityState &s, int b, bool away) {
	ParkingState &p = s.traffic.park;
	Bay &bay = p.bays[size_t(b)];
	if (bay.kerb || bay.dock) return;
	const int k = int(bay.kind);
	if (bay.filed != FILED_NONE) {
		std::vector<int> &list = bay.filed == FILED_ROW ? p.row_of[k] : p.free_of[k];
		const int last = list.back();
		list[size_t(bay.slot)] = last;
		p.bays[size_t(last)].slot = bay.slot;
		list.pop_back();
		bay.filed = FILED_NONE;
	}
	if (away || bay.holder >= 0 || (bay.way_read && !bay.reached)) return;
	std::vector<int> &list = bay.has_row ? p.row_of[k] : p.free_of[k];
	bay.filed = bay.has_row ? FILED_ROW : FILED_FREE;
	bay.slot = int(list.size());
	list.push_back(b);
}

void step_parking_sources(CityState &s) {
	const TrafficParams &tp = s.catalog.traffic;
	ParkingState &p = s.traffic.park;
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float dx = cx - p.win_x, dz = cz - p.win_z;
	if (!p.looked || p.relook || dx * dx + dz * dz > tp.park_window_step_m * tp.park_window_step_m) {
		p.looked = true;
		p.relook = false;
		p.win_x = cx;
		p.win_z = cz;
		look_round(s, cx, cz);
	}
	for (int k = 0; k < int(tp.park_reads_per_step); k++) {
		if (!p.lots_wanted.empty()) {
			const int id = p.lots_wanted.back();
			p.lots_wanted.pop_back();
			if (p.lots.count(id) == 0) read_lot(s, id);
		} else if (!p.kerbs_wanted.empty()) {
			const int seg = p.kerbs_wanted.back();
			p.kerbs_wanted.pop_back();
			if (p.kerbs.count(seg) == 0) read_kerb(s, seg);
			k += int(tp.park_kerb_reads); // a street reads the lots along it
		} else break;
	}
}

void forget_parking_seg(CityState &s, int seg) {
	ParkingState &p = s.traffic.park;
	auto it = p.kerbs.find(seg);
	if (it != p.kerbs.end()) {
		drop_source(s, it->second, true);
		p.kerbs.erase(it);
	}
	p.relook = true;
}

void forget_parking_lot(CityState &s, int id) {
	ParkingState &p = s.traffic.park;
	p.yards.erase(id);
	if (p.planner_building == id) p.planner.reset();
	auto it = p.lots.find(id);
	if (it != p.lots.end()) {
		drop_source(s, it->second, true);
		p.lots.erase(it);
	}
	p.relook = true;
	if (size_t(id) >= s.buildings.items.size()) return;
	const Building &b = s.buildings.items[size_t(id)];
	const float reach = std::max(b.w, b.d) + s.catalog.traffic.kerb_gate_reach_m;
	std::vector<int> streets;
	s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int seg) {
		if (parks_along(s.roads, seg)) streets.push_back(seg);
	});
	// its car gates move the street's bays and their paint
	std::lock_guard<std::mutex> lk(s.log.mutex);
	for (int seg : streets) {
		auto k = p.kerbs.find(seg);
		if (k != p.kerbs.end()) {
			drop_source(s, k->second, true);
			p.kerbs.erase(k);
		}
		s.roads.mark_segment(seg);
	}
}

void publish_parked(CityState &s, PoseFrame &f) {
	ParkingState &p = s.traffic.park;
	if (p.rows_dirty) {
		const float lift = s.catalog.traffic.body_lift_m;
		auto out = std::make_shared<std::vector<ParkedRecord>>();
		out->reserve(p.rows.size());
		for (int b : p.rows) {
			const Bay &bay = p.bays[size_t(b)];
			ParkedRecord rec;
			rec.at.x = bay.at.x;
			rec.at.z = bay.at.y;
			rec.at.y = bay.y + lift;
			rec.at.fx = -bay.open.x;
			rec.at.fz = -bay.open.y;
			rec.id = bay.row_id;
			rec.look = uint32_t(bay.row_type) | (uint32_t(bay.row_paint) << LOOK_PAINT_SHIFT);
			out->push_back(rec);
		}
		p.published = std::move(out);
		p.rows_dirty = false;
	}
	f.parked = p.published;
}

} // namespace traffic
} // namespace city
