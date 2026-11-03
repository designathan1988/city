// Builds the street furniture chunk meshes.
#include "render/props/prop_renderer.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
} // namespace

bool PropRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("prop", errors);
	look_ = &look;
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!t.has("props")) {
		errors.push_back("streets.json: falta props");
		return false;
	}
	return furniture_.load(t, errors) && kit_.build(t["props"], errors);
}

void PropRenderer::reset() {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
	}
	for (auto &kv : chunks_) kv.second.free();
	chunks_.clear();
}

PropRenderer::Built PropRenderer::build_cpu(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	std::vector<PropPlace> props;
	place_street_props(s, *look_, furniture_, key, props, out.mb);
	for (const PropPlace &p : props) PropKit::append(out.mb, kit_.mesh(p.kind), p.basis, p.origin);
	return out;
}

void PropRenderer::commit_one(Built &b) {
	MeshDraw &d = chunks_[b.key];
	d.reset(scenario_, material_, true);
	if (!b.mb.empty()) b.mb.commit_to(d.mesh);
	float x0 = key_x(b.key) * CHUNK, z0 = key_y(b.key) * CHUNK;
	RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, AABB(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK)));
	d.visible(!b.mb.empty());
}

void PropRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	Built b = build_cpu(s, key);
	commit_one(b);
}

void PropRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
	}
	pool.submit(key * 4 + 1, priority, [this, &s, key, seq]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = build_cpu(s, key);
		b.seq = seq;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void PropRenderer::commit(const std::chrono::steady_clock::time_point &until) {
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

int PropRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return int(requested_.size() + done_.size());
}

void PropRenderer::drop_chunk(int64_t key) {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		requested_.erase(key);
	}
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	it->second.free();
	chunks_.erase(it);
}

} // namespace city
