// The map layers' chunk jobs, their stream and the congestion levels (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "render/data_reader.h"
#include "render/owned_segments.h"
#include "render/roads/road_cut.h"
#include "render/surface_data.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, DROP_K = 1.5f;
constexpr int JAM_STOPS = 3; // the congestion ramp's colours: free, middling, jammed (overlays.json congestion)

float distance_to(int64_t key, float cx, float cz) {
	const float dx = (float(key_x(key)) + HALF) * CHUNK - cx, dz = (float(key_y(key)) + HALF) * CHUNK - cz;
	return std::sqrt(dx * dx + dz * dz);
}
} // namespace

// The ribbons made a mesh whole with their material (surface_data.h), on the worker that bakes.
void OverlayRenderer::Built::bake() {
	if (!r.v.is_empty()) {
		Array arrays;
		arrays.resize(RenderingServer::ARRAY_MAX);
		arrays[RenderingServer::ARRAY_VERTEX] = r.v;
		if (!r.c.is_empty()) arrays[RenderingServer::ARRAY_COLOR] = r.c;
		if (!r.slot.is_empty()) arrays[RenderingServer::ARRAY_TEX_UV] = r.slot;
		mesh = OwnedMesh::of(mesh_of_arrays(arrays, material));
		if (made) made->fetch_add(1);
	}
	r.v = PackedVector3Array();
	r.c = PackedColorArray();
	r.slot = PackedVector2Array();
	baked = true;
}

OverlayRenderer::OverlayRenderer() {
	zone_queue_.hand_one = [this](Built &b) { hand(zones_, b, on_[int(Overlay::ZONES)]); };
	flow_queue_.hand_one = [this](Built &b) {
		levels_.fit(b.r.slots); // before the ribbons are drawn: every segment id they carry has its texel
		hand(flow_, b, on_[int(Overlay::CONGESTION)]);
	};
}

bool OverlayRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("overlay", errors);
	flow_material_ = ctx.material("overlay_flow", errors);
	const Dictionary t = ctx.table("overlays", errors);
	if (!errors.is_empty()) return false;
	const int64_t before = errors.size();
	DataReader r{ errors, "overlays.json" }; // render/data_reader.h: every key present and of its type
	zone_depth_ = float(r.num(t, "zone_depth_m"));
	zone_alpha_ = float(r.num(t, "zone_alpha"));
	zone_alpha_tool_ = float(r.num(t, "zone_alpha_tool"));
	congestion_alpha_ = float(r.num(t, "congestion_alpha"));
	lift_ = float(r.num(t, "lift_m"));
	sample_ = std::max(1.f, float(r.num(t, "sample_m")));
	refresh_ = float(r.num(t, "refresh_s"));
	const float free_share = float(r.num(t, "free_share")), jam_share = float(r.num(t, "jam_share"));
	const Array cols = r.list(t, "congestion", JAM_STOPS);
	if (errors.size() != before) return false;
	// The ramp as the shader mixes it by a level, jammed first: screen colours, as the tokens are (a plain vec3, never converted).
	PackedVector3Array ramp;
	for (int i = JAM_STOPS - 1; i >= 0; i--) {
		const Color c = rgba8_to_color(r.in("congestion").colour_at(cols, i));
		ramp.push_back(Vector3(c.r, c.g, c.b));
	}
	if (errors.size() != before) return false;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(flow_material_, "ramp", ramp);
	rs->material_set_param(flow_material_, "opacity", congestion_alpha_);
	levels_.init(flow_material_, free_share, jam_share);
	return true;
}

void OverlayRenderer::reset() {
	for (auto &kv : zones_) kv.second.free();
	for (auto &kv : flow_) kv.second.free();
	zones_.clear();
	flow_.clear();
	roads_known_.clear();
	zones_all_ = true;
	since_ = refresh_;
	waiting_ = 0;
	zone_queue_.clear();
	flow_queue_.clear();
	levels_.reset();
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
		for (auto &kv : flow_) kv.second.visible(on);
	}
}

