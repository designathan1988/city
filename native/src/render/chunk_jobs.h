// The commit half of the per-chunk job queues: finished mesh builds wait here until the frame budget allows
// uploading them, and a result whose chunk was rebuilt meanwhile (newer seq) is dropped.
#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

template <typename Built, typename Commit>
void commit_done(std::mutex &mutex, std::vector<Built> &done, std::unordered_map<int64_t, uint64_t> &requested,
		const std::chrono::steady_clock::time_point &until, Commit &&commit_one) {
	std::vector<Built> local;
	{
		std::lock_guard<std::mutex> lk(mutex);
		local.swap(done);
	}
	std::sort(local.begin(), local.end(), [](const Built &a, const Built &b) { return a.priority < b.priority; });
	for (Built &b : local) {
		if (std::chrono::steady_clock::now() >= until) {
			std::lock_guard<std::mutex> lk(mutex);
			done.push_back(std::move(b));
			continue;
		}
		bool fresh = false;
		{
			std::lock_guard<std::mutex> lk(mutex);
			auto it = requested.find(b.key);
			fresh = it != requested.end() && it->second == b.seq;
			if (fresh) requested.erase(it);
		}
		if (fresh) commit_one(b);
	}
}

} // namespace city
