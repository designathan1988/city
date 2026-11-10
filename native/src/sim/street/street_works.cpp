// The player's street records and their place index, see street_works.h.
#include "sim/street/street_works.h"

#include <cmath>
#include <limits>

namespace city {

namespace {
template <typename T>
void bounds(const T &b, float &x0, float &z0, float &x1, float &z1) {
	x0 = z0 = std::numeric_limits<float>::max();
	x1 = z1 = -std::numeric_limits<float>::max();
	for (size_t i = 0; i + 1 < b.pts.size(); i += 2) {
		x0 = std::min(x0, b.pts[i]);
		x1 = std::max(x1, b.pts[i]);
		z0 = std::min(z0, b.pts[i + 1]);
		z1 = std::max(z1, b.pts[i + 1]);
	}
}
} // namespace

void StreetWorks::clear() {
	items.clear();
	poles.clear();
	spans.clear();
	barriers.clear();
	cuts.clear();
	names = NamePlates();
	item_hash_.clear();
	pole_hash_.clear();
	barrier_hash_.clear();
	cut_hash_.clear();
}

StreetWorks StreetWorks::cut(float minx, float minz, float maxx, float maxz) const {
	constexpr int SPANS_AWAY = 2; // a pole's arms read the poles a span away; a span drawn reads the arms of both its poles
	StreetWorks c;
	c.items.items = IdTable<StreetItem>::cut(items.items.size());
	c.poles.items = IdTable<UtilityPole>::cut(poles.items.size());
	c.spans.items = IdTable<UtilitySpan>::cut(spans.items.size());
	c.barriers.items = IdTable<Barrier>::cut(barriers.items.size());
	c.cuts.items = IdTable<StreetCut>::cut(cuts.items.size());
	c.item_hash_ = item_hash_.window(minx, minz, maxx, maxz);
	c.pole_hash_ = pole_hash_.window(minx, minz, maxx, maxz);
	c.barrier_hash_ = barrier_hash_.window(minx, minz, maxx, maxz);
	c.cut_hash_ = cut_hash_.window(minx, minz, maxx, maxz);
	for (int id : c.item_hash_.filed()) c.items.items.put(size_t(id), items.items[size_t(id)]);
	for (int id : c.barrier_hash_.filed()) c.barriers.items.put(size_t(id), barriers.items[size_t(id)]);
	for (int id : c.cut_hash_.filed()) c.cuts.items.put(size_t(id), cuts.items[size_t(id)]);
	std::vector<int> fresh = c.pole_hash_.filed();
	for (int id : fresh) c.poles.items.put(size_t(id), poles.items[size_t(id)]);
	for (int ring = 0; ring < SPANS_AWAY; ring++) {
		std::vector<int> from;
		from.swap(fresh);
		for (int id : from)
			for (int span : poles.items[size_t(id)].spans) {
				const UtilitySpan &sp = spans.items[size_t(span)];
				c.spans.items.put(size_t(span), sp);
				for (int end : { sp.a, sp.b })
					if (c.poles.items.put(size_t(end), poles.items[size_t(end)])) fresh.push_back(end);
			}
	}
	for_chunks(minx, minz, maxx, maxz, [&](int64_t key) {
		const auto at = names.by_chunk.find(key);
		if (at == names.by_chunk.end()) return;
		c.names.by_chunk.emplace(key, at->second);
		for (int item : at->second) c.names.by_item.emplace(item, names.by_item.at(item));
	});
	return c;
}

int StreetWorks::add_item(const StreetItem &r) {
	int id = items.add(r);
	item_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
	if (r.kind == ItemKind::STREETNAME) names.dirty.push_back(id);
	return id;
}

void StreetWorks::put_item(int id, const StreetItem &r) {
	items.put(id, r);
	item_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
	if (r.kind == ItemKind::STREETNAME) names.dirty.push_back(id);
}

void StreetWorks::remove_item(int id) {
	const StreetItem &r = items.items[size_t(id)];
	item_hash_.remove_aabb(id, r.x, r.z, r.x, r.z);
	if (r.kind == ItemKind::STREETNAME) names.dirty.push_back(id);
	items.drop(id);
}

int StreetWorks::add_pole(const UtilityPole &r) {
	int id = poles.add(r);
	pole_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
	return id;
}

void StreetWorks::put_pole(int id, const UtilityPole &r) {
	poles.put(id, r);
	poles.items[size_t(id)].spans.clear(); // the spans come back on their own
	pole_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
}

void StreetWorks::remove_pole(int id) {
	const UtilityPole &r = poles.items[size_t(id)];
	pole_hash_.remove_aabb(id, r.x, r.z, r.x, r.z);
	poles.drop(id);
}

int StreetWorks::add_span(const UtilitySpan &r) {
	int id = spans.add(r);
	poles.items[size_t(r.a)].spans.push_back(id);
	poles.items[size_t(r.b)].spans.push_back(id);
	return id;
}

void StreetWorks::put_span(int id, const UtilitySpan &r) {
	spans.put(id, r);
	poles.items[size_t(r.a)].spans.push_back(id);
	poles.items[size_t(r.b)].spans.push_back(id);
}

void StreetWorks::remove_span(int id) {
	const UtilitySpan r = spans.items[size_t(id)];
	for (int end : { r.a, r.b }) {
		std::vector<int> &at = poles.items[size_t(end)].spans;
		at.erase(std::remove(at.begin(), at.end(), id), at.end());
	}
	spans.drop(id);
}

int StreetWorks::span_between(int a, int b) const {
	for (int id : poles.items[size_t(a)].spans) {
		const UtilitySpan &s = spans.items[size_t(id)];
		if ((s.a == a && s.b == b) || (s.a == b && s.b == a)) return id;
	}
	return -1;
}

void StreetWorks::file_barrier(int id, const Barrier &b, bool in) {
	float x0, z0, x1, z1;
	bounds(b, x0, z0, x1, z1);
	if (in) barrier_hash_.insert_aabb(id, x0, z0, x1, z1);
	else barrier_hash_.remove_aabb(id, x0, z0, x1, z1);
}

int StreetWorks::add_barrier(const Barrier &r) {
	int id = barriers.add(r);
	file_barrier(id, r, true);
	return id;
}

void StreetWorks::put_barrier(int id, const Barrier &r) {
	barriers.put(id, r);
	file_barrier(id, r, true);
}

void StreetWorks::remove_barrier(int id) {
	file_barrier(id, barriers.items[size_t(id)], false);
	barriers.drop(id);
}

int StreetWorks::add_cut(const StreetCut &r) {
	int id = cuts.add(r);
	cut_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
	return id;
}

void StreetWorks::put_cut(int id, const StreetCut &r) {
	cuts.put(id, r);
	cut_hash_.insert_aabb(id, r.x, r.z, r.x, r.z);
}

void StreetWorks::remove_cut(int id) {
	const StreetCut &r = cuts.items[size_t(id)];
	cut_hash_.remove_aabb(id, r.x, r.z, r.x, r.z);
	cuts.drop(id);
}

int StreetWorks::item_near(float x, float z, float reach) const {
	int best = -1;
	float best_d = reach;
	items_in(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		const StreetItem &r = items.items[size_t(id)];
		const float d = std::hypot(r.x - x, r.z - z);
		if (d <= best_d) {
			best_d = d;
			best = id;
		}
	});
	return best;
}

