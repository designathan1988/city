// Streams the tree chunks (see tree_renderer.h).
#include "render/trees/tree_renderer.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_MIN_Y = -100.f, AABB_H = 700.f;
constexpr float HALF = 0.5f, EVICT_CHUNKS = 2.f;
} // namespace

bool TreeRenderer::init(const RenderContext &ctx, const StreetLook &look, const FurnitureLook &furniture, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("prop", errors);
	look_ = &look;
	furniture_ = &furniture;
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!t.has("trees")) {
		errors.push_back("streets.json: falta trees");
		return false;
	}
	return trees_.load(t["trees"], errors) && kit_.build(t["trees"], errors);
}

void TreeRenderer::reset() {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
	}
	for (auto &kv : chunks_) {
		for (MultiMeshDraw &d : kv.second.near) d.free();
		for (MultiMeshDraw &d : kv.second.far) d.free();
	}
	chunks_.clear();
	dirty_.clear();
	pending_ = 0;
}

TreeRenderer::Built TreeRenderer::build_cpu(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	std::vector<TreeSpot> spots;
	place_trees(s, *look_, *furniture_, trees_, kit_, key, spots);
	int n = kit_.count();
	out.bufs.resize(static_cast<size_t>(n));
	for (const TreeSpot &t : spots) {
		float c = std::cos(t.yaw) * t.scale, sn = std::sin(t.yaw) * t.scale;
		const Color &tint = trees_.tints[size_t(std::abs(int(t.p.x * HALF) ^ int(t.p.z * HALF))) % trees_.tints.size()];
		mm_push(out.bufs[size_t(t.species)], Vector3(c, 0, -sn), Vector3(0, t.scale, 0), Vector3(sn, 0, c), t.p, Color(1, 1, 1, 1), tint.r,
				tint.g, tint.b, 0);
	}
	return out;
}

void TreeRenderer::commit_one(Built &b) {
	int n = kit_.count();
	Chunk &ch = chunks_[b.key];
	for (MultiMeshDraw &d : ch.near) d.free();
	for (MultiMeshDraw &d : ch.far) d.free();
	ch.near.resize(size_t(n));
	ch.far.resize(size_t(n));
	float x0 = key_x(b.key) * CHUNK, z0 = key_y(b.key) * CHUNK;
	AABB box(Vector3(x0, AABB_MIN_Y, z0), Vector3(CHUNK, AABB_H, CHUNK));
	for (int i = 0; i < n; i++) {
		if (b.near) {
			ch.near[size_t(i)].set(scenario_, kit_.species(i).mesh[0]->get_rid(), material_, b.bufs[size_t(i)], box, true);
			ch.near[size_t(i)].range(0, trees_.near_m);
		}
		ch.far[size_t(i)].set(scenario_, kit_.species(i).mesh[1]->get_rid(), material_, b.bufs[size_t(i)], box, false);
		ch.far[size_t(i)].range(trees_.near_m, trees_.view_m);
	}
	ch.has_near = b.near;
}

void TreeRenderer::submit(const CityState &s, int64_t key, float priority) {
	if (!s.jobs) return;
	bool near = priority <= trees_.near_m + CHUNK;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
	}
	s.jobs->submit(key * 4 + 2, priority, [this, &s, key, seq, near]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = build_cpu(s, key);
		b.seq = seq;
		b.near = near;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void TreeRenderer::commit(const std::chrono::steady_clock::time_point &until) {
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

int TreeRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return int(requested_.size() + done_.size());
}

void TreeRenderer::sync(const CityState &s, const Vector3 &cam, BuildBudget &budget) {
	for (auto it = dirty_.begin(); it != dirty_.end();) {
		{
			std::lock_guard<std::mutex> lk(queue_mutex_);
			if (requested_.count(*it)) { it = dirty_.erase(it); continue; }
		}
		if (!budget.take()) break;
		float dx = (key_x(*it) + HALF) * CHUNK - cam.x, dz = (key_y(*it) + HALF) * CHUNK - cam.z;
		submit(s, *it, std::sqrt(dx * dx + dz * dz));
		it = dirty_.erase(it);
	}
	int r = int(std::ceil(trees_.view_m / CHUNK)), cx = int(std::floor(cam.x / CHUNK)), cz = int(std::floor(cam.z / CHUNK));
	std::vector<std::pair<float, int64_t>> want;
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			float dx = (i + HALF) * CHUNK - cam.x, dz = (j + HALF) * CHUNK - cam.z, d = std::sqrt(dx * dx + dz * dz);
			if (d > trees_.view_m + CHUNK || !s.terrain.in_bounds((i + HALF) * CHUNK, (j + HALF) * CHUNK)) continue;
			int64_t k = cell_key(i, j);
			auto built = chunks_.find(k);
			bool need = built == chunks_.end() || (!built->second.has_near && d <= trees_.near_m + CHUNK);
			if (need) {
				std::lock_guard<std::mutex> lk(queue_mutex_);
				if (!requested_.count(k)) want.push_back({ d, k });
			}
		}
	for (auto it = chunks_.begin(); it != chunks_.end();) {
		float dx = (key_x(it->first) + HALF) * CHUNK - cam.x, dz = (key_y(it->first) + HALF) * CHUNK - cam.z;
		if (std::sqrt(dx * dx + dz * dz) <= trees_.view_m + EVICT_CHUNKS * CHUNK) {
			++it;
			continue;
		}
		{
			std::lock_guard<std::mutex> lk(queue_mutex_);
			requested_.erase(it->first);
		}
		for (MultiMeshDraw &d : it->second.near) d.free();
		for (MultiMeshDraw &d : it->second.far) d.free();
		it = chunks_.erase(it);
	}
	std::sort(want.begin(), want.end());
	int n = 0;
	for (const auto &w : want) {
		if (!budget.take()) break;
		submit(s, w.second, w.first);
		++n;
	}
	pending_ = int(want.size()) - n;
}

} // namespace city
