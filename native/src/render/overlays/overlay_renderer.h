// Map layers of the Layers popover, one mesh per 256 m chunk: the lots coloured by their zone, each drawn whole by the chunk its
// middle stands in; with the Zones tool in hand every lot with its outline and its front (the side its building faces, an arrow
// to the street) and the lots the streets propose, outlined (built in a JobPool job when the chunk is dirty and the layer is on,
// asked and handed over in the frame's admission) and the congestion map (each carriageway coloured by its vehicles' mean speed against the limit, coloured
// again every refresh_s from the PoseFrame's per-segment speed share, only for chunks within the road reach: a chunk's ribbons
// are laid once per change of its roads, a refresh only colours them). Only reads the state.
#pragma once

#include "render/chunk_results.h"
#include "render/frame_admission.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <atomic>
#include <chrono>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

enum class Overlay : int { ZONES = 0, CONGESTION = 1, COUNT = 2 };

class OverlayRenderer {
public:
	OverlayRenderer();
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void set(Overlay o, bool on);
	void show_zone_tool(bool on, bool works); // the Zones tool in hand, its proposal cut for works or not (proposed_lots.h)
	bool is_on(Overlay o) const { return on_[int(o)]; }
	void mark_dirty(int64_t key); // a road chunk changed: its zone mesh and its congestion ribbons are stale
	void mark_zones(int64_t key); // a chunk's lots or buildings changed: its zone mesh is stale
	void update(const CityState &s, const PoseFrame &f, double dt, FrameAdmission &admission);

private:
	struct Ribbon {
		godot::PackedVector3Array v;
		godot::PackedColorArray c;
	};
	struct Built {
		int64_t key = 0;
		JobKey job;
		uint64_t ticket = 0;
		float priority = 0;
		bool baked = false;
		Ribbon r;
		bool bakes() const { return false; }
		int64_t bytes() const { return ribbon_bytes(r); }
		void bake() {}
	};
	// A chunk's congestion ribbons as laid: their vertices, and for each carriageway its segment and its vertex count.
	struct Lanes {
		godot::PackedVector3Array v;
		std::vector<std::pair<int, int>> strips;
	};
	static int64_t ribbon_bytes(const Ribbon &r) { return r.v.size() * int64_t(sizeof(godot::Vector3)) + r.c.size() * int64_t(sizeof(godot::Color)); }
	// What a lot is drawn with (overlays.json "lots"): strokes' widths and colours, the zone's alpha by what stands, the front's arrow.
	struct LotLook {
		float line_m = 0, proposal_m = 0, front_m = 0, arrow_m = 0;
		godot::Color line, proposal, front;
		float alpha_tool = 0, alpha_built = 0;
		float arrow_most_m = 0, arrow_least_m = 0, arrow_of_front = 0, arrow_of_depth = 0, arrow_tip = 0, arrow_head = 0, arrow_spread = 0;
	};
	void strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const godot::Color &col, bool on_road) const;
	void lot_cover(Ribbon &out, const CityState &s, const std::vector<XZ> &ring, const godot::Color &col) const;
	void edge(Ribbon &out, const CityState &s, const XZ &a, const XZ &b, float width, const godot::Color &col) const;
	void lot_shape(Ribbon &out, const CityState &s, const std::vector<XZ> &ring, int zone, bool built, bool tool) const;
	Ribbon build_zone_cpu(const CityState &s, int64_t key) const;
	Lanes lay_congestion(const CityState &s, int64_t key) const;
	godot::Color congestion_colour(const PoseFrame &f, int seg) const;
	void submit_zone(const CityState &s, int64_t key, float priority);
	void upload(MeshDraw &d, Ribbon &r, bool on);
	void update_congestion(const CityState &s, const PoseFrame &f, FrameAdmission &admission);

	godot::RID scenario_, material_;
	std::unordered_map<int64_t, MeshDraw> zones_, congestion_;
	std::unordered_set<int64_t> roads_known_;
	std::unordered_set<int64_t> lots_known_; // chunks whose lots changed since the map was drawn (marked with the roads' on a redraw)
	std::unordered_map<int64_t, Lanes> lanes_; // the congestion ribbons laid, by chunk (dropped when its roads change)
	bool zones_all_ = false;
	std::atomic<bool> every_zone_{ false }, zone_works_{ false };
	float zone_alpha_ = 0, congestion_alpha_ = 0;
	LotLook look_;
	bool on_[int(Overlay::COUNT)] = {};
	ChunkResults<Built> zone_queue_; // revisions of the zone chunks (a chunk asked again once its result lands), results to hand
	double since_ = 0;
	float lift_ = 0, sample_ = 1, refresh_ = 1, free_share_ = 1, jam_share_ = 0;
	godot::Color jam_colors_[3];
};

} // namespace city
