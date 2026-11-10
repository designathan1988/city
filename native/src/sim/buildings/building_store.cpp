// Building record storage.
#include "sim/buildings/building_store.h"

#include "sim/catalog/catalog.h"

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
	looks.clear();
	items.clear();
	free_ids.clear();
	hash.clear();
	hash.cell = BUILDING_HASH_CELL;
	count = 0;
	residents = jobs = 0;
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
	tally(b, -1);
	b.alive = false;
	b.body = nullptr; // a dead slot holds no version: what the history keeps of it, the history holds
	free_ids.release(id);
	count--;
}

bool BuildingStore::covers(const Building &b, float x, float z, float pad) {
	float c = std::cos(b.rot), sn = std::sin(b.rot), dx = x - b.x, dz = z - b.z;
	return std::fabs(dx * c - dz * sn) < b.w / 2 + pad && std::fabs(dx * sn + dz * c) < b.d / 2 + pad;
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

void BuildingStore::mark(int id) const {
	if (!log) return;
	log->lots_changed.push_back(id);
	mark_into(id, log->building_chunks);
}

void BuildingStore::mark_look(int id) const {
	if (log) mark_into(id, log->view_chunks);
}

void BuildingStore::mark_into(int id, std::vector<int64_t> &out) const {
	const Building &b = items[id];
	float r = std::max(b.w, b.d) * 0.5f;
	for (int j = int(std::floor((b.z - r) / CHUNK)); j <= int(std::floor((b.z + r) / CHUNK)); j++)
		for (int i = int(std::floor((b.x - r) / CHUNK)); i <= int(std::floor((b.x + r) / CHUNK)); i++) out.push_back(cell_key(i, j));
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
