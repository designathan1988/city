// What no declaration states, summarized from the declarations themselves (heap/summaries.py): the values a declared
// call takes in open positions are its pool; a property-key text names the member of its open object arguments the
// pool reaches and is read and called on them; an open result holds the pool; a function in it is called back. The
// platform's kept state: a function a declared method takes under a function type, called back by a later call on the
// same receiver that passes a value of a plain named type the function's parameters name.
#include "semantics.h"

namespace ftengine::summaries {

namespace {

void key_values(Analysis& an, Value tree, std::vector<Value>& out, std::vector<Value>& seen) {
    if (!an.truthy(tree)) return;
    Value kind = an.item(tree, 0);
    if (kind == an.V.T_VALUES) {
        for (Value v : an.at(an.item(tree, 1))) out.push_back(v);
    } else if (kind == an.V.T_UNION) {
        for (Value part : an.at(an.item(tree, 1))) key_values(an, part, out, seen);
    } else if (kind == an.V.T_NAMED) {
        Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
        if (found.kind() != Kind::None && an.item(found, 0) == an.V.ALIAS &&
            std::find(seen.begin(), seen.end(), found) == seen.end()) {
            seen.push_back(found);
            key_values(an, an.unit(an.item(found, 1).data()).result_type, out, seen);
            seen.pop_back();
        }
    }
}

bool keyed(Analysis& an, Value tree) {
    std::vector<Value> kinds, seen;
    key_values(an, tree, kinds, seen);
    if (kinds.empty()) return false;
    bool all_texts = true;
    for (Value v : kinds) {
        if (v == an.V.SYMBOL) return true;
        if (!an.starts(v, an.V.TEXT)) all_texts = false;
    }
    return all_texts;
}

bool hands(Analysis& an, Value tree) {
    if (!an.truthy(tree)) return false;
    Value kind = an.item(tree, 0);
    if (kind == an.V.T_FN) return true;
    if (kind == an.V.T_UNION) {
        for (Value part : an.at(an.item(tree, 1))) {
            if (hands(an, part)) return true;
        }
        return false;
    }
    if (kind != an.V.T_NAMED) return false;
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    if (found.kind() != Kind::None && an.item(found, 0) == an.V.ALIAS) {
        return hands(an, an.unit(an.item(found, 1).data()).result_type);
    }
    return found.kind() != Kind::None && !typesys::lookup(an, an.item(found, 1), an.V.CALL_SIGNATURE).empty();
}

Value plain_type(Analysis& an, Value tree) {
    if (!an.truthy(tree) || an.item(tree, 0) != an.V.T_NAMED || an.truthy(an.item(tree, 2))) return NONE;
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    if (found.kind() == Kind::None || an.item(found, 0) != an.V.CLASS) return NONE;
    Value key = an.item(found, 1);
    for (Value k : typesys::group(an, key)) {
        const ClassRec* c = an.in.cls(k);
        if (c && !c->tparams.empty()) return NONE;
    }
    return key;
}

std::vector<Value> arguments(Analysis& an, Obj fn) {
    const UnitRec& unit = an.unit(an.function(fn));
    std::vector<Value> out;
    for (size_t i = 0; i < unit.params.size(); ++i) {
        Value tree = i < unit.param_types.size() ? unit.param_types[i] : an.tuple({});
        if (an.truthy(tree) && !is_open(an, tree)) {
            Value site = an.tuple({an.tuple({an.tuple({an.V.GIVEN, I(unit.gid), I(static_cast<int64_t>(i))})}),
                                   an.tuple({})});
            out.push_back(values::produce(an, tree, an.V.NO_ENV, site));
        } else {
            Value node = an.tuple({an.V.UNKNOWN_ARG});
            an.add(node, an.make({an.V.SITE_UNKNOWN}));
            out.push_back(node);
        }
    }
    return out;
}

void named(Analysis& an, Value node, Obj obj, Value name, Value where) {
    an.subscribe(pool(an, node), members::field(an, obj, name), an.tuple({an.V.FIELDS, I(obj)}),
                 pack(an, kSummaryFields, {I(obj)}));
    Value read = an.tuple({an.V.MEMBER_OF, node, I(obj), name});
    members::read(an, read, obj, name);
    Value call = an.tuple({an.V.REFLECT, an.item(where, 0), name});
    an.relay(node, call);
    an.subscribe(read, pool(an, node), an.tuple({an.V.MEMBER_OF, I(obj), name}),
                 pack(an, kSummaryMemberOf, {node, call, I(obj)}));
}

}  // namespace

bool is_open(Analysis& an, Value tree) {
    if (!an.truthy(tree)) return true;
    Value kind = an.item(tree, 0);
    if (kind == an.V.T_OPEN) return true;
    if (kind == an.V.T_VALUES) {
        for (Value v : an.at(an.item(tree, 1))) {
            if (v == an.V.LIB_ENGINE || v == an.V.OBJECT) return true;
        }
        return false;
    }
    if (kind == an.V.T_UNION) {
        for (Value part : an.at(an.item(tree, 1))) {
            if (is_open(an, part)) return true;
        }
        return false;
    }
    if (kind == an.V.T_OBJECT) {
        for (const auto& [name, gids] : an.in.members_of(an.item(tree, 1))) {
            if (!gids.empty()) return false;
        }
        return true;
    }
    if (kind == an.V.T_NAMED) {
        Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
        return found.kind() != Kind::None && an.item(found, 0) == an.V.CLASS &&
               typesys::is_open_type(an, an.item(found, 1));
    }
    return false;
}

Value pool(Analysis& an, Value origin) { return an.tuple({an.V.POOL, origin}); }

void summarize(Analysis& an, Value node, const UnitRec& unit, std::span<const Value> args, Value site) {
    Value where = an.tuple({I(an.make({an.V.SUMMARY_SITE, an.item(site, 0)})), an.item(site, 1)});
    std::vector<Value> names;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].kind() == Kind::None || !keyed(an, declared::parameter(an, unit, i))) continue;
        Value found = texts::argument(an, node, i);
        for (Value text : an.at(found)) {
            if (std::find(names.begin(), names.end(), text) == names.end()) names.push_back(text);
        }
    }
    if (!names.empty()) {
        Value known = TV(an.s.term(Shape::FrozenSet, 0, names));
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i].kind() == Kind::None || !is_open(an, declared::parameter(an, unit, i))) continue;
            an.subscribe(args[i], an.tuple({an.V.RECEIVE, node, I(static_cast<int64_t>(i))}), an.tuple({an.V.RECEIVE}),
                         pack(an, kSummaryReceive, {node, known, where}));
        }
    }
    an.subscribe(pool(an, node), an.tuple({an.V.CALLED, node}), an.tuple({an.V.CALLED}),
                 pack(an, kSummaryCalled, {node, where}));
    if (is_open(an, unit.result_type)) an.follow(pool(an, node), node);
}

