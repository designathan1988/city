// See transit_renderer.h. An item is a track or a stop; its signature is what it is drawn from, so a plan's edit makes again only what it moved.
#include "render/transit/transit_renderer.h"

#include "core/grid.h"
#include "render/agents/people_renderer.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;
constexpr int RGB_BITS = 8, RECT_INTS = 4;
constexpr uint32_t OPAQUE = 0xFFu;

int64_t track_key(int id) { return int64_t(id) * 2; }
int64_t stop_key(int id) { return int64_t(id) * 2 + 1; }

// Each surface's material, in the order they were added.
void add_surface(RenderingServer *rs, const RID &mesh, const MeshBuilder &mb, const RID &material, int &surface) {
	if (mb.empty()) return;
	mb.commit_to(mesh);
	rs->mesh_surface_set_material(mesh, surface++, material);
}
} // namespace

bool TransitRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("transit", errors);
	glass_ = ctx.material("transit_glass", errors);
	for (int i = 0; i < TRANSIT_XRAY_LAYERS; i++) xray_[i] = ctx.material((std::string("transit_xray_") + std::to_string(i)).c_str(), errors);
	const Dictionary t = ctx.table("transit", errors);
	if (!errors.is_empty() || !look_.read(t, errors)) return false;
	look_.kerb = Dictionary(Dictionary(ctx.table("streets", errors))["heights"])["sidewalk"];
	radius_ = t["radius_m"];
	TransitMeshes car;
	transit_box(car, look_, look_.part("car"), Vector3(0, look_.car_up, 0), V2(0, 1), look_.car);
	RenderingServer *rs = RenderingServer::get_singleton();
	car_.reset();
	car_.rid = rs->mesh_create();
	int surface = 0;
	add_surface(rs, car_.rid, car.solid, material_, surface);
	car_bounds_ = mesh_outline(car.solid);
	cars_.configure(scenario_, car_.rid, RID(), true, LAYER_VEHICLES, float(ctx.catalog->tune.render_instance_sector_m));
	return true;
}

void TransitRenderer::drop(Piece &p) {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (p.instance.is_valid()) rs->free_rid(p.instance);
	if (p.cut_instance.is_valid()) rs->free_rid(p.cut_instance);
	p.instance = p.cut_instance = RID();
	p.mesh.reset();
	p.cut.reset();
}

void TransitRenderer::reset() {
	for (auto &e : items_)
		for (auto &part : e.second.pieces) drop(part.second);
	items_.clear();
	queue_.clear();
	for (auto &part : preview_.pieces) drop(part.second);
	preview_ = Item(); preview_track_ = RailTrack(); preview_turn_ = true;
	cars_.reset();
	plan_seen_ = ~uint64_t(0);
	drawn_cars_ = 0;
}

void TransitRenderer::want(int64_t key, Item &item) {
	item.work.reset();
	if (item.waiting) return;
	item.waiting = true;
	queue_.push_back(key);
}

void TransitRenderer::touch(const std::vector<int64_t> &chunks) {
	if (chunks.empty()) return;
	if (preview_track_.points.size() >= 2) { preview_.work.reset(); preview_.waiting = true; }
	if (items_.empty()) return;
	const std::unordered_set<int64_t> changed(chunks.begin(), chunks.end());
	for (auto &e : items_) {
		Item &it = e.second;
		bool met = false;
		for (float x = it.x0; x <= it.x1 + CHUNK && !met; x += CHUNK)
			for (float z = it.z0; z <= it.z1 + CHUNK && !met; z += CHUNK) met = changed.count(chunk_of(std::min(x, it.x1), std::min(z, it.z1))) > 0;
		if (met) want(e.first, it);
	}
}

void TransitRenderer::touch_terrain(const CityState &s, const std::vector<int> &rects) {
	std::vector<int64_t> chunks;
	for (size_t i = 0; i + RECT_INTS <= rects.size(); i += RECT_INTS) {
		const float x0 = s.terrain.x_of(rects[i]), z0 = s.terrain.x_of(rects[i + 1]), x1 = s.terrain.x_of(rects[i + 2]), z1 = s.terrain.x_of(rects[i + 3]);
		for (float x = x0; x <= x1 + CHUNK; x += CHUNK)
			for (float z = z0; z <= z1 + CHUNK; z += CHUNK) chunks.push_back(chunk_of(std::min(x, x1), std::min(z, z1)));
	}
	touch(chunks);
}

