// What a declared function does: what its declaration says (heap/declared.py): each argument where its parameter's
// type says, the result its type gives, a function handed to a function type called back, an object of the program
// given to a type it is not nominally read through what that type declares, construct and call signatures.
#include "semantics.h"

namespace ftengine::declared {

namespace {

std::vector<Value> heritage(Analysis& an, Value key) {
    std::vector<Value> out, todo{key};
    std::unordered_set<uint64_t> seen;
    while (!todo.empty()) {
        Value k = todo.back();
        todo.pop_back();
        for (Value g : typesys::group(an, k)) {
            if (!an.in.cls(g)) continue;
            for (Value base : typesys::base_keys(an, g)) {
                base = typesys::identity(an, base);
                if (seen.insert(base.bits()).second) {
                    out.push_back(base);
                    todo.push_back(base);
                }
            }
        }
    }
    return out;
}

std::vector<Value> function_types(Analysis& an, Value tree) {
    if (an.item(tree, 0) == an.V.T_FN) return {tree};
    if (an.item(tree, 0) != an.V.T_NAMED) return {};
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    if (found.kind() == Kind::None || an.item(found, 0) != an.V.CLASS) return {};
    std::vector<Value> out;
    for (int64_t g : typesys::lookup(an, an.item(found, 1), an.V.CALL_SIGNATURE)) out.push_back(signature(an, an.unit(g)));
    return out;
}

Value argument(Analysis& an, Value node, Value tree, Value env, Value site, Value origin) {
    Value given = values::produce(an, tree, env, site);
    if (origin.kind() == Kind::None || !summaries::is_open(an, tree)) return given;
    Value held = an.tuple({an.V.CALLBACK_ARG, node, site});
    an.follow(given, held);
    an.follow(summaries::pool(an, origin), held);
    return held;
}

void signatures(Analysis& an, Value node, Obj obj, Value name, std::span<const Value> args, Value where) {
    for (Value key : typesys::own_types(an, obj)) {
        for (int64_t gid : typesys::lookup(an, key, name)) {
            an.reached(node, gid, name == an.V.CONSTRUCT_SIGNATURE ? an.V.NEW : an.V.CALL);
            apply(an, node, gid, obj, args, where);
        }
    }
}

void structural(Analysis& an, Obj obj, Value key, Value env, Value site, Value origin) {
    const Vocab& V = an.V;
    Value done = an.tuple({V.STRUCTURAL, I(obj), key, env});
    if (!an.member_of(an.okind(obj), V.PROGRAM_MADE) || !an.wire_once(done)) return;
    Value own = an.class_of(obj);
    std::vector<Value> keys{key};
    for (Value h : heritage(an, key)) keys.push_back(h);
    for (Value k : keys) {
        for (Value g : typesys::group(an, k)) {
            for (const auto& [name, gids] : an.in.members_of(g)) {
                for (int64_t gid : gids) {
                    const UnitRec& unit = an.unit(gid);
                    for (const auto& [key_type, value] : unit.indexes) {
                        auto vs = typesys::values(an, key_type);
                        Value held = std::find(vs.begin(), vs.end(), V.NUMBER) != vs.end()
                                         ? members::field(an, obj, V.ELEMENTS_FIELD) : members::every(an, obj);
                        values::consume(an, value, env, held, values::member_site(an, gid, site), origin);
                    }
                    if (!unit.indexes.empty()) continue;
                    if (name == V.CALL_SIGNATURE) {
                        if (an.okind(obj) == V.SITE_FUNCTION) {
                            callback(an, obj, signature(an, unit), env, values::member_site(an, gid, site), origin, -1);
                        }
                        continue;
                    }
                    if (own.kind() == Kind::None || !an.in.cls(own) || typesys::lookup(an, own, name).empty()) continue;
                    bool function = an.is_function_unit(gid);
                    Value tree = function ? signature(an, unit) : unit.result_type;
                    if (!function && !reaches(an, tree) && !summaries::is_open(an, tree)) continue;
                    Value read = an.tuple({V.READ_MEMBER, I(obj), name});
                    members::read(an, read, obj, name);
                    if (function) {
                        an.subscribe(read, an.tuple({V.READ_MEMBER, I(obj), name, I(gid)}), an.tuple({V.T_FN}),
                                     pack(an, kStructuralFn, {I(gid), env, site, origin, I(obj)}));
                    } else {
                        values::consume(an, tree, env, read, values::member_site(an, gid, site), origin);
                    }
                }
            }
        }
    }
}

}  // namespace

Value call_site(Analysis& an, Value node) {
    auto items = an.at(node);
    Value ctx = !items.empty() && an.is_tuple(items.back()) ? items.back() : an.ROOT;
    Obj site = an.make(an.tuple(items.empty() ? items : items.first(items.size() - 1)));
    return an.tuple({I(site), contexts::heap(an, ctx)});
}

void apply(Analysis& an, Value node, int64_t gid, int64_t holder, std::span<const Value> args, Value site) {
    const UnitRec& unit = an.unit(gid);
    Value key = holder >= 0 ? typesys::identity(an, unit.owner) : NONE;
    Value env = an.tuple({holder >= 0 ? I(holder) : NONE, key, an.tuple({site, I(gid)})});
    for (size_t i = 0; i < args.size(); ++i) {
        Value tree = parameter(an, unit, i);
        if (an.truthy(tree) && args[i].kind() != Kind::None) {
            values::consume(an, tree, env, args[i], values::sub(an, site, an.tuple({I(gid), I(static_cast<int64_t>(i))})),
                            node);
        }
    }
    if (an.truthy(unit.this_type) && holder >= 0) {
        values::consume(an, unit.this_type, env, an_node(an, static_cast<Obj>(holder)),
                        values::sub(an, site, an.tuple({I(gid), an.V.D_THIS})), node);
    }
    if (an.truthy(unit.result_type)) {
        an.follow(values::produce(an, unit.result_type, env, values::sub(an, site, I(gid))), node);
    }
    summaries::summarize(an, node, unit, args, site);
    if (holder >= 0) summaries::kept(an, node, static_cast<Obj>(holder), unit, args, site);
}

Value parameter(Analysis& an, const UnitRec& unit, size_t index) {
    Value types = an.tuple(unit.param_types);
    int64_t rest = unit.variadic ? static_cast<int64_t>(unit.param_types.size()) - 1 : -1;
    return positional(an, types, rest, index);
}

Value positional(Analysis& an, Value types, int64_t rest, size_t index) {
    auto all = an.at(types);
    if (rest >= 0 && rest < static_cast<int64_t>(all.size()) && static_cast<int64_t>(index) >= rest) {
        Value tree = all[static_cast<size_t>(rest)];
        Value kind = an.item(tree, 0);
        if (an.truthy(tree) && kind == an.V.T_NAMED && an.item(tree, 1) == an.V.ARRAY_TYPE &&
            an.truthy(an.item(tree, 2))) {
            return an.item(an.item(tree, 2), 0);
        }
        if (an.truthy(tree) && kind == an.V.T_TUPLE) {
            auto parts = an.at(an.item(tree, 1));
            size_t at = index - static_cast<size_t>(rest);
            return at < parts.size() ? parts[at] : an.tuple({});
        }
        return tree;
    }
    return index < all.size() ? all[index] : an.tuple({});
}

void taken(Analysis& an, Obj obj, Value tree, Value env, Value site, Value origin) {
    const Vocab& V = an.V;
    Value kind = an.item(tree, 0);
    if (an.item(env, 0).kind() != Kind::None && an.function(obj) >= 0) {
        for (Value kept : function_types(an, tree)) {
            summaries::keep(an, static_cast<Obj>(an.item(env, 0).data()), obj, kept);
        }
    }
    if (kind == V.T_FN) {
        callback(an, obj, tree, env, site, origin, -1);
    } else if (kind == V.T_TUPLE) {
        int64_t i = 0;
        for (Value part : an.at(an.item(tree, 1))) {
            values::consume(an, part, env, members::field(an, obj, V.ELEMENTS_FIELD), values::sub(an, site, I(i++)), origin);
        }
    } else if (kind == V.T_OBJECT) {
        structural(an, obj, an.item(tree, 1), env, site, origin);
    } else if (kind == V.T_NAMED) {
        Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
        if (found.kind() == Kind::None) return;
        Value args = an.item(tree, 2);
        if (an.item(found, 0) == V.ALIAS) {
            const UnitRec& alias = an.unit(an.item(found, 1).data());
            if (!values::on_path(an, site, found)) {
                Value expanded = values::substitute(an, alias.result_type, values::arguments(an, args, alias.tparams));
                values::consume(an, expanded, env, an_node(an, obj), values::sub(an, site, found), origin);
            }
            return;
        }
        Value key = an.item(found, 1);
        auto owns = typesys::own_types(an, obj);
        bool nominal = std::find(owns.begin(), owns.end(), key) != owns.end();
        for (size_t i = 0; !nominal && i < owns.size(); ++i) {
            auto h = heritage(an, owns[i]);
            nominal = std::find(h.begin(), h.end(), key) != h.end();
        }
        int64_t i = 0;
        if (nominal) {
            for (Value arg : an.at(args)) {
                values::consume(an, arg, env, typesys::place(an, obj, key, i), values::sub(an, site, I(i)), origin);
                ++i;
            }
            return;
        }
        Value bound_site = values::sub(an, site, an.tuple({V.BOUND, key}));      // one per site and type
        Obj bound = an.make({V.SITE_DECLARED, key, an.item(bound_site, 0), NONE, an.item(bound_site, 1)});
        for (Value arg : an.at(args)) {
            Value held = typesys::place(an, bound, key, i);
            values::consume(an, arg, env, held, values::sub(an, site, I(i)), origin);
            an.follow(values::produce(an, arg, env, values::sub(an, site, I(i))), held);
            ++i;
        }
        structural(an, obj, key, an.tuple({I(bound), key, an.item(env, 2)}), site, origin);
    }
}

Value an_node(Analysis& an, Obj obj) {
    Value node = an.tuple({an.V.READ_MEMBER, I(obj)});
    an.add(node, obj);
    return node;
}

bool reaches(Analysis& an, Value tree) {
    if (!an.is_tuple(tree) || !an.truthy(tree)) return false;
    Value kind = an.item(tree, 0);
    if (kind == an.V.T_FN || kind == an.V.T_PARAM) return true;
    auto items = an.at(tree);
    for (size_t i = 1; i < items.size(); ++i) {
        if (an.is_tuple(items[i]) && reaches(an, items[i])) return true;
    }
    return false;
}

Value signature(Analysis& an, const UnitRec& unit) {
    int64_t rest = unit.variadic ? static_cast<int64_t>(unit.param_types.size()) - 1 : -1;
    Value open = an.tuple({an.V.T_OPEN});
    return an.tuple({an.V.T_FN, an.tuple(unit.param_types), an.truthy(unit.result_type) ? unit.result_type : open,
                     I(rest), open});
}

void callback(Analysis& an, Obj fn, Value tree, Value env, Value site, Value origin, int64_t this_obj) {
    const Vocab& V = an.V;
    Obj stat = an.make({V.CALLBACK_SITE, an.item(site, 0)});
    Value node = an.tuple({V.CALLBACK, I(stat), I(fn), env, an.item(site, 1)});
    Value where = an.tuple({I(stat), an.item(site, 1)});
    int64_t gid = an.function(fn);
    size_t count = gid >= 0 ? an.unit(gid).params.size() : an.at(an.item(tree, 1)).size();
    std::vector<Value> args;
    for (size_t i = 0; i < count; ++i) {
        Value part = positional(an, an.item(tree, 1), an.item(tree, 3).data(), i);
        args.push_back(argument(an, node, part, env, values::sub(an, site, I(static_cast<int64_t>(i))), origin));
    }
    an.relay(origin, node);
    an.call(node, fn, this_obj, args, V.HANDED, where);
    Value receiver_type = an.item(tree, 4);
    if (this_obj < 0 && an.truthy(receiver_type) && an.item(receiver_type, 0) != V.T_OPEN) {
        Value receivers = values::produce(an, receiver_type, env, values::sub(an, site, V.D_THIS));
        an.subscribe(receivers, an.tuple({V.RECEIVERS, node}), an.tuple({V.RECEIVERS}),
                     pack(an, kReceivers, {node, I(fn), an.tuple(args), where}));
    }
    if (an.truthy(an.item(tree, 2))) {
        values::consume(an, an.item(tree, 2), env, node, values::sub(an, site, V.RESULT), origin);
    }
}

void call_object(Analysis& an, Value node, Obj fn, int64_t receiver, std::span<const Value> args, Value where) {
    const Vocab& V = an.V;
    Value key = an.okey(fn), kind = an.okind(fn);
    if (kind == V.SITE_DECLARED_FN) {
        Value tree = an.item(key, 1), env = an.item(key, 2);
        Value site = an.tuple({an.item(key, 3), an.item(key, 4)});
        for (size_t i = 0; i < args.size(); ++i) {
            Value part = positional(an, an.item(tree, 1), an.item(tree, 3).data(), i);
            if (an.truthy(part) && args[i].kind() != Kind::None) {
                values::consume(an, part, env, args[i], values::sub(an, site, I(static_cast<int64_t>(i))), node);
            }
        }
        if (an.truthy(an.item(tree, 2))) {
            an.follow(values::produce(an, an.item(tree, 2), env, values::sub(an, site, V.RESULT)), node);
        }
    } else if (kind == V.SITE_UNKNOWN) {
        summaries::open_call(an, node, args, where);
    } else if (kind == V.SITE_RESOLVER) {
        if (!args.empty() && args[0].kind() != Kind::None) protocols::settle(an, an.item(key, 1), args[0]);
    } else {
        signatures(an, node, fn, V.CALL_SIGNATURE, args, where);
    }
    (void)receiver;
}

void construct(Analysis& an, Value node, Obj obj, std::span<const Value> args) {
    signatures(an, node, obj, an.V.CONSTRUCT_SIGNATURE, args, call_site(an, node));
}

}  // namespace ftengine::declared
