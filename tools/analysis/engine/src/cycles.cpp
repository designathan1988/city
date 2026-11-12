// The solver's cycles of identity edges (log/trace-research.md F30; Hardekopf & Lin, "The Ant and the Grasshopper",
// PLDI 2007): every node of such a cycle ends holding the same objects, so the cycle is collapsed into one node, its
// representative, which takes every member's objects, edges and subscriptions; each member's edges and subscriptions
// are handed what the union holds that the member had not handed them (a Pending item), never the whole set again.
// The least fixpoint is the same; each object then crosses the cycle once instead of once per member. The cycles are
// found by Tarjan's algorithm (iterative) over the representatives, every `collapse_every` items.
#include <algorithm>

#include "solver.h"

namespace ftengine {

std::vector<std::vector<Node>> Solver::sccs() {
    const auto n = static_cast<uint32_t>(held_.size());
    constexpr uint32_t kUnseen = UINT32_MAX;
    std::vector<uint32_t> index(n, kUnseen), low(n, 0);
    std::vector<uint8_t> on(n, 0);
    std::vector<Node> stack;
    std::vector<std::pair<Node, uint32_t>> calls;           // (node, next edge) of the explicit depth-first search
    std::vector<std::vector<Node>> out;
    uint32_t next = 0;
    for (Node root = 0; root < n; ++root) {
        if (index[root] != kUnseen || find(root) != root) continue;
        calls.push_back({root, 0});
        while (!calls.empty()) {
            Node v = calls.back().first;
            uint32_t& e = calls.back().second;
            if (e == 0 && index[v] == kUnseen) {
                index[v] = low[v] = next++;
                stack.push_back(v);
                on[v] = 1;
            }
            const auto& targets = edges_[v].order;
            if (e < targets.size()) {
                Node w = find(targets[e++]);
                if (w == v) continue;
                if (index[w] == kUnseen) {
                    calls.push_back({w, 0});
                } else if (on[w]) {
                    low[v] = std::min(low[v], index[w]);
                }
                continue;
            }
            calls.pop_back();
            if (!calls.empty()) low[calls.back().first] = std::min(low[calls.back().first], low[v]);
            if (low[v] != index[v]) continue;
            std::vector<Node> scc;
            Node w;
            do {
                w = stack.back();
                stack.pop_back();
                on[w] = 0;
                scc.push_back(w);
            } while (w != v);
            if (scc.size() > 1) out.push_back(std::move(scc));
        }
    }
    return out;
}

std::string Solver::cycles() {
    uint64_t nodes = 0, largest = 0, objects = 0;
    auto found = sccs();
    for (const auto& scc : found) {
        nodes += scc.size();
        largest = std::max<uint64_t>(largest, scc.size());
        for (Node v : scc) objects += held_[v].order.size();
    }
    return std::to_string(nodes) + " nodes in " + std::to_string(found.size()) + " cycles, the largest " +
           std::to_string(largest) + " nodes, " + std::to_string(objects) + " objects held in them; " +
           std::to_string(collapsed_) + " nodes collapsed so far";
}

void Solver::collapse() {
    auto found = sccs();
    if (found.empty()) return;
    std::vector<uint8_t> member(held_.size(), 0);
    for (const auto& scc : found) {
        for (Node v : scc) member[v] = 1;
    }
    std::deque<First> kept;                                // a member's first applications, made Pending items now:
    for (const First& f : firsts_) {                       // their `upto` counts the member's own objects
        if (!member[f.source]) {
            kept.push_back(f);
            continue;
        }
        const auto& own = held_[f.source].order;
        Pending p;
        p.objects.assign(own.begin(), own.begin() + f.upto);
        if (f.edge) {
            p.targets.push_back(f.target);
        } else {
            p.subs.push_back(f.sub);
        }
        pending_.push_back(std::move(p));
    }
    firsts_.swap(kept);
    for (const auto& scc : found) {
        Node r = scc[0];
        Set all;                                           // the union of the members' objects
        for (Node v : scc) {
            for (Obj o : held_[v].order) all.insert(o);
        }
        for (Node v : scc) {                               // what each member's edges and subs have not been handed
            if (edges_[v].order.empty() && subs_[v].empty()) continue;
            Pending p;
            for (Obj o : all.order) {
                if (!held_[v].contains(o)) p.objects.push_back(o);
            }
            const auto& own = held_[v].order;
            p.objects.insert(p.objects.end(), own.begin() + seen_[v], own.end());
            if (p.objects.empty()) continue;
            p.targets = edges_[v].order;
            p.subs = subs_[v];
            pending_.push_back(std::move(p));
        }
        held_[r] = std::move(all);
        seen_[r] = static_cast<uint32_t>(held_[r].order.size());
        for (size_t i = 1; i < scc.size(); ++i) {
            Node v = scc[i];
            rep_[v] = r;
            for (Node t : edges_[v].order) edges_[r].insert(t);
            for (const Sub& s : subs_[v]) insert_sub(r, s);
            held_[v] = Set{};
            edges_[v] = Set{};
            std::vector<Sub>().swap(subs_[v]);
            sub_index_[v].reset();
            ++collapsed_;
        }
        Set targets;                                       // the representative's edges, each to a representative
        for (Node t : edges_[r].order) {
            Node u = find(t);
            if (u != r) targets.insert(u);
        }
        edges_[r] = std::move(targets);
    }
}

}  // namespace ftengine
