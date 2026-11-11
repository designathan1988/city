// Members of one object (heap/members.py; ECMA-262 [[Get]] and [[Set]] through the prototype chain), the index's
// lookup of a member by class (Index.lookup) and an accessor a property access runs (Unit.runs).
#include "semantics.h"

namespace ftengine::members {

namespace {

void accessor(Analysis& an, Value target, Obj obj, const UnitRec& prop, Value local_id, std::span<const Value> args) {
    if (local_id.kind() != Kind::Int) return;
    int64_t gid = an.offset(prop) + local_id.data();
    Value node = an.tuple({an.V.ACCESSOR, I(obj), I(gid)});
    an.call(node, an.function_object(gid, an.ROOT), obj, args, an.V.CALL);
    an.follow(node, target);
}

void statics(Analysis& an, Value target, Obj obj, Value name) {
    Value cls = an.class_itself(obj);
    if (cls.kind() != Kind::None && an.in.cls(cls)) {
        for (int64_t gid : typesys::lookup(an, cls, name)) {
            if (!an.unit(gid).is_static) continue;
            if (an.is_function_unit(gid)) {
                an.add(target, an.function_object(gid, method_env(an, obj, gid)));
            } else if (an.is_state(gid)) {
                an.follow(an.hold(gid), target);
            }
        }
    }
    if (name != an.V.PROTOTYPE) return;
    int64_t gid = an.function(obj);
    if (cls.kind() != Kind::None) {
        an.add(target, prototype(an, cls));
    } else if (gid >= 0 && an.truthy(an.unit(gid).constructs)) {
        an.add(target, prototype(an, an.unit(gid).constructs, obj));
    }
}

void made_from(Analysis& an, Value target, Obj maker, Value name) {
    Value held = field(an, maker, an.V.PROTOTYPE);
    an.subscribe(held, target, an.tuple({an.V.PROTOTYPES, name}), pack(an, kPrototypes, {name}));
}

std::vector<int64_t> declared(Analysis& an, Obj obj, Value name) {
    Value key = an.class_of(obj);
    if (!an.truthy(key)) key = an.class_itself(obj);
    if (key.kind() == Kind::None || !an.in.cls(key)) return {};
    std::vector<int64_t> out;
    for (int64_t g : lookup(an, key, name)) {
        if (an.is_state(g)) out.push_back(g);
    }
    return out;
}

std::vector<int64_t> lookup_in(Analysis& an, Value key, Value name, std::unordered_set<uint64_t>& seen) {
    if (!an.truthy(key) || !seen.insert(key.bits()).second) return {};
    if (an.in.modules.count(key.bits())) return an.in.export_of(key, name).gids;
    std::vector<int64_t> found = an.in.member(key, name);
    if (!found.empty()) return found;
    if (an.in.cls(key)) {
        for (Value base : typesys::base_keys(an, key)) {
            auto more = lookup_in(an, base, name, seen);
            found.insert(found.end(), more.begin(), more.end());
        }
    }
    return found;
}

}  // namespace

std::vector<int64_t> lookup(Analysis& an, Value key, Value name) {
    Value memo_key = an.tuple({I(1), key, name});
    auto found = an.memo.find(memo_key.bits());
    if (found != an.memo.end()) {
        std::vector<int64_t> out;
        for (Value v : an.at(found->second)) out.push_back(v.data());
        return out;
    }
    std::unordered_set<uint64_t> seen;
    std::vector<int64_t> out = lookup_in(an, key, name, seen);
    std::vector<Value> items;
    for (int64_t g : out) items.push_back(I(g));
    an.memo[memo_key.bits()] = an.tuple(items);
    return out;
}

Value runs(Analysis& an, const UnitRec& prop, Value which, const UnitRec* reader) {
    auto found = prop.accessors.find(which.bits());
    if (found == prop.accessors.end()) return NONE;
    if (reader != nullptr && reader->accessor_of == prop.name && reader->owner == prop.owner) return NONE;
    (void)an;
    return found->second;
}

void read(Analysis& an, Value target, Obj obj, Value name) {
    const Vocab& V = an.V;
    Value kind = an.okind(obj);
    if (kind == V.SITE_MODULE) {
        const Export& e = an.in.export_of(an.item(an.okey(obj), 1), name);
        for (int64_t gid : e.gids) names::value_of(an, gid, target);
        for (Value m : e.modules) an.add(target, an.make({V.SITE_MODULE, m}));
        return;
    }
    if (kind == V.SITE_UNKNOWN) {
        an.add(target, obj);
        return;
    }
    if (kind == V.SITE_PRIMITIVE) {                 // no property of its own: its wrapper's prototype's
        for (Value own : typesys::own_types(an, obj)) {
            if (declared_read(an, target, obj, own, name)) break;
        }
        return;
    }
    Value key = an.class_of(obj);
    int64_t maker = an.maker(obj);
    for (Value own : typesys::own_types(an, obj)) {
        if (declared_read(an, target, obj, own, name)) break;
    }
    if (key.kind() != Kind::None && an.in.cls(key)) {
        for (Value k : typesys::chain(an, key)) {
            an.follow(field(an, prototype(an, k, k == key ? maker : -1), name), target);
        }
    }
    if (maker >= 0) made_from(an, target, static_cast<Obj>(maker), name);
    if (key.kind() != Kind::None && name == V.PROTOTYPE_CONSTRUCTOR) {
        an.add(target, maker >= 0 ? static_cast<Obj>(maker) : an.make({V.SITE_CLASS, key}));
    }
    statics(an, target, obj, name);
    an.follow(field(an, obj, name), target);
    an.follow(an.any_field(obj), target);
}

bool declared_read(Analysis& an, Value target, Obj obj, Value key, Value name) {
    std::vector<int64_t> found;
    if (an.in.cls(key)) found = typesys::lookup(an, key, name);
    Value own = an.okind(obj) == an.V.SITE_LITERAL ? an.class_of(obj) : NONE;
    for (int64_t gid : found) {
        const UnitRec& unit = an.unit(gid);
        if (an.is_function_unit(gid)) {
            if (own.kind() == Kind::None || unit.owner != own) {
                an.add(target, an.function_object(gid, method_env(an, obj, gid)));
            }
        } else if (an.is_state(gid) && !unit.is_static) {
            Value getter = runs(an, unit, an.V.GETTER, nullptr);
            if (getter.kind() != Kind::None) {
                accessor(an, target, obj, unit, getter, {});
            } else if (typesys::declared(an, gid) && an.truthy(unit.result_type)) {
                Value site = an.tuple({an.tuple({an.V.PROPERTY, I(gid)}), an.object_heap(obj)});
                an.follow(values::produce(an, unit.result_type, environment(an, obj, unit.owner), site), target);
            } else {
                an.follow(an.initial(gid), target);
            }
        }
    }
    return !found.empty();
}

Value method_env(Analysis& an, Obj obj, int64_t gid) {
    const UnitRec& unit = an.unit(gid);
    int64_t cls = an.okind(obj) == an.V.SITE_CLASS ? static_cast<int64_t>(obj) : an.maker(obj);
    Value env;
    int64_t holder = -1;
    if (cls < 0 || unit.outer < 0 || !an.class_env(static_cast<Obj>(cls), env, holder)) return an.ROOT;
    return names::scope(an, &an.unit(holder), env, unit.outer);
}

Value environment(Analysis& an, Obj obj, Value owner) {
    Value env = an.declared_env(obj);
    if (env.kind() != Kind::None && an.okind(obj) == an.V.SITE_DECLARED && an.class_of(obj) == owner) return env;
    return an.tuple({I(obj), typesys::identity(an, owner), NONE});
}

void read_all(Analysis& an, Value target, Obj obj) {
    if (an.okind(obj) == an.V.SITE_PRIMITIVE || an.okind(obj) == an.V.SITE_UNKNOWN) return;
    an.follow(field(an, obj, an.V.ELEMENTS_FIELD), target);
    an.follow(an.any_field(obj), target);
}

void write(Analysis& an, Value source, Obj obj, Value name) {
    if (an.okind(obj) == an.V.SITE_PRIMITIVE || an.okind(obj) == an.V.SITE_UNKNOWN) return;
    Value cls = an.class_itself(obj);
    if (cls.kind() != Kind::None && an.in.cls(cls)) {
        for (int64_t gid : lookup(an, cls, name)) {
            if (an.unit(gid).is_static && an.is_state(gid)) an.follow(source, an.hold(gid));
        }
    }
    Value key = an.class_of(obj);
    if (key.kind() != Kind::None && an.in.cls(key)) {
        for (int64_t gid : lookup(an, key, name)) {
            if (!an.is_state(gid)) continue;
            Value setter = runs(an, an.unit(gid), an.V.SETTER, nullptr);
            if (setter.kind() != Kind::None) {
                Value args[] = {source};
                accessor(an, an.tuple({an.V.ACCESSOR, I(obj), I(-gid)}), obj, an.unit(gid), setter, args);
            }
        }
    }
    an.follow(source, field(an, obj, name));
}

void link_fields(Analysis& an, const UnitRec& unit, Obj obj, Value name, int64_t line, bool writing) {
    for (int64_t gid : declared(an, obj, name)) {
        an.link(unit.gid, gid, writing ? an.V.WRITE : an.V.READ, line);
        const UnitRec& prop = an.unit(gid);
        Value r = runs(an, prop, writing ? an.V.SETTER : an.V.GETTER, &unit);
        if (r.kind() == Kind::Int) an.link(unit.gid, an.offset(prop) + r.data(), an.V.CALL, line);
    }
}

Obj prototype(Analysis& an, Value key, int64_t maker) {
    return maker < 0 ? an.make({an.V.SITE_PROTOTYPE, key}) : an.make({an.V.SITE_PROTOTYPE, key, I(maker)});
}

Value field(Analysis& an, Obj obj, Value name) {
    Value node = an.field(obj, name);
    if (an.wire_once(node)) an.follow(node, every(an, obj));
    return node;
}

Value every(Analysis& an, Obj obj) {
    Value node = an.tuple({an.V.ALL, I(obj)});
    if (!an.wire_once(node)) return node;
    Value key = an.class_of(obj);
    bool own = key.kind() != Kind::None && an.in.cls(key) && !an.truthy(an.declared_env(obj)) &&
               an.okind(obj) != an.V.SITE_DECLARED;
    if (!own) return node;
    for (Value k : typesys::chain(an, key)) {
        for (const auto& [n, gids] : an.in.members_of(k)) {
            for (int64_t gid : gids) {
                if (an.is_state(gid) && !an.unit(gid).is_static) an.follow(an.initial(gid), node);
            }
        }
    }
    return node;
}

}  // namespace ftengine::members
