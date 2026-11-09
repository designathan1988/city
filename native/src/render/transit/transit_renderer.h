// Public transport on the map: each track and stop its own meshes, made again only when it, or the ground or the roads under it, changed,
// a few a frame; the metro's cut drawn through the ground while the tool is on; the run being laid as the real track, see-through, in
// two meshes kept for the tool's life (what is laid, and the length to the pointer), each written again only when its points move; the
// cars of the trains as a MultiMesh, each tinted with its line's colour. Tables from data/render/transit.json.
#pragma once

#include "render/frame_admission.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/transit/transit_parts.h"
#include "sim/state/city_state.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class TransitRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	// What changed under the plan since the last frame (road and terrain chunks): what stands there is made again.
	void touch(const std::vector<int64_t> &chunks);
	void touch_terrain(const CityState &s, const std::vector<int> &rects); // height-sample rectangles, ChangeLog::terrain_rects
	// Caller holds the map shared. Items are made while the frame's deadline lasts; their meshes' bytes are paid into its ledger.
	void sync(const CityState &s, FrameAdmission &f);
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	void set_xray(bool on);
	void set_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode);
	int drawn_cars() const { return drawn_cars_; }

private:
	struct Piece {
		OwnedMesh mesh, cut;
		godot::RID instance, cut_instance;
	};
	// The run being laid, one part of it: its meshes, and its mode and points (in look.preview_step) as last drawn.
	struct Sketch {
		Piece piece;
		std::vector<int64_t> key;
	};
	struct Item {
		uint64_t signature = 0;
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
		bool waiting = false;
		std::vector<Piece> pieces;
	};

	void see_plan(const CityState &s);
	void want(int64_t key, Item &item);
	int64_t build(const CityState &s, int64_t key, Item &item); // returns the bytes its meshes sent
	int64_t draw(const TransitMeshes &m, Piece &out);
	void drop(Piece &p);
	void sketch(const CityState &s, const std::vector<V2> &points, TransitMode mode, Sketch &out); // written again when its key moved
	void refill(const TransitMeshes &m, Piece &out); // into its own meshes and instances, made the first time

	godot::RID scenario_, material_, glass_, xray_[TRANSIT_XRAY_LAYERS];
	TransitLook look_, preview_look_; // the second: the parts named in transit.json `preview` see-through
	std::unordered_map<int64_t, Item> items_;
	std::deque<int64_t> queue_;
	Sketch laid_, leg_; // the run being laid: up to its last point, and its last point to the pointer
	OwnedMesh car_;
	MultiMeshDraw cars_;
	uint64_t plan_seen_ = ~uint64_t(0);
	float radius_ = 0;
	int per_frame_ = 0, drawn_cars_ = 0;
	bool xray_on_ = false;
};

} // namespace city
