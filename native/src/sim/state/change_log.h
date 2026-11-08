// The one channel through which the simulation announces what changed: each consumer drains its own lists. Render adapters
// consume and clear theirs on the main thread; the traffic drains the net_* lists; the transit drains transit_segs; the lot
// cache consumes lot_cells and street_cells. Nothing else keeps its own dirty flags: what an owner has still to work out of its own data (the
// network's shapes and grades left to settle, growth's queue of cells) is that owner's state, and what is kept by the city's
// revision (sim/street/furniture_kept.h) or by an exact key (sim/buildings/b40/body_cache.h) needs no mark.
// Locking: producers append while holding CityState::map_mutex exclusively (commands, growth, load); a consumer that holds
// the map lock only shared drains under `mutex` too (swapping the lists out), so draining never needs the map exclusively and
// never waits for the mesh jobs that read the map.
#pragma once

#include "sim/demolition/damage_event.h"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <vector>

namespace city {

struct ChangeLog {
	std::mutex mutex; // consumers under the shared map lock (see above)
	std::vector<int64_t> road_chunks; // CHUNK keys whose road meshes and street props must be rebuilt
	std::vector<int64_t> prop_chunks; // CHUNK keys whose street props alone must be rebuilt (what the player placed)
	std::vector<int64_t> lot_cells; // CHUNK keys whose proposed lots must be recomputed (streets or zones changed)
	std::vector<int64_t> street_cells; // CHUNK keys whose streets or ground changed: the lots proposed there are cut again (proposed_lots.h)
	std::vector<int64_t> lot_chunks; // CHUNK keys whose lots were derived again (the streets', the player's): the zone layer draws them again
	std::vector<int64_t> building_chunks; // CHUNK keys whose building meshes must be rebuilt
	std::vector<int64_t> view_chunks; // CHUNK keys whose near building meshes must be drawn again though no building changed (Interior layer)
	std::vector<int> terrain_rects; // x0, z0, x1, z1 height-sample rectangles (inclusive)
	std::vector<int> paint_rects; // x0, z0, x1, z1: the same samples, their painted ground changed
	std::vector<int64_t> nature_chunks; // CHUNK keys whose trees and ground cover must be placed again
	std::vector<int64_t> water_chunks; // CHUNK keys whose water surface must be built again
	bool network_changed = false; // lanes and routes must be refreshed
	// The lane graph's incremental work: drained by the traffic (not by the render). `net_nodes` are the nodes whose
	// connectors and junction must be rebuilt; the segment lists are the ids added, removed or reprofiled.
	std::vector<int> net_nodes, net_segs_removed, net_segs_changed;
	// The same segments, added, removed or reprofiled, for the transit: its stops on them find their lanes again (transit_plan.cpp).
	std::vector<int> transit_segs;
	// Segments whose street furniture must be placed again though their lanes did not change (a hydrant moving to
	// another corner of its square, a zone change moving a bin): drained by the traffic with the net_* lists.
	std::vector<int> furniture_segs;
	// Buildings that came or went: their bays, and the kerbs their car gates keep bays off, are laid out again by the traffic.
	std::vector<int> lots_changed;
	// Junctions whose signal settings changed: only their plan is made again, the lanes and connectors stay.
	std::vector<int> signal_nodes;
	// Blows, shots and falls for the render to play once (damage_event.h): drained with the other render lists.
	std::vector<DamageEvent> damage_events;

	static void dedup(std::vector<int64_t> &v) {
		std::sort(v.begin(), v.end());
		v.erase(std::unique(v.begin(), v.end()), v.end());
	}
	static void dedup(std::vector<int> &v) {
		std::sort(v.begin(), v.end());
		v.erase(std::unique(v.begin(), v.end()), v.end());
	}
	void net_node(int n) { net_nodes.push_back(n); }
	void net_seg(int seg) {
		net_segs_changed.push_back(seg);
		transit_segs.push_back(seg);
	}
	void net_seg_removed(int seg) {
		net_segs_removed.push_back(seg);
		transit_segs.push_back(seg);
	}
	void clear_net() {
		net_nodes.clear();
		net_segs_removed.clear();
		net_segs_changed.clear();
		transit_segs.clear();
		furniture_segs.clear();
		lots_changed.clear();
		signal_nodes.clear();
	}
	// What another log holds appended to this one, that log left empty: a city read aside marks its own log, and the live
	// one takes those marks with the city (take_city).
	void absorb(ChangeLog &from) {
		const auto add = [](auto &to, auto &more) {
			to.insert(to.end(), more.begin(), more.end());
			more.clear();
		};
		add(road_chunks, from.road_chunks);
		add(prop_chunks, from.prop_chunks);
		add(lot_cells, from.lot_cells);
		add(street_cells, from.street_cells);
		add(lot_chunks, from.lot_chunks);
		add(building_chunks, from.building_chunks);
		add(view_chunks, from.view_chunks);
		add(terrain_rects, from.terrain_rects);
		add(paint_rects, from.paint_rects);
		add(nature_chunks, from.nature_chunks);
		add(water_chunks, from.water_chunks);
		network_changed = network_changed || from.network_changed;
		from.network_changed = false;
		add(net_nodes, from.net_nodes);
		add(net_segs_removed, from.net_segs_removed);
		add(net_segs_changed, from.net_segs_changed);
		add(transit_segs, from.transit_segs);
		add(furniture_segs, from.furniture_segs);
		add(lots_changed, from.lots_changed);
		add(signal_nodes, from.signal_nodes);
		add(damage_events, from.damage_events);
	}
	void clear_render() {
		road_chunks.clear();
		prop_chunks.clear();
		lot_chunks.clear();
		building_chunks.clear();
		view_chunks.clear();
		terrain_rects.clear();
		damage_events.clear();
		paint_rects.clear();
		nature_chunks.clear();
		water_chunks.clear();
	}
};

} // namespace city