void TransitRenderer::see_plan(const CityState &s) {
	std::unordered_set<int64_t> alive;
	auto seen = [&](int64_t key, uint64_t signature, uint64_t extra) {
		alive.insert(key);
		Item &it = items_[key];
		if (!it.removed && it.signature == signature && it.extra_signature == extra) return;
		it.removed = false; it.signature = signature; it.extra_signature = extra; want(key, it);
	};
	for (const RailTrack &t : s.transit.tracks()) seen(track_key(t.id), s.transit.track_version(t.id), 0);
	for (const TransitStop &q : s.transit.stops()) {
		const auto colour = s.transit.first_line_colour(q.id);
		constexpr uint64_t HAS_COLOUR = uint64_t(1) << 32;
		seen(stop_key(q.id), s.transit.stop_version(q.id), colour ? HAS_COLOUR | uint64_t(*colour) : 0);
	}
	for (auto &entry : items_) {
		if (alive.count(entry.first) || entry.second.removed) continue;
		entry.second.removed = true; want(entry.first, entry.second);
	}
}

void TransitRenderer::draw(const TransitMeshes &m, Piece &out) {
	RenderingServer *rs = RenderingServer::get_singleton();
	drop(out);
	if (!m.solid.empty() || !m.glass.empty()) {
		out.mesh.rid = rs->mesh_create();
		int surface = 0;
		add_surface(rs, out.mesh.rid, m.solid, material_, surface);
		add_surface(rs, out.mesh.rid, m.glass, glass_, surface);
		out.instance = rs->instance_create2(out.mesh.rid, scenario_);
	}
	int surface = 0;
	for (int i = 0; i < TRANSIT_XRAY_LAYERS; i++) {
		if (m.xray[i].empty()) continue;
		if (!out.cut.valid()) out.cut.rid = rs->mesh_create();
		add_surface(rs, out.cut.rid, m.xray[i], xray_[i], surface);
	}
	if (!out.cut.valid()) return;
	out.cut_instance = rs->instance_create2(out.cut.rid, scenario_);
	rs->instance_geometry_set_cast_shadows_setting(out.cut_instance, RenderingServer::SHADOW_CASTING_SETTING_OFF);
	rs->instance_set_visible(out.cut_instance, xray_on_);
}

bool TransitRenderer::advance(const CityState &s, int64_t key, Item &item, const RailTrack *preview) {
	if (item.removed) {
		if (item.pieces.empty()) return true;
		const auto first = item.pieces.begin(); drop(first->second); item.pieces.erase(first); return false;
	}
	const std::array<uint64_t, 5> inputs{s.generation, item.signature, item.extra_signature, s.roads.geometry_revision, s.terrain.revision};
	if (!item.work || item.work->inputs != inputs) {
		item.work = std::make_unique<Work>(); item.work->inputs = inputs;
		item.x0 = item.z0 = std::numeric_limits<float>::max();
		item.x1 = item.z1 = std::numeric_limits<float>::lowest();
	}
	Work &work = *item.work;
	if (work.finished) {
		const auto next = work.retired_after ? item.pieces.upper_bound(*work.retired_after) : item.pieces.begin();
		if (next == item.pieces.end()) { item.work.reset(); return true; }
		work.retired_after = next->first;
		if (next->second.source != work.source) { drop(next->second); item.pieces.erase(next); }
		return false;
	}
	auto reach = [&](const V2 &p, float pad) {
		item.x0 = std::min(item.x0, p.x - pad); item.z0 = std::min(item.z0, p.y - pad);
		item.x1 = std::max(item.x1, p.x + pad); item.z1 = std::max(item.z1, p.y + pad);
	};
	const int id = int(key / 2);
	if (preview || key % 2 == 0) {
		const RailTrack *track = preview ? preview : s.transit.track(id);
		if (!track) { work.finished = true; return false; }
		// Input segment setup reads at most two points. Keep original full-point reach, including tiny skipped legs.
		if (work.cursor.pieces == 0 && work.cursor.point < track->points.size()) {
			reach(track->points[work.cursor.point - 1], look_.ballast.wide);
			reach(track->points[work.cursor.point], look_.ballast.wide);
		} else if (track->points.size() == 1) reach(track->points.front(), look_.ballast.wide);
		size_t stretch = 0;
		const TransitTrackStep progress = transit_track_step(s, look_, *track, work.cursor, work.meshes, stretch);
		if (progress == TransitTrackStep::READY) {
			Piece &part = item.pieces[stretch]; draw(work.meshes, part); part.source = work.source;
			if (preview && part.cut_instance.is_valid()) RenderingServer::get_singleton()->instance_set_visible(part.cut_instance, true);
			work.meshes = TransitMeshes();
		} else if (progress == TransitTrackStep::FINISHED) work.finished = true;
	} else {
		if (const TransitStop *stop = s.transit.stop(id)) {
			transit_stop_parts(s, look_, *stop, work.meshes);
			Piece &part = item.pieces[0]; draw(work.meshes, part); part.source = work.source;
			reach(stop->at, CHUNK * HALF); work.meshes = TransitMeshes();
		}
		work.finished = true;
	}
	return false;
}

