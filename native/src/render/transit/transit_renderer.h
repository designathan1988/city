// Public transport on the map: each track and stop its own meshes, made again only when it, or the ground or the roads under it, changed.
// An item is built by a job: its plan copied with the map held, the city under it cut a stretch at a time (sim/state/city_cut.h), its
// boxes laid and its meshes baked with the map let go, handed over in the frame's admission. The metro's cut is drawn through the
// ground while the tool is on; the run being laid is the real track, see-through, in two meshes kept for the tool's life (what is
// laid, and the length to the pointer), each written again only when its points move; the cars of the trains are a MultiMesh, each
// tinted with its line's colour. Tables from data/render/transit.json.
#pragma once

#include "render/chunk_results.h"
#include "render/frame_admission.h"
#include "render/mesh_draw.h"
#include "render/render_context.h"
#include "render/transit/transit_parts.h"

#include <unordered_map>
#include <utility>
#include <vector>

namespace city {

class TransitRenderer {
public:
	TransitRenderer();
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	// What changed under the plan since the last frame (road and terrain chunks): what stands there is made again.
	void touch(const std::vector<int64_t> &chunks);
	void touch_terrain(const CityState &s, const std::vector<int> &rects); // height-sample rectangles, ChangeLog::terrain_rects
	// Caller holds the map shared: the plan is seen (what changed in it waits to be made) and the waiting items become the frame's
	// build units, the ones not drawn yet first, then nearest first (frame_admission.h).
	void offer(const CityState &s, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &out);
	AdmittedQueue *queue() { return &queue_; }
	int pending() const { return pending_ + queue_.pending(); } // items waiting, asked or on their way
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	void set_xray(bool on);
	void set_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode);
	int drawn_cars() const { return drawn_cars_; }
	int drawn_items() const; // tests: the tracks and stops of the plan that stand drawn

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
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0; // its box on the map, once drawn
		bool drawn = false;
		std::vector<Piece> pieces;
	};
	// What a job copies of the plan for its item, with the map held: a track, or a stop with the track it stands on and its line's colour.
	struct Plan {
		enum class Kind : uint8_t { NONE, TRACK, STOP } kind = Kind::NONE;
		RailTrack track; // the track itself, or the one a stop stands on (`on_track`)
		TransitStop stop;
		bool on_track = false;
		godot::Color tint;
		float street = 0; // how far past a point a piece reads the street and the ground
	};
	// A job's result: each stretch's surfaces, made its meshes (the body, and the cut through the ground) by a job the ledger admits.
	struct Built {
		int64_t key = 0;
		JobKey job;
		uint64_t ticket = 0;
		float priority = 0;
		bool baked = false;
		const TransitRenderer *owner = nullptr; // whose materials the meshes are made with
		std::vector<TransitMeshes> parts;
		std::vector<std::pair<OwnedMesh, OwnedMesh>> meshes;
		float x0 = 0, z0 = 0, x1 = 0, z1 = 0;
		bool bakes() const { return true; }
		int64_t bytes() const;
		void bake();
	};

	// An item's key: its id times KEY_KINDS, a track even and a stop odd.
	static constexpr int KEY_KINDS = 2;
	static int64_t track_key(int id) { return int64_t(id) * KEY_KINDS; }
	static int64_t stop_key(int id) { return int64_t(id) * KEY_KINDS + 1; }
	// A surface of `mb` with its material added to `surfaces` (none for an empty builder): a mesh is made whole with its materials
	// (surface_data.h), so the engine has its pipelines as it is made.
	static void add_surface(godot::TypedArray<godot::Dictionary> &surfaces, const MeshBuilder &mb, const godot::RID &material);
	void see_plan(const CityState &s);
	void want(int64_t key) { queue_.revs.mark(key); }
	// The job's side (transit_build.cpp): the item asked, its plan and cuts taken and its surfaces laid on a worker, its meshes baked.
	void submit(const CityState &s, int64_t key, float priority);
	Plan plan_of(const CityState &live, int64_t key) const;
	void build(const CityState &s, int64_t key, Built &b) const;
	std::pair<OwnedMesh, OwnedMesh> bake_piece(const TransitMeshes &m) const;
	void hand(Built &b);
	void drop(Piece &p);
	void sketch(const CityState &s, const std::vector<V2> &points, TransitMode mode, Sketch &out); // written again when its key moved
	void refill(const TransitMeshes &m, Piece &out); // into its own meshes and instances, made the first time

	godot::RID scenario_, material_, glass_, xray_[TRANSIT_XRAY_LAYERS];
	TransitLook look_, preview_look_; // the second: the parts named in transit.json `preview` see-through
	std::unordered_map<int64_t, Item> items_;
	ChunkResults<Built> queue_; // the items' revisions (one asked again once its result lands) and their results to hand
	Sketch laid_, leg_; // the run being laid: up to its last point, and its last point to the pointer
	OwnedMesh car_;
	MultiMeshDraw cars_;
	uint64_t plan_seen_ = ~uint64_t(0);
	float radius_ = 0;
	int pending_ = 0, drawn_cars_ = 0; // units offered this frame and not taken; cars in the MultiMesh
	bool xray_on_ = false;
};

} // namespace city
