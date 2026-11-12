// A unit's references lowered in one context (heap/refs.py): a call is the node of its call tree and each reference
// written for it a link at its line; `new` writes its line on each constructor call; a write puts its value in each
// receiver object's property or the variable it names; a read of a declared field and a function written as a value
// are links too; an import runs the imported module's code.
#include "semantics.h"

namespace ftengine::refs {

namespace {

bool local_name(const UnitRec& unit, Value name) {
    return unit.types.count(name.bits()) || unit.param_index(name) >= 0;
}

void construct(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx) {
    Value node = an.node(an.tuple({an.V.CONSTRUCT, ref.base, I(ref.site), ref.args}), unit, ctx);
    an.subscribe(an.node(ref.base, unit, ctx), an.tuple({an.V.WRITTEN, node}), an.tuple({an.V.NEW}),
                 pack(an, kNewWritten, {node, I(unit.gid), I(ref.line)}));
}

void write(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx) {
    if (!an.truthy(ref.args)) return;
    Value value = an.node(an.item(ref.args, 0), unit, ctx);
    if (ref.base.kind() == Kind::None) {
        if (local_name(unit, ref.name)) {
            an.local(unit, ref.name, ctx);
            return;
        }
        if (names::enclosing(an, unit, ref.name, an.ROOT).first != nullptr) return;
        for (int64_t gid : names::definitions(an, unit, ref.name)) {
            if (an.is_state(gid)) {
                an.follow(value, an.hold(gid));
                an.link(unit.gid, gid, an.V.WRITE, ref.line);
            }
        }
        return;
    }
    an.subscribe(an.node(ref.base, unit, ctx), an.tuple({an.V.WRITE, I(unit.gid), I(ref.line), ref.name, ctx}),
                 an.tuple({an.V.WRITE}), pack(an, kWriteByObject, {value, ref.name, I(unit.gid), I(ref.line), ctx}));
}

void store(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx) {
    if (!an.truthy(ref.args) || ref.key.kind() == Kind::None) return;
    Value value = an.node(an.item(ref.args, 0), unit, ctx);
    an.node(ref.key, unit, ctx);
    Value known = texts::of(an, ref.key, unit);
    an.subscribe(an.node(ref.base, unit, ctx), an.tuple({an.V.STORE, I(unit.gid), I(ref.line), ctx}),
                 an.tuple({an.V.STORE}), pack(an, kStoreByObject, {value, known, I(unit.gid), I(ref.line), ctx,
                                                                an.node(ref.key, unit, ctx)}));
}

void read(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx) {
    if (ref.base.kind() == Kind::None) {
        if (local_name(unit, ref.name) || unit.lambdas.count(ref.name.bits())) return;
        for (int64_t gid : names::definitions(an, unit, ref.name)) {
            if (an.is_state(gid)) an.link(unit.gid, gid, an.V.READ, ref.line);
        }
        return;
    }
    an.subscribe(an.node(ref.base, unit, ctx), an.tuple({an.V.READ, I(unit.gid), I(ref.line), ref.name, ctx}),
                 an.tuple({an.V.READ}), pack(an, kReadLink, {I(unit.gid), ref.name, I(ref.line)}));
}

}  // namespace

void escape(Analysis& an, int64_t gid, int64_t line, Value value, Value ctx) {
    Value node = an.tuple({an.V.ESCAPE, I(gid), I(line), ctx});
    if (!an.wire_once(node)) return;
    an.written(node, gid, line);
    Value args[] = {value};
    summaries::open_call(an, node, args, declared::call_site(an, node));
}

void lower(Analysis& an, const UnitRec& unit, Value ctx) {
    const Vocab& V = an.V;
    for (const RefRec& ref : unit.refs) {
        Value kind = ref.kind;
        if (kind == V.CALL) {
            Value callee = ref.base.kind() != Kind::None ? an.tuple({V.MEMBER, ref.base, ref.name, B(false)})
                                                         : an.tuple({V.NAME, ref.name});
            an.written(an.node(an.tuple({V.CALL, callee, ref.args}), unit, ctx), unit.gid, ref.line);
        } else if (kind == V.INVOKE && ref.base.kind() != Kind::None) {
            an.written(an.node(an.tuple({V.CALL, ref.base, ref.args}), unit, ctx), unit.gid, ref.line);
        } else if (kind == V.NEW && ref.base.kind() != Kind::None) {
            construct(an, unit, ref, ctx);
        } else if (kind == V.WRITE) {
            write(an, unit, ref, ctx);
        } else if (kind == V.STORE) {
            store(an, unit, ref, ctx);
        } else if (kind == V.READ) {
            read(an, unit, ref, ctx);
        } else if (kind == V.LAMBDA && ref.target >= 0) {
            an.link(unit.gid, an.offset(unit) + ref.target, V.LAMBDA, ref.line);
        } else if (kind == V.JSX && ref.base.kind() != Kind::None) {
            jsx::written(an, unit, ref, ctx);
        } else if (kind == V.IMPORT && an.truthy(ref.text)) {
            auto found = an.in.imports.find({unit.file.bits(), ref.text.bits(), 0});
            if (found == an.in.imports.end()) {
                an.in.miss("import", {unit.file, ref.text});
                continue;
            }
            Value target = found->second;
            if (target.kind() == Kind::None) continue;
            auto modules = an.in.module_units.find(target.bits());
            if (modules == an.in.module_units.end()) continue;
            for (int64_t gid : modules->second) an.link(unit.gid, gid, V.CALL, ref.line);
        }
    }
}

}  // namespace ftengine::refs
