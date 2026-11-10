// The job queue of a renderer whose chunk results are one surface each (roads, verges, structures, far roads, furniture): a job
// fills the chunk's builder while it holds the map shared (submit_job), its bake is admitted by the frame's upload ledger and its
// hand-over by the frame's deadline, in the frame's one order (chunk_jobs.h ChunkResults). What a result carries besides its
// surface (`Extra`: the furniture's instances) travels inside it, so it is drawn with the mesh of the same build or not at all.
#pragma once

#include "render/chunk_jobs.h"

#include <type_traits>

namespace city {

// What a chunk's mesh holds, kept once the builder is gone (the probes count it).
struct MeshCount {
	int64_t triangles = 0, vertices = 0, bytes = 0;
};

struct NoExtra {};

// A chunk's result: the builder its job filled, then the mesh baked from it, and what the job placed besides.
template <typename Extra>
struct BuiltMeshOf {
	int64_t key = 0;
	JobKey job;
	float priority = 0;
	uint64_t ticket = 0; // the chunk's revision asked (ChunkRevisions)
	MeshBuilder mb;
	OwnedMesh mesh;
	MeshCount count;
	godot::AABB bounds; // of the vertices the job emitted
	Extra extra;
	bool baked = false;
	bool bakes() const { return true; }
	int64_t bytes() const { return mb.bytes(); }
	void bake() {
		mesh = mb.bake();
		count = { mb.idx.size() / 3, mb.v.size(), mb.bytes() };
		bounds = mb.bounds();
		mb = MeshBuilder();
		baked = true;
	}
};
using BuiltMesh = BuiltMeshOf<NoExtra>;

// The queue of such a renderer: it fills a chunk's builder (submit_job) and hands the baked mesh to its instance (commit_one).
template <typename Extra>
class MeshQueueOf : public ChunkResults<BuiltMeshOf<Extra>> {
public:
	using Built = BuiltMeshOf<Extra>;
	MeshQueueOf() {
		this->hand_one = [this](Built &b) { commit_one(b); };
	}
	MeshQueueOf(const MeshQueueOf &) = delete;
	MeshQueueOf &operator=(const MeshQueueOf &) = delete;

protected:
	virtual ~MeshQueueOf() = default;
	virtual void commit_one(Built &b) = 0;
	// `build(mb)` (or `build(mb, extra)`) fills the chunk's builder; it runs on a worker, the map held shared while it reads.
	template <typename Build>
	void submit_job(const CityState &s, JobPool &pool, JobKey job, int64_t key, float priority, Build &&build) {
		this->set_pool(&pool);
		const uint64_t ticket = this->revs.ask(key);
		pool.submit(job, priority, [&s, this, key, job, ticket, priority, build]() {
			auto b = std::make_shared<Built>();
			b->key = key;
			b->job = job;
			b->ticket = ticket;
			b->priority = priority;
			{
				MapRead map(s);
				if constexpr (std::is_same_v<Extra, NoExtra>) build(b->mb);
				else build(b->mb, b->extra);
			}
			this->push(std::move(b));
		});
	}
	// A job in two steps: `gather()` copies what the build reads of the map (it takes the map itself, shared, for the copy alone:
	// sim/state/city_cut.h cut_city) and `build(cut, mb)` (or `build(cut, mb, extra)`) fills the chunk's builder from the copy,
	// on the same worker, with no lock held.
	template <typename Gather, typename Build>
	void submit_cut(JobPool &pool, JobKey job, int64_t key, float priority, Gather &&gather, Build &&build) {
		this->set_pool(&pool);
		const uint64_t ticket = this->revs.ask(key);
		pool.submit(job, priority, [this, key, job, ticket, priority, gather, build]() {
			auto b = std::make_shared<Built>();
			b->key = key;
			b->job = job;
			b->ticket = ticket;
			b->priority = priority;
			const auto cut = gather();
			if constexpr (std::is_same_v<Extra, NoExtra>) build(cut, b->mb);
			else build(cut, b->mb, b->extra);
			this->push(std::move(b));
		});
	}
	void forget(int64_t key) { this->drop(key); } // the chunk left: a result still on its way is dropped when it lands
	void clear_queue() { this->clear(); }
};
using MeshQueue = MeshQueueOf<NoExtra>;

} // namespace city
