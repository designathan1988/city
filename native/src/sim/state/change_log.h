// The one channel through which the simulation announces what changed: each consumer drains its own lists. Render adapters
// consume and clear theirs on the main thread (lot_boxes among them); the traffic drains the net_* lists, lots_changed and lot_fronts; the
// transit drains transit_segs; the lot cache consumes lot_cells. Nothing else keeps its own dirty flags: what an owner has still to work out of its own data (the
// network's shapes and grades left to settle, growth's queue of cells) is that owner's state, and what is kept by the city's
// revision (sim/roads/furniture_kept.h) or by an exact key (sim/buildings/b40/body_cache.h) needs no mark.
// Locking: producers append while holding CityState::map_mutex exclusively (commands, growth, load); a consumer that holds
// the map lock only shared drains under `mutex` too (swapping the lists out), so draining never needs the map exclusively and
// never waits for the mesh jobs that read the map.
#pragma once

#include "sim/demolition/damage_event.h"
#include "sim/state/flow_mark.h"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <tuple>
#include <vector>

namespace city {

// A building that came, went or changed, with the box its lot reached then (x0, z0, x1, z1: its centre plus half its diagonal).
// The box, not the record: the id may be freed and taken by another building before a consumer drains it.
struct LotBox {
	struct Box {
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
	};
	int id = -1;
	Box box;
};

// The drain sorts and deduplicates them (lane_graph_edit.cpp apply_network_changes): by id, then by the corners of the box.
inline auto lot_box_key(const LotBox &l) { return std::tie(l.id, l.box.x0, l.box.z0, l.box.x1, l.box.z1); }
inline bool operator<(const LotBox &a, const LotBox &b) { return lot_box_key(a) < lot_box_key(b); }
inline bool operator==(const LotBox &a, const LotBox &b) { return lot_box_key(a) == lot_box_key(b); }

struct ChangeLog {
	std::mutex mutex; // consumers under the shared map lock (see above)
	std::vector<int64_t> road_chunks; // CHUNK keys whose road meshes and street props must be rebuilt
	std::vector<int64_t> prop_chunks; // CHUNK keys whose street props alone must be rebuilt (what the player placed)
	std::vector<int64_t> lot_cells; // CHUNK keys whose proposed lots must be recomputed (streets or zones changed)
	std::vector<int64_t> building_chunks; // CHUNK keys whose building meshes must be rebuilt
	std::vector<int64_t> view_chunks; // CHUNK keys whose near building meshes must be drawn again though no building changed (Interior layer)
	std::vector<int> terrain_rects; // x0, z0, x1, z1 height-sample rectangles (inclusive)
	std::vector<int> paint_rects; // x0, z0, x1, z1: the same samples, their painted ground changed
	std::vector<int64_t> nature_chunks; // CHUNK keys whose trees and ground cover must be placed again
	std::vector<int64_t> water_chunks; // CHUNK keys whose water surface must be built again
	// CHUNK keys whose zone overlay must be laid again though no road changed: a lot of the player's came, went, moved or took a zone
	// (lot_commands.cpp mark_box). The overlay colours a zoned lot on the ground, in chunks that may hold no road at all.
	std::vector<int64_t> overlay_chunks;
	// The lane graph's incremental work: drained by the traffic (not by the render). `net_nodes` are the nodes whose
	// connectors and junction must be rebuilt; the segment lists are the ids added, removed or reprofiled.
	std::vector<int> net_nodes, net_segs_removed, net_segs_changed;
	// The nodes removed (j_remove_node_if_orphan), also on net_nodes: what the traffic keys by a node (the flows measured at it, the
	// control chosen for it) goes with it, even when the id is taken again before the drain.
	std::vector<int> net_nodes_removed;
	// The same segments, added, removed or reprofiled, for the transit: its stops on them find their lanes again (transit_plan.cpp).
	std::vector<int> transit_segs;
	// Segments whose street furniture must be placed again though their lanes did not change (a hydrant moving to
	// another corner of its square, a zone change moving a bin): drained by the traffic with the net_* lists.
	std::vector<int> furniture_segs;
	// Buildings that came or went: their bays, and the kerbs their car gates keep bays off, are laid out again by the traffic.
	std::vector<int> lots_changed;
	// The same buildings' boxes (x0, z0, x1, z1), for the render: the streets facing them draw their car gates again (the kerbs
	// dropped across them, the parking paint kept off them) and their furniture, laid off the gates. Boxes, not ids: a slot freed
	// and taken again before the render drains would lose where the old lot stood.
	std::vector<float> lot_boxes;
	// The same buildings with their boxes, for the traffic (BuildingStore::changed): each one's bays are laid out again, and the kerbs
	// its car gates keep bays off and the walkers' furniture and doors of the streets facing it are read again where its lot stood
	// (the pieces keep off the lots' gates, a door's foot keeps off the pieces), never where the record at its id stands at the drain.
	std::vector<LotBox> lot_fronts;
	// Junctions whose signal settings changed: only their plan is made again, the lanes and connectors stay.
	std::vector<int> signal_nodes;
	// Blows, shots and falls for the render to play once (damage_event.h): drained with the other render lists.
	std::vector<DamageEvent> damage_events;
	// The far traffic's streets whose drawn numbers changed (flow_mark.h): appended by the traffic's step under `mutex`, drained
	// with the other render lists.
	std::vector<FlowMark> flow_marks;

