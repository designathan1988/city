// The value of each shape of expression tree (heap/trees.py): a name what it names, a member what each object's
// property holds, a call what each function it can run returns, `new` the object made at that site, a literal its
// object, either side of a union.
#include "semantics.h"

namespace ftengine::trees {

namespace {

Value tag(Analysis& an, Value tree) { return an.item(tree, 0); }

void wire_index(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    Value key = an.node(an.item(tree, 2), unit, ctx);
    Value known = texts::of(an, an.item(tree, 2), unit);
    an.subscribe(an.node(an.item(tree, 1), unit, ctx), node, an.tuple({an.V.INDEX}),
                 pack(an, kIndexByObject, {key, known}));
}

Value arg_nodes(Analysis& an, Value trees, const UnitRec& unit, Value ctx) {
    std::vector<Value> out;
    for (Value a : an.at(trees)) out.push_back(an.node(a, unit, ctx));
    return an.tuple(out);
}

void wire_call(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    Value callee = an.item(tree, 1);
    Value args = arg_nodes(an, an.item(tree, 2), unit, ctx);
    Value super_ = an.tuple({an.V.SUPER});
    if (tag(an, callee) == an.V.MEMBER && an.item(callee, 1) != super_) {
        Value name = an.item(callee, 2);
        an.subscribe(an.node(an.item(callee, 1), unit, ctx), node, an.tuple({an.V.CALL, name}),
                     pack(an, kCallByReceiver, {name, args}));
    } else if (tag(an, callee) == an.V.MEMBER) {
        Value name = an.item(callee, 2);
        an.subscribe(this_of(an, unit, ctx), node, an.tuple({an.V.SUPER, name}),
                     pack(an, kSuperByThis, {name, args, I(unit.gid)}));
    } else {
        an.subscribe(an.node(callee, unit, ctx), node, an.tuple({an.V.CALL}), pack(an, kPlainCall, {args}));
    }
}

void literal_methods(Analysis& an, Obj obj, Value key, const UnitRec& unit, Value ctx) {
    for (const auto& [name, gids] : an.in.members_of(key)) {
        for (int64_t gid : gids) {
            const UnitRec& method = an.unit(gid);
            if (an.is_function_unit(gid) && !an.truthy(method.accessor_of)) {
                Value env = names::scope(an, &unit, ctx, method.outer);
                an.add(members::field(an, obj, name), an.function_object(gid, env));
            }
        }
    }
}

void wire_cast(Analysis& an, Value node, Value text, const UnitRec& unit, Value ctx) {
    if (!an.starts(text, an.V.CLASS_VALUE)) return;
    Value key = an.rest(text, an.V.CLASS_VALUE);
    const ClassRec* c = an.in.cls(key);
    int64_t made = c ? c->made : -1;
    if (made == an.V.MADE_LITERAL.data()) {
        Obj obj = an.make({an.V.SITE_LITERAL, key, contexts::heap(an, ctx)});
        an.add(node, obj);
        an.instance_made(obj);
        literal_methods(an, obj, key, unit, ctx);
    } else if (made == an.V.MADE_CLASS.data()) {
        an.add(node, an.make({an.V.SITE_CLASS, key, ctx, I(unit.gid)}));
    }
}

bool is_made(Analysis& an, Value text) {
    if (!an.starts(text, an.V.CLASS_VALUE)) return false;
    const ClassRec* c = an.in.cls(an.rest(text, an.V.CLASS_VALUE));
    return c && c->made > 0;
}

std::pair<const UnitRec*, Value> lexical(Analysis& an, const UnitRec& start, Value ctx) {
    const UnitRec* unit = &start;
    while (unit->this_of == an.V.ARROW && unit->outer >= 0) {
        unit = &an.unit(an.offset(*unit) + unit->outer);
        ctx = contexts::environment(an, ctx);
    }
    return {unit, ctx};
}

void construct(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    Value args = arg_nodes(an, an.item(tree, 3), unit, ctx);
    an.subscribe(an.node(an.item(tree, 1), unit, ctx), node, an.tuple({an.V.CONSTRUCT}),
                 pack(an, kConstructByValue, {node, args}));
}

}  // namespace

void wire(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    const Vocab& V = an.V;
    Value kind = tag(an, tree);
    if (kind == V.NAME) {
        names::wire_name(an, node, unit, an.item(tree, 1), ctx);
    } else if (kind == V.MEMBER) {
        Value name = an.item(tree, 2);
        if (an.item(tree, 1) == an.tuple({V.SUPER})) {
            an.subscribe(this_of(an, unit, ctx), node, an.tuple({V.SUPER, name}),
                         pack(an, kSuperRead, {name, I(unit.gid)}));
            return;
        }
        an.subscribe(an.node(an.item(tree, 1), unit, ctx), node, an.tuple({V.MEMBER, name}),
                     pack(an, kMemberRead, {name}));
    } else if (kind == V.INDEX) {
        wire_index(an, node, tree, unit, ctx);
    } else if (kind == V.CALL) {
        wire_call(an, node, tree, unit, ctx);
    } else if (kind == V.CONSTRUCT) {
        construct(an, node, tree, unit, ctx);
    } else if (kind == V.CALLABLE) {
        int64_t gid = an.offset(unit) + an.item(tree, 1).data();
        an.add(node, an.function_object(gid, names::scope(an, &unit, ctx, an.unit(gid).outer)));
    } else if (kind == V.CAST) {
        wire_cast(an, node, an.item(tree, 1), unit, ctx);
    } else if (kind == V.COLLECTION) {
        Value site = an.at(tree).size() > 2 ? an.item(tree, 2) : I(tree.id());
        Obj array = an.make({V.SITE_ARRAY, unit.file, site, contexts::heap(an, ctx)});
        an.add(node, array);
        for (Value value : an.at(an.item(tree, 1))) {
            an.follow(an.node(value, unit, ctx), members::field(an, array, V.ELEMENTS_FIELD));
        }
    } else if (kind == V.ELEMENTS || kind == V.EACH) {
        protocols::iterate(an, node, an.node(an.item(tree, 1), unit, ctx));
    } else if (kind == V.UNION) {
        an.follow(an.node(an.item(tree, 1), unit, ctx), node);
        an.follow(an.node(an.item(tree, 2), unit, ctx), node);
    } else if (kind == V.AWAIT) {
        protocols::settle(an, node, an.node(an.item(tree, 1), unit, ctx));
    } else if (kind == V.NARROW) {
        an.follow(an.node(an.item(tree, 1), unit, ctx), node);
    } else if (kind == V.SELF) {
        an.follow(this_of(an, unit, ctx), node);
    } else if (kind == V.LITERAL) {
        for (Value k : literal_kinds(an, an.item(tree, 1))) an.add(node, an.make({V.SITE_PRIMITIVE, k}));
    } else if (kind == V.STRNAME) {
        an.add(node, an.make({V.SITE_PRIMITIVE, V.STRING_KIND}));
    } else if (kind == V.OP || kind == V.UNARY) {
        auto found = an.in.operators.find({an.item(tree, 1).bits(), static_cast<uint64_t>(kind == V.UNARY), 0});
        if (found == an.in.operators.end()) {
            an.in.miss("operator", {an.item(tree, 1)});
            return;
        }
        for (Value k : found->second) an.add(node, an.make({V.SITE_PRIMITIVE, k}));
    } else if (kind == V.JSX) {
        jsx::value(an, node, tree, unit, ctx);
    } else if (kind == V.KEYS) {
        an.add(node, an.make({V.SITE_PRIMITIVE, V.STRING_KIND}));
        an.subscribe(an.node(an.item(tree, 1), unit, ctx), node, an.tuple({V.KEYS}), pack(an, kKeys, {}));
    }
}

void index_by_object(Analysis& an, Value target, Value key, Value known, std::span<const Obj> objs) {
    for (Obj o : objs) {
        if (known.kind() != Kind::None) {
            for (Value text : an.at(known)) members::read(an, target, o, text);
            continue;
        }
        an.subscribe(key, target, an.tuple({an.V.INDEX, I(o)}), pack(an, kIndexKeys, {I(o)}));
    }
}

Value this_of(Analysis& an, const UnitRec& unit, Value ctx) {
    auto [owner, owner_ctx] = lexical(an, unit, ctx);
    return an.this_(owner->gid, owner_ctx);
}

std::vector<Value> base_keys(Analysis& an, const UnitRec& unit) {
    Value owner = lexical(an, unit, an.ROOT).first->owner;
    return an.in.cls(owner) ? typesys::base_keys(an, owner) : std::vector<Value>{};
}

void wire_local(Analysis& an, Value node, const UnitRec& unit, Value name, Value ctx) {
    int index = unit.param_index(name);
    if (index >= 0) an.follow(an.param(unit.gid, index, ctx), node);
    auto types = unit.types.find(name.bits());
    if (types != unit.types.end()) {
        for (Value tree : types->second) {
            bool value = an.truthy(tree) && (tag(an, tree) != an.V.CAST || is_made(an, an.item(tree, 1)));
            if (value) an.follow(an.node(tree, unit, ctx), node);
        }
    }
    if (unit.captured.count(name.bits()) && unit.outer >= 0) {
        const UnitRec& owner = an.unit(an.offset(unit) + unit.outer);
        Value outer = an.local(owner, name, contexts::environment(an, ctx));
        an.follow(outer, node);
        auto writers = owner.written_by.find(name.bits());
        if (writers != owner.written_by.end()) {
            for (int64_t id : writers->second) {
                if (id == unit.local_id) {
                    an.follow(node, outer);
                    break;
                }
            }
        }
    }
}

Value constructed(Analysis& an, Value node, Value made) {
    return an.tuple({an.V.CONSTRUCTED, an.item(node, 1), an.item(node, 2), made, an.item(node, 3)});
}

Obj made(Analysis& an, Obj obj, Value file, Value site, Value ctx) {
    Value key = an.okey(obj);
    bool class_run = an.okind(obj) == an.V.SITE_CLASS && an.at(key).size() > 3;
    int64_t maker = an.okind(obj) == an.V.SITE_FUNCTION || class_run ? static_cast<int64_t>(obj) : -1;
    Obj m = an.make({an.V.SITE_NEW, an.made_by(obj), file, site, I(maker), contexts::heap(an, ctx)});
    an.instance_made(m);
    return m;
}

std::vector<Value> literal_kinds(Analysis& an, Value kind) {
    auto found = an.in.literals.find(kind.bits());
    if (found == an.in.literals.end()) {
        an.in.miss("literal", {kind});
        return {};
    }
    return found->second;
}

void construct_by_value(Analysis& an, Value target, Value node, Value args, std::span<const Obj> objs) {
    const UnitRec& unit = an.unit(an.item(node, 1).data());
    Value tree = an.item(node, 2), ctx = an.item(node, 3);
    Value site = an.at(tree).size() > 2 ? an.item(tree, 2) : I(-1);
    auto arg_list = an.at(args);
    std::vector<Value> argv(arg_list.begin(), arg_list.end());
    for (Obj obj : objs) {
        Value key = an.made_by(obj);
        Value call = constructed(an, node, key.kind() != Kind::None ? key : I(obj));
        if (key.kind() == Kind::None) {
            declared::construct(an, call, obj, argv);
            an.follow(call, target);
            continue;
        }
        Obj instance = made(an, obj, unit.file, site, ctx);
        an.add(target, instance);
        std::vector<Obj> ctors;
        if (an.okind(obj) == an.V.SITE_FUNCTION) {
            ctors.push_back(obj);
        } else {
            for (int64_t g : typesys::lookup(an, key, an.V.CONSTRUCTOR)) {
                if (an.is_function_unit(g)) ctors.push_back(an.function_object(g, members::method_env(an, obj, g)));
            }
        }
        for (Obj fn : ctors) an.call(call, fn, instance, argv, an.V.NEW);
        an.subscribe(call, target, an.tuple({an.V.CONSTRUCT, key}), pack(an, kConstructReturned, {}));
    }
}

}  // namespace ftengine::trees
