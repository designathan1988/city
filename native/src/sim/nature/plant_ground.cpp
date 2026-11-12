// Plant bases keep the original 80 m candidate selection and ecology window arithmetic. Indices only reject
// impossible exclusion candidates; the original closest-curve and rotated-lot predicates make the final decision.
#include "sim/nature/plant_ground.h"
#include "sim/state/city_state.h"
#include "core/job_pool.h"
#include "core/retained_memory.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace city {
namespace {
constexpr float LIST_MARGIN_M = 80.f, MARGIN_BOX_SCALE = 2.f;
constexpr double ROUND_ULPS = 64, BYTES_PER_MB = 1024 * 1024;
uint32_t bits_of(float x) { uint32_t bits; std::memcpy(&bits, &x, sizeof bits); return bits; }
double round_pad(double scale) { return (1 + scale) * std::numeric_limits<float>::epsilon() * ROUND_ULPS; }
Bounds2 conservative_box(Bounds2 box) {
	if (std::isfinite(box.minx) && std::isfinite(box.minz) && std::isfinite(box.maxx) && std::isfinite(box.maxz)
		&& box.minx <= box.maxx && box.minz <= box.maxz) return box;
	const double unbounded = std::numeric_limits<double>::infinity();
	return {-unbounded, -unbounded, unbounded, unbounded};
}
std::shared_ptr<const PlantOccupancy> prepare_occupancy(const CityState &s, float x0, float z0) {
	auto out = std::make_shared<PlantOccupancy>();
	s.roads.for_segments_near(x0 - LIST_MARGIN_M, z0 - LIST_MARGIN_M, x0 + CHUNK + LIST_MARGIN_M, z0 + CHUNK + LIST_MARGIN_M, [&](int sid) {
		const RoadSegment &source = s.roads.segs[size_t(sid)];
		const Quad &q = source.q;
		const float half = s.catalog.profile(source.profile).half();
		const int index = int(out->roads.size()); out->roads.push_back({q, half});
		const double pad = half + round_pad(std::max({std::abs(q.x0), std::abs(q.cx), std::abs(q.x1), std::abs(q.z0), std::abs(q.cz), std::abs(q.z1), double(half)}));
		out->road_index.insert(index, conservative_box({std::min({q.x0, q.cx, q.x1}) - pad, std::min({q.z0, q.cz, q.z1}) - pad,
			std::max({q.x0, q.cx, q.x1}) + pad, std::max({q.z0, q.cz, q.z1}) + pad}));
	});
	s.buildings.query(x0 - LIST_MARGIN_M, z0 - LIST_MARGIN_M, x0 + CHUNK + LIST_MARGIN_M, z0 + CHUNK + LIST_MARGIN_M, [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		const float c = std::cos(b.rot), sn = std::sin(b.rot);
		const int index = int(out->rectangles.size());
		out->lots.push_back(id); out->rectangles.push_back({b.x, b.z, c, sn, b.w, b.d});
		const double den = double(c) * c + double(sn) * sn;
		const double pad = round_pad(std::max({std::abs(double(b.x)), std::abs(double(b.z)), double(b.w), double(b.d)}));
		const double hx = (std::abs(double(c)) * b.w + std::abs(double(sn)) * b.d) / (2 * den) + pad;
		const double hz = (std::abs(double(sn)) * b.w + std::abs(double(c)) * b.d) / (2 * den) + pad;
		out->lot_index.insert(index, conservative_box({b.x - hx, b.z - hz, b.x + hx, b.z + hz}));
	});
	return out;
}
} // namespace

bool PlantOccupancy::near_road(float x, float z, float margin) const {
	bool found = false;
	const double pad = std::max(0.f, margin) + round_pad(std::max({std::abs(double(x)), std::abs(double(z)), std::abs(double(margin))}));
	const Bounds2 query{x - pad, z - pad, x + pad, z + pad};
	road_index.query_if([&](const Bounds2 &box) { return !found && box.overlaps(query); }, [&](int i) {
		const Road &road = roads[size_t(i)]; double t, distance;
		closest_t(road.curve, x, z, t, distance);
		if (distance < road.half + margin) found = true;
	});
	return found;
}
bool PlantOccupancy::in_lot(float x, float z, float margin) const {
	bool found = false;
	const double pad = std::max(0.f, margin) * MARGIN_BOX_SCALE + round_pad(std::max({std::abs(double(x)), std::abs(double(z)), std::abs(double(margin))}));
	const Bounds2 query{x - pad, z - pad, x + pad, z + pad};
	lot_index.query_if([&](const Bounds2 &box) { return !found && box.overlaps(query); }, [&](int i) {
		const Lot &lot = rectangles[size_t(i)];
		const float dx = x - lot.x, dz = z - lot.z;
		const float lx = dx * lot.c - dz * lot.s, lz = dx * lot.s + dz * lot.c;
		if (std::fabs(lx) < lot.w * .5f + margin && std::fabs(lz) < lot.d * .5f + margin) found = true;
	});
	return found;
}
size_t PlantOccupancy::retained_bytes() const {
	return sizeof(*this) + roads.capacity() * sizeof(Road) + rectangles.capacity() * sizeof(Lot) + lots.capacity() * sizeof(int)
		+ road_index.retained_bytes() + lot_index.retained_bytes() + RetainedMemory::shared_control_bytes();
}

