// The per-chunk job queues. A job builds a chunk's surfaces on a worker (the one-surface queue of roads and furniture is
// render/mesh_queue.h). Converting them into meshes (bake) runs on a worker too, but the RenderingServer runs a worker's upload at
// the main thread's next call into it (Godot server_wrap_mt_common.h: flush_if_pending), and bakes that started in different
// frames can finish together: so a bake is admitted only while the bytes still baking leave room in the frame's upload ledger
// (frame_admission.h UploadBudget), one bake per chunk at a time, and baked results go to their instances within the frame's
// deadline. Each chunk's revisions (ChunkRevisions) say which result may still be drawn: a newer result than the one shown,
// never one from before the chunk was dropped.
#pragma once

#include "core/job_pool.h"
#include "render/frame_admission.h"
#include "render/mesh_builder.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace city {

// The bakes of a queue still on a worker (main thread only): per chunk, the result's seq and bytes.
struct Baking {
	uint64_t seq = 0;
	int64_t bytes = 0;
};
using BakingSet = std::unordered_map<int64_t, Baking>;

inline int64_t baking_bytes(const BakingSet &set) {
	int64_t n = 0;
	for (const auto &kv : set) n += kv.second.bytes;
	return n;
}

// The revisions of each chunk of a renderer (main thread only). `mark` wants a chunk built again; `ask` gives the job its
// ticket and holds the chunk in flight, so a chunk under a continuous brush is asked once and asked again when its result
// lands, never cancelled and resubmitted every frame (which starved it); the wanted mark stays meanwhile. A result is drawn when
// its ticket is newer than the one shown and not older than the chunk's birth (asked again after it was dropped).
class ChunkRevisions {
public:
	void mark(int64_t key) { wanted_.insert(key); }
	bool wanted(int64_t key) const { return wanted_.count(key) != 0; }
	const std::unordered_set<int64_t> &wanted() const { return wanted_; }
	void unwant(int64_t key) { wanted_.erase(key); }
	bool busy(int64_t key) const {
		auto it = revs_.find(key);
		return it != revs_.end() && it->second.flight;
	}
	bool known(int64_t key) const { return revs_.count(key) != 0; } // asked since its birth
	bool shown_any(int64_t key) const { // a result of it stands drawn
		auto it = revs_.find(key);
		return it != revs_.end() && it->second.shown != 0;
	}
	uint64_t ask(int64_t key) {
		Rev &r = revs_[key];
		r.asked = ++ticket_;
		if (r.born == 0) r.born = r.asked;
		r.flight = true;
		wanted_.erase(key);
		return r.asked;
	}
	bool fresh(int64_t key, uint64_t ticket) const {
		auto it = revs_.find(key);
		return it != revs_.end() && ticket >= it->second.born && ticket > it->second.shown;
	}
	// The result of `ticket` left the pipeline (drawn or retired): the chunk may be asked again.
	void land(int64_t key, uint64_t ticket) {
		auto it = revs_.find(key);
		if (it != revs_.end() && it->second.asked == ticket) it->second.flight = false;
	}
	void show(int64_t key, uint64_t ticket) {
		auto it = revs_.find(key);
		if (it != revs_.end()) it->second.shown = ticket;
	}
	void drop(int64_t key) {
		revs_.erase(key);
		wanted_.erase(key);
	}
	void clear() {
		revs_.clear();
		wanted_.clear();
	}
	size_t flying() const {
		size_t n = 0;
		for (const auto &kv : revs_) n += kv.second.flight ? 1 : 0;
		return n;
	}

private:
	struct Rev {
		uint64_t asked = 0, shown = 0, born = 0;
		bool flight = false;
	};
	std::unordered_map<int64_t, Rev> revs_;
	std::unordered_set<int64_t> wanted_;
	uint64_t ticket_ = 0;
};

// `list` (sorted by `less` up to some point, new arrivals after) sorted again: the tail sorted and merged, never the whole.
template <typename T, typename Less>
void keep_sorted(std::vector<T> &list, Less &&less) {
	auto tail = std::is_sorted_until(list.begin(), list.end(), less);
	if (tail == list.end()) return;
	std::sort(tail, list.end(), less);
	std::inplace_merge(list.begin(), tail, list.end(), less);
}

// Built: key, job, priority, seq, baked, bytes() and bake() (BuildingRenderer::Built / SuperBuilt). The queue holds pointers: a
// building result is heavy (its shells, then its meshes). `waiting` gets the bytes of the built results left waiting for the
// window (RenderWorld holds new builds back while they pile up: their shells are CPU memory).
template <typename Built>
using Results = std::vector<std::shared_ptr<Built>>;