void TransitRenderer::sync(const CityState &s, BuildBudget &budget) {
	if (!material_.is_valid()) return;
	if (s.transit_revision != plan_seen_) { plan_seen_ = s.transit_revision; see_plan(s); }
	while ((preview_.waiting || !queue_.empty()) && budget.take()) {
		const bool preview = preview_.waiting && (preview_turn_ || queue_.empty());
		preview_turn_ = !preview_turn_;
		if (preview) {
			if (advance(s, 0, preview_, &preview_track_)) preview_.waiting = false;
			continue;
		}
		const int64_t key = queue_.front(); queue_.pop_front();
		const auto it = items_.find(key);
		if (it == items_.end()) continue;
		if (advance(s, key, it->second)) {
			it->second.waiting = false;
			if (it->second.removed) items_.erase(it);
		} else queue_.push_back(key);
	}
}

void TransitRenderer::set_xray(bool on) {
	if (xray_on_ == on) return;
	xray_on_ = on;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (auto &e : items_)
		for (const auto &part : e.second.pieces)
			if (part.second.cut_instance.is_valid()) rs->instance_set_visible(part.second.cut_instance, on);
}

void TransitRenderer::set_preview(const CityState &, std::vector<V2> points, TransitMode mode) {
	bool same = preview_track_.mode == mode && preview_track_.points.size() == points.size();
	if (same)
		for (size_t i = 0; i < points.size(); i++)
			if (std::memcmp(&preview_track_.points[i].x, &points[i].x, sizeof(float)) != 0
				|| std::memcmp(&preview_track_.points[i].y, &points[i].y, sizeof(float)) != 0) { same = false; break; }
	if (same) return;
	preview_track_.mode = mode; preview_track_.points = std::move(points);
	preview_.work.reset(); preview_.signature = next_generation();
	if (preview_track_.points.size() < 2 || !material_.is_valid()) {
		for (auto &part : preview_.pieces) drop(part.second);
		preview_.pieces.clear(); preview_.waiting = false; return;
	}
	preview_.waiting = true;
}

void TransitRenderer::update(const PoseFrame &f, const Vector3 &cam, const PoseSample &at) {
	if (!car_.valid()) return;
	cars_.begin();
	const float r2 = radius_ * radius_;
	drawn_cars_ = 0;
	for (const TrainRecord &rec : f.trains) {
		if (rec.id != 0) continue; // the metro runs under the ground
		const Pose p = record_pose(rec, at);
		const float dx = p.x - cam.x, dz = p.z - cam.z;
		if (dx * dx + dz * dz > r2) continue;
		const Color line = Color::hex((rec.look << RGB_BITS) | OPAQUE);
		float row[MM_STRIDE];
		mm_row(row, Vector3(p.fz, 0, -p.fx), Vector3(0, 1, 0), Vector3(p.fx, 0, p.fz), Vector3(p.x, p.y, p.z), line, 0, 0, 0, 0);
		cars_.put(rec.row_id, row, instance_bounds(row, car_bounds_));
		drawn_cars_++;
	}
	cars_.finish();
}

} // namespace city
