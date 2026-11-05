// Public transport on the map: each track and stop its own meshes, made again only when it, or the ground or the roads under it, changed,
// a few a frame; the metro's cut drawn through the ground while the tool is on; the run being laid as the real track; the cars of the
// trains as a MultiMesh, each tinted with its line's colour. Tables from data/render/transit.json.
#pragma once

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
	void sync(const CityState &s); // caller holds the map shared
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	void set_xray(bool on);
	void set_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode);
	int drawn_cars() const { return drawn_cars_; }

private:
	struct Piece {
		OwnedMesh mesh, cut;
		godot::RID instance, cut_instance;
	};
	struct Item {
		uint64_t signature = 0;
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
		bool waiting = false;
		std::vector<Piece> pieces;
	};

	void see_plan(const CityState &s);
	void want(int64_t key, Item &item);
	void build(const CityState &s, int64_t key, Item &item);
	void draw(const TransitMeshes &m, Piece &out);
	void drop(Piece &p);

	godot::RID scenario_, material_, glass_, xray_[TRANSIT_XRAY_LAYERS];
	TransitLook look_;
	std::unordered_map<int64_t, Item> items_;
	std::deque<int64_t> queue_;
	Piece preview_;
	OwnedMesh car_;
	MultiMeshDraw cars_;
	uint64_t plan_seen_ = ~uint64_t(0);
	float radius_ = 0;
	int per_frame_ = 0, drawn_cars_ = 0;
	bool xray_on_ = false;
};

} // namespace city