// Where a commit_baked call spends its time, ms, summed (render_cost probe): the locked intake, freeing the stale, the
// ordering, the admissions, the hand-over of baked results to their instances and the tail (the rest put back under the lock).
struct CommitTimes {
	double scan = 0, freed = 0, sort = 0, admit = 0, hand = 0, tail = 0;
};

// The building queues' commit (their newest-seq protocol, `requested` written by the main thread alone): the results taken
// under the lock, the stale retired outside it, the rest kept in priority order (the carried ones already are: only the
// arrivals are sorted), bakes admitted by the ledger, baked results handed over while the deadline lasts.
template <typename Built, typename Commit>
void commit_baked(JobPool &pool, std::mutex &mutex, Results<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		BakingSet &baking, UploadBudget &upload, const std::chrono::steady_clock::time_point &until, int64_t &waiting,
		Commit &&commit_one, CommitTimes *times = nullptr) {
	Results<Built> local, later;
	waiting = 0;
	using clock = std::chrono::steady_clock;
	auto mark = clock::now();
	auto lap = [&](double CommitTimes::*part) {
		const auto now = clock::now();
		if (times) times->*part += std::chrono::duration<double, std::milli>(now - mark).count();
		mark = now;
	};
	{
		std::lock_guard<std::mutex> lk(mutex);
		local.swap(done);
	}
	lap(&CommitTimes::scan);
	size_t kept = 0;
	for (std::shared_ptr<Built> &b : local) {
		if (b->baked) { // back from its bake: its upload is queued for the next call into the RenderingServer
			auto it = baking.find(b->key);
			if (it != baking.end() && it->second.seq == b->seq) baking.erase(it);
		}
		auto it = requested.find(b->key);
		if (it == requested.end() || it->second != b->seq) {
			b.reset(); // newer on its way, or gone: freed here, outside the lock
			continue;
		}
		local[kept++] = std::move(b);
	}
	local.resize(kept);
	lap(&CommitTimes::freed);
	keep_sorted(local, [](const std::shared_ptr<Built> &a, const std::shared_ptr<Built> &b) { return a->priority < b->priority; });
	lap(&CommitTimes::sort);
	for (std::shared_ptr<Built> &b : local) {
		if (!b->baked) {
			const int64_t bytes = b->bytes();
			if (baking.count(b->key) || !upload.take(bytes)) {
				waiting += bytes;
				later.push_back(std::move(b));
				continue;
			}
			baking[b->key] = Baking{ b->seq, bytes };
			pool.submit(b->job.bake(), b->priority, [job = std::move(b), &mutex, &done]() {
				job->bake();
				std::lock_guard<std::mutex> lk(mutex);
				done.push_back(job);
			});
			lap(&CommitTimes::admit);
		} else if (clock::now() < until) {
			requested.erase(b->key);
			commit_one(*b);
			lap(&CommitTimes::hand);
		} else {
			later.push_back(std::move(b));
		}
	}
	local.clear(); // the handed results (their meshes went to the instances)
	{
		std::lock_guard<std::mutex> lk(mutex);
		// carried first, in order: the workers' arrivals of this frame (if any) come after them and are sorted next frame
		later.insert(later.end(), std::make_move_iterator(done.begin()), std::make_move_iterator(done.end()));
		done.swap(later);
	}
	lap(&CommitTimes::tail);
}

// A results inbox the workers push into and the main thread empties (one lock each way).
template <typename Built>
class Inbox {
public:
	void push(Built &&b) {
		std::lock_guard<std::mutex> lk(mutex_);
		items_.push_back(std::move(b));
	}
	void take(std::vector<Built> &out) {
		std::lock_guard<std::mutex> lk(mutex_);
		out.swap(items_);
	}
	void clear() {
		std::vector<Built> gone;
		take(gone); // freed outside the lock
	}

private:
	std::mutex mutex_;
	std::vector<Built> items_;
};

