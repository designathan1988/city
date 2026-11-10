// See transit_renderer.h. An item is a track or a stop; its signature is what it is drawn from, so a plan's edit makes again only what it moved.
#include "render/transit/transit_renderer.h"

#include "core/grid.h"
#include "render/agents/people_renderer.h"
#include "render/data_reader.h"
#include "render/surface_data.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_set>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;
constexpr int RGB_BITS = 8, RECT_INTS = 4; // a line's colour shifted over its alpha byte; a rectangle's four ints
constexpr uint32_t OPAQUE = 0xFFu; // that alpha byte
constexpr uint64_t HASH_START = 14695981039346656037ull, HASH_STEP = 1099511628211ull; // FNV-1a, 64 bits: its offset basis and prime

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

// `mesh` made again of `surfaces` and shown by `instance` (made the first time); none when there is no surface.
void remake(OwnedMesh &mesh, RID &instance, const TypedArray<Dictionary> &surfaces, const RID &scenario) {
	RenderingServer *rs = RenderingServer::get_singleton();
	OwnedMesh next = OwnedMesh::of(surfaces.is_empty() ? RID() : create_mesh(surfaces));
	if (next.valid()) {
		if (instance.is_valid()) rs->instance_set_base(instance, next.rid);
		else instance = rs->instance_create2(next.rid, scenario);
	}
	if (instance.is_valid()) rs->instance_set_visible(instance, next.valid());
	if (next.valid()) mesh = std::move(next); // the old one goes after the instance left it
}
} // namespace

TransitRenderer::TransitRenderer() {
	queue_.hand_one = [this](Built &b) { hand(b); };
}

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
	if (errors.size() != before) return false;
	preview_look_ = look_;
	for (const auto &[part, opacity] : look_.preview) {
		preview_look_.parts[size_t(part)].colour.a = opacity;
		preview_look_.parts[size_t(part)].glass = opacity < 1;
	}
	TransitMeshes car;
	transit_box(car, look_, look_.car_part, Vector3(0, look_.car_up, 0), V2(0, 1), look_.car);
	TypedArray<Dictionary> surfaces;
	add_surface(surfaces, car.solid, material_);
	car_.reset();
	if (!surfaces.is_empty()) car_.rid = create_mesh(surfaces);
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
	drop(laid_.piece);
	drop(leg_.piece);
	laid_.key.clear();
	leg_.key.clear();
	cars_.free();
	plan_seen_ = ~uint64_t(0);
	pending_ = drawn_cars_ = 0;
}

void TransitRenderer::touch(const std::vector<int64_t> &chunks) {
	if (chunks.empty() || items_.empty()) return;
	const std::unordered_set<int64_t> changed(chunks.begin(), chunks.end());
	for (auto &e : items_) {
		const Item &it = e.second;
		if (!it.drawn) { // its box is not known yet: a result on its way may be of the city before this change, so it is asked again
			if (queue_.revs.busy(e.first)) want(e.first);
			continue;
		}
		bool met = false;
		for (float x = it.x0; x <= it.x1 + CHUNK && !met; x += CHUNK)
			for (float z = it.z0; z <= it.z1 + CHUNK && !met; z += CHUNK) met = changed.count(chunk_of(std::min(x, it.x1), std::min(z, it.z1))) > 0;
		if (met) want(e.first);
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
		want(key);
	};
	for (const RailTrack &t : s.transit.tracks) seen(track_key(t.id), signature_of(t));
	for (const TransitStop &q : s.transit.stops) seen(stop_key(q.id), signature_of(s, q));
	for (auto e = items_.begin(); e != items_.end();) {
		if (alive.count(e->first)) {
			++e;
			continue;
		}
		for (Piece &p : e->second.pieces) drop(p);
		queue_.drop(e->first); // a result on its way is retired when it lands
		e = items_.erase(e);
	}
}

// A baked item shown in place of what stood for it: its pieces' meshes swapped, their instances made, its box known.
void TransitRenderer::hand(Built &b) {
	const auto at = items_.find(b.key);
	if (at == items_.end()) return;
	Item &item = at->second;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (size_t i = b.meshes.size(); i < item.pieces.size(); i++) drop(item.pieces[i]);
	item.pieces.resize(b.meshes.size());
	for (size_t i = 0; i < b.meshes.size(); i++) {
		Piece &p = item.pieces[i];
		drop(p);
		p.mesh = std::move(b.meshes[i].first);
		p.cut = std::move(b.meshes[i].second);
		if (p.mesh.valid()) p.instance = rs->instance_create2(p.mesh.rid, scenario_);
		if (!p.cut.valid()) continue;
		p.cut_instance = rs->instance_create2(p.cut.rid, scenario_);
		rs->instance_geometry_set_cast_shadows_setting(p.cut_instance, RenderingServer::SHADOW_CASTING_SETTING_OFF);
		rs->instance_set_visible(p.cut_instance, xray_on_);
	}
	item.x0 = b.x0;
	item.z0 = b.z0;
	item.x1 = b.x1;
	item.z1 = b.z1;
	item.drawn = true;
}

