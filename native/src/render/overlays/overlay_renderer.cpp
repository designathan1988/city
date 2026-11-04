// Zone-colour chunk jobs and the congestion layer's refresh cycle (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"
#include "render/chunk_jobs.h"

#include "core/color.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, DROP_K = 1.5f;
constexpr float RGBA_MAX = 255.f;
constexpr int R_SHIFT = 24, G_SHIFT = 16, B_SHIFT = 8, JAM_STOPS = 3;
constexpr uint32_t BYTE = 0xff;
} // namespace

bool OverlayRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("overlay", errors);
	Dictionary t = ctx.table("overlays", errors);
	if (!errors.is_empty()) return false;
	zone_depth_ = float(t.get("zone_depth_m", 0));
	lift_ = float(t.get("lift_m", 0));
	sample_ = std::max(1.f, float(t.get("sample_m", 1)));
	refresh_ = float(t.get("refresh_s", 1));
	free_share_ = float(t.get("free_share", 1));
	jam_share_ = float(t.get("jam_share", 0));
	Array cols = t.get("congestion", Array());
	if (cols.size() != JAM_STOPS) {
		errors.push_back("overlays.json: congestion precisa de 3 cores");
		return false;
	}
	for (int i = 0; i < JAM_STOPS; i++) {
		uint32_t c = parse_hex_color(String(cols[i]).utf8().get_data());
		jam_colors_[i] = Color(float((c >> R_SHIFT) & BYTE) / RGBA_MAX, float((c >> G_SHIFT) & BYTE) / RGBA_MAX,
				float((c >> B_SHIFT) & BYTE) / RGBA_MAX, float(c & BYTE) / RGBA_MAX);
	}
	return true;
}

void OverlayRenderer::reset() {
	for (auto &kv : zones_) kv.second.free();
	for (auto &kv : congestion_) kv.second.free();
	zones_.clear();
	congestion_.clear();
	zones_dirty_.clear();
	roads_known_.clear();
	zones_all_ = true;
	since_ = refresh_;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
	}
}

void OverlayRenderer::set(Overlay o, bool on) {
	on_[int(o)] = on;
	if (o == Overlay::ZONES) {
		if (on) zones_all_ = true;
		else zones_dirty_.clear();
		for (auto &kv : zones_) kv.second.visible(on);
	} else {
		if (on) since_ = refresh_;
		for (auto &kv : congestion_) kv.second.visible(on);
	}
}

void OverlayRenderer::mark_dirty(int64_t key) {
	roads_known_.insert(key);
	if (on_[int(Overlay::ZONES)]) zones_dirty_.insert(key);
}

void OverlayRenderer::submit_zone(const CityState &s, int64_t key) {
	if (!s.jobs) return;
	float cx = s.camera_x.load(), cz = s.camera_z.load();
	float dx = (float(key_x(key)) + HALF) * CHUNK - cx, dz = (float(key_y(key)) + HALF) * CHUNK - cz;
	float priority = std::sqrt(dx * dx + dz * dz);
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
	}
	s.jobs->submit(JobKey{ JobLane::ZONES, key }, priority, [this, &s, key, seq, priority]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b;
		b.key = key;
		b.seq = seq;
		b.priority = priority;
		b.r = build_zone_cpu(s, key);
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void OverlayRenderer::upload(MeshDraw &d, Ribbon &r, bool on) {
	d.reset(scenario_, material_, false);
	if (!r.v.is_empty()) {
		Array arrays;
		arrays.resize(RenderingServer::ARRAY_MAX);
		arrays[RenderingServer::ARRAY_VERTEX] = r.v;
		arrays[RenderingServer::ARRAY_COLOR] = r.c;
		RenderingServer::get_singleton()->mesh_add_surface_from_arrays(d.mesh, RenderingServer::PRIMITIVE_TRIANGLES, arrays);
	}
	d.visible(on && !r.v.is_empty());
}

void OverlayRenderer::commit(const std::chrono::steady_clock::time_point &until) {
	commit_done(queue_mutex_, done_, requested_, until, [this](Built &b) { upload(zones_[b.key], b.r, on_[int(Overlay::ZONES)]); });
}

void OverlayRenderer::update_congestion(const CityState &s, const PoseFrame &f) {
	float reach = s.catalog.tune.render_road_end_m;
	float cx = s.camera_x.load(), cz = s.camera_z.load();
	for (int64_t key : roads_known_) {
		float dx = (float(key_x(key)) + HALF) * CHUNK - cx, dz = (float(key_y(key)) + HALF) * CHUNK - cz;
		if (std::sqrt(dx * dx + dz * dz) > reach) continue;
		Ribbon r = build_congestion_cpu(s, f, key);
		upload(congestion_[key], r, on_[int(Overlay::CONGESTION)]);
	}
	for (auto it = congestion_.begin(); it != congestion_.end();) {
		float dx = (float(key_x(it->first)) + HALF) * CHUNK - cx, dz = (float(key_y(it->first)) + HALF) * CHUNK - cz;
		if (std::sqrt(dx * dx + dz * dz) <= reach * DROP_K) {
			++it;
			continue;
		}
		it->second.free();
		it = congestion_.erase(it);
	}
}

void OverlayRenderer::update(const CityState &s, const PoseFrame &f, double dt) {
	const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(int64_t(s.catalog.tune.render_commit_budget_ms));
	if (on_[int(Overlay::ZONES)]) {
		if (zones_all_) {
			for (int64_t key : roads_known_) zones_dirty_.insert(key);
			zones_all_ = false;
		}
		for (auto it = zones_dirty_.begin(); it != zones_dirty_.end();) {
			bool inflight = false;
			{
				std::lock_guard<std::mutex> lk(queue_mutex_);
				inflight = requested_.count(*it) != 0;
			}
			if (inflight) {
				++it; // rebuilt once the in-flight result lands
				continue;
			}
			submit_zone(s, *it);
			it = zones_dirty_.erase(it);
		}
		commit(until);
	}
	if (on_[int(Overlay::CONGESTION)]) {
		since_ += dt;
		if (since_ >= refresh_) {
			since_ = 0;
			update_congestion(s, f);
		}
	} else {
		since_ = refresh_;
	}
}

} // namespace city
