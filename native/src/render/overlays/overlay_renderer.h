// Map layers of the Layers popover, one mesh per 256 m chunk, each made by a JobPool job from a cut of the city (sim/state/
// city_cut.h: the job copies the roads and the ground it lays its ribbons on, then builds and bakes with the map let go) and
// handed over in the frame's admission. Zone colours: ribbons beside the zoned road sides and over the player's zoned lots,
// built when the chunk is dirty and the layer is on. The congestion map: a chunk's carriageways laid once per change of its
// roads, for the chunks within the road reach; every vertex carries its segment's id, and the shader (render/shaders/
// overlay_flow.gdshader) colours it from the texture of levels (flow_levels.h), refreshed every refresh_s: a refresh makes no
// mesh. The main thread reads nothing of the map for them.
#pragma once

#include "render/chunk_results.h"
#include "render/frame_admission.h"
#include "render/mesh_draw.h"
#include "render/overlays/flow_levels.h"
#include "render/render_context.h"
#include "sim/state/city_cut.h"

#include <atomic>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace city {

enum class Overlay : int { ZONES = 0, CONGESTION = 1, COUNT = 2 };

class OverlayRenderer {
public:
	OverlayRenderer();
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void set(Overlay o, bool on);
	void show_every_zone(bool on); // the brush in hand: every zoned side, not only the player's
	bool is_on(Overlay o) const { return on_[int(o)]; }
	void mark_dirty(int64_t key); // a road chunk changed: its ribbons are stale
	// After the frame's sync, in its admission: the chunks of the layers that are on asked nearest first, the results handed over,
	// the congestion levels refreshed. `s`: the tuning, the eye and the pool; nothing of the map.
	void update(const CityState &s, const PoseFrame &f, double dt, FrameAdmission &admission);
	int pending() const { return zone_queue_.pending() + flow_queue_.pending() + waiting_; } // chunks asked or to ask, not drawn yet
	// Tests: the chunks drawn of each layer, the chunks still to come, the meshes made since the core began, and the levels'
	// own (FlowLevels::probe).
	godot::Dictionary probe() const;

private:
	// A chunk's ribbons: the vertices and, of a zone's, each one's colour; of a carriageway's, its segment's id (the mesh's UV.x).
	struct Ribbon {
		godot::PackedVector3Array v;
		godot::PackedColorArray c;
		godot::PackedVector2Array slot;
		int slots = 0; // carriageways: one more than the highest segment id laid
	};
	// A job's result: the ribbons, made a mesh with their material by a job the upload ledger admits.
	struct Built {
		int64_t key = 0;
		JobKey job;
		uint64_t ticket = 0;
		float priority = 0;
		bool baked = false;
		Ribbon r;
		godot::RID material;
		std::atomic<int64_t> *made = nullptr; // the renderer's count of meshes made
		OwnedMesh mesh;
		bool bakes() const { return true; }
		int64_t bytes() const {
			return r.v.size() * int64_t(sizeof(godot::Vector3)) + r.c.size() * int64_t(sizeof(godot::Color)) + r.slot.size() * int64_t(sizeof(godot::Vector2));
		}
		void bake();
	};
	using Queue = ChunkResults<Built>;
	// What a zone job reads: the cut of its roads and ground, and the player's zoned lots near its chunk.
	struct ZoneInputs {
		CityCut cut;
		std::vector<PlayerLot> lots;
	};
	CutBox ribbon_box(const CityState &live, int64_t key, float beside) const;
	ZoneInputs gather_zone(const CityState &s, int64_t key) const;
	int strip(godot::PackedVector3Array &v, const CityView &s, int seg, float u0, float u1, bool on_road) const;
	void lot_cover(Ribbon &out, const CityView &s, const PlayerLot &lot, int64_t key) const;
	Ribbon build_zone_cpu(const CityView &s, const std::vector<PlayerLot> &lots, int64_t key) const;
	Ribbon lay_congestion(const CityView &s, int64_t key) const;
	void submit(const CityState &s, Overlay o, int64_t key, float priority);
	void offer(const CityState &s, Overlay o, FrameAdmission &admission);
	void stream_zones(const CityState &s, float cx, float cz, FrameAdmission &admission);
	void stream_flow(const CityState &s, float cx, float cz, FrameAdmission &admission);
	void hand(std::unordered_map<int64_t, MeshDraw> &chunks, Built &b, bool on);

	godot::RID scenario_, material_, flow_material_;
	std::unordered_map<int64_t, MeshDraw> zones_, flow_;
	std::unordered_set<int64_t> roads_known_;
	bool zones_all_ = false;
	std::atomic<bool> every_zone_{ false };
	float zone_alpha_ = 0, zone_alpha_tool_ = 0, congestion_alpha_ = 0;
	bool on_[int(Overlay::COUNT)] = {};
	Queue zone_queue_, flow_queue_; // the chunks' revisions (a chunk asked again once its result lands) and their results to hand
	std::vector<std::pair<float, int64_t>> order_; // the chunks to ask this frame, by distance (capacity kept)
	int waiting_ = 0; // chunks to ask that this frame's credits did not reach
	FlowLevels levels_; // the congestion levels the carriageways are coloured from
	double since_ = 0; // seconds since the levels were refreshed
	std::atomic<int64_t> meshes_made_{ 0 }; // what the probe counts: bumped by the workers that bake
	float zone_depth_ = 0, lift_ = 0, sample_ = 1, refresh_ = 1;
};

} // namespace city
