// Map layers of the Layers popover, one mesh per 256 m chunk: zone colours
// on the zoned road sides (built in a JobPool job when the chunk is dirty and the layer is on) and the congestion map
// (each carriageway coloured by its vehicles' mean speed against the limit, rebuilt every refresh_s from the
// PoseFrame's per-segment speed share, only for chunks within the road reach). Only reads the state.
#pragma once

#include "render/chunk_results.h"

#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <array>
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
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void set(Overlay o, bool on);
	void show_every_zone(bool on); // the brush in hand: every zoned side, not only the player's
	bool is_on(Overlay o) const { return on_[int(o)]; }
	void mark_dirty(int64_t key, JobPool *pool = nullptr); // a road chunk changed: its zone mesh is stale
	void update(const CityState &s, const PoseFrame &f, double dt);

private:
	struct ColourSpan {
		int segment = -1;
		int64_t first = 0, count = 0;
		std::array<uint8_t, 4> rgba{};
	};
	struct CongestionDraw {
		MeshDraw draw;
		std::vector<ColourSpan> colours;
	};
	struct Ribbon {
		godot::PackedVector3Array v;
		godot::PackedColorArray c;
		std::vector<ColourSpan> colours;
	};
	struct Built {
		int64_t key = 0;
		uint64_t seq = 0;
		float priority = 0;
		Ribbon r;
	};
	void strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const godot::Color &col, bool on_road) const;
	void lot_cover(Ribbon &out, const CityState &s, const PlayerLot &lot, int64_t key) const;
	Ribbon build_zone_cpu(const CityState &s, int64_t key) const;
	Ribbon build_congestion_cpu(const CityState &s, const PoseFrame &f, int64_t key) const;
	void submit_zone(const CityState &s, int64_t key);
	void commit(const std::chrono::steady_clock::time_point &until);
	void upload(MeshDraw &d, Ribbon &r, bool on);
	void update_congestion(const CityState &s, const PoseFrame &f);
	godot::Color congestion_colour(float share) const;
	static std::array<uint8_t, 4> rgba8(const godot::Color &colour);
	bool update_colours(CongestionDraw &draw, const PoseFrame &frame);

	godot::RID scenario_, material_;
	std::unordered_map<int64_t, MeshDraw> zones_;
	std::unordered_map<int64_t, CongestionDraw> congestion_;
	std::unordered_set<int64_t> congestion_dirty_;
	std::unordered_set<int64_t> zones_dirty_, roads_known_;
	bool zones_all_ = false;
	std::atomic<bool> every_zone_{ false };
	float zone_alpha_ = 0, zone_alpha_tool_ = 0, congestion_alpha_ = 0;
	bool on_[int(Overlay::COUNT)] = {};
	std::mutex queue_mutex_;
	CompletedResults<Built> done_;
	std::unordered_map<int64_t, uint64_t> requested_;
	uint64_t seq_ = 0;
	double since_ = 0;
	float zone_depth_ = 0, lift_ = 0, sample_ = 1, refresh_ = 1, free_share_ = 1, jam_share_ = 0;
	godot::Color jam_colors_[3];
};

} // namespace city
