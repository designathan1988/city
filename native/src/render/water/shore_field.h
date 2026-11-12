// Shore levels are prepared from local water/terrain snapshots with a propagation halo. Two physical layers per
// page and one small bank map publish a coherent revision without uploading unchanged field pages.
#pragma once
#include "sim/state/city_state.h"
#include "core/async_result.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/texture2d_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <atomic>
#include <memory>
#include <optional>
#include <set>

namespace city {
class ShoreField {
public:
	void sync(const CityState &s);
	godot::Dictionary view() const;
	void touch(const CityState &s, const std::vector<int64_t> &water_keys, const std::vector<int> &terrain_rects);
	void reset();
private:
	struct Page { int index = 0; godot::PackedByteArray texels; };
	struct Raster {
		std::vector<int> requested;
		std::vector<Page> pages;
		uint64_t revision = 0;
		bool complete = false;
	};
	struct Layout { int side = 0, page = 0, across = 0, passes = 0; float half = 0, step = 1; };
	using Wanted = std::shared_ptr<std::atomic<uint64_t>>;
	using Ticket = AsyncResult<Raster>::Ticket;
	static Raster work(const CityState &s, Layout layout, std::vector<int> requested, uint64_t revision, const Wanted &wanted, const Ticket &ticket);
	bool mark_world(float x0, float z0, float x1, float z1);
	void configure(const CityState &s);
	bool publish(const Raster &raster);
	godot::Ref<godot::Texture2DArray> texture_;
	godot::Ref<godot::ImageTexture> bank_map_;
	godot::PackedByteArray bank_bytes_;
	std::vector<godot::PackedByteArray> published_;
	std::vector<uint8_t> banks_;
	std::set<int> dirty_;
	AsyncResult<Raster> result_;
	std::optional<Raster> ready_;
	Wanted wanted_ = std::make_shared<std::atomic<uint64_t>>(next_generation());
	Layout layout_;
	int uploads_ = 0;
	bool visible_ = false;
};
} // namespace city
