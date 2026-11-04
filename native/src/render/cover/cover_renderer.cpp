// Places and streams the ground cover (see cover_renderer.h).
#include "render/cover/cover_renderer.h"

#include "core/math.h"
#include "core/random.h"
#include "render/chunk_jobs.h"
#include "render/trees/tree_place.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float COVER_MIN_Y = -100.f, COVER_SPAN_Y = 700.f, HALF = 0.5f, DROP_CHUNKS = 2.f;
constexpr uint32_t COVER_SALT = 0xc07e12u;
const float DIAGONAL = std::sqrt(2.f);
// Hash streams of one lattice cell.
enum Stream : uint32_t { KEEP = 0, POS_X, POS_Z, SIZE, YAW, TINT, VARIANT };

float h01(uint32_t h, Stream s) { return hash_float(h + s); }

float distance_to(int64_t key, const Vector3 &cam) {
	float dx = (key_x(key) + HALF) * CHUNK - cam.x, dz = (key_y(key) + HALF) * CHUNK - cam.z;
	return std::sqrt(dx * dx + dz * dz);
}

// How much of a kind grows at a point: what the player painted; the grass stands up among the painted scrub and flowers;
// cactus is the biome's own.
float density(const CityState &s, const CoverKit &kit, CoverPiece p, const EcologyField &eco, float x, float z) {
	const GroundPaint &g = s.nature.ground;
	switch (p) {
		case CoverPiece::ROCKS: return g.cover_at(CoverKind::ROCKS, x, z);
		case CoverPiece::SCRUB: return g.cover_at(CoverKind::SCRUB, x, z);
		case CoverPiece::CACTUS: return eco.near(x, z).cactus;
		case CoverPiece::TUFTS: return std::max(g.cover_at(CoverKind::SCRUB, x, z), g.cover_at(CoverKind::FLOWERS, x, z)) * kit.tuft_share;
		default: return g.cover_at(CoverKind::FLOWERS, x, z);
	}
}

bool any_cactus(const EcologyField &eco) {
	return std::any_of(eco.at.begin(), eco.at.end(), [](const Community &c) { return c.cactus > 0; });
}
} // namespace

bool CoverRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	plant_material_ = ctx.material("cover_plant", errors);
	rock_material_ = ctx.material("cover_rock", errors);
	Dictionary t = ctx.table("cover", errors);
	if (!errors.is_empty() || !kit_.build(t, errors) || !models_.load(t.get("elements", Dictionary()), errors)) return false;
	draws_ = 0;
	for (int i = 0; i < COVER_PIECES; i++) {
		first_[size_t(i)] = draws_;
		draws_ += int(kit_.look(CoverPiece(i)).variants.size());
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	for (const RID &m : { plant_material_, rock_material_ }) {
		rs->material_set_param(m, "near_m", kit_.reach_m);
		rs->material_set_param(m, "fade_m", kit_.fade_m);
	}
	return true;
}

void CoverRenderer::reset() {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.clear();
		requested_.clear();
	}
	dirty_.clear();
	for (auto &kv : chunks_)
		for (MultiMeshDraw &d : kv.second) d.free();
	chunks_.clear();
	pending_ = 0;
}

void CoverRenderer::place(const CityState &s, int64_t key, Built &out) const {
	ChunkGround l = chunk_ground(s, key);
	if (l.eco.side == 0) return;
	const bool painted = s.nature.ground.any_cover(), cactus = any_cactus(l.eco);
	for (int p = 0; p < COVER_PIECES; p++) {
		if (CoverPiece(p) == CoverPiece::CACTUS ? !cactus : !painted) continue;
		const CoverLook &k = kit_.look(CoverPiece(p));
		int n = int(CHUNK / k.spacing_m), cx = key_x(key), cz = key_y(key);
		for (int j = 0; j < n; j++)
			for (int i = 0; i < n; i++) {
				int ix = cx * n + i, iz = cz * n + j;
				uint32_t h = hash_combine(hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), COVER_SALT ^ s.seed), uint32_t(p));
				float x = (float(ix) + HALF + (h01(h, POS_X) - HALF)) * k.spacing_m, z = (float(iz) + HALF + (h01(h, POS_Z) - HALF)) * k.spacing_m;
				float d = density(s, kit_, CoverPiece(p), l.eco, x, z);
				if (d < k.least || h01(h, KEEP) > std::sqrt(d)) continue;
				if (!s.terrain.in_bounds(x, z, k.spacing_m) || s.terrain.is_water(x, z, 0.f)) continue;
				if (near_road(s, l, x, z, k.road_margin_m) || in_lot(s, l, k.road_margin_m, x, z)) continue;
				float size = k.size0 + (k.size1 - k.size0) * h01(h, SIZE), yaw = TAU_F * h01(h, YAW);
				float c = std::cos(yaw) * size, sn = std::sin(yaw) * size, tint = 1.f + (h01(h, TINT) * 2.f - 1.f) * k.tint_vary;
				float height = size * k.tall, y = s.terrain.height_at(x, z) - height * k.sink;
				int v = std::min(int(k.variants.size()) - 1, int(h01(h, VARIANT) * float(k.variants.size())));
				mm_push(out.bufs[size_t(first_[size_t(p)] + v)], Vector3(c, 0, -sn), Vector3(0, height, 0), Vector3(sn, 0, c), Vector3(x, y, z), Color(1, 1, 1, 1), tint, tint, tint, 0.f);
			}
	}
}

