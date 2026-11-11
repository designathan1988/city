// Declared types in the analysis (heap/typesys.py): what a type name names where it is written, the declarations
// merged under one global name, a member looked up through them and their bases, the types an object is of, and an
// object's place of a type parameter (linked to its index signatures, its intrinsic's slot and its heritage).
#include "semantics.h"

namespace ftengine::typesys {

namespace {

std::vector<int64_t> lookup_in(Analysis& an, Value key, Value name, std::unordered_set<uint64_t>& seen) {
    key = identity(an, key);
    if (!seen.insert(key.bits()).second) return {};
    std::vector<int64_t> found;
    for (Value k : group(an, key)) {
        const auto& gids = an.in.member(k, name);
        found.insert(found.end(), gids.begin(), gids.end());
    }
    if (!found.empty()) return found;
    for (Value k : group(an, key)) {
        if (!an.in.cls(k)) continue;
        for (Value base : base_keys(an, k)) {
            auto more = lookup_in(an, base, name, seen);
            found.insert(found.end(), more.begin(), more.end());
        }
    }
    return found;
}

void keyed(Analysis& an, Obj obj, Value key, int64_t index, Value node) {
    Value wanted = an.tuple({an.V.T_PARAM, an.V.LEVEL_TYPE, I(index)});
    for (int64_t gid : lookup(an, key, an.V.INDEX_SIGNATURE)) {
        for (const auto& [key_type, value] : an.unit(gid).indexes) {
            auto ps = parts(an, value);
            if (std::find(ps.begin(), ps.end(), wanted) == ps.end()) continue;
            auto vs = values(an, key_type);
            Value held = std::find(vs.begin(), vs.end(), an.V.NUMBER) != vs.end()
                             ? members::field(an, obj, an.V.ELEMENTS_FIELD) : an.field(obj, an.V.ANY_FIELD);
            an.follow(held, node);
            an.follow(node, held);
        }
    }
}

void slotted(Analysis& an, Obj obj, Value key, int64_t index, Value node) {
    Value kind = an.okind(obj);
    Value slot = an.lookup_dict(an.V.SLOTS, kind);
    if (slot.kind() == Kind::None || an.item(slot, 1).data() != index) return;
    auto found = an.in.intrinsics.find(kind.bits());
    if (found == an.in.intrinsics.end()) return;
    if (std::find(found->second.begin(), found->second.end(), key) == found->second.end()) return;
    Value held = members::field(an, obj, an.item(slot, 0));
    an.follow(held, node);
    an.follow(node, held);
}

void inherited(Analysis& an, Obj obj, Value key, int64_t index, Value node) {
    std::unordered_set<uint64_t> seen;
    std::vector<Value> todo = own_types(an, obj);
    while (!todo.empty()) {
        Value sub = todo.back();
        todo.pop_back();
        if (sub == key || !seen.insert(sub.bits()).second) continue;
        for (Value k : group(an, sub)) {
            const ClassRec* c = an.in.cls(k);
            if (!c) continue;
            for (Value base : c->heritage) {
                if (an.item(base, 0) != an.V.T_NAMED) continue;
                Value found = resolve(an, an.item(base, 1), an.item(base, 3), an.item(base, 4));
                if (found.kind() == Kind::None || an.item(found, 0) != an.V.CLASS) continue;
                todo.push_back(an.item(found, 1));
                auto args = an.at(an.item(base, 2));
                if (an.item(found, 1) != key || index >= static_cast<int64_t>(args.size())) continue;
                Value arg = args[static_cast<size_t>(index)];
                if (an.item(arg, 0) == an.V.T_PARAM && an.item(arg, 1) == an.V.LEVEL_TYPE) {
                    Value other = place(an, obj, sub, an.item(arg, 2).data());
                    an.follow(other, node);
                    an.follow(node, other);
                } else {
                    Value site = an.tuple({an.tuple({an.V.T_PARAM, key, I(index)}), an.ROOT});
                    an.follow(values::produce(an, arg, an.tuple({I(obj), sub, NONE}), site), node);
                }
            }
        }
    }
}

}  // namespace

Value resolve(Analysis& an, Value text, Value file, Value owner) { return an.in.resolve(text, file, owner); }

const std::vector<Value>& group(Analysis& an, Value key) {
    static std::unordered_map<uint64_t, std::vector<Value>> alone;
    const ClassRec* c = an.in.cls(key);
    if (c && !c->group.empty()) return c->group;
    auto [found, made] = alone.try_emplace(key.bits());
    if (made) found->second.push_back(key);
    return found->second;
}

Value identity(Analysis& an, Value key) {
    const ClassRec* c = an.in.cls(key);
    return c ? c->identity : key;
}

Value global_type(Analysis& an, Value kind, size_t index) {
    auto found = an.in.intrinsics.find(kind.bits());
    return found != an.in.intrinsics.end() && index < found->second.size() ? found->second[index] : NONE;
}

std::vector<int64_t> lookup(Analysis& an, Value key, Value name) {
    Value memo_key = an.tuple({I(2), key, name});
    auto found = an.memo.find(memo_key.bits());
    std::vector<int64_t> out;
    if (found != an.memo.end()) {
        for (Value v : an.at(found->second)) out.push_back(v.data());
        return out;
    }
    std::unordered_set<uint64_t> seen;
    out = lookup_in(an, key, name, seen);
    std::vector<Value> items;
    for (int64_t g : out) items.push_back(I(g));
    an.memo[memo_key.bits()] = an.tuple(items);
    return out;
}

std::vector<Value> own_types(Analysis& an, Obj obj) {
    std::vector<Value> out;
    Value key = an.class_of(obj);
    if (key.kind() != Kind::None && an.in.cls(key)) out.push_back(identity(an, key));
    Value kind = an.okind(obj);
    Value intrinsic_kind = kind == an.V.SITE_PRIMITIVE ? an.item(an.okey(obj), 1) : kind;
    auto found = an.in.intrinsics.find(intrinsic_kind.bits());
    if (found == an.in.intrinsics.end()) return out;
    for (Value t : found->second) {
        if (t.kind() != Kind::None && std::find(out.begin(), out.end(), t) == out.end()) out.push_back(t);
    }
    return out;
}

bool is_open_type(Analysis& an, Value key) {
    for (Value kind : an.at(an.V.OPEN_INTRINSICS)) {
        auto found = an.in.intrinsics.find(kind.bits());
        if (found == an.in.intrinsics.end()) continue;
        for (Value t : found->second) {
            if (t.kind() != Kind::None && t == key) return true;
        }
    }
    return false;
}

std::vector<Value> ancestors(Analysis& an, Value key) {
    std::vector<Value> seen, todo{key};
    while (!todo.empty()) {
        Value k = todo.back();
        todo.pop_back();
        if (std::find(seen.begin(), seen.end(), k) != seen.end() || !an.in.cls(k)) continue;
        seen.push_back(k);
        for (Value b : base_keys(an, k)) todo.push_back(b);
    }
    return seen;
}

bool has_member(Analysis& an, Obj obj, Value name) {
    for (Value key : own_types(an, obj)) {
        if (!lookup(an, key, name).empty()) return true;
    }
    return false;
}

bool declared(Analysis& an, int64_t gid) {
    const UnitRec& unit = an.unit(gid);
    const ClassRec* c = an.in.cls(unit.owner);
    return unit.ambient || (c && c->shape);
}

Value place(Analysis& an, Obj obj, Value key, int64_t index) {
    Value node = an.field(obj, an.tuple({an.V.T_PARAM, key, I(index)}));
    if (an.wire_once(node)) {
        keyed(an, obj, key, index, node);
        slotted(an, obj, key, index, node);
        inherited(an, obj, key, index, node);
    }
    return node;
}

std::vector<Value> parts(Analysis& an, Value tree) {
    Value kind = an.item(tree, 0);
    if (an.truthy(tree) && (kind == an.V.T_UNION || kind == an.V.T_BOTH)) {
        std::vector<Value> out;
        for (Value part : an.at(an.item(tree, 1))) {
            auto more = parts(an, part);
            out.insert(out.end(), more.begin(), more.end());
        }
        return out;
    }
    return {tree};
}

std::vector<Value> values(Analysis& an, Value tree) {
    std::vector<Value> out;
    for (Value p : parts(an, tree)) {
        if (an.truthy(p) && an.item(p, 0) == an.V.T_VALUES) {
            for (Value v : an.at(an.item(p, 1))) out.push_back(v);
        }
    }
    return out;
}

const std::vector<Value>& chain(Analysis& an, Value key) {
    static const std::vector<Value> kNone;
    const ClassRec* c = an.in.cls(key);
    return c ? c->chain : kNone;
}

const std::vector<Value>& base_keys(Analysis& an, Value key) {
    static const std::vector<Value> kNone;
    const ClassRec* c = an.in.cls(key);
    return c ? c->bases : kNone;
}

}  // namespace ftengine::typesys
