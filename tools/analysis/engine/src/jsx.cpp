// A JSX element (heap/jsx.py): a call of the factory the file's TypeScript configuration chooses (the classic
// transform's factory name, or the automatic runtime's function), an open call where none is configured; the props'
// children named as the runtime declares them. The configuration, runtime and children name come from the input.
#include "semantics.h"

namespace ftengine::jsx {

namespace {

const JsxRow* row(Analysis& an, const UnitRec& unit) {
    auto found = an.in.jsx.find(unit.file.bits());
    if (found == an.in.jsx.end()) {
        an.in.miss("jsx", {unit.file});
        return nullptr;
    }
    return &found->second;
}

Value mode_of(Analysis& an, const JsxRow* r) {
    if (!r) return NONE;
    auto found = r->options.find(an.V.JSX_MODE_OPTION.bits());
    return found == r->options.end() ? NONE : found->second;
}

Value dotted(Analysis& an, std::string_view text) {
    size_t dot = text.find('.');
    Value tree = an.tuple({an.V.NAME, an.str(text.substr(0, dot))});
    while (dot != std::string_view::npos) {
        size_t next = text.find('.', dot + 1);
        tree = an.tuple({an.V.MEMBER, tree, an.str(text.substr(dot + 1, next - dot - 1)), B(false)});
        dot = next;
    }
    return tree;
}

bool automatic(Analysis& an, Value mode) { return an.lookup_dict(an.V.JSX_AUTOMATIC, mode).kind() != Kind::None; }

}  // namespace

void value(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    const Vocab& V = an.V;
    Value tag = an.node(an.item(tree, 1), unit, ctx), props = an.node(an.item(tree, 2), unit, ctx);
    const JsxRow* r = row(an, unit);
    Value mode = mode_of(an, r);
    Value call = call_node(an, node, tree, unit, ctx);
    Value module = r ? r->runtime : NONE;
    Value args = an.tuple({tag, props});
    if (automatic(an, mode) && module.kind() != Kind::None) {
        Value name = an.item(an.lookup_dict(V.JSX_AUTOMATIC, mode), 1);
        Value factories = an.tuple({V.FACTORY, module, name});
        for (int64_t gid : an.in.export_of(module, name).gids) names::value_of(an, gid, factories);
        an.subscribe(factories, call, an.tuple({V.FACTORY}), pack(an, kJsxFactory, {args}));
    } else if (!an.member_of(mode, V.JSX_CLASSIC)) {
        Value argv[] = {tag, props};
        summaries::open_call(an, call, argv, declared::call_site(an, call));
    }
    an.follow(call, node);
    Value children = r ? r->children : NONE;
    if (an.truthy(children)) {
        an.subscribe(props, an.tuple({V.CHILDREN, props}), an.tuple({V.CHILDREN}), pack(an, kJsxChildren, {children}));
    }
}

Value call_node(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    const JsxRow* r = row(an, unit);
    if (an.member_of(mode_of(an, r), an.V.JSX_CLASSIC)) {
        auto found = r->options.find(an.V.JSX_FACTORY_OPTION.bits());
        Value factory = found != r->options.end() && an.truthy(found->second) ? found->second : an.V.JSX_FACTORY_DEFAULT;
        Value call = an.tuple({an.V.CALL, dotted(an, an.text(factory)), an.tuple({an.item(tree, 1), an.item(tree, 2)})});
        return an.node(call, unit, ctx);
    }
    std::vector<Value> items{an.V.JSX_CALL};
    auto rest = an.at(node).subspan(1);
    items.insert(items.end(), rest.begin(), rest.end());
    return an.tuple(items);
}

void written(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx) {
    Value props = an.truthy(ref.args) ? an.item(ref.args, 0) : an.tuple({an.V.LITERAL, an.str("")});
    Value tree = an.tuple({an.V.JSX, ref.base, props});
    Value node = an.node(tree, unit, ctx);
    an.written(call_node(an, node, tree, unit, ctx), unit.gid, ref.line);
}

void named_children(Analysis& an, Obj obj, Value name) {
    Value key = an.class_of(obj);
    if (key.kind() == Kind::None) return;
    if (!an.in.member(key, an.V.JSX_CHILDREN).empty()) {     // the props' children, as the literal defined them
        an.follow(members::field(an, obj, an.V.JSX_CHILDREN), members::field(an, obj, name));
    }
}

}  // namespace ftengine::jsx