void OverlayRenderer::mark_dirty(int64_t key) {
	roads_known_.insert(key);
	if (on_[int(Overlay::ZONES)]) zone_queue_.revs.mark(key);
	if (flow_.count(key) || flow_queue_.revs.busy(key)) flow_queue_.revs.mark(key); // its roads changed: its carriageways are laid again
}

// The box a chunk's ribbons read: the chunk, the segments it owns, as far past them as the widest section's half, the ribbon
// `beside` it and a cell of the ground round its last sample.
CutBox OverlayRenderer::ribbon_box(const CityState &live, int64_t key, float beside) const {
	const float x0 = float(key_x(key)) * CHUNK, z0 = float(key_y(key)) * CHUNK;
	CutBox box{ x0, z0, x0 + CHUNK, z0 + CHUNK };
	each_owned_segment(live, key, [&](int sid) {
		const RoadSegment &g = live.roads.segs[size_t(sid)];
		box.take(g.minx, g.minz, g.maxx, g.maxz);
	});
	return box.grown(widest_half(live.catalog) + beside + live.terrain.spacing);
}

// What chunk `key`'s zone ribbons read, copied under the map's shared lock: its roads with the ground beside them, and the
// player's zoned lots near it (the lot book is no part of a cut: only this job reads it).
OverlayRenderer::ZoneInputs OverlayRenderer::gather_zone(const CityState &s, int64_t key) const {
	ZoneInputs in;
	const double cx = double(key_x(key)) * CHUNK, cz = double(key_y(key)) * CHUNK;
	in.cut = cut_city(
			s, CUT_ROADS, [&](const CityState &live) { return ribbon_box(live, key, zone_depth_); },
			[&](CityCut &, const CityState &live) {
				for (int i : live.lot_book.near(cx, cz, cx + CHUNK, cz + CHUNK)) {
					const PlayerLot &lot = live.lot_book.all()[size_t(i)];
					if (lot.zone != 0) in.lots.push_back(lot);
				}
			});
	return in;
}

void OverlayRenderer::submit(const CityState &s, Overlay o, int64_t key, float priority) {
	const bool zones = o == Overlay::ZONES;
	Queue &queue = zones ? zone_queue_ : flow_queue_;
	queue.set_pool(s.jobs.get());
	const uint64_t ticket = queue.revs.ask(key);
	const JobKey job{ zones ? JobLane::ZONES : JobLane::CONGESTION, key };
	s.jobs->submit(job, priority, [this, &s, &queue, zones, key, job, ticket, priority]() {
		auto b = std::make_shared<Built>();
		b->key = key;
		b->job = job;
		b->ticket = ticket;
		b->priority = priority;
		b->made = &meshes_made_;
		// The job copies what its ribbons read and lays them from the copy, the map let go.
		if (zones) {
			const ZoneInputs in = gather_zone(s, key);
			b->material = material_;
			b->r = build_zone_cpu(in.cut, in.lots, key);
		} else {
			const CityCut cut = cut_city(s, CUT_ROADS, [&](const CityState &live) { return ribbon_box(live, key, 0.f); });
			b->material = flow_material_;
			b->r = lay_congestion(cut, key);
		}
		queue.push(std::move(b));
	});
}

// A baked result shown in place of the chunk's mesh before it (none when the chunk has no ribbon).
void OverlayRenderer::hand(std::unordered_map<int64_t, MeshDraw> &chunks, Built &b, bool on) {
	MeshDraw &d = chunks[b.key];
	const RID next = b.mesh.release();
	d.adopt(scenario_, false, next, LAYER_OVERLAYS);
	d.visible(on && next.is_valid());
}

