// The named-street plate base: source-order discovery once, chunk-only projection thereafter. No text duplicated in the spatial base.
#include "sim/street/named_plates.h"

#include "core/grid.h"
#include "sim/street/street_names.h"
#include "sim/street/street_spots.h"

#include <unordered_map>

namespace city {
struct NamedPlates::Base {
	uint64_t names = 0;
	int topology = 0;
	struct Street { int item; std::vector<int> chain; };
	std::vector<Street> streets;
	std::unordered_set<std::string> used;
	std::unordered_set<int> named;
};
struct NamedPlates::Geometry {
	std::shared_ptr<const Base> source;
	uint64_t roads = 0;
	struct Plate { SignStand stand; int item; };
	std::unordered_map<int64_t, std::vector<Plate>> chunks;
};

std::shared_ptr<const NamedPlates::Base> NamedPlates::base(const RoadNetwork &roads, const StreetWorks &works) const {
	const auto current = [&](const Base &b) {
		return b.topology == roads.version && b.names == works.names_revision;
	};
	std::shared_ptr<const Base> base;
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (topology_ && current(*topology_)) base = topology_;
	}
	if (!base) {
		// Concurrent cold misses prepare independently outside the metadata lock, never wait on reader geometry.
		auto next = std::make_shared<Base>();
		next->topology = roads.version; next->names = works.names_revision;
		for (size_t id = 0; id < works.items.items.size(); id++) {
			const StreetItem &item = works.items.items[id];
			if (!item.alive || item.kind != ItemKind::STREETNAME) continue;
			next->used.insert(item.text); // even an unanchored name reserves its text, as names_for did
			const int named = name_segment(roads, item);
			if (named < 0) continue;
			auto chain = street_chain(roads, named);
			for (int seg : chain) next->named.insert(seg);
			next->streets.push_back({int(id), std::move(chain)});
		}
		std::shared_ptr<const Base> retired;
		{
			std::lock_guard<std::mutex> lk(mutex_);
			if (!topology_ || !current(*topology_)) { retired = std::move(topology_); topology_ = next; }
			base = topology_;
		} // replaced roots are destroyed outside the metadata lock
	}
	return base;
}

void NamedPlates::coverage(const RoadNetwork &roads, const StreetWorks &works, std::unordered_set<std::string> &used, std::unordered_set<int> &named) const {
	const auto source = base(roads, works); used = source->used; named = source->named;
}

void NamedPlates::append(const RoadNetwork &roads, const StreetWorks &works, int64_t chunk, std::vector<SignStand> &out) const {
	const auto source = base(roads, works);
	std::shared_ptr<const Geometry> base;
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (ready_ && ready_->source == source && ready_->roads == roads.geometry_revision) base = ready_;
	}
	if (!base) {
		auto next = std::make_shared<Geometry>(); next->source = source; next->roads = roads.geometry_revision;
		for (const auto &street : source->streets) {
			std::vector<SignStand> plates;
			name_plates(roads, StreetItem{}, street.chain, plates);
			for (auto &plate : plates) next->chunks[chunk_of(plate.x, plate.z)].push_back({std::move(plate), street.item});
		}
		std::shared_ptr<const Geometry> retired;
		{
			std::lock_guard<std::mutex> lk(mutex_);
			if (!ready_ || ready_->source != source || ready_->roads != roads.geometry_revision) { retired = std::move(ready_); ready_ = next; }
			base = ready_;
		}
	}
	const auto found = base->chunks.find(chunk);
	if (found == base->chunks.end()) return;
	for (const auto &plate : found->second) {
		SignStand stand = plate.stand;
		stand.text = works.items.items[size_t(plate.item)].text;
		out.push_back(std::move(stand));
	}
}
} // namespace city
