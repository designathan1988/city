// Building record storage.
#include "sim/buildings/building_store.h"

#include "sim/buildings/growth.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/furniture_kept.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint8_t ZONE_RES_LOW = 5, ZONE_RES_HIGH = 6, ZONE_IND_LOW = 7, ZONE_IND_HIGH = 8;
} // namespace

void BuildingStore::init(ChangeLog *l, const OccupancyDef *occupancy) {
	log = l;
	occupancy_ = occupancy;
	clear();
}

void BuildingStore::clear() {
	items.clear();
	free_ids.clear();
	hash.clear();
	hash.cell = BUILDING_HASH_CELL;
	lots.clear();
	lots.cell = BUILDING_HASH_CELL;
	count = 0;
	residents = jobs = 0;
	sites_due.clear();
}

int BuildingStore::add(const Building &b) {
	int id;
	id = free_ids.next(int(items.size()));
	items.resize(size_t(free_ids.claim(id, int(items.size()))));
	place(id, b);
	return id;
}

void BuildingStore::remove(int id) {
	Building &b = items[id];
	if (!b.alive) return;
	mark(id);
	hash.remove_aabb(id, b.x, b.z, b.x, b.z);
	float x0, z0, x1, z1;
	bounds(b, x0, z0, x1, z1);
	lots.remove_aabb(id, x0, z0, x1, z1);
	tally(b, -1);
	b.alive = false;
	b.body = nullptr; // a dead slot holds no version: what the history keeps of it, the history holds
	free_ids.release(id);
	count--;
}

bool BuildingRecords::covers(const Building &b, float x, float z, float pad) {
	float c = std::cos(b.rot), sn = std::sin(b.rot), dx = x - b.x, dz = z - b.z;
	return std::fabs(dx * c - dz * sn) < b.w / 2 + pad && std::fabs(dx * sn + dz * c) < b.d / 2 + pad;
}

void BuildingRecords::bounds(const Building &b, float &minx, float &minz, float &maxx, float &maxz) {
	const float c = std::fabs(std::cos(b.rot)), sn = std::fabs(std::sin(b.rot));
	const float hx = b.w / 2 * c + b.d / 2 * sn, hz = b.w / 2 * sn + b.d / 2 * c; // the turned rectangle's half box (covers' frame)
	minx = b.x - hx;
	maxx = b.x + hx;
	minz = b.z - hz;
	maxz = b.z + hz;
}

void BuildingStore::put(int id, const Building &b, bool evict) {
	if (id < int(items.size()) && items[id].alive) {
		Building moved = items[id];
		remove(id);
		const int to = int(items.size()); // the record moves to a new slot at the end; `id` stays taken for `b`
		items.resize(size_t(free_ids.claim(to, to)));
		place(to, moved);
	}
	items.resize(size_t(free_ids.claim(id, int(items.size()))));
	if (evict) {
		std::vector<int> doomed;
		float r = std::max(b.w, b.d);
		query(b.x - r, b.z - r, b.x + r, b.z + r, [&](int o) {
			const Building &ob = items[o];
			if (covers(b, ob.x, ob.z, 0.f) || covers(ob, b.x, b.z, 0.f)) doomed.push_back(o);
		});
		for (int o : doomed) remove(o);
	}
	place(id, b);
}

void BuildingStore::place(int id, const Building &b) {
	items[id] = b;
	items[id].alive = true;
	hash.insert_aabb(id, b.x, b.z, b.x, b.z);
	float x0, z0, x1, z1;
	bounds(b, x0, z0, x1, z1);
	lots.insert_aabb(id, x0, z0, x1, z1);
	count++;
	tally(b, 1);
	mark(id);
}

void BuildingStore::tally(const Building &b, int sign) {
	if (!occupancy_) return;
	const OccupancyDef &o = *occupancy_;
	const float per[4] = { o.m2_per_resident, o.m2_per_job_commercial, o.m2_per_job_industrial, o.m2_per_person_mixed };
	int r, j;
	occupancy(b, o.floor_share, per, r, j);
	residents += sign * r;
	jobs += sign * j;
}

void BuildingStore::mark(int id) {
	sites_due.push_back(id);
	changed(id);
}

void BuildingStore::set_edges(int id, const b40::LotEdges &edges) {
	Building &b = items[size_t(id)];
	// a body made from the seed, under edges it was made to, is made again to the new ones: the storeys are the new body's (a record
	// whose edges were never read keeps the storeys it came with: they stand under the edges read now)
	const bool remade = b.edges_read && !b.body && b.floors > 0;
	b.edges = edges;
	b.edges_read = true;
	if (remade) {
		tally(b, -1);
		b.floors = uint16_t(body_floors(designs, b));
		tally(b, 1);
	}
	changed(id);
}

void BuildingStore::changed(int id) {
	const Building &b = items[size_t(id)];
	const float r = reach(b);
	if (furniture) furniture->forget_meeting({ b.x - r, b.z - r, b.x + r, b.z + r }); // under the exclusive lock, before any reader places again
	if (!log) return;
	log->lots_changed.push_back(id);
	mark_into(id, log->building_chunks);
	log->lot_boxes.insert(log->lot_boxes.end(), { b.x - r, b.z - r, b.x + r, b.z + r });
	log->lot_fronts.insert(log->lot_fronts.end(), { b.x - r, b.z - r, b.x + r, b.z + r });
}

void BuildingStore::mark_look(int id) const {
	if (log) mark_into(id, log->view_chunks);
}

float BuildingStore::reach(const Building &b) { return 0.5f * std::hypot(b.w, b.d); }

void BuildingStore::mark_into(int id, std::vector<int64_t> &out) const {
	const Building &b = items[size_t(id)];
	const float r = reach(b);
	for_chunks(b.x - r, b.z - r, b.x + r, b.z + r, [&](int64_t k) { out.push_back(k); });
}

void BuildingStore::occupancy(const Building &b, float floor_share, const float per_m2[4], int &residents, int &jobs) {
	float area = b.w * b.d * floor_share * b.floors;
	residents = jobs = 0;
	int base = b.zone == ZONE_RES_LOW || b.zone == ZONE_RES_HIGH ? 1 : b.zone == ZONE_IND_LOW || b.zone == ZONE_IND_HIGH ? 2 : b.zone;
	switch (base) {
		case 1: residents = int(std::lround(area / per_m2[0])); break;
		case 2: jobs = int(std::lround(area / per_m2[1])); break;
		case 3: jobs = int(std::lround(area / per_m2[2])); break;
		case 4: residents = jobs = int(std::lround(area / per_m2[3])); break;
		default: break;
	}
}

} // namespace city