// The chunks of order_ asked nearest first, a credit each while the frame has them; the rest wait for the next frame.
void OverlayRenderer::offer(const CityState &s, Overlay o, FrameAdmission &admission) {
	std::sort(order_.begin(), order_.end());
	size_t asked = 0;
	for (; asked < order_.size(); asked++) {
		if (!admission.due() || !admission.build.take()) break;
		submit(s, o, order_[asked].second, order_[asked].first);
	}
	waiting_ += int(order_.size() - asked);
}

// The zones: every marked chunk of the map; a chunk whose result is on its way keeps its mark and is asked again when that lands.
void OverlayRenderer::stream_zones(const CityState &s, float cx, float cz, FrameAdmission &admission) {
	if (zones_all_) {
		for (int64_t key : roads_known_) zone_queue_.revs.mark(key);
		zones_all_ = false;
	}
	order_.clear();
	for (int64_t key : zone_queue_.revs.wanted())
		if (!zone_queue_.revs.busy(key)) order_.push_back({ distance_to(key, cx, cz), key });
	offer(s, Overlay::ZONES, admission);
}

// The congestion map: the road chunks within the road reach not laid yet and the laid ones whose roads changed; the chunks the
// eye left are let go.
void OverlayRenderer::stream_flow(const CityState &s, float cx, float cz, FrameAdmission &admission) {
	const float reach = s.catalog.tune.render_road_end_m;
	for (auto it = flow_.begin(); it != flow_.end();) {
		if (distance_to(it->first, cx, cz) <= reach * DROP_K) {
			++it;
			continue;
		}
		flow_queue_.drop(it->first);
		it->second.free();
		it = flow_.erase(it);
	}
	order_.clear();
	std::vector<int64_t> unmark;
	for (int64_t key : flow_queue_.revs.wanted()) {
		if (flow_queue_.revs.busy(key)) continue;
		if (flow_.count(key)) order_.push_back({ distance_to(key, cx, cz), key });
		else unmark.push_back(key); // not drawn: laid fresh when it comes in reach
	}
	for (int64_t key : unmark) flow_queue_.revs.unwant(key);
	const int r = int(std::ceil(reach / CHUNK)), kx = int(std::floor(cx / CHUNK)), kz = int(std::floor(cz / CHUNK));
	for (int j = kz - r; j <= kz + r; j++)
		for (int i = kx - r; i <= kx + r; i++) {
			const int64_t key = cell_key(i, j);
			if (!roads_known_.count(key) || flow_.count(key) || flow_queue_.revs.busy(key)) continue;
			const float d = distance_to(key, cx, cz);
			if (d <= reach) order_.push_back({ d, key });
		}
	offer(s, Overlay::CONGESTION, admission);
}

Dictionary OverlayRenderer::probe() const {
	const auto drawn = [](const std::unordered_map<int64_t, MeshDraw> &chunks) {
		int n = 0;
		for (const auto &kv : chunks) n += kv.second.mesh.is_valid() ? 1 : 0;
		return n;
	};
	Dictionary d;
	d["zone_chunks"] = drawn(zones_);
	d["flow_chunks"] = drawn(flow_);
	d["pending"] = pending();
	d["meshes_made"] = meshes_made_.load();
	levels_.probe(d);
	return d;
}

void OverlayRenderer::update(const CityState &s, const PoseFrame &f, double dt, FrameAdmission &admission) {
	waiting_ = 0;
	if (!s.jobs) return;
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	if (on_[int(Overlay::ZONES)]) stream_zones(s, cx, cz, admission);
	if (on_[int(Overlay::CONGESTION)]) stream_flow(s, cx, cz, admission);
	// both queues, whatever is on: a result that lands after its layer was switched off is still handed (hidden), never left waiting
	admission.commit({ &zone_queue_, &flow_queue_ });
	if (on_[int(Overlay::CONGESTION)]) {
		since_ += dt;
		if (since_ >= refresh_) { // this publication's speeds: the level texture sent, and paid into the ledger, when a level moved
			since_ = 0;
			admission.upload.charge(levels_.refresh(f));
		}
	} else {
		since_ = refresh_;
	}
}

} // namespace city