void CoverRenderer::place_elements(const CityState &s, int64_t key, Built &out) const {
	const std::vector<ElementItem> *items = s.nature.elements.in_chunk(key);
	if (!items) return;
	for (const ElementItem &e : *items) {
		if (e.kind >= models_.kinds()) continue; // an effect: render/element_effects.gd
		int n = models_.variants(e.kind), at = models_.first(e.kind) + std::min(n - 1, int(e.seed * float(n)));
		const ElementModel &m = models_.all()[size_t(at)];
		float k = e.size * m.fit, c = std::cos(e.yaw) * k, sn = std::sin(e.yaw) * k;
		Vector3 foot(e.x, s.terrain.height_at(e.x, e.z) - m.foot * k, e.z);
		mm_push(out.bufs[size_t(draws_ + at)], Vector3(c, 0, -sn), Vector3(0, k, 0), Vector3(sn, 0, c), foot, Color(1, 1, 1, 1), 1, 1, 1, 0);
	}
}

CoverRenderer::Built CoverRenderer::build(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	out.bufs.resize(size_t(draws_) + models_.all().size());
	if (s.nature.nature.region >= 0 || s.nature.ground.any_cover()) place(s, key, out);
	place_elements(s, key, out);
	return out;
}

void CoverRenderer::commit_one(Built &b) {
	std::vector<MultiMeshDraw> &ch = chunks_[b.key];
	ch.resize(size_t(draws_) + models_.all().size());
	AABB box(Vector3(key_x(b.key) * CHUNK, COVER_MIN_Y, key_y(b.key) * CHUNK), Vector3(CHUNK, COVER_SPAN_Y, CHUNK));
	float end = kit_.reach_m + kit_.fade_m + CHUNK * DIAGONAL * HALF;
	for (int p = 0; p < COVER_PIECES; p++) {
		const CoverLook &k = kit_.look(CoverPiece(p));
		for (size_t v = 0; v < k.variants.size(); v++) {
			size_t at = size_t(first_[size_t(p)]) + v;
			ch[at].set(scenario_, k.variants[v]->get_rid(), k.sways ? plant_material_ : rock_material_, b.bufs[at], box, k.casts, LAYER_VEGETATION);
			ch[at].range(0, end);
		}
	}
	for (size_t i = 0; i < models_.all().size(); i++) {
		size_t at = size_t(draws_) + i;
		ch[at].set(scenario_, models_.all()[i].mesh->get_rid(), RID(), b.bufs[at], box, false, LAYER_VEGETATION);
		ch[at].range(0, end);
	}
}

void CoverRenderer::submit(const CityState &s, int64_t key, float priority) {
	if (!s.jobs) return;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		requested_[key] = seq;
	}
	s.jobs->submit(JobKey{ JobLane::COVER, key }, priority, [this, &s, key, seq, priority]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = build(s, key);
		b.seq = seq;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		done_.push_back(std::move(b));
	});
}

void CoverRenderer::commit(const std::chrono::steady_clock::time_point &until) {
	commit_done(queue_mutex_, done_, requested_, until, [this](Built &b) { commit_one(b); });
}

int CoverRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return pending_ + int(dirty_.size() + requested_.size() + done_.size());
}

// Rebuild the dirty chunks, ask the missing ones round the camera, nearest first, and let go of the far.
void CoverRenderer::sync(const CityState &s, const Vector3 &cam, BuildBudget &budget) {
	float reach = kit_.reach_m + kit_.fade_m;
	auto busy = [&](int64_t k) {
		std::lock_guard<std::mutex> lk(queue_mutex_);
		return requested_.count(k) > 0;
	};
	for (auto it = dirty_.begin(); it != dirty_.end();) {
		if (chunks_.count(*it) == 0 || busy(*it)) { it = dirty_.erase(it); continue; }
		if (!budget.take()) break;
		submit(s, *it, distance_to(*it, cam));
		it = dirty_.erase(it);
	}
	for (auto it = chunks_.begin(); it != chunks_.end();) {
		if (distance_to(it->first, cam) <= reach + DROP_CHUNKS * CHUNK) { ++it; continue; }
		{
			std::lock_guard<std::mutex> lk(queue_mutex_);
			requested_.erase(it->first);
		}
		for (MultiMeshDraw &d : it->second) d.free();
		it = chunks_.erase(it);
	}
	int r = int(std::ceil(reach / CHUNK)), cx = int(std::floor(cam.x / CHUNK)), cz = int(std::floor(cam.z / CHUNK));
	std::vector<std::pair<float, int64_t>> want;
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			int64_t k = cell_key(i, j);
			float d = distance_to(k, cam);
			if (d > reach + CHUNK * DIAGONAL * HALF || !s.terrain.in_bounds((i + HALF) * CHUNK, (j + HALF) * CHUNK, -CHUNK * HALF)) continue;
			if (chunks_.count(k) == 0 && !busy(k)) want.push_back({ d, k });
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
