// The analysis over abstract objects (the oracle: featuretrace/heap/analysis.py): nodes and objects keyed by the same
// tuples the Python semantics use, as hash-consed terms; calls found by the objects a callee holds, each a link. The
// semantics modules (one file each, as heap/ has) are free functions over an Analysis; their subscriptions are
// Transfers of a kind with operands, dispatched by transfers.cpp.
#pragma once

#include <algorithm>
#include <deque>
#include <initializer_list>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "flat.h"
#include "input.h"
#include "solver.h"
#include "vocab.h"

namespace ftengine {

inline Value I(int64_t v) { return Value::make(Kind::Int, v); }
inline Value TV(uint32_t id) { return Value::make(Kind::Term, id); }
inline Value B(bool b) { return Value::make(b ? Kind::True : Kind::False, 0); }
inline constexpr Value NONE = Value();

struct Link {
    int64_t src, dst;
    Value kind;
    int64_t line;
};

class Analysis {
public:
    explicit Analysis(Input& in);

    Input& in;
    Store& s;
    Vocab V;
    Solver solver;
    Value ROOT;                                    // the empty context

    // ---- terms --------------------------------------------------------------------------------------------------
    Value tuple(std::initializer_list<Value> items) { return tuple(std::span<const Value>(items.begin(), items.size())); }
    Value tuple(std::span<const Value> items) { return TV(s.term(Shape::Tuple, 0, items)); }
    std::span<const Value> at(Value v) const { return v.kind() == Kind::Term ? s.items(v.id()) : std::span<const Value>(); }
    Value item(Value v, size_t i) const { auto a = at(v); return i < a.size() ? a[i] : NONE; }
    bool truthy(Value v) const;
    bool is_tuple(Value v) const { return v.kind() == Kind::Term && s.shape(v.id()) == Shape::Tuple; }
    bool member_of(Value value, Value collection) const;   // `value in collection` (a sequence, set, dict's keys)
    Value str(std::string_view text) { return Value::make(Kind::Str, s.string(text)); }
    std::string_view text(Value v) const { return v.kind() == Kind::Str ? s.text(v.id()) : std::string_view(); }
    bool starts(Value v, Value prefix) const { return text(v).starts_with(text(prefix)); }
    Value rest(Value v, Value prefix) { return str(text(v).substr(text(prefix).size())); }
    Value lookup_dict(Value dict, Value key) const; // a dict term's value for key, NONE when absent

    // ---- nodes --------------------------------------------------------------------------------------------------
    Node N(Value key);                             // the solver node of a key (made the first time)
    Value key_of(Node n) const { return node_keys_[n]; }
    bool wire_once(Value key) { return wired_.insert(key.bits()); }
    void add(Value node, Obj o) { solver.add(N(node), o); }
    void add(Value node, std::span<const Obj> objs) { solver.add(N(node), objs); }
    void follow(Value source, Value target) { solver.follow(N(source), N(target)); }
    void subscribe(Value source, Value target, Value key, Transfer t);
    std::span<const Obj> held(Value node) { return solver.held(N(node)); }

    Value node(Value tree, const UnitRec& unit, Value ctx);
    Value local(const UnitRec& unit, Value name, Value ctx);
    Value param(int64_t gid, int64_t index, Value ctx) { return tuple({V.PARAM, I(gid), I(index), ctx}); }
    Value this_(int64_t gid, Value ctx) { return tuple({V.THIS, I(gid), ctx}); }
    Value ret(int64_t gid, Value ctx);
    Value hold(int64_t gid);
    Value initial(int64_t gid);
    Value field(Obj obj, Value name) { return tuple({V.FIELD, I(obj), name}); }
    Value any_field(Obj obj) { return field(obj, V.ANY_FIELD); }
    Obj function_object(int64_t gid, Value env);

    // ---- objects ------------------------------------------------------------------------------------------------
    Obj make(Value key);                           // the object of a site key (a tuple, its kind first)
    Obj make(std::initializer_list<Value> key) { return make(tuple(key)); }
    Value okey(Obj o) const { return objects_[o]; }
    Value okind(Obj o) const { return item(objects_[o], 0); }
    size_t objects() const { return objects_.size(); }
    // What an object's key says (heap/objects.py).
    Value class_of(Obj o) const;                  // a `new`'s class, a literal's, a declared object's type
    int64_t maker(Obj o) const;                   // the function object whose `new` made it, -1
    int64_t function(Obj o) const;                // the definition a function object runs, -1
    Value class_itself(Obj o) const;              // the class a class value is
    bool class_env(Obj o, Value& env, int64_t& holder) const;   // the run a class value was evaluated in
    Value env_of(Obj fn) const;                   // a function object's environment
    Value object_heap(Obj o);                     // the heap context it was made in
    Obj object_site(Obj o);                       // its site without that context
    Value declared_env(Obj o) const;              // a declared object's environment, NONE
    Obj primitive(Value kind) { return make({V.SITE_PRIMITIVE, kind}); }
    // The object standing for the instances of a class of the program that come from outside the code read: one.
    Obj from_outside(Value key) { return make({V.SITE_DECLARED, key, tuple({I(-1)}), NONE, ROOT}); }

