// See transit_renderer.h. An item is a track or a stop; its signature is what it is drawn from, so a plan's edit makes again only what it moved.
#include "render/transit/transit_renderer.h"
#include "render/data_reader.h"

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
constexpr int RGB_BITS = 8, RECT_INTS = 4; // a line's colour shifted over its alpha byte; a rectangle's four ints
constexpr uint32_t OPAQUE = 0xFFu; // that alpha byte
constexpr uint64_t HASH_START = 14695981039346656037ull, HASH_STEP = 1099511628211ull; // FNV-1a, 64 bits: its offset basis and prime

int64_t track_key(int id) { return int64_t(id) * 2; }
int64_t stop_key(int id) { return int64_t(id) * 2 + 1; }

template <typename T>
void mix(uint64_t &h, const T &v) {
	unsigned char bytes[sizeof(T)];
	std::memcpy(bytes, &v, sizeof(T));
	for (unsigned char b : bytes) h = (h ^ b) * HASH_STEP;
}

uint64_t signature_of(const RailTrack &t) {
	uint64_t h = HASH_START;
	mix(h, int(t.mode));
	for (const V2 &p : t.points) {
		mix(h, p.x);
		mix(h, p.y);
	}
	return h;
}

uint64_t signature_of(const CityState &s, const TransitStop &q) {
	uint64_t h = HASH_START;
	mix(h, int(q.mode));
	mix(h, q.at.x);
	mix(h, q.at.y);
	mix(h, q.terminal);
	mix(h, q.track);
	mix(h, q.entrance.x);
	mix(h, q.entrance.y);
	for (const TransitLine &l : s.transit.lines)
		if (std::find(l.stops.begin(), l.stops.end(), q.id) != l.stops.end()) {
			mix(h, l.colour);
			break;
		}
	return h;
}

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
	const int64_t before = errors.size();
	look_.kerb = ctx.catalog->furniture.sidewalk.height_m;
	const DataValue table = DataReader{ errors, "transit.json" }.at(t);
	radius_ = table["radius_m"].f();
	per_frame_ = table["items_per_frame"].whole();
	if (errors.size() != before) return false;
	TransitMeshes car;
	transit_box(car, look_, look_.car_part, Vector3(0, look_.car_up, 0), V2(0, 1), look_.car);
	RenderingServer *rs = RenderingServer::get_singleton();
	car_.reset();
	car_.rid = rs->mesh_create();
	int surface = 0;
	add_surface(rs, car_.rid, car.solid, material_, surface);
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
		for (Piece &p : e.second.pieces) drop(p);
	items_.clear();
	queue_.clear();
	drop(preview_);
	cars_.free();
	plan_seen_ = ~uint64_t(0);
	drawn_cars_ = 0;
}

void TransitRenderer::want(int64_t key, Item &item) {
	if (item.waiting) return;
	item.waiting = true;
	queue_.push_back(key);
}

void TransitRenderer::touch(const std::vector<int64_t> &chunks) {
	if (chunks.empty() || items_.empty()) return;
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
	auto seen = [&](int64_t key, uint64_t signature) {
		alive.insert(key);
		Item &it = items_[key];
		if (it.signature == signature) return;
		it.signature = signature;
		want(key, it);
	};
	for (const RailTrack &t : s.transit.tracks) seen(track_key(t.id), signature_of(t));
	for (const TransitStop &q : s.transit.stops) seen(stop_key(q.id), signature_of(s, q));
	for (auto e = items_.begin(); e != items_.end();) {
		if (alive.count(e->first)) {
			++e;
			continue;
		}
		for (Piece &p : e->second.pieces) drop(p);
		e = items_.erase(e);
	}
}

int64_t TransitRenderer::draw(const TransitMeshes &m, Piece &out) {
	RenderingServer *rs = RenderingServer::get_singleton();
	drop(out);
	int64_t bytes = m.solid.bytes() + m.glass.bytes();
	for (const MeshBuilder &x : m.xray) bytes += x.bytes();
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
	if (!out.cut.valid()) return bytes;
	out.cut_instance = rs->instance_create2(out.cut.rid, scenario_);
	rs->instance_geometry_set_cast_shadows_setting(out.cut_instance, RenderingServer::SHADOW_CASTING_SETTING_OFF);
	rs->instance_set_visible(out.cut_instance, xray_on_);
	return bytes;
}

