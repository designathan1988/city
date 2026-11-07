// The rail nets kept while their tracks stand and the legs found on them (rail_net.h).
#include "sim/transit/rail_net.h"

#include <cstring>
#include <iterator>

namespace city {
namespace transit {

namespace {
// Whether the net was made from these very tracks of the mode (point for point, bit for bit) at this joining reach.
bool same_tracks(const TransitData &t, TransitMode mode, float join, const RailNet &net) {
	if (!net.made || net.join != join) return false;
	size_t k = 0;
	for (const RailTrack &track : t.tracks) {
		if (track.mode != mode) continue;
		if (k >= net.tracks.size()) return false;
		const std::vector<V2> &was = net.tracks[k++];
		if (was.size() != track.points.size() || (!was.empty() && std::memcmp(was.data(), track.points.data(), was.size() * sizeof(V2)) != 0)) return false;
	}
	return k == net.tracks.size();
}

} // namespace

void ready_net(const TransitData &t, TransitMode mode, float join, RailNet &net) {
	net.pass++;
	if (same_tracks(t, mode, join, net)) return;
	net.made = true;
	net.join = join;
	net.tracks.clear();
	for (const RailTrack &track : t.tracks)
		if (track.mode == mode) net.tracks.push_back(track.points);
	net.graph = rail_graph(t, mode, join);
	net.legs.clear();
}

const std::vector<V2> &leg_of(RailNet &net, const V2 &from, const V2 &to) {
	std::array<uint32_t, 4> key;
	std::memcpy(&key[0], &from, sizeof(V2));
	std::memcpy(&key[2], &to, sizeof(V2));
	auto hit = net.legs.find(key);
	if (hit == net.legs.end()) hit = net.legs.emplace(key, RailLeg{ rail_path(net.graph, from, to, net.search), 0 }).first;
	hit->second.used = net.pass;
	return hit->second.points;
}

void drop_unused_legs(RailNet &net) {
	for (auto it = net.legs.begin(); it != net.legs.end();) it = it->second.used == net.pass ? std::next(it) : net.legs.erase(it);
}

} // namespace transit
} // namespace city
