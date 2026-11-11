// The engine's string table and hash-consed terms: an open-addressing table of term ids keyed by each term's content.
#include "terms.h"

#include <algorithm>

namespace ftengine {

namespace {
constexpr size_t kFirstSlots = size_t{1} << 16;   // the table's first size (a power of two)
constexpr uint64_t kMix = 0x9E3779B97F4A7C15ULL;  // the golden ratio's fraction, Fibonacci hashing's multiplier
constexpr int kShift = 29;

uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v + kMix + (h << 6) + (h >> 2);
    return h * kMix ^ (h >> kShift);
}
}  // namespace

Store::Store() : slots_(kFirstSlots, 0) {
    string("");                                    // id 0: the empty string, the head of every term but an object
}

uint32_t Store::string(std::string_view text) {
    auto found = string_ids_.find(text);
    if (found != string_ids_.end()) {
        return found->second;
    }
    auto id = static_cast<uint32_t>(texts_.size());
    texts_.emplace_back(text);
    string_ids_.emplace(texts_.back(), id);
    return id;
}

uint64_t Store::hash_of(Shape shape, uint32_t head, std::span<const Value> items) {
    uint64_t h = mix(static_cast<uint64_t>(shape), head);
    for (Value v : items) {
        h = mix(h, v.bits());
    }
    return h;
}

bool Store::equal(uint32_t id, Shape shape, uint32_t head, std::span<const Value> items) const {
    const Rec& r = recs_[id];
    if (r.shape != shape || r.head != head || r.count != items.size()) {
        return false;
    }
    return std::equal(items.begin(), items.end(), pool_.begin() + static_cast<ptrdiff_t>(r.first));
}

uint32_t Store::term(Shape shape, uint32_t head, std::span<const Value> items) {
    if (shape == Shape::Set || shape == Shape::FrozenSet) {
        scratch_.assign(items.begin(), items.end());
        std::sort(scratch_.begin(), scratch_.end());
        scratch_.erase(std::unique(scratch_.begin(), scratch_.end()), scratch_.end());
        items = scratch_;
    }
    uint64_t h = hash_of(shape, head, items);
    size_t mask = slots_.size() - 1;
    for (size_t at = h & mask;; at = (at + 1) & mask) {
        uint32_t slot = slots_[at];
        if (slot == 0) {
            auto id = static_cast<uint32_t>(recs_.size());
            recs_.push_back({shape, head, pool_.size(), static_cast<uint32_t>(items.size()), h});
            pool_.insert(pool_.end(), items.begin(), items.end());
            slots_[at] = id + 1;
            if (recs_.size() * 2 > slots_.size()) {
                grow();
            }
            return id;
        }
        if (recs_[slot - 1].hash == h && equal(slot - 1, shape, head, items)) {
            return slot - 1;
        }
    }
}

void Store::grow() {
    std::vector<uint32_t> slots(slots_.size() * 2, 0);
    size_t mask = slots.size() - 1;
    for (uint32_t id = 0; id < recs_.size(); ++id) {
        size_t at = recs_[id].hash & mask;
        while (slots[at] != 0) {
            at = (at + 1) & mask;
        }
        slots[at] = id + 1;
    }
    slots_.swap(slots);
}

}  // namespace ftengine
