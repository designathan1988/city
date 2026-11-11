// What a name is where it is read (heap/names.py): a local function, a local or parameter (and the enclosing
// function's, for a closure), else a definition of the module, a namespace, an import or the global scope.
#include "semantics.h"

namespace ftengine::names {

void wire_name(Analysis& an, Value node, const UnitRec& unit, Value name, Value ctx) {
    auto lambda = unit.lambdas.find(name.bits());
    if (lambda != unit.lambdas.end()) {
        int64_t gid = an.offset(unit) + lambda->second;
        an.add(node, an.function_object(gid, scope(an, &unit, ctx, an.unit(gid).outer)));
        return;
    }
    if (unit.types.count(name.bits()) || unit.param_index(name) >= 0) {
        an.follow(an.local(unit, name, ctx), node);
        return;
    }
    auto [owner, owner_ctx] = enclosing(an, unit, name, ctx);
    if (owner != nullptr) {
        an.follow(an.local(*owner, name, owner_ctx), node);
        return;
    }
    std::vector<int64_t> found = definitions(an, unit, name);
    for (int64_t gid : found) value_of(an, gid, node);
    if (!found.empty()) return;
    const NameRow& row = an.in.name(unit.gid, name);
    for (Value m : row.bound) an.add(node, an.make({an.V.SITE_MODULE, m}));
    if (row.class_of.kind() != Kind::None && an.in.cls(row.class_of)) {
        an.add(node, an.make({an.V.SITE_CLASS, row.class_of}));
    }
}

Value scope(Analysis& an, const UnitRec* unit, Value ctx, int64_t local_id) {
    while (unit->local_id != local_id && unit->outer >= 0) {
        unit = &an.unit(an.offset(*unit) + unit->outer);
        ctx = contexts::environment(an, ctx);
    }
    return ctx;
}

std::pair<const UnitRec*, Value> enclosing(Analysis& an, const UnitRec& start, Value name, Value ctx) {
    const UnitRec* unit = &start;
    while (unit->outer >= 0) {
        unit = &an.unit(an.offset(*unit) + unit->outer);
        ctx = contexts::environment(an, ctx);
        if (unit->types.count(name.bits()) || unit->param_index(name) >= 0) return {unit, ctx};
    }
    return {nullptr, ctx};
}

std::vector<int64_t> definitions(Analysis& an, const UnitRec& unit, Value name) {
    return an.in.name(unit.gid, name).definitions;
}

void value_of(Analysis& an, int64_t gid, Value node) {
    const UnitRec& unit = an.unit(gid);
    if (an.is_function_unit(gid)) {
        an.add(node, an.function_object(gid, an.ROOT));
        return;
    }
    const ClassRec* c = an.in.cls(unit.qual);
    if (unit.kind == an.V.CLASS_UNIT && !(c && c->shape)) {
        an.add(node, an.make({an.V.SITE_CLASS, unit.qual}));
    } else if (an.is_state(gid)) {
        an.follow(an.hold(gid), node);
    }
}

}  // namespace ftengine::names