void open_call(Analysis& an, Value node, std::span<const Value> args, Value site) {
    Value where = an.tuple({I(an.make({an.V.SUMMARY_SITE, an.item(site, 0)})), an.item(site, 1)});
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].kind() == Kind::None) continue;
        an.subscribe(args[i], an.tuple({an.V.CALLED, node}), an.tuple({an.V.CALLED, I(static_cast<int64_t>(i))}),
                     pack(an, kOpenCalled, {node, where}));
    }
    an.add(node, an.make({an.V.SITE_UNKNOWN}));
}

void receive(Analysis& an, Value node, Obj obj, Value names, Value where) {
    for (Value name : an.at(names)) named(an, node, obj, name, where);
}

void reflected(Analysis& an, Value node, Value call, Obj obj, Obj value) {
    if (an.function(value) < 0) {
        an.add(pool(an, node), value);
        return;
    }
    an.call(call, value, obj, arguments(an, value), an.V.CALL);
    an.follow(call, pool(an, node));
}

void called(Analysis& an, Value node, Obj fn, Value where) {
    Value back = an.tuple({an.V.CALLED, an.item(where, 0), I(fn)});
    an.relay(node, back);
    an.call(back, fn, -1, arguments(an, fn), an.V.HANDED, where, true);   // the same arguments everywhere: one run
    an.follow(back, pool(an, node));
}

