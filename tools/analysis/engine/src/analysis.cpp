// The analysis's core (heap/analysis.py): nodes by key, a unit's runs, calls and their links, the entry points
// (objects.cpp: the objects' keys and the contexts; diagnostics.cpp: what grows and what costs).
#include "analysis.h"

#include "semantics.h"

namespace ftengine {

Analysis::Analysis(Input& input) : in(input), s(input.store), V(input) {
    ROOT = tuple({});
}

bool Analysis::truthy(Value v) const {
    switch (v.kind()) {
    case Kind::None: case Kind::False: return false;
    case Kind::True: case Kind::Class: return true;
    case Kind::Int: return v.data() != 0;
    case Kind::Str: return !s.text(v.id()).empty();
    case Kind::Term: return !s.items(v.id()).empty();
    }
    return false;
}

bool Analysis::member_of(Value value, Value collection) const {
    if (collection.kind() != Kind::Term) return false;
    auto items = s.items(collection.id());
    bool dict = s.shape(collection.id()) == Shape::Dict;
    for (size_t i = 0; i < items.size(); i += dict ? 2 : 1) {
        if (items[i] == value) return true;
    }
    return false;
}

Value Analysis::lookup_dict(Value dict, Value key) const {
    auto items = at(dict);
    for (size_t i = 0; i + 1 < items.size(); i += 2) {
        if (items[i] == key) return items[i + 1];
    }
    return NONE;
}

Node Analysis::N(Value key) {
    bool made = false;
    Node n = nodes_.get(key.bits(), made, [&] { return solver.node(); });
    if (made) node_keys_.push_back(key);
    return n;
}

void Analysis::subscribe(Value source, Value target, Value key, Transfer t) {
    solver.subscribe(N(source), N(target), key.id(), t);
}

Value Analysis::node(Value tree, const UnitRec& u, Value ctx) {
    Value key = tuple({V.TREE, I(u.gid), tree, ctx});
    if (wire_once(key)) trees::wire(*this, key, tree, u, ctx);
    return key;
}

Value Analysis::local(const UnitRec& u, Value name, Value ctx) {
    Value key = tuple({V.LOCAL, I(u.gid), name, ctx});
    if (wire_once(key)) trees::wire_local(*this, key, u, name, ctx);
    return key;
}

Value Analysis::ret(int64_t gid, Value ctx) {
    Value key = tuple({V.RET, I(gid), ctx});
    if (wire_once(key)) {
        const UnitRec& u = unit(gid);
        for (Value tree : u.returns) follow(node(tree, u, ctx), key);
    }
    return key;
}

Value Analysis::hold(int64_t gid) {
    Value key = tuple({V.HOLD, I(gid)});
    if (wire_once(key)) follow(initial(gid), key);
    return key;
}

Value Analysis::initial(int64_t gid) {
    Value key = tuple({V.INITIAL, I(gid)});
    if (wire_once(key)) {
        const UnitRec& u = unit(gid);
        for (Value tree : u.values) follow(node(tree, u, ROOT), key);
        if (u.ambient && truthy(u.result_type) && is_state(gid)) {
            Value site = tuple({tuple({I(gid)}), ROOT});
            follow(values::produce(*this, u.result_type, V.NO_ENV, site), key);
        }
    }
    return key;
}

void Analysis::activate(int64_t gid, Value ctx) {
    if (active[static_cast<uint64_t>(gid)].insert(ctx.bits()).second) {
        active_order[static_cast<uint64_t>(gid)].push_back(ctx);
        refs::lower(*this, unit(gid), ctx);
    }
}

namespace {
Value last(const Analysis& an, Value key) {
    auto items = an.at(key);
    return items.empty() ? NONE : items.back();
}
}  // namespace

bool Analysis::callable(Obj fn) {
    if (callable_of.size() <= fn) callable_of.resize(objects(), -1);
    if (callable_of[fn] >= 0) return callable_of[fn] != 0;
    Value kind = okind(fn);
    bool runs = kind == V.SITE_FUNCTION || kind == V.SITE_DECLARED_FN || kind == V.SITE_UNKNOWN ||
                kind == V.SITE_RESOLVER;
    for (size_t i = 0; !runs && i < typesys::own_types(*this, fn).size(); ++i) {
        runs = !typesys::lookup(*this, typesys::own_types(*this, fn)[i], V.CALL_SIGNATURE).empty();
    }
    callable_of[fn] = runs ? 1 : 0;
    return runs;
}

void Analysis::call(Value node, Obj fn, int64_t receiver, std::span<const Value> args, Value kind, Value site) {
    if (!callable(fn)) return;                     // calling what is no function runs nothing (it throws)
    Value where = site.kind() != Kind::None ? site : declared::call_site(*this, node);
    Value key = okey(fn);
    if (okind(fn) != V.SITE_FUNCTION) {
        declared::call_object(*this, node, fn, receiver, args, where);
        return;
    }
    int64_t gid = item(key, 1).data();
    const UnitRec& u = unit(gid);
    if (u.decl_only) {                         // no body: nothing runs (no context, parameter or result of its own)
        reached(node, gid, kind);
        if (typesys::declared(*this, gid)) declared::apply(*this, node, gid, receiver, args, where);
        return;
    }
    Value env = last(*this, key);
    Value ctx;
    if (!in.contextual.count(gid)) {               // nothing that differs between callers reaches out
        ctx = contexts::of_call(*this, env, V.ONCE);
    } else if (receiver >= 0) {
        auto r = static_cast<Obj>(receiver);
        ctx = contexts::of_method(*this, env, I(object_site(r)), object_heap(r));
    } else {
        ctx = contexts::of_call(*this, env, item(where, 0));
    }
    activate(gid, ctx);
    for (size_t index = 0; index < args.size(); ++index) {
        if (args[index].kind() != Kind::None && index < u.params.size()) {
            follow(args[index], param(gid, static_cast<int64_t>(index), ctx));
        }
    }
    if (receiver >= 0) add(this_(gid, ctx), static_cast<Obj>(receiver));
    follow(result(gid, ctx, where), node);
    reached(node, gid, kind);
}

Value Analysis::result(int64_t gid, Value ctx, Value where) {
    const UnitRec& u = unit(gid);
    if (!u.generator && !u.asynchronous) return ret(gid, ctx);
    Obj made = make({u.generator ? V.SITE_GENERATOR : V.SITE_PROMISE, I(gid), item(where, 0), item(where, 1)});
    Value held = tuple({V.MADE_BY_CALL, I(made)});
    if (wire_once(held)) add(held, made);
    if (u.generator) {
        Value slot = members::field(*this, made, V.ELEMENTS_FIELD);
        for (Value tree : u.yields) follow(node(tree, u, ctx), slot);
    } else {
        protocols::settle(*this, members::field(*this, made, V.RESULT_FIELD), ret(gid, ctx));
    }
    return held;
}

void Analysis::reached(Value node, int64_t gid, Value kind) {
    if (!has_target_.insert({node.bits(), static_cast<uint64_t>(gid), kind.bits()})) return;
    targets_[node.bits()].emplace_back(gid, kind);
    auto lines = lines_[node.bits()];
    for (const auto& [caller, line] : lines) link(caller, gid, kind, line);
}

void Analysis::relay(Value origin, Value node) {
    if (origin.kind() == Kind::None) return;
    if (!has_relay_.insert({origin.bits(), node.bits(), 0})) return;
    relays_[origin.bits()].push_back(node);
    auto lines = lines_[origin.bits()];
    for (const auto& [caller, line] : lines) written(node, caller, line);
}

void Analysis::written(Value node, int64_t caller, int64_t line) {
    if (!has_line_.insert({node.bits(), static_cast<uint64_t>(caller), static_cast<uint64_t>(line)})) return;
    lines_[node.bits()].emplace_back(caller, line);
    auto found = targets_[node.bits()];
    for (const auto& [gid, kind] : found) link(caller, gid, kind, line);
    auto relayed = relays_.find(node.bits());
    if (relayed == relays_.end()) return;
    auto others = relayed->second;
    for (Value other : others) written(other, caller, line);
}

void Analysis::link(int64_t src, int64_t dst, Value kind, int64_t line) {
    uint64_t pair = static_cast<uint64_t>(src) << 32 | static_cast<uint32_t>(dst);
    if (!known_.insert({pair, kind.bits(), static_cast<uint64_t>(line)})) return;
    links.push_back({src, dst, kind, line});
}

bool Analysis::entries(bool modules) {
    Obj unknown = make({V.SITE_UNKNOWN});
    bool started = false;
    std::unordered_set<int64_t> called;
    if (!modules) {
        for (const Link& l : links) {
            if (l.kind == V.CALL || l.kind == V.NEW || l.kind == V.HANDED) called.insert(l.dst);
        }
    }
    for (const UnitRec& u : in.units) {
        if (!is_function_unit(u.gid) || u.decl_only || (u.name == V.MODULE_UNIT) != modules) continue;
        if (modules) {
            started |= enter(u.gid, ROOT, -1);
        } else if (!called.count(u.gid)) {
            auto found = closure_order.find(static_cast<uint64_t>(u.gid));
            std::vector<Value> envs = found != closure_order.end() ? found->second : std::vector<Value>{ROOT};
            for (Value env : envs) started |= enter(u.gid, contexts::of_call(*this, env, I(unknown)), unknown);
        }
    }
    return started;
}

bool Analysis::enter(int64_t gid, Value ctx, int64_t unknown) {
    if (!entered_.insert({static_cast<uint64_t>(gid), ctx.bits(), 0})) return false;
    activate(gid, ctx);
    if (unknown < 0) return true;
    const UnitRec& u = unit(gid);
    auto o = static_cast<Obj>(unknown);
    for (size_t index = 0; index < u.params.size(); ++index) {
        Value tree = index < u.param_types.size() ? u.param_types[index] : tuple({});
        Value p = param(gid, static_cast<int64_t>(index), ctx);
        if (truthy(tree) && item(tree, 0) != V.T_OPEN) {
            Value site = tuple({tuple({I(gid), I(static_cast<int64_t>(index))}), ROOT});
            follow(values::produce(*this, tree, V.NO_ENV, site), p);
        } else {
            add(p, o);
        }
    }
    add(this_(gid, ctx), o);
    if (in.cls(u.owner) && !u.is_static) follow(instances(u.owner), this_(gid, ctx));
    return true;
}

}  // namespace ftengine
