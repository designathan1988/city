// The inclusion solver's worklist: nodes whose objects their edges and subscriptions have not all seen, and the first
// applications of new edges and subscriptions; each item hands only the objects not yet handed.
#include "solver.h"

#include <algorithm>

namespace ftengine {

namespace {
constexpr size_t kIndexAt = 16;                  // a set this large gets an index; below it, a scan is cheaper
constexpr uint64_t kMix = 0x9E3779B97F4A7C15ULL; // Fibonacci hashing's multiplier
constexpr int kHalf = 32;

size_t slot_of(Obj o, size_t mask) {
    return static_cast<size_t>((o + uint64_t{1}) * kMix >> kHalf) & mask;
}
}  // namespace

bool Solver::Set::contains(Obj o) const {
    if (slots.empty()) {
        for (Obj x : order) {
            if (x == o) return true;
        }
        return false;
    }
    size_t mask = slots.size() - 1;
    for (size_t at = slot_of(o, mask);; at = (at + 1) & mask) {
        if (slots[at] == 0) return false;
        if (slots[at] == o + 1) return true;
    }
}

bool Solver::Set::insert(Obj o) {
    if (contains(o)) return false;
    order.push_back(o);
    if (order.size() < kIndexAt) return true;
    if (slots.size() < order.size() * 2) {
        size_t size = kIndexAt * 2;
        while (size < order.size() * 4) size *= 2;
        slots.assign(size, 0);
        size_t mask = size - 1;
        for (Obj x : order) {
            size_t at = slot_of(x, mask);
            while (slots[at] != 0) at = (at + 1) & mask;
            slots[at] = x + 1;
        }
        return true;
    }
    size_t mask = slots.size() - 1;
    size_t at = slot_of(o, mask);
    while (slots[at] != 0) at = (at + 1) & mask;
    slots[at] = o + 1;
    return true;
}

size_t Solver::KeyHash::operator()(const Key& k) const {
    uint64_t h = (static_cast<uint64_t>(k.source) << kHalf | k.target) * kMix;
    return static_cast<size_t>(h ^ (k.key * kMix >> 1));
}

Node Solver::node() {
    held_.emplace_back();
    seen_.push_back(0);
    edges_.emplace_back();
    subs_.emplace_back();
    queued_.push_back(0);
    return static_cast<Node>(held_.size() - 1);
}

bool Solver::add(Node n, Obj o) {
    if (!held_[n].insert(o)) return false;
    if (edges_[n].empty() && subs_[n].empty()) {
        seen_[n] = static_cast<uint32_t>(held_[n].order.size());   // nothing to hand them to: seen
    } else {
        queue(n);
    }
    return true;
}

void Solver::add(Node n, std::span<const Obj> objects) {
    for (Obj o : objects) add(n, o);
}

void Solver::queue(Node n) {
    if (!queued_[n]) {
        queued_[n] = 1;
        work_.push_back(n);
    }
}

void Solver::follow(Node source, Node target) {
    if (source == target) return;
    if (!edge_keys_.insert(static_cast<uint64_t>(source) << kHalf | target).second) return;
    edges_[source].push_back(target);
    auto upto = static_cast<uint32_t>(held_[source].order.size());
    if (upto) firsts_.push_back({source, upto, true, target});
}

bool Solver::subscribe(Node source, Node target, uint32_t key, const Transfer& transfer) {
    if (!sub_keys_.insert({source, target, key}).second) return false;
    subs_[source].push_back({target, key, transfer});
    auto upto = static_cast<uint32_t>(held_[source].order.size());
    if (upto) firsts_.push_back({source, upto, false, static_cast<uint32_t>(subs_[source].size() - 1)});
    return true;
}

void Solver::apply_sub(const Sub& sub, std::span<const Obj> objects, Transfers& transfers) {
    transfers.apply(*this, sub.target, sub.transfer, objects);
}

void Solver::deliver(Node source, uint32_t from, uint32_t upto, Transfers& transfers) {
    const auto& order = held_[source].order;
    scratch_.assign(order.begin() + from, order.begin() + upto);
    std::vector<Obj> objects;
    objects.swap(scratch_);                        // a transfer may deliver nothing, but may add to this node
    size_t edges = edges_[source].size(), subs = subs_[source].size();
    if (cost_.size() <= source) cost_.resize(held_.size(), 0);
    cost_[source] += objects.size() * (edges + subs);
    for (size_t i = 0; i < edges; ++i) add(edges_[source][i], objects);
    for (size_t i = 0; i < subs; ++i) {
        Sub sub = subs_[source][i];
        apply_sub(sub, objects, transfers);
    }
    objects.swap(scratch_);
}

std::vector<std::pair<uint64_t, Node>> Solver::take_costs(size_t count) {
    std::vector<std::pair<uint64_t, Node>> rows;
    for (Node n = 0; n < cost_.size(); ++n) {
        if (cost_[n]) rows.emplace_back(cost_[n], n);
    }
    size_t keep = std::min(count, rows.size());
    std::partial_sort(rows.begin(), rows.begin() + keep, rows.end(), std::greater<>());
    rows.resize(keep);
    std::fill(cost_.begin(), cost_.end(), 0);
    return rows;
}

void Solver::run(Transfers& transfers, const std::function<void(uint64_t)>& tick) {
    while (!firsts_.empty() || !work_.empty()) {
        ++items_;
        if (tick && items_ % kTickItems == 0) tick(items_);
        if (!firsts_.empty()) {
            First f = firsts_.front();
            firsts_.pop_front();
            const auto& order = held_[f.source].order;
            std::vector<Obj> objects(order.begin(), order.begin() + f.upto);
            if (f.edge) {
                add(f.index, objects);
            } else {
                Sub sub = subs_[f.source][f.index];
                apply_sub(sub, objects, transfers);
            }
            continue;
        }
        Node n = work_.front();
        work_.pop_front();
        queued_[n] = 0;
        auto upto = static_cast<uint32_t>(held_[n].order.size());
        uint32_t from = seen_[n];
        if (from == upto) continue;
        seen_[n] = upto;
        deliver(n, from, upto, transfers);
    }
}

}  // namespace ftengine