std::shared_ptr<const PlantGround> PlantGroundCache::get(const CityState &s, int64_t key) const {
	const float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	const float cx = x0 + CHUNK * .5f, cz = z0 + CHUNK * .5f;
	const auto &def = s.catalog.nature.ecology;
	// Full ecology sample window: water distance can depend on every wet sample in this finite window.
	const float halo = float(def.mean_cells + 1) * def.grid_m;
	const float span = std::ceil(CHUNK / def.grid_m) * def.grid_m;
	const float reach = span + 2 * halo;
	const float ex0 = x0 - halo - s.terrain.spacing, ez0 = z0 - halo - s.terrain.spacing;
	const float ex1 = x0 + span + halo + s.terrain.spacing, ez1 = z0 + span + halo + s.terrain.spacing;
	const Revisions revisions{
		s.roads.ground_regions.stamp(x0 - LIST_MARGIN_M, z0 - LIST_MARGIN_M, x0 + CHUNK + LIST_MARGIN_M, z0 + CHUNK + LIST_MARGIN_M),
		s.buildings.ground_regions.stamp(x0 - LIST_MARGIN_M, z0 - LIST_MARGIN_M, x0 + CHUNK + LIST_MARGIN_M, z0 + CHUNK + LIST_MARGIN_M),
		s.terrain.base_stamp(cx, cz, std::max(CHUNK, reach) * .5f + def.grid_m + s.terrain.spacing),
		s.terrain.water_stamp(ex0, ez0, ex1, ez1), s.nature.ground.biome_stamp(ex0, ez0, ex1, ez1),
		bits_of(s.terrain.water_level), uint64_t(int64_t(s.nature.nature.region)), s.nature.nature.seed, uint64_t(s.nature.ground.any_cover())};
	std::shared_ptr<const PlantGround> previous;
	Revisions old{};
	{
		std::lock_guard<std::mutex> lk(mutex_);
		const auto found = entries_.find(key);
		if (found != entries_.end()) {
			recent_.splice(recent_.end(), recent_, found->second.recent);
			if (found->second.revisions == revisions) return found->second.value;
			previous = found->second.value; old = found->second.revisions;
		}
	}
	auto made = std::make_shared<PlantGround>();
	if (previous && old[0] == revisions[0] && old[1] == revisions[1]) made->occupancy = previous->occupancy;
	else made->occupancy = prepare_occupancy(s, x0, z0);
	if (previous && std::equal(old.begin() + 2, old.end(), revisions.begin() + 2)) made->eco = previous->eco;
	else if (s.nature.nature.region >= 0 || s.nature.ground.any_cover())
		made->eco = std::make_shared<const EcologyField>(compute_ecology(s.terrain, s.nature.ground, s.nature.nature, def, x0, z0, CHUNK));
	else made->eco = std::make_shared<const EcologyField>();
	const size_t bytes = sizeof(PlantGround) + RetainedMemory::shared_control_bytes() + made->occupancy->retained_bytes()
		+ sizeof(EcologyField) + made->eco->at.capacity() * sizeof(Community) + RetainedMemory::shared_control_bytes()
		+ sizeof(Entry) + 2 * sizeof(key) + 9 * sizeof(void *);
	const size_t limit = size_t(std::max(0.0, double(s.catalog.tune.sim_plant_ground_cache_mb)) * BYTES_PER_MB);
	if (bytes > limit || JobPool::cancelled()) return made;
	std::vector<std::shared_ptr<const PlantGround>> released;
	{
		std::lock_guard<std::mutex> lk(mutex_);
		const auto found = entries_.find(key);
		if (found != entries_.end()) {
			if (found->second.revisions == revisions) return found->second.value;
			retained_ -= found->second.bytes; released.push_back(std::move(found->second.value));
			recent_.erase(found->second.recent); entries_.erase(found);
		}
		while (!recent_.empty() && retained_ > limit - bytes) {
			const auto it = entries_.find(recent_.front()); retained_ -= it->second.bytes;
			released.push_back(std::move(it->second.value)); entries_.erase(it); recent_.pop_front();
		}
		recent_.push_back(key); entries_.emplace(key, Entry{revisions, made, bytes, std::prev(recent_.end())}); retained_ += bytes;
	}
	return made;
}
} // namespace city