void keep(Analysis& an, Obj holder, Obj fn, Value tree) {
    if (an.okind(holder) == an.V.SITE_PRIMITIVE || an.okind(holder) == an.V.SITE_UNKNOWN) return;
    an.add(an.field(holder, an.V.PLATFORM_FIELD), an.make({an.V.KEPT_ENTRY, I(fn), tree}));
}

void kept(Analysis& an, Value node, Obj holder, const UnitRec& unit, std::span<const Value> args, Value site) {
    for (size_t i = 0; i < unit.param_types.size(); ++i) {
        if (hands(an, declared::parameter(an, unit, i))) return;
    }
    std::vector<std::pair<Value, std::vector<Value>>> offered;
    for (size_t i = 0; i < args.size(); ++i) {
        Value key = plain_type(an, declared::parameter(an, unit, i));
        if (key.kind() == Kind::None || args[i].kind() == Kind::None) continue;
        auto found = std::find_if(offered.begin(), offered.end(), [&](const auto& kv) { return kv.first == key; });
        if (found == offered.end()) {
            offered.push_back({key, {args[i]}});
        } else {
            found->second.push_back(args[i]);
        }
    }
    if (offered.empty()) return;
    std::vector<Value> rows;
    for (const auto& [key, sources] : offered) rows.push_back(an.tuple({key, an.tuple(sources)}));
    Value where = an.tuple({I(an.make({an.V.KEPT_SITE, an.item(site, 0)})), an.item(site, 1)});
    an.subscribe(an.field(holder, an.V.PLATFORM_FIELD), an.tuple({an.V.KEPT, node}), an.tuple({an.V.KEPT}),
                 pack(an, kKept, {node, an.tuple(rows), where}));
}

void kept_call(Analysis& an, Value node, Obj entry, Value offered, Value where) {
    Value key = an.okey(entry);
    Obj fn = static_cast<Obj>(an.item(key, 1).data());
    Value tree = an.item(key, 2);
    auto sources_of = [&](Value k) -> std::span<const Value> {
        for (Value row : an.at(offered)) {
            if (an.item(row, 0) == k) return an.at(an.item(row, 1));
        }
        return {};
    };
    std::vector<Value> wanted;
    bool any = false;
    for (Value p : an.at(an.item(tree, 1))) {
        Value k = plain_type(an, p);
        wanted.push_back(k);
        if (k.kind() != Kind::None && !sources_of(k).empty()) any = true;
    }
    if (!any) return;
    std::vector<Value> given;
    for (size_t i = 0; i < wanted.size(); ++i) {
        Value arg = an.tuple({an.V.GIVEN, node, I(entry), I(static_cast<int64_t>(i))});
        if (wanted[i].kind() != Kind::None) {
            for (Value source : sources_of(wanted[i])) an.follow(source, arg);
        }
        given.push_back(arg);
    }
    Value back = an.tuple({an.V.KEPT, an.item(where, 0), I(fn)});
    an.relay(node, back);
    an.call(back, fn, -1, given, an.V.HANDED, where);
}

bool is_object(Analysis& an, Obj obj) {
    Value kind = an.okind(obj);
    return kind != an.V.SITE_PRIMITIVE && kind != an.V.SITE_TEXT && kind != an.V.SITE_KEY && kind != an.V.SITE_UNKNOWN;
}

}  // namespace ftengine::summaries
