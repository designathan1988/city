// A renderer's chunk results queue (ChunkResults): the workers' inbox, the results waiting for their bake's admission, the
// bakes on a worker and the results ready to hand over, each act offered in the frame's one order (frame_admission.h). The
// pieces it is made of (the bake ledger, the chunks' revisions, the inbox, the tail sort) are chunk_jobs.h.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_jobs.h"
#include "render/frame_admission.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace city {

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