    // ---- running units ------------------------------------------------------------------------------------------
    const UnitRec& unit(int64_t gid) const { return in.units[static_cast<size_t>(gid)]; }
    int64_t offset(const UnitRec& u) const { return u.gid - u.local_id; }
    void activate(int64_t gid, Value ctx);
    void call(Value node, Obj fn, int64_t receiver, std::span<const Value> args, Value kind, Value site = NONE,
              bool shared = false);           // shared: every such call hands the same arguments (unknown code's)
    Value result(int64_t gid, Value ctx, Value where);
    void reached(Value node, int64_t gid, Value kind);
    void relay(Value origin, Value node);
    void written(Value node, int64_t caller, int64_t line);
    void link(int64_t src, int64_t dst, Value kind, int64_t line);
    Value made_by(Obj obj);                        // a class key, or NONE
    void instance_made(Obj obj);
    Value instances(Value key) { return tuple({V.INSTANCES, key}); }
    bool is_function_unit(int64_t gid) const { return in_set(unit(gid).kind, V.FUNCTION_KINDS); }
    bool is_state(int64_t gid) const { return in_set(unit(gid).kind, V.STATE_KINDS); }
    bool in_set(Value v, Value collection) const { return member_of(v, collection); }

    // The nodes and objects made since the last call, counted by their key's first item (what grows).
    std::string growth();
    std::string largest(size_t count);           // the nodes holding the most objects, named
    std::string composition(Node n);             // the kinds of object a node holds, the most first
    std::string runs(size_t count);              // the functions run in the most contexts, and their nodes
    std::string context_spread(const std::vector<Value>& ctxs);   // how many values each level of contexts takes
    std::string callers(const std::vector<Value>& ctxs);          // the units whose calls the runs come from
    std::string costliest(size_t count);         // the nodes whose deliveries cost the most, named
    std::string stores(size_t count);            // the stores by unknown keys writing the most
    std::string describe(Value v, int depth = 0);  // a key as text, objects by their keys
    std::string site_chain(Value key);             // the objects a key's site is made from, outward
    bool entries(bool modules);
    const std::unordered_set<uint64_t>& named();   // the names the program's code reads or calls
    bool enter(int64_t gid, Value ctx, int64_t unknown);
    void run();

    std::vector<Link> links;
    std::unordered_map<uint64_t, std::unordered_set<uint64_t>> active;     // gid -> contexts (bits)
    std::unordered_map<uint64_t, std::vector<Value>> active_order;         // gid -> contexts, in order
    std::unordered_map<uint64_t, std::unordered_set<uint64_t>> closures;   // gid -> environments (bits)
    std::unordered_map<uint64_t, std::vector<Value>> closure_order;
    // Answers that never change, keyed by what they are asked with (no term made to look one up).
    std::unordered_map<Key3, std::vector<int64_t>, Key3Hash> type_lookups, index_lookups;
    std::unordered_map<uint64_t, std::vector<Value>> alone_groups;        // a key merged with no other: [key]
    std::unordered_map<uint64_t, bool> reads_memo;                         // whether a declared type reads its env
    std::unordered_map<uint64_t, Value> call_sites;                        // a call node -> its static site
    std::deque<std::vector<Value>> own_types_of;   // an object -> its types (once known); a deque: never moved
    std::vector<int8_t> callable_of;               // an object -> whether a call of it runs anything (-1: not known)
    std::vector<int8_t> named_by_index_of;         // an object -> whether every name reads its dictionary part
    bool callable(Obj fn);                         // a function, a declared function, the unknown, a resolver, or
                                                   // an object a type of which has a call signature
    std::vector<uint8_t> own_types_known;
    bool contexts = true;                                                  // false: every run in ROOT
    uint64_t progress_every = 0;
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> store_probe;   // store site -> (bases, values)
    std::unordered_map<Obj, std::vector<Obj>> copies;        // an object -> the objects its properties are copied into
    std::unordered_map<Obj, std::vector<Value>> names_of;    // an object -> the names of its properties made so far
    std::unordered_set<uint64_t> copied;                     // (source, target) pairs copied
    std::unordered_map<Obj, std::vector<std::pair<Value, Obj>>> key_puts;   // an object -> (value node, target) of
                                                                            // each write by a key enumerating it
    std::unordered_map<uint64_t, std::vector<Value>> required_members;   // an object type -> what it requires

private:
    size_t grown_nodes_ = 0, grown_objects_ = 0;
    FlatMap<uint64_t, Node, U64Hash> nodes_;
    std::vector<Value> node_keys_;
    FlatSet<uint64_t, U64Hash> wired_;
    FlatMap<uint64_t, Obj, U64Hash> object_ids_;
    std::vector<Value> objects_;
    std::unordered_map<uint64_t, std::vector<std::pair<int64_t, int64_t>>> lines_;   // node -> (caller, line)
    std::unordered_map<uint64_t, std::vector<std::pair<int64_t, Value>>> targets_;   // node -> (gid, kind)
    std::unordered_map<uint64_t, std::vector<Value>> relays_;
    FlatSet<Key3, Key3Hash> known_, entered_;
    FlatSet<Key3, Key3Hash> has_target_, has_line_, has_relay_;   // the lists above, indexed
    std::unordered_set<uint64_t> named_;
};

// The contexts (heap/contexts.py).
namespace contexts {
Value of_call(Analysis& an, Value env, Value site);
Value of_method(Analysis& an, Value env, Value site, Value heap);
Value environment(Analysis& an, Value ctx);
Value heap(Analysis& an, Value ctx);
}  // namespace contexts

}  // namespace ftengine