// A renderer's chunk results on their way to the screen: the workers' inbox, the results waiting for their bake's admission
// (those whose upload a worker converts: `bakes()`), the bakes on a worker, the results ready to hand over (in priority order,
// kept so on insert) and the chunks' revisions. Each act is offered in the frame's one order (frame_admission.h); a result
// whose upload runs on the main thread pays its bytes when handed. `Built` has key, job, ticket, priority, baked, bakes(),
// bytes() and bake(); `hand_one` draws one result.
template <typename Built>
class ChunkResults : public AdmittedQueue {
public:
	using Ptr = std::shared_ptr<Built>;
	ChunkRevisions revs;
	std::function<void(Built &)> hand_one;
	void push(Ptr b) { inbox_.push(std::move(b)); } // a worker's finished result (or bake)
	void intake() override {
		std::vector<Ptr> in;
		inbox_.take(in);
		for (Ptr &b : in) {
			if (b->baked) {
				auto it = baking_.find(b->key);
				if (it != baking_.end() && it->second.seq == b->ticket) baking_.erase(it);
			}
			if (!revs.fresh(b->key, b->ticket)) {
				revs.land(b->key, b->ticket);
				continue; // the chunk went, or this is older than what it shows: freed here, outside the lock
			}
			(b->bakes() && !b->baked ? built_ : ready_).push_back(std::move(b));
		}
		keep_sorted(built_, by_priority);
		keep_sorted(ready_, by_priority);
		waiting_ = 0;
		for (const Ptr &b : built_) waiting_ += b->bytes();
	}
	void offer_bakes(std::vector<Offer> &out) override {
		for (size_t i = 0; i < built_.size(); i++) out.push_back(Offer{ rank_of(built_[i]->key), built_[i]->priority, this, i });
	}
	void bake(size_t index, FrameAdmission &f) override {
		Ptr &b = built_[index];
		if (!revs.fresh(b->key, b->ticket)) { // the chunk went since it landed
			acted_bakes_.push_back(index);
			return;
		}
		const int64_t bytes = b->bytes();
		if (!pool_ || baking_.count(b->key) || !f.upload.take(bytes)) return;
		baking_[b->key] = Baking{ b->ticket, bytes };
		waiting_ -= bytes;
		const JobKey job = b->job.bake();
		const float priority = b->priority;
		pool_->submit(job, priority, [job = std::move(b), this]() mutable {
			job->bake();
			inbox_.push(std::move(job));
		});
		acted_bakes_.push_back(index);
	}
	void offer_ready(std::vector<Offer> &out) override {
		for (size_t i = 0; i < ready_.size(); i++) out.push_back(Offer{ rank_of(ready_[i]->key), ready_[i]->priority, this, i });
	}
	void hand(size_t index, FrameAdmission &f) override {
		Ptr &b = ready_[index];
		if (!revs.fresh(b->key, b->ticket)) { // the chunk went since it landed
			acted_ready_.push_back(index);
			return;
		}
		if (!b->bakes() && !f.upload.take(b->bytes())) return; // its upload runs here, on the main thread
		revs.land(b->key, b->ticket);
		revs.show(b->key, b->ticket);
		hand_one(*b);
		b.reset(); // what it held went to the instances
		acted_ready_.push_back(index);
	}
	void settle() override {
		compact(built_, acted_bakes_);
		compact(ready_, acted_ready_);
	}
	void set_pool(JobPool *pool) { pool_ = pool; } // where the bakes run
	void drop(int64_t key) { revs.drop(key); } // the chunk went: its results are retired as they land
	void clear() {
		inbox_.clear();
		built_.clear();
		ready_.clear();
		acted_bakes_.clear();
		acted_ready_.clear();
		baking_.clear();
		revs.clear();
		waiting_ = 0;
	}
	int pending() const { return int(revs.flying()); } // chunks asked whose result is not drawn yet
	int64_t baking() const { return baking_bytes(baking_); } // bytes of its bakes still on a worker
	int64_t waiting() const { return waiting_; } // bytes of its results waiting for a bake (last intake)
	bool busy(int64_t key) const { return revs.busy(key); }
	int rank_of(int64_t key) const { return revs.shown_any(key) ? 1 : 0; } // nothing drawn of the chunk yet: a hole

private:
	static bool by_priority(const Ptr &a, const Ptr &b) { return a->priority < b->priority; }
	static void compact(std::vector<Ptr> &list, std::vector<size_t> &acted) {
		if (acted.empty()) return;
		for (size_t i : acted) list[i].reset();
		list.erase(std::remove(list.begin(), list.end(), nullptr), list.end());
		acted.clear();
	}
	JobPool *pool_ = nullptr;
	Inbox<Ptr> inbox_;
	std::vector<Ptr> built_, ready_;
	std::vector<size_t> acted_bakes_, acted_ready_;
	BakingSet baking_;
	int64_t waiting_ = 0;
};

} // namespace city
