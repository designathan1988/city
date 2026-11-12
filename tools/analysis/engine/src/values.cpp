// What a declared type gives and takes (heap/values.py): a type parameter's place, the language's values, an object
// of a named or written object type made at its site with its type arguments in its places, a function of a function
// type; an argument goes where its parameter's type says (`consume`).
#include "semantics.h"

namespace ftengine::values {

namespace {

Value level_place(Analysis& an, Value env, Value level, Value index) {
    if (env.kind() == Kind::None) return NONE;
    if (level == an.V.LEVEL_TYPE && an.item(env, 0).kind() != Kind::None) {
        return typesys::place(an, static_cast<Obj>(an.item(env, 0).data()), an.item(env, 1), index.data());
    }
    if (level == an.V.LEVEL_OWN && an.item(env, 2).kind() != Kind::None) {
        return an.tuple({an.V.OWN_PLACE, an.item(env, 2), index});
    }
    return NONE;
}

void indexes(Analysis& an, Obj made, Value key, Value env, Value site) {
    for (int64_t gid : typesys::lookup(an, key, an.V.INDEX_SIGNATURE)) {
        for (const auto& [key_type, value] : an.unit(gid).indexes) {
            bool place = false;
            for (Value p : typesys::parts(an, value)) {
                if (an.truthy(p) && an.item(p, 0) == an.V.T_PARAM && an.item(p, 1) == an.V.LEVEL_TYPE) place = true;
            }
            if (place) continue;
            auto vs = typesys::values(an, key_type);
            Value field = std::find(vs.begin(), vs.end(), an.V.NUMBER) != vs.end()
                              ? members::field(an, made, an.V.ELEMENTS_FIELD) : an.any_field(made);
            an.follow(produce(an, value, env, member_site(an, gid, site)), field);
        }
    }
}

void named(Analysis& an, Value node, Value tree, Value env, Value site) {
    Value found = typesys::resolve(an, an.item(tree, 1), an.item(tree, 3), an.item(tree, 4));
    Value args = an.item(tree, 2);
    if (found.kind() == Kind::None) {
        if (an.item(tree, 1) == an.V.ARRAY_TYPE && an.truthy(args)) {
            Obj array = an.make({an.V.SITE_ARRAY, NONE, site, an.ROOT});
            an.add(node, array);
            an.follow(produce(an, an.item(args, 0), env, sub(an, site, I(0))),
                      members::field(an, array, an.V.ELEMENTS_FIELD));
        } else {
            an.add(node, an.make({an.V.SITE_UNKNOWN}));
        }
        return;
    }
    if (an.item(found, 0) == an.V.ALIAS) {
        if (on_path(an, site, found)) return;
        const UnitRec& alias = an.unit(an.item(found, 1).data());
        Value expanded = substitute(an, alias.result_type, arguments(an, args, alias.tparams));
        an.follow(produce(an, expanded, env, sub(an, site, found)), node);
        return;
    }
    Value key = an.item(found, 1);
    const ClassRec* cls = an.in.cls(key);      // a class of the program: one from outside is one the program made
    bool own = cls && !cls->shape && !cls->ambient;
    Obj made = own ? an.from_outside(key)
                   : an.make({an.V.SITE_DECLARED, key, an.item(site, 0), NONE, an.item(site, 1)});
    an.add(node, made);
    if (own) an.follow(an.instances(key), node);
    size_t params = 0;
    for (Value k : typesys::group(an, key)) {
        const ClassRec* c = an.in.cls(k);
        if (c) params = std::max(params, c->tparams.size());
    }
    auto written = an.at(args);
    for (size_t i = 0; i < params && i < written.size(); ++i) {
        an.follow(produce(an, written[i], env, sub(an, site, I(static_cast<int64_t>(i)))),
                  typesys::place(an, made, key, static_cast<int64_t>(i)));
    }
    indexes(an, made, key, an.tuple({I(made), key, NONE}), site);
}

void both(Analysis& an, Value node, Value tree, Value env, Value site) {
    std::vector<Value> parts;
    int64_t i = 0;
    for (Value part : an.at(an.item(tree, 1))) parts.push_back(produce(an, part, env, sub(an, site, I(i++))));
    for (size_t a = 0; a < parts.size(); ++a) {
        an.follow(parts[a], node);
        for (size_t b = 0; b < parts.size(); ++b) {
            if (a == b) continue;
            an.subscribe(parts[a], an.tuple({an.V.T_BOTH, node, I(static_cast<int64_t>(a)), I(static_cast<int64_t>(b))}),
                         an.tuple({an.V.T_BOTH}), pack(an, kBothOuter, {parts[b]}));
        }
    }
}

void wire(Analysis& an, Value node, Value tree, Value env, Value site) {
    const Vocab& V = an.V;
    Value kind = an.truthy(tree) ? an.item(tree, 0) : NONE;
    if (kind == V.T_PARAM) {
        Value held = level_place(an, env, an.item(tree, 1), an.item(tree, 2));
        if (held.kind() != Kind::None) an.follow(held, node);
    } else if (kind == V.T_UNION) {
        int64_t i = 0;
        for (Value part : an.at(an.item(tree, 1))) an.follow(produce(an, part, env, sub(an, site, I(i++))), node);
    } else if (kind == V.T_VALUES) {
        an.add(node, language_objects(an, an.item(tree, 1)));
    } else if (kind == V.T_BOTH) {
        both(an, node, tree, env, site);
    } else if (kind == V.T_NAMED) {
        named(an, node, tree, env, site);
    } else if (kind == V.T_OBJECT) {
        Obj made = an.make({V.SITE_DECLARED, an.item(tree, 1), an.item(site, 0), env, an.item(site, 1)});
        an.add(node, made);
        indexes(an, made, an.item(tree, 1), env, site);
    } else if (kind == V.T_FN) {
        an.add(node, an.make({V.SITE_DECLARED_FN, tree, env, an.item(site, 0), an.item(site, 1)}));
    } else if (kind == V.T_TUPLE) {
        Obj array = an.make({V.SITE_ARRAY, NONE, site, an.ROOT});
        an.add(node, array);
        int64_t i = 0;
        for (Value part : an.at(an.item(tree, 1))) {
            an.follow(produce(an, part, env, sub(an, site, I(i++))), members::field(an, array, V.ELEMENTS_FIELD));
        }
    } else if (kind == V.T_INDEX) {
        std::vector<Value> texts;
        for (Value v : typesys::values(an, an.item(tree, 2))) {
            if (an.starts(v, V.TEXT)) texts.push_back(an.rest(v, V.TEXT));
        }
        an.subscribe(produce(an, an.item(tree, 1), env, sub(an, site, I(0))), node, an.tuple({V.T_INDEX}),
                     pack(an, kTypeIndex, {an.tuple(texts)}));
    } else if (kind == V.T_QUERY) {
        for (int64_t gid : an.in.query(an.item(tree, 2), an.item(tree, 3), an.item(tree, 1))) {
            names::value_of(an, gid, node);
        }
    }
}

}  // namespace

Value sub(Analysis& an, Value site, Value part) {
    return an.tuple({an.tuple({an.item(site, 0), part}), an.item(site, 1)});
}

Value member_site(Analysis& an, int64_t gid, Value site) {
    return an.tuple({an.tuple({an.V.V_MEMBER, I(gid)}), an.item(site, 1)});
}

Value produce(Analysis& an, Value tree, Value env, Value site) {
    const Vocab& V = an.V;
    Value kind = an.truthy(tree) ? an.item(tree, 0) : NONE;
    bool reading = reads(an, tree);
    Value node;
    if (kind == V.T_VALUES) {
        node = an.tuple({V.PRODUCED, tree});
    } else if (kind == V.T_PARAM) {
        Value held = level_place(an, env, an.item(tree, 1), an.item(tree, 2));
        if (held.kind() != Kind::None) return held;
        node = an.tuple({V.PRODUCED, an.tuple({})});
    } else if (kind == V.T_THIS) {                 // the receiver itself: one node per object
        if (an.item(env, 0).kind() != Kind::None) return declared::an_node(an, static_cast<Obj>(an.item(env, 0).data()));
        node = an.tuple({V.PRODUCED, an.tuple({})});
    } else {
        node = an.tuple({V.PRODUCED, tree, reading ? env : V.NO_ENV, site});
    }
    if (an.wire_once(node)) wire(an, node, tree, reading ? env : V.NO_ENV, site);
    return node;
}

bool reads(Analysis& an, Value tree) {
    if (!an.is_tuple(tree)) return false;
    auto found = an.reads_memo.find(tree.bits());
    if (found != an.reads_memo.end()) return found->second;
    auto items = an.at(tree);
    bool tagged = !items.empty() && items[0].kind() == Kind::Str;
    bool out = tagged && (items[0] == an.V.T_PARAM || items[0] == an.V.T_THIS);
    for (size_t i = tagged ? 1 : 0; !out && i < items.size(); ++i) {
        if (an.is_tuple(items[i]) && reads(an, items[i])) out = true;
    }
    an.reads_memo[tree.bits()] = out;
    return out;
}

void consume(Analysis& an, Value tree, Value env, Value source, Value site, Value origin) {
    const Vocab& V = an.V;
    Value kind = an.truthy(tree) ? an.item(tree, 0) : NONE;
    if (summaries::is_open(an, tree) && origin.kind() != Kind::None) an.follow(source, summaries::pool(an, origin));
    if (kind == V.T_PARAM) {
        Value held = level_place(an, env, an.item(tree, 1), an.item(tree, 2));
        if (held.kind() != Kind::None) an.follow(source, held);
    } else if (kind == V.T_UNION || kind == V.T_BOTH) {
        int64_t i = 0;
        for (Value part : an.at(an.item(tree, 1))) consume(an, part, env, source, sub(an, site, I(i++)), origin);
    } else if (kind == V.T_FN || kind == V.T_NAMED || kind == V.T_OBJECT || kind == V.T_TUPLE) {
        an.subscribe(source, an.tuple({V.CONSUMED, tree, env, site, origin}), an.tuple({V.CONSUMED}),
                     pack(an, kConsumed, {tree, env, site, origin}));
    }
}

std::vector<Obj> language_objects(Analysis& an, Value values) {
    std::vector<Obj> out;
    for (Value v : an.at(values)) {
        if (v == an.V.ENGINE) {
            out.push_back(an.make({an.V.SITE_UNKNOWN}));
        } else if (an.starts(v, an.V.TEXT)) {        // a string literal type: the value of its text (F33)
            out.push_back(an.make({an.V.SITE_TEXT, an.rest(v, an.V.TEXT)}));
        } else if (an.starts(v, an.V.LANG)) {
            out.push_back(an.primitive(an.rest(v, an.V.LANG)));
        }
    }
    return out;
}

Value arguments(Analysis& an, Value args, std::span<const Value> defaults) {
    auto written = an.at(args);
    std::vector<Value> out;
    size_t count = std::max(written.size(), defaults.size());
    for (size_t i = 0; i < count; ++i) out.push_back(i < written.size() ? written[i] : defaults[i]);
    return an.tuple(out);
}

Value substitute(Analysis& an, Value tree, Value args) {
    if (!an.is_tuple(tree) || !an.truthy(tree)) return tree;
    if (an.item(tree, 0) == an.V.T_PARAM && an.item(tree, 1) == an.V.LEVEL_ALIAS) {
        int64_t i = an.item(tree, 2).data();
        auto given = an.at(args);
        return i < static_cast<int64_t>(given.size()) ? given[static_cast<size_t>(i)] : an.tuple({});
    }
    std::vector<Value> out;
    for (Value part : an.at(tree)) out.push_back(an.is_tuple(part) ? substitute(an, part, args) : part);
    return an.tuple(out);
}

bool on_path(Analysis& an, Value site, Value part) {
    Value where = an.item(site, 0);
    while (an.is_tuple(where) && an.at(where).size() == 2) {
        if (an.item(where, 1) == part) return true;
        where = an.item(where, 0);
    }
    return false;
}

void type_index(Analysis& an, Value target, Value texts, std::span<const Obj> objs) {
    for (Obj o : objs) {
        if (an.truthy(texts)) {
            for (Value text : an.at(texts)) members::read(an, target, o, text);
        } else {
            members::read_all(an, target, o);
        }
    }
}

void both_outer(Analysis& an, Value other, std::span<const Obj> objs) {
    for (Obj o : objs) {
        an.subscribe(other, an.any_field(o), an.tuple({an.V.T_BOTH, I(o)}), pack(an, kBothInner, {I(o)}));
    }
}

void both_inner(Analysis& an, Value, Obj o, std::span<const Obj> srcs) {   // one object that is both: by name
    std::vector<Obj> list(srcs.begin(), srcs.end());
    for (Obj s : list) copies::copy(an, s, o);
}

}  // namespace ftengine::values
