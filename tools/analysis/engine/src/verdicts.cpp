// What the language's tests can give of one abstract value (heap/verdicts.py; log/trace-research.md F31): `typeof`
// (ECMA-262 13.5.3.1 with B.3.6 [[IsHTMLDDA]]), ToBoolean (7.1.2), IsStrictlyEqual with a constant, `== null`,
// `instanceof` (13.10.2, OrdinaryHasInstance 7.3.21), and, for a declared type predicate, whether a value can be of a
// declared type (a class of the program by its chain, an object type or a declared class by the members it requires)
// and whether it surely is one. What the analysis cannot decide can give either.
#include <string>

#include "semantics.h"

namespace ftengine::verdicts {

namespace {

bool known_kind(Analysis& an, Value k) {          // one of the language's kinds `typeof` decides alone
    const Vocab& V = an.V;
    return k == V.GD_UNDEFINED || k == V.GD_NULL || k == V.GD_TRUE || k == V.GD_FALSE || k == V.GD_NUMBER ||
           k == V.GD_BIGINT || k == V.GD_STRING || k == V.GD_SYMBOL || k == V.GD_OBJECT || k == V.GD_FUNCTION;
}

const ClassRec* chain_of(Analysis& an, Value key) { return key.kind() == Kind::None ? nullptr : an.in.cls(key); }

bool on_chain(Analysis& an, Value of, Value key, size_t from) {
    const ClassRec* c = chain_of(an, of);
    if (!c) return false;
    for (size_t i = from; i < c->proto_chain.size(); ++i) {
        if (c->proto_chain[i] == key) return true;
    }
    return false;
}

bool unresolved(Analysis& an, Value of) {
    const ClassRec* c = chain_of(an, of);
    return !c || c->proto_unresolved;
}

std::string joined(Analysis& an, Value prefix, Value rest) {
    return std::string(an.text(prefix)) + std::string(an.text(rest));
}

bool value_admits(Analysis& an, Value v, Obj o) {
    const Vocab& V = an.V;
    if (v == V.ENGINE || an.okind(o) == V.SITE_UNKNOWN) return true;
    if (an.starts(v, V.TEXT)) return can_equal(an, o, v);
    if (v == V.OBJECT || v == V.S_FUNCTION) {
        return !primitive(an, o) && (v == V.OBJECT || has_typeof(an, o, V.TYPEOF_FUNCTION));
    }
    Value kind = an.okind(o);
    if (kind == V.SITE_TEXT || kind == V.SITE_KEY) return v == V.S_STRING;
    return kind == V.SITE_PRIMITIVE && an.text(v) == joined(an, V.LANG, prim(an, o));
}

bool admits_in(Analysis& an, Value tree, Obj o, std::vector<Value>& seen);

bool admits_named(Analysis& an, Value tree, Obj o, std::vector<Value>& seen) {
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    if (found.kind() == Kind::None || std::find(seen.begin(), seen.end(), found) != seen.end()) return true;
    if (an.item(found, 0) == an.V.ALIAS) {
        seen.push_back(found);
        bool out = admits_in(an, an.unit(an.item(found, 1).data()).result_type, o, seen);
        seen.pop_back();
        return out;
    }
    const ClassRec* c = an.in.cls(an.item(found, 1));
    if (!c) return true;
    if (c->shape || c->ambient) return structural(an, an.item(found, 1), o);
    return !primitive(an, o) && can(of_class(an, o, an.item(found, 1)), true);
}

bool admits_in(Analysis& an, Value tree, Obj o, std::vector<Value>& seen) {
    const Vocab& V = an.V;
    Value kind = an.item(tree, 0);
    if (kind == V.T_UNION || kind == V.T_BOTH) {
        for (Value part : an.at(an.item(tree, 1))) {
            bool in = admits_in(an, part, o, seen);
            if (kind == V.T_UNION && in) return true;
            if (kind == V.T_BOTH && !in) return false;
        }
        return kind == V.T_BOTH;
    }
    if (kind == V.T_VALUES) {
        for (Value v : an.at(an.item(tree, 1))) {
            if (value_admits(an, v, o)) return true;
        }
        return false;
    }
    if (kind == V.T_NAMED) return admits_named(an, tree, o, seen);
    if (kind == V.T_OBJECT) return structural(an, an.item(tree, 1), o);
    if (kind == V.T_TUPLE) return can(array(an, o), true);
    if (kind == V.T_FN) return has_typeof(an, o, V.TYPEOF_FUNCTION);
    return true;
}

}  // namespace

Value prim(Analysis& an, Obj o) { return an.item(an.okey(o), 1); }

bool primitive(Analysis& an, Obj o) {
    Value kind = an.okind(o);
    return kind == an.V.SITE_TEXT || kind == an.V.SITE_KEY ||
           (kind == an.V.SITE_PRIMITIVE && !an.in_set(prim(an, o), an.V.GD_OBJECT_KINDS));
}

bool open_kind(Analysis& an, Obj o) {
    Value kind = an.okind(o);
    if (kind == an.V.SITE_UNKNOWN || kind == an.V.SITE_DECLARED) return true;
    if (kind == an.V.SITE_PRIMITIVE) return an.in_set(prim(an, o), an.V.GD_OBJECT_KINDS);
    if (kind != an.V.SITE_NEW || an.maker(o) >= 0) return false;
    const ClassRec* c = chain_of(an, an.class_of(o));
    return !c || c->proto_unresolved || c->proto_ambient;
}

std::vector<Value> typeof_of(Analysis& an, Obj o) {
    const Vocab& V = an.V;
    Value kind = an.okind(o);
    if (kind == V.SITE_PRIMITIVE) {
        Value k = prim(an, o);
        if (k == V.GD_UNDEFINED) return {V.TYPEOF_UNDEFINED};
        if (k == V.GD_NULL) return {V.TYPEOF_OBJECT};
        if (k == V.GD_TRUE || k == V.GD_FALSE) return {V.TYPEOF_BOOLEAN};
        if (k == V.GD_NUMBER) return {V.TYPEOF_NUMBER};
        if (k == V.GD_BIGINT) return {V.TYPEOF_BIGINT};
        if (k == V.GD_STRING) return {V.TYPEOF_STRING};
        if (k == V.GD_SYMBOL) return {V.TYPEOF_SYMBOL};
        if (k == V.GD_OBJECT) return {V.TYPEOF_OBJECT, V.TYPEOF_FUNCTION};
        if (k == V.GD_FUNCTION) return {V.TYPEOF_FUNCTION};
    } else if (kind == V.SITE_TEXT || kind == V.SITE_KEY) {
        return {V.TYPEOF_STRING};
    } else if (an.in_set(kind, V.GD_CALLABLE)) {
        return {V.TYPEOF_FUNCTION};
    } else if (kind == V.SITE_DECLARED) {
        return {V.TYPEOF_OBJECT, V.TYPEOF_FUNCTION, V.TYPEOF_UNDEFINED};
    } else if (kind == V.SITE_NEW) {
        if (open_kind(an, o)) return {V.TYPEOF_OBJECT, V.TYPEOF_FUNCTION};
        return {V.TYPEOF_OBJECT};
    } else if (an.in_set(kind, V.GD_ORDINARY)) {
        return {V.TYPEOF_OBJECT};
    }
    auto every = an.at(V.GD_EVERY_TYPEOF);
    return {every.begin(), every.end()};
}

bool has_typeof(Analysis& an, Obj o, Value t) {
    for (Value r : typeof_of(an, o)) {
        if (r == t) return true;
    }
    return false;
}

std::pair<bool, bool> truthiness(Analysis& an, Obj o) {
    Value kind = an.okind(o);
    if (kind == an.V.SITE_PRIMITIVE) {
        Value k = prim(an, o);
        bool falsy = an.in_set(k, an.V.GD_FALSY_KINDS);
        return {!falsy, falsy || an.in_set(k, an.V.GD_EITHER_KINDS) || !known_kind(an, k)};
    }
    if (kind == an.V.SITE_TEXT) {
        bool empty = an.text(prim(an, o)).empty();
        return {!empty, empty};
    }
    if (kind == an.V.SITE_KEY || kind == an.V.SITE_UNKNOWN || kind == an.V.SITE_DECLARED) return {true, true};
    return {true, false};
}

bool can_equal(Analysis& an, Obj o, Value constant) {
    Value kind = an.okind(o);
    if (kind == an.V.SITE_UNKNOWN) return true;
    if (kind == an.V.SITE_TEXT) return an.text(constant) == joined(an, an.V.TEXT, prim(an, o));
    if (kind == an.V.SITE_KEY) return an.starts(constant, an.V.TEXT);
    if (kind != an.V.SITE_PRIMITIVE || an.in_set(prim(an, o), an.V.GD_OBJECT_KINDS)) return false;
    Value k = prim(an, o);
    if (an.starts(constant, an.V.TEXT)) return k == an.V.GD_STRING;
    return an.text(constant) == joined(an, an.V.LANG, k);
}

bool can_differ(Analysis& an, Obj o, Value constant) {
    Value kind = an.okind(o);
    if (kind == an.V.SITE_TEXT) return an.text(constant) != joined(an, an.V.TEXT, prim(an, o));
    if (kind == an.V.SITE_PRIMITIVE) {
        Value k = prim(an, o);
        if (k == an.V.GD_UNDEFINED || k == an.V.GD_NULL || k == an.V.GD_TRUE || k == an.V.GD_FALSE) {
            return an.text(constant) != joined(an, an.V.LANG, k);
        }
    }
    return true;
}

uint8_t nullish(Analysis& an, Obj o) {
    Value kind = an.okind(o);
    if (kind == an.V.SITE_PRIMITIVE) {
        Value k = prim(an, o);
        return k == an.V.GD_UNDEFINED || k == an.V.GD_NULL ? kYes : !known_kind(an, k) ? kEither : kNo;
    }
    return kind == an.V.SITE_UNKNOWN || kind == an.V.SITE_DECLARED ? kEither : kNo;
}

uint8_t array(Analysis& an, Obj o) {
    if (an.okind(o) == an.V.SITE_ARRAY) return kYes;
    return open_kind(an, o) ? kEither : kNo;
}

uint8_t of_class(Analysis& an, Obj o, Value key) {
    if (primitive(an, o)) return kNo;
    Value kind = an.okind(o);
    if (kind == an.V.SITE_NEW) {
        if (an.maker(o) >= 0) return kEither;
        Value of = an.class_of(o);
        return on_chain(an, of, key, 0) ? kYes : unresolved(an, of) ? kEither : kNo;
    }
    if (kind == an.V.SITE_PROTOTYPE) {
        Value site = an.okey(o), of = an.item(site, 1);
        return on_chain(an, of, key, 1) ? kYes : unresolved(an, of) || an.at(site).size() > 2 ? kEither : kNo;
    }
    if (kind == an.V.SITE_LITERAL) {
        for (const auto& [name, gids] : an.in.members_of(an.class_of(o))) {
            if (name == an.V.PROTO_PROPERTY) return kEither;
        }
        return kNo;
    }
    return an.in_set(kind, an.V.GD_NOT_INSTANCES) ? kNo : kEither;
}

uint8_t outcomes(Analysis& an, Obj o, Obj target) {
    const Vocab& V = an.V;
    Value kind = an.okind(target);
    if (kind == V.SITE_UNKNOWN || (kind == V.SITE_PRIMITIVE && an.in_set(prim(an, target), V.GD_OBJECT_KINDS))) {
        return kEither;
    }
    if (kind == V.SITE_PRIMITIVE || kind == V.SITE_TEXT || kind == V.SITE_KEY) return kNeither;
    if (kind == V.SITE_CLASS) {
        Value key = an.class_itself(target);
        if (!an.in.cls(key)) return kEither;
        for (int64_t g : typesys::lookup(an, key, V.HAS_INSTANCE)) {
            if (an.unit(g).is_static) return kEither;    // its own @@hasInstance decides
        }
        return of_class(an, o, key);
    }
    if (primitive(an, o)) return an.in_set(kind, V.GD_CALLABLE) || kind == V.SITE_DECLARED ? kNo : kEither;
    if (kind == V.SITE_FUNCTION && an.okind(o) == V.SITE_NEW && an.maker(o) == static_cast<int64_t>(target)) {
        return kYes;
    }
    return kEither;
}

bool admits(Analysis& an, Value tree, Obj o) {
    std::vector<Value> seen;
    return admits_in(an, tree, o, seen);
}

const std::vector<Value>& required(Analysis& an, Value key) {   // each member it requires (not optional)
    auto [found, made] = an.required_members.try_emplace(key.bits());
    if (!made) return found->second;
    const Vocab& V = an.V;
    for (Value k : typesys::ancestors(an, key)) {
        for (Value g : typesys::group(an, k)) {
            for (const auto& [name, gids] : an.in.members_of(g)) {
                if (name == V.CALL_SIGNATURE || name == V.CONSTRUCT_SIGNATURE || name == V.INDEX_SIGNATURE) continue;
                bool needed = false;
                for (int64_t g2 : gids) needed = needed || !an.unit(g2).optional;
                if (needed && std::find(found->second.begin(), found->second.end(), name) == found->second.end()) {
                    found->second.push_back(name);
                }
            }
        }
    }
    return found->second;
}

bool structural(Analysis& an, Value key, Obj o) {
    if (an.okind(o) == an.V.SITE_UNKNOWN || an.okind(o) == an.V.SITE_DECLARED) return true;
    if (nullish(an, o) == kYes) return false;
    auto names = required(an, key);
    auto written = an.names_of.find(o);
    for (Value name : names) {
        if (typesys::has_member(an, o, name)) continue;
        if (written == an.names_of.end() ||
            std::find(written->second.begin(), written->second.end(), name) == written->second.end()) {
            return false;
        }
    }
    return true;
}

namespace {

bool surely_value(Analysis& an, Value v, Obj o) {
    const Vocab& V = an.V;
    Value kind = an.okind(o);
    if (an.starts(v, V.TEXT)) return kind == V.SITE_TEXT && an.text(v) == joined(an, V.TEXT, prim(an, o));
    if (v == V.S_STRING) {
        return kind == V.SITE_TEXT || kind == V.SITE_KEY || (kind == V.SITE_PRIMITIVE && prim(an, o) == V.GD_STRING);
    }
    return kind == V.SITE_PRIMITIVE && an.text(v) == joined(an, V.LANG, prim(an, o)) &&
           !an.in_set(prim(an, o), V.GD_OBJECT_KINDS);
}

bool surely_in(Analysis& an, Value tree, Obj o, std::vector<Value>& seen) {
    const Vocab& V = an.V;
    Value kind = an.item(tree, 0);
    if (kind == V.T_UNION) {
        for (Value part : an.at(an.item(tree, 1))) {
            if (surely_in(an, part, o, seen)) return true;
        }
        return false;
    }
    if (kind == V.T_VALUES) {
        for (Value v : an.at(an.item(tree, 1))) {
            if (surely_value(an, v, o)) return true;
        }
        return false;
    }
    if (kind != V.T_NAMED || primitive(an, o) || an.okind(o) == V.SITE_UNKNOWN || an.okind(o) == V.SITE_DECLARED) {
        return false;
    }
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    if (found.kind() == Kind::None || std::find(seen.begin(), seen.end(), found) != seen.end()) return false;
    if (an.item(found, 0) == V.ALIAS) {
        seen.push_back(found);
        bool out = surely_in(an, an.unit(an.item(found, 1).data()).result_type, o, seen);
        seen.pop_back();
        return out;
    }
    Value key = an.item(found, 1);
    for (Value own : typesys::own_types(an, o)) {
        if (own == key) return true;
    }
    return of_class(an, o, key) == kYes;
}

}  // namespace

bool surely(Analysis& an, Value tree, Obj o) {
    std::vector<Value> seen;
    return surely_in(an, tree, o, seen);
}

}  // namespace ftengine::verdicts
