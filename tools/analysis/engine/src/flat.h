// The engine's sets and maps of keys (log/trace-research.md F22): open addressing with linear probing over one array,
// so an insert allocates nothing but the array's growth (the standard library's node-based tables allocate a node per
// insert, which was a third of the engine's time on the lab). Nothing is ever removed, so no tombstone is needed.
#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace ftengine {

struct U64Hash {
    size_t operator()(uint64_t k) const {
        constexpr uint64_t kMix = 0x9E3779B97F4A7C15ULL;   // Fibonacci hashing's multiplier
        constexpr int kFold = 32;
        k *= kMix;
        return static_cast<size_t>(k ^ (k >> kFold));
    }
};

template <class Key, class Value, class Hash>
class FlatMap {
public:
    // The value of `key`, made from `make()` the first time; `made` tells which.
    template <class Make>
    Value& get(const Key& key, bool& made, Make make) {
        if ((size_ + 1) * 2 > slots_.size()) grow();
        size_t at = slot(key);
        made = !used_[at];
        if (made) {
            used_[at] = 1;
            slots_[at] = {key, make()};
            ++size_;
        }
        return slots_[at].second;
    }
    const Value* find(const Key& key) const {
        if (slots_.empty()) return nullptr;
        size_t at = slot(key);
        return used_[at] ? &slots_[at].second : nullptr;
    }
    size_t size() const { return size_; }

private:
    static constexpr size_t kFirst = 1024;
    size_t slot(const Key& key) const {
        size_t mask = slots_.size() - 1;
        size_t at = Hash()(key) & mask;
        while (used_[at] && !(slots_[at].first == key)) at = (at + 1) & mask;
        return at;
    }
    void grow() {
        std::vector<std::pair<Key, Value>> old = std::move(slots_);
        std::vector<uint8_t> was = std::move(used_);
        size_t size = old.empty() ? kFirst : old.size() * 2;
        slots_.assign(size, {});
        used_.assign(size, 0);
        for (size_t i = 0; i < old.size(); ++i) {
            if (!was[i]) continue;
            size_t at = slot(old[i].first);
            used_[at] = 1;
            slots_[at] = std::move(old[i]);
        }
    }
    std::vector<std::pair<Key, Value>> slots_;
    std::vector<uint8_t> used_;
    size_t size_ = 0;
};

template <class Key, class Hash>
class FlatSet {
public:
    bool insert(const Key& key) {               // true when the key is new
        bool made = false;
        map_.get(key, made, [] { return Empty{}; });
        return made;
    }
    bool contains(const Key& key) const { return map_.find(key) != nullptr; }
    size_t size() const { return map_.size(); }

private:
    struct Empty {};
    FlatMap<Key, Empty, Hash> map_;
};

}  // namespace ftengine
