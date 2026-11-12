// The inclusion solver (log/trace-engine-plan.md, Task E2; the Python oracle is featuretrace/heap/solver.py): a node
// holds a set of objects that only grows; an identity edge copies a node's objects into another; a subscription hands
// each object new to its source to a transfer, once per (source, target, key). Difference propagation (Pearce, Kelly
// and Hankin): a node keeps its objects in the order they came and how many its edges and subscriptions have seen, so
// the worklist holds nodes, never copies of sets; a new edge or subscription is first applied, as an item of its own,
// to what its source already held. The least fixpoint is the same in any order, as the transfers are monotone.
#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "flat.h"

namespace ftengine {

using Node = uint32_t;
using Obj = uint32_t;

// What a subscription does with new objects: its kind (the semantics' own) and its operands.
struct Transfer {
    uint32_t kind = 0;
    uint32_t a = 0, b = 0, c = 0;
};

class Solver;

// The semantics behind the subscriptions: called with the target node, the transfer and the objects new to the source.
class Transfers {
public:
    virtual ~Transfers() = default;
    virtual void apply(Solver& solver, Node target, const Transfer& transfer, std::span<const Obj> objects) = 0;
};

class Solver {
public:
    Node node();                                           // a new node, holding nothing
    size_t nodes() const { return held_.size(); }
    bool add(Node n, Obj o);                               // true when o is new to n
    void add(Node n, std::span<const Obj> objects);
    void follow(Node source, Node target);
    bool subscribe(Node source, Node target, uint32_t key, const Transfer& transfer);
    // Runs to the fixpoint; `tick` (when given) is called every kTickItems items with the items so far.
    void run(Transfers& transfers, const std::function<void(uint64_t)>& tick = {});
    static constexpr uint64_t kTickItems = uint64_t{1} << 18;
    size_t pending() const { return work_.size() + firsts_.size() + pending_.size(); }
    std::span<const Obj> held(Node n) const { return held_[find(n)].order; }
    bool holds(Node n, Obj o) const { return held_[find(n)].contains(o); }
    Node find(Node n) const {                              // the representative of a node's cycle (itself if none)
        while (rep_[n] != n) n = rep_[n] = rep_[rep_[n]];
        return n;
    }
    void collapse();                                       // every cycle of identity edges into one node (cycles.cpp)
    uint64_t collapse_every = 0;                           // items between two collapses (0: never)
    uint64_t items() const { return items_; }              // worklist items processed so far
    // Per node, objects handed times the edges and subscriptions they were handed to, since the last take.
    std::vector<std::pair<uint64_t, Node>> take_costs(size_t count);
    // The cycles of identity edges (Tarjan, iterative): nodes in a cycle, cycles, the largest's nodes, and the
    // objects held by the nodes of the cycles (what one copy per cycle would save).
    std::string cycles();

private:
    // A node's objects: in the order they came, with an index once they are many.
    struct Set {
        std::vector<Obj> order;
        std::vector<Obj> slots;                            // open addressing: object + 1, 0 empty
        bool contains(Obj o) const;
        bool insert(Obj o);
    };
    struct Sub {
        Node target;
        uint32_t key;
        Transfer transfer;
    };
    struct First {                                         // a new edge's or subscription's first application
        Node source;
        uint32_t upto;                                     // the objects the source held then
        bool edge;
        Node target;                                       // the edge's target
        Sub sub;                                           // or the subscription
    };
    struct Pending {                                       // what a collapse leaves a member's edges and subs to see
        std::vector<Obj> objects;
        std::vector<Node> targets;
        std::vector<Sub> subs;
    };
    void queue(Node n);
    bool insert_sub(Node source, const Sub& sub);          // false when (target, key) is the source's already
    std::vector<std::vector<Node>> sccs();                 // the cycles of identity edges, over representatives
    void deliver(Node source, uint32_t from, uint32_t upto, Transfers& transfers);
    void apply_sub(const Sub& sub, std::span<const Obj> objects, Transfers& transfers);

    mutable std::vector<Node> rep_;                        // per node: the representative it was collapsed into
    uint64_t collapsed_ = 0;
    std::vector<Set> held_;
    std::vector<uint32_t> seen_;                           // per node: how many objects its edges and subs have seen
    std::vector<Set> edges_;                               // per node: its targets, indexed once they are many
    std::vector<std::vector<Sub>> subs_;
    std::vector<std::unique_ptr<FlatSet<uint64_t, U64Hash>>> sub_index_;   // per node: (target, key), once many
    std::vector<uint8_t> queued_;
    std::deque<Node> work_;
    std::deque<First> firsts_;
    std::deque<Pending> pending_;
    std::vector<Obj> scratch_;
    uint64_t items_ = 0;
    std::vector<uint64_t> cost_;
};

}  // namespace ftengine