	// Whether the traffic has something of an edit still to drain (every list traffic::apply_network_changes takes). Asked with the
	// map held (shared or exclusive: the producers hold it exclusively), as the lists are read without `mutex`.
	bool traffic_owed() const {
		return !(net_nodes.empty() && net_nodes_removed.empty() && net_segs_removed.empty() && net_segs_changed.empty() &&
				furniture_segs.empty() && lots_changed.empty() && lot_fronts.empty() && signal_nodes.empty());
	}

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
		net_nodes_removed.clear();
		transit_segs.clear();
		furniture_segs.clear();
		lots_changed.clear();
		lot_fronts.clear();
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
		add(building_chunks, from.building_chunks);
		add(view_chunks, from.view_chunks);
		add(terrain_rects, from.terrain_rects);
		add(paint_rects, from.paint_rects);
		add(nature_chunks, from.nature_chunks);
		add(water_chunks, from.water_chunks);
		add(net_nodes, from.net_nodes);
		add(net_segs_removed, from.net_segs_removed);
		add(net_segs_changed, from.net_segs_changed);
		add(net_nodes_removed, from.net_nodes_removed);
		add(transit_segs, from.transit_segs);
		add(furniture_segs, from.furniture_segs);
		add(lots_changed, from.lots_changed);
		add(lot_boxes, from.lot_boxes);
		add(lot_fronts, from.lot_fronts);
		add(signal_nodes, from.signal_nodes);
		add(damage_events, from.damage_events);
		add(flow_marks, from.flow_marks);
	}
	// The blows of a city that is gone (a new map, a load), dropped where it goes: made before it, they would play on nothing. Those made
	// after are the new city's, kept for the render to play. The map held exclusively; takes `mutex` itself.
	void drop_damage() {
		std::lock_guard<std::mutex> lk(mutex);
		damage_events.clear();
	}
	// What a full rebuild draws of the map as it stands: the chunks marked so far. Not the blows: an event is played once, not drawn
	// from the map, and those made since the rebuild's cause are still to play (drop_damage is the cause's, when the city goes).
	void clear_render() {
		road_chunks.clear();
		prop_chunks.clear();
		building_chunks.clear();
		view_chunks.clear();
		terrain_rects.clear();
		paint_rects.clear();
		nature_chunks.clear();
		water_chunks.clear();
		overlay_chunks.clear();
		lot_boxes.clear();
		flow_marks.clear();
	}
};

} // namespace city