void TransitRenderer::offer(const CityState &s, const Vector3 &cam, std::vector<FrameAdmission::Unit> &out) {
	pending_ = 0;
	if (!material_.is_valid() || !s.jobs) return;
	if (s.transit_revision != plan_seen_) {
		plan_seen_ = s.transit_revision;
		see_plan(s);
	}
	for (int64_t key : queue_.revs.wanted()) {
		if (queue_.revs.busy(key)) continue; // asked again when its result lands
		const auto at = items_.find(key);
		if (at == items_.end()) continue;
		const Item &item = at->second;
		// an item not drawn yet is a hole in the plan (rank 0); one drawn is made again by its distance
		const float d = item.drawn ? std::hypot((item.x0 + item.x1) * HALF - cam.x, (item.z0 + item.z1) * HALF - cam.z) : 0.f;
		pending_++;
		out.push_back(FrameAdmission::Unit{ item.drawn ? 1 : 0, d, 1, [this, &s, key, d]() {
					   pending_--;
					   submit(s, key, d);
				   } });
	}
}

int TransitRenderer::drawn_items() const {
	int n = 0;
	for (const auto &e : items_)
		n += std::any_of(e.second.pieces.begin(), e.second.pieces.end(), [](const Piece &p) { return p.mesh.valid() || p.cut.valid(); }) ? 1 : 0;
	return n;
}

void TransitRenderer::set_xray(bool on) {
	if (xray_on_ == on) return;
	xray_on_ = on;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (auto &e : items_)
		for (const Piece &p : e.second.pieces)
			if (p.cut_instance.is_valid()) rs->instance_set_visible(p.cut_instance, on);
}

// Into the piece's own meshes and instances: made the first time, cleared and filled again after; an empty one hidden.
void TransitRenderer::refill(const TransitMeshes &m, Piece &out) {
	TypedArray<Dictionary> solid, cut;
	add_surface(solid, m.solid, material_);
	add_surface(solid, m.glass, glass_);
	remake(out.mesh, out.instance, solid, scenario_);
	for (int i = 0; i < TRANSIT_XRAY_LAYERS; i++) add_surface(cut, m.xray[i], xray_[i]);
	remake(out.cut, out.cut_instance, cut, scenario_); // the run being laid shows its cut: the tool that lays it is on
	if (out.cut_instance.is_valid()) RenderingServer::get_singleton()->instance_geometry_set_cast_shadows_setting(out.cut_instance, RenderingServer::SHADOW_CASTING_SETTING_OFF);
}

void TransitRenderer::sketch(const CityState &s, const std::vector<V2> &points, TransitMode mode, Sketch &out) {
	std::vector<int64_t> key{ int64_t(mode) };
	for (const V2 &p : points) {
		key.push_back(int64_t(std::llround(p.x / preview_look_.preview_step)));
		key.push_back(int64_t(std::llround(p.y / preview_look_.preview_step)));
	}
	if (key == out.key) return;
	out.key = std::move(key);
	TransitMeshes m;
	if (points.size() >= 2) {
		RailTrack run;
		run.mode = mode;
		run.points = points;
		transit_track_whole(s, preview_look_, run, m);
	}
	if (points.size() >= 2 || out.piece.mesh.valid()) refill(m, out.piece);
}

// A track's pieces are each a length's own (transit_track_whole): what is laid and the length to the pointer drawn apart are the run
// drawn whole, and a move of the pointer writes only the last. The pointer's own picture: drawn here, by the caller that holds the map.
void TransitRenderer::set_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode) {
	if (!material_.is_valid()) return;
	const size_t n = points.size();
	sketch(s, n >= 2 ? std::vector<V2>(points.begin(), points.end() - 1) : std::vector<V2>(), mode, laid_);
	sketch(s, n >= 2 ? std::vector<V2>(points.end() - 2, points.end()) : std::vector<V2>(), mode, leg_);
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
	cars_.set(scenario_, car_.rid, buf, box, true, LAYER_VEHICLES);
}

} // namespace city
