// The names streets are given, see street_names.h.
#include "sim/street/street_names.h"

#include "sim/catalog/catalog.h"
#include "sim/roads/road_network.h"
#include "sim/state/change_log.h"
#include "sim/street/footway.h"
#include "sim/street/street_spots.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_set>

namespace city {

namespace {

constexpr uint32_t NAME_HASH = 2654435761u; // Knuth's multiplicative hash: a street's first try in the list

const std::string &form_of(const StreetNamesDef &d, const Profile &p) {
	auto by_id = d.forms.find(p.id);
	if (by_id != d.forms.end()) return by_id->second;
	auto by_category = d.forms.find(p.category);
	return by_category != d.forms.end() ? by_category->second : d.form;
}

// A given name as one text, to tell two apart: its keys and its number (no language's words).
std::string given_key(const GivenName &g) { return g.form + '\n' + g.name + '\n' + std::to_string(g.ordinal); }

GivenName pick_name(const StreetNamesDef &d, const std::string &form, uint32_t k, const std::unordered_set<std::string> &used) {
	const size_t n = d.names.size();
	GivenName out;
	out.form = form;
	for (size_t tries = 0; tries < n; tries++, k++) {
		out.name = d.names[k % n];
		if (!used.count(given_key(out))) return out;
	}
	out.name = d.names[k % n];
	out.ordinal = uint32_t(used.size() + 1);
	return out;
}

} // namespace

int name_segment(const RoadNetwork &net, const StreetItem &item) {
	double t, d;
	return net.find_segment_near(item.x, item.z, net.cat->furniture.street_names.reach_m, t, d);
}

std::vector<StreetName> names_for(const RoadNetwork &net, const StreetWorks &works, std::vector<int> segs) {
	std::vector<StreetName> out;
	const StreetNamesDef &d = net.cat->furniture.street_names;
	if (d.names.empty()) return out; // a style without its names
	std::unordered_set<std::string> used;
	std::unordered_set<int> named; // segments whose street holds a name, one given before or one given here
	for (const StreetItem &it : works.items.items)
		if (it.alive && it.kind == ItemKind::STREETNAME && !it.given.empty()) used.insert(given_key(it.given));
	for (const auto &kv : works.names.by_item) named.insert(kv.second.chain.begin(), kv.second.chain.end());
	std::sort(segs.begin(), segs.end());
	for (int id : segs) {
		const RoadSegment &g = net.segs[size_t(id)];
		const Profile &p = net.profile_of(id);
		if (named.count(id) || !g.alive || p.rail) continue;
		for (int s : whole_street(net, id)) named.insert(s);
		StreetName name;
		name.given = pick_name(d, form_of(d, p), uint32_t(id) * NAME_HASH, used);
		used.insert(given_key(name.given));
		double x, z, y;
		footway_point(net, id, g.len / 2, 0.f, x, z, y);
		name.x = float(x);
		name.z = float(z);
		out.push_back(std::move(name));
	}
	return out;
}

const StreetItem *name_item_of(const StreetWorks &works, int seg) {
	const auto named = works.names.by_seg.find(seg);
	if (named == works.names.by_seg.end() || named->second.empty()) return nullptr;
	const int first = *std::min_element(named->second.begin(), named->second.end());
	return first >= 0 && size_t(first) < works.items.items.size() ? &works.items.items[size_t(first)] : nullptr;
}

DoorNumbers door_numbers(const RoadNetwork &net, int seg) {
	const std::vector<StreetLeg> run = street_run(net, seg);
	const auto end_of = [&](const StreetLeg &c, bool first) { // the street's way in or out of a piece
		const Quad &q = net.segs[size_t(c.seg)].q;
		return c.forward == first ? std::hypot(q.x0, q.z0) : std::hypot(q.x1, q.z1);
	};
	double total = 0;
	for (const StreetLeg &c : run) total += net.segs[size_t(c.seg)].len;
	// numbered from the street's end nearer the map's middle; `run` goes the way `seg` does
	const bool from_first = end_of(run.front(), true) <= end_of(run.back(), false);
	const std::vector<int> road = net.pieces_of(seg);
	double low = total, high = 0, at = 0;
	for (const StreetLeg &c : run) {
		const double len = net.segs[size_t(c.seg)].len;
		if (std::find(road.begin(), road.end(), c.seg) != road.end()) {
			const double from = from_first ? at : total - at - len;
			low = std::min(low, from);
			high = std::max(high, from + len);
		}
		at += len;
	}
	return { int(std::lround(low)), int(std::lround(high)), from_first };
}

void settle_name_plates(StreetWorks &works, RoadNetwork &net, ChangeLog &log) {
	NamePlates &np = works.names;
	std::vector<int> segs, again;
	std::vector<RoadNetwork::ChangedBox> boxes;
	net.take_edits(segs, boxes);
	again.swap(np.dirty);
	for (int seg : segs) {
		auto it = np.by_seg.find(seg);
		if (it != np.by_seg.end()) again.insert(again.end(), it->second.begin(), it->second.end());
	}
	const float reach = net.cat->furniture.street_names.reach_m; // name_segment looks this far
	for (const RoadNetwork::ChangedBox &b : boxes)
		works.items_in(float(b.minx) - reach, float(b.minz) - reach, float(b.maxx) + reach, float(b.maxz) + reach, [&](int id) {
			if (works.items.items[size_t(id)].kind == ItemKind::STREETNAME) again.push_back(id);
		});
	std::sort(again.begin(), again.end());
	again.erase(std::unique(again.begin(), again.end()), again.end());
	for (int id : again) {
		auto old = np.by_item.find(id);
		if (old != np.by_item.end()) {
			for (int64_t k : old->second.chunks) {
				log.prop_chunks.push_back(k);
				std::vector<int> &v = np.by_chunk[k];
				v.erase(std::remove(v.begin(), v.end(), id), v.end());
				if (v.empty()) np.by_chunk.erase(k);
			}
			for (int s : old->second.chain) {
				std::vector<int> &v = np.by_seg[s];
				v.erase(std::remove(v.begin(), v.end(), id), v.end());
				if (v.empty()) np.by_seg.erase(s);
			}
			np.by_item.erase(old);
		}
		if (id < 0 || id >= int(works.items.items.size())) continue;
		const StreetItem &it = works.items.items[size_t(id)];
		if (!it.alive || it.kind != ItemKind::STREETNAME) continue;
		NamePlates::Entry e;
		e.seg = name_segment(net, it);
		if (e.seg < 0) continue;
		e.chain = whole_street(net, e.seg);
		name_plates(net, it, e.chain, e.plates);
		for (const SignStand &p : e.plates) e.chunks.push_back(chunk_of(p.x, p.z));
		ChangeLog::dedup(e.chunks);
		for (int64_t k : e.chunks) {
			log.prop_chunks.push_back(k);
			std::vector<int> &v = np.by_chunk[k];
			v.insert(std::upper_bound(v.begin(), v.end(), id), id);
		}
		std::vector<int> held = e.chain;
		ChangeLog::dedup(held);
		for (int s : held) np.by_seg[s].push_back(id);
		np.by_item.emplace(id, std::move(e));
	}
}

} // namespace city