int StreetWorks::pole_near(float x, float z, float reach) const {
	int best = -1;
	float best_d = reach;
	poles_in(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		const UtilityPole &r = poles.items[size_t(id)];
		const float d = std::hypot(r.x - x, r.z - z);
		if (d <= best_d) {
			best_d = d;
			best = id;
		}
	});
	return best;
}

int StreetWorks::barrier_near(float x, float z, float reach) const {
	int best = -1;
	float best_d = reach;
	barriers_in(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		const float d = barrier_distance(barriers.items[size_t(id)], x, z);
		if (d <= best_d) {
			best_d = d;
			best = id;
		}
	});
	return best;
}

float barrier_distance(const Barrier &b, float x, float z) {
	float best = std::numeric_limits<float>::max();
	for (size_t i = 0; i + 3 < b.pts.size(); i += 2) {
		const float ax = b.pts[i], az = b.pts[i + 1], dx = b.pts[i + 2] - ax, dz = b.pts[i + 3] - az;
		const float l2 = dx * dx + dz * dz;
		const float t = l2 > 0 ? std::clamp(((x - ax) * dx + (z - az) * dz) / l2, 0.f, 1.f) : 0.f;
		best = std::min(best, std::hypot(ax + dx * t - x, az + dz * t - z));
	}
	return best;
}

} // namespace city
