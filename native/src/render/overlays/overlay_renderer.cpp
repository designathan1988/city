// Zone-colour chunk jobs and the congestion layer's refresh cycle (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "core/color.h"
#include "render/surface_data.h"

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

OverlayRenderer::OverlayRenderer() {
	zone_queue_.hand_one = [this](Built &b) { upload(zones_[b.key], b.r, on_[int(Overlay::ZONES)]); };
}

bool OverlayRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("overlay", errors);
	Dictionary t = ctx.table("overlays", errors);
	if (!errors.is_empty()) return false;
	zone_depth_ = float(t.get("zone_depth_m", 0));
	zone_alpha_ = float(t.get("zone_alpha", 0));
	zone_alpha_tool_ = float(t.get("zone_alpha_tool", 0));
	congestion_alpha_ = float(t.get("congestion_alpha", 0));
	lift_ = float(t.get("lift_m", 0));
	sample_ = std::max(1.f, float(t.get("sample_m", 1)));
	refresh_ = float(t.get("refresh_s", 1));
	free_share_ = float(t.get("free_share", 1));
	jam_share_ = float(t.get("jam_share", 0));
	Array cols = t.get("congestion", Array());
	if (cols.size() != JAM_STOPS) {
		errors.push_back("overlays.json: congestion needs 3 colours");
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
	lanes_.clear();
	roads_known_.clear();
	zones_all_ = true;
	since_ = refresh_;
	zone_queue_.clear();
}

void OverlayRenderer::show_every_zone(bool on) {
	if (every_zone_.exchange(on) != on) zones_all_ = on_[int(Overlay::ZONES)];
}

void OverlayRenderer::set(Overlay o, bool on) {
	on_[int(o)] = on;
	if (o == Overlay::ZONES) {
		if (on) zones_all_ = true;
		else
			for (int64_t key : roads_known_) zone_queue_.revs.unwant(key);
		for (auto &kv : zones_) kv.second.visible(on);
	} else {
		if (on) since_ = refresh_;
		for (auto &kv : congestion_) kv.second.visible(on);
	}
}

void OverlayRenderer::mark_dirty(int64_t key) {
	roads_known_.insert(key);
	lanes_.erase(key); // its roads changed: the congestion ribbons are laid again
	if (on_[int(Overlay::ZONES)]) zone_queue_.revs.mark(key);
}

void OverlayRenderer::submit_zone(const CityState &s, int64_t key, float priority) {
	zone_queue_.set_pool(s.jobs.get());
	const uint64_t ticket = zone_queue_.revs.ask(key);
	const JobKey job{ JobLane::ZONES, key };
	s.jobs->submit(job, priority, [this, &s, key, job, ticket, priority]() {
		auto b = std::make_shared<Built>();
		b->key = key;
		b->job = job;
		b->ticket = ticket;
		b->priority = priority;
		{
			MapRead map(s);
			b->r = build_zone_cpu(s, key);
		}
		zone_queue_.push(std::move(b));
	});
}

// The chunk's ribbon as a mesh made whole with the overlay's material (surface_data.h), in place of the one it showed; none when empty.
void OverlayRenderer::upload(MeshDraw &d, Ribbon &r, bool on) {
	RID mesh;
	if (!r.v.is_empty()) {
		Array arrays;
		arrays.resize(RenderingServer::ARRAY_MAX);
		arrays[RenderingServer::ARRAY_VERTEX] = r.v;
		arrays[RenderingServer::ARRAY_COLOR] = r.c;
		mesh = mesh_of_arrays(arrays, material_);
	}
	d.adopt(scenario_, false, mesh, LAYER_OVERLAYS);
	d.visible(on && mesh.is_valid());
}

// The chunks in reach coloured by this publication's speeds: their ribbons laid once (lay_congestion), coloured here; each
// upload paid into the frame's ledger.
void OverlayRenderer::update_congestion(const CityState &s, const PoseFrame &f, FrameAdmission &admission) {
	float reach = s.catalog.tune.render_road_end_m;
	float cx = s.camera_x.load(), cz = s.camera_z.load();
	for (int64_t key : roads_known_) {
		float dx = (float(key_x(key)) + HALF) * CHUNK - cx, dz = (float(key_y(key)) + HALF) * CHUNK - cz;
		if (std::sqrt(dx * dx + dz * dz) > reach) continue;
		auto laid = lanes_.find(key);
		if (laid == lanes_.end()) laid = lanes_.emplace(key, lay_congestion(s, key)).first;
		const Lanes &l = laid->second;
		Ribbon r;
		r.v = l.v;
		r.c.resize(l.v.size());
		Color *c = r.c.ptrw();
		int64_t at = 0;
		for (const auto &[seg, count] : l.strips) {
			const Color col = congestion_colour(f, seg);
			for (int i = 0; i < count; i++) c[at++] = col;
		}
		admission.upload.charge(ribbon_bytes(r));
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

void OverlayRenderer::update(const CityState &s, const PoseFrame &f, double dt, FrameAdmission &admission) {
	if (on_[int(Overlay::ZONES)]) {
		if (zones_all_) {
			for (int64_t key : roads_known_) zone_queue_.revs.mark(key);
			zones_all_ = false;
		}
		// The marked chunks nearest first, a credit each while the frame has them; a chunk whose result is on its way keeps its
		// mark and is asked again when that one lands.
		if (s.jobs) {
			const float cx = s.camera_x.load(), cz = s.camera_z.load();
			std::vector<std::pair<float, int64_t>> order;
			for (int64_t key : zone_queue_.revs.wanted()) {
				if (zone_queue_.revs.busy(key)) continue;
				const float dx = (float(key_x(key)) + HALF) * CHUNK - cx, dz = (float(key_y(key)) + HALF) * CHUNK - cz;
				order.push_back({ std::sqrt(dx * dx + dz * dz), key });
			}
			std::sort(order.begin(), order.end());
			for (const auto &[d, key] : order) {
				if (!admission.due() || !admission.build.take()) break;
				submit_zone(s, key, d);
			}
		}
		admission.commit({ &zone_queue_ });
	}
	if (on_[int(Overlay::CONGESTION)]) {
		since_ += dt;
		if (since_ >= refresh_) {
			since_ = 0;
			update_congestion(s, f, admission);
		}
	} else {
		since_ = refresh_;
	}
}

} // namespace city