int64_t TransitRenderer::build(const CityState &s, int64_t key, Item &item) {
	std::vector<TransitMeshes> parts;
	item.x0 = item.z0 = std::numeric_limits<float>::max();
	item.x1 = item.z1 = std::numeric_limits<float>::lowest();
	auto reach = [&](const V2 &p, float pad) {
		item.x0 = std::min(item.x0, p.x - pad);
		item.z0 = std::min(item.z0, p.y - pad);
		item.x1 = std::max(item.x1, p.x + pad);
		item.z1 = std::max(item.z1, p.y + pad);
	};
	const int id = int(key / 2);
	if (key % 2 == 0) {
		for (const RailTrack &t : s.transit.tracks) {
			if (t.id != id) continue;
			transit_track_parts(s, look_, t, parts);
			for (const V2 &p : t.points) reach(p, look_.ballast.wide);
		}
	} else if (const TransitStop *q = s.transit.stop(id)) {
		parts.resize(1);
		transit_stop_parts(s, look_, *q, parts[0]);
		reach(q->at, CHUNK * HALF);
	}
	for (size_t i = parts.size(); i < item.pieces.size(); i++) drop(item.pieces[i]);
	item.pieces.resize(parts.size());
	int64_t bytes = 0;
	for (size_t i = 0; i < parts.size(); i++) bytes += draw(parts[i], item.pieces[i]);
	return bytes;
}

void TransitRenderer::sync(const CityState &s, FrameAdmission &f) {
	if (!material_.is_valid()) return;
	if (s.transit_revision != plan_seen_) {
		plan_seen_ = s.transit_revision;
		see_plan(s);
	}
	for (int n = 0; n < per_frame_ && !queue_.empty() && f.due(); n++) {
		const int64_t key = queue_.front();
		queue_.pop_front();
		const auto it = items_.find(key);
		if (it == items_.end()) continue;
		it->second.waiting = false;
		f.upload.charge(build(s, key, it->second));
	}
}

void TransitRenderer::set_xray(bool on) {
	if (xray_on_ == on) return;
	xray_on_ = on;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (auto &e : items_)
		for (const Piece &p : e.second.pieces)
			if (p.cut_instance.is_valid()) rs->instance_set_visible(p.cut_instance, on);
}

void TransitRenderer::set_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode) {
	drop(preview_);
	if (points.size() < 2 || !material_.is_valid()) return;
	RailTrack run;
	run.mode = mode;
	run.points = points;
	std::vector<TransitMeshes> parts;
	TransitLook one = look_; // the run being laid is one mesh
	one.run = std::numeric_limits<float>::max();
	transit_track_parts(s, one, run, parts);
	if (parts.empty()) return;
	draw(parts[0], preview_);
	if (preview_.cut_instance.is_valid()) RenderingServer::get_singleton()->instance_set_visible(preview_.cut_instance, true);
}

void TransitRenderer::update(const PoseFrame &f, const Vector3 &cam, const PoseSample &at) {
	if (!car_.valid()) return;
	PackedFloat32Array buf;
	const float r2 = radius_ * radius_;
	drawn_cars_ = 0;
	for (const AgentRecord &rec : f.trains) {
		if (rec.id != 0) continue; // the metro runs under the ground
		const Pose p = record_pose(rec, at);
		const float dx = p.x - cam.x, dz = p.z - cam.z;
		if (dx * dx + dz * dz > r2) continue;
		const Color line = Color::hex((rec.look << RGB_BITS) | OPAQUE);
		mm_push(buf, Vector3(p.fz, 0, -p.fx), Vector3(0, 1, 0), Vector3(p.fx, 0, p.fz), Vector3(p.x, p.y, p.z), line, 0, 0, 0, 0);
		drawn_cars_++;
	}
	const AABB box(Vector3(cam.x - radius_, cam.y - AABB_HALF_H, cam.z - radius_), Vector3(radius_ * 2, AABB_HALF_H * 2, radius_ * 2));
	cars_.set(scenario_, car_.rid, RID(), buf, box, true, LAYER_VEHICLES);
}

} // namespace city
