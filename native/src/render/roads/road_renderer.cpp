// Road chunk meshes: every segment (by its midpoint) and node of a 256 m chunk meshed into one surface drawn with the
// "road" material; the pieces are in road_segment.cpp, road_junction.cpp and road_markings.cpp.
#include "render/roads/road_renderer.h"

#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
} // namespace

bool RoadRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road", errors);
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	return look_.load(t, errors);
}

void RoadRenderer::reset() {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
	}
	for (auto &kv : chunks_) kv.second.surface.free();
	chunks_.clear();
}

RoadRenderer::Built RoadRenderer::build_cpu(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	RoadEmit e{ s, look_, out.mb };
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0 - CHUNK, z0 - CHUNK, x0 + 2 * CHUNK, z0 + 2 * CHUNK, [&](int sid) {
		double mx, mz;
		s.roads.segs[sid].q.at(0.5, mx, mz);
		if (chunk_of(float(mx), float(mz)) != key) return;
		emit_segment(e, sid);
		emit_markings(e, sid);
	});
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key) emit_node(e, n);
	});
	return out;
}

void RoadRenderer::commit_one(Built &b) {
	float x0 = key_x(b.key) * CHUNK, z0 = key_y(b.key) * CHUNK;
	Chunk &c = chunks_[b.key];
	c.surface.reset(scenario_, material_, false);
	if (!b.mb.empty()) b.mb.commit_to(c.surface.mesh);
	RenderingServer::get_singleton()->instance_set_custom_aabb(c.surface.inst, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK)));
	c.surface.visible(!b.mb.empty());
}

void RoadRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	Built b = build_cpu(s, key);
	commit_one(b);
}

void RoadRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
	}
	pool.submit(key * 4 + 0, priority, [this, &s, key, seq]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = build_cpu(s, key);
		b.seq = seq;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void RoadRenderer::commit(const std::chrono::steady_clock::time_point &until) {
	std::vector<Built> local;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		local.swap(done_);
	}
	std::sort(local.begin(), local.end(), [](const Built &a, const Built &b) { return a.priority < b.priority; });
	for (Built &b : local) {
		if (std::chrono::steady_clock::now() >= until) {
			std::lock_guard<std::mutex> lk(queue_mutex_);
			done_.push_back(std::move(b));
			continue;
		}
		bool fresh = false;
		{
			std::lock_guard<std::mutex> lk(queue_mutex_);
			auto it = requested_.find(b.key);
			fresh = it != requested_.end() && it->second == b.seq;
			if (fresh) requested_.erase(it);
		}
		if (fresh) commit_one(b);
	}
}

int RoadRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return int(requested_.size() + done_.size());
}

void RoadRenderer::drop_chunk(int64_t key) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		requested_.erase(key);
	}
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.surface.free();
	chunks_.erase(it);
}

void RoadRenderer::update_view(const Vector3 &) {}

} // namespace city
