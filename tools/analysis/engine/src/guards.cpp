// What a test of the program proves where it holds (heap/guards.py; log/trace-research.md F31): a value read where a
// guard is true (positive) or false of it keeps each object the guard can be so of, once and for all as it arrives
// (verdicts.cpp says what each test can give of one value); a property's truthiness or equality by that object's
// property as its values arrive; `instanceof` by each value its right side holds, as either grows; a declared type
// predicate or assertion by the type its callee declares (the program's or the platform's declaration): where true,
// the values that can be of it, where false those not surely of it. No function is known by its name.
#include "semantics.h"

namespace ftengine::guards {

namespace {

using namespace verdicts;   // NOLINT: the verdicts are this file's vocabulary

bool passes(Analysis& an, Obj o, Value guard, bool positive) {
    const Vocab& V = an.V;
    Value kind = an.item(guard, 0);
    if (kind == V.G_TYPEOF) {
        auto results = typeof_of(an, o);
        size_t same = 0;
        for (Value r : results) same += r == an.item(guard, 1);
        return positive ? same > 0 : results.size() > same;
    }
    if (kind == V.G_TRUTHY) return positive ? truthiness(an, o).first : truthiness(an, o).second;
    if (kind == V.G_EQUALS) return positive ? can_equal(an, o, an.item(guard, 1)) : can_differ(an, o, an.item(guard, 1));
    if (kind == V.G_NULLISH) return can(nullish(an, o), positive);
    if (kind == V.G_HAS) return !primitive(an, o);
    return true;
}

bool defined(Analysis& an, Obj o, Value name) {   // it always has the property, with a value
    Value kind = an.okind(o);
    if (kind == an.V.SITE_DECLARED || kind == an.V.SITE_UNKNOWN) return true;
    if (kind != an.V.SITE_NEW && kind != an.V.SITE_LITERAL) return false;
    for (Value key : typesys::own_types(an, o)) {
        for (int64_t gid : typesys::lookup(an, key, name)) {
            const UnitRec& u = an.unit(gid);
            if (an.is_function_unit(gid) || !u.accessors.empty() || !u.values.empty()) return true;
        }
    }
    return false;
}

Value key_of(Analysis& an, std::initializer_list<Value> items) { return an.tuple(items); }

}  // namespace

void wire(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx) {
    const Vocab& V = an.V;
    Value guard = an.item(tree, 2);
    bool positive = an.truthy(an.item(tree, 3));
    Value source = an.node(an.item(tree, 1), unit, ctx), kind = an.item(guard, 0);
    if (kind == V.G_MEMBER || kind == V.G_MEMBER_EQUALS) {
        Value constant = kind == V.G_MEMBER_EQUALS ? an.item(guard, 2) : NONE;
        if ((kind == V.G_MEMBER && !positive) || (positive && constant == V.S_UNDEFINED)) {
            an.follow(source, node);                    // a property found nowhere reads undefined
            return;
        }
        bool eager = !positive && constant != V.S_UNDEFINED;
        an.subscribe(source, node, key_of(an, {V.NARROW, guard, B(positive)}),
                     pack(an, kNarrowMember, {guard, B(positive), B(eager)}));
    } else if (kind == V.G_INSTANCE) {
        Value targets = an.node(an.item(guard, 1), unit, ctx);
        an.subscribe(source, node, key_of(an, {V.NARROW, V.G_INSTANCE, B(positive)}),
                     pack(an, kInstanceObjects, {targets, B(positive)}));
        an.subscribe(targets, node, key_of(an, {V.NARROW, V.G_INSTANCE, B(positive), targets}),
                     pack(an, kInstanceTargets, {source, B(positive)}));
    } else if (kind == V.G_PREDICATE || kind == V.G_ASSERTS) {
        Value callee = an.node(an.item(guard, 1), unit, ctx), index = an.item(guard, 2);
        an.subscribe(callee, node, key_of(an, {V.NARROW, V.G_PREDICATE, index, B(positive)}),
                     pack(an, kPredicateFunctions, {source, index, B(positive)}));
    } else {
        an.subscribe(source, node, key_of(an, {V.NARROW, guard, B(positive)}),
                     pack(an, kNarrowValue, {guard, B(positive)}));
    }
}

void narrow_value(Analysis& an, Value target, Value guard, bool positive, std::span<const Obj> objs) {
    std::vector<Obj> kept;
    for (Obj o : objs) {
        if (passes(an, o, guard, positive)) kept.push_back(o);
    }
    an.add(target, kept);
}

void narrow_member(Analysis& an, Value target, Value guard, bool positive, bool eager, std::span<const Obj> objs) {
    const Vocab& V = an.V;
    Value name = an.item(guard, 1);
    Value constant = an.item(guard, 0) == V.G_MEMBER_EQUALS ? an.item(guard, 2) : NONE;
    std::vector<Obj> list(objs.begin(), objs.end());
    for (Obj o : list) {
        if (eager && !defined(an, o, name)) {
            an.add(target, o);
            continue;
        }
        Value held = key_of(an, {V.GD_MEMBER, I(o), name});
        if (an.wire_once(held)) members::read(an, held, o, name);
        an.subscribe(held, target, key_of(an, {V.NARROW, guard, B(positive), I(o)}),
                     pack(an, kMemberPasses, {I(o), constant, B(positive)}));
    }
}

void member_passes(Analysis& an, Value target, Obj obj, Value constant, bool positive, std::span<const Obj> values) {
    for (Obj v : values) {
        bool ok = constant.kind() == Kind::None ? truthiness(an, v).first
                  : positive                    ? can_equal(an, v, constant)
                                                : can_differ(an, v, constant);
        if (ok) {
            an.add(target, obj);
            return;
        }
    }
}

void instance_objects(Analysis& an, Value target, Value targets, bool positive, std::span<const Obj> objs) {
    auto held = an.held(targets);
    std::vector<Obj> classes(held.begin(), held.end()), kept;
    for (Obj o : objs) {
        for (Obj c : classes) {
            if (can(outcomes(an, o, c), positive)) {
                kept.push_back(o);
                break;
            }
        }
    }
    an.add(target, kept);
}

void instance_targets(Analysis& an, Value target, Value source, bool positive, std::span<const Obj> targets) {
    auto held = an.held(source);
    std::vector<Obj> values(held.begin(), held.end()), classes(targets.begin(), targets.end()), kept;
    for (Obj o : values) {
        for (Obj c : classes) {
            if (can(outcomes(an, o, c), positive)) {
                kept.push_back(o);
                break;
            }
        }
    }
    an.add(target, kept);
}

void predicate_functions(Analysis& an, Value target, Value source, int64_t index, bool positive,
                         std::span<const Obj> fns) {
    std::vector<Obj> list(fns.begin(), fns.end());
    for (Obj fn : list) {
        int64_t gid = an.function(fn);
        Value found = gid >= 0 ? an.unit(gid).predicate_type : NONE;
        if (!an.truthy(found) || an.item(found, 0).data() != index) {
            an.follow(source, target);                  // no predicate on that argument: nothing is proved
            continue;
        }
        an.subscribe(source, target, key_of(an, {an.V.NARROW, an.V.G_PREDICATE, I(gid), B(positive)}),
                     pack(an, kPredicateAdmits, {an.item(found, 1), B(positive)}));
    }
}

void predicate_admits(Analysis& an, Value target, Value tree, bool positive, std::span<const Obj> objs) {
    std::vector<Obj> kept;
    for (Obj o : objs) {
        if (positive ? admits(an, tree, o) : !surely(an, tree, o)) kept.push_back(o);
    }
    an.add(target, kept);
}

}  // namespace ftengine::guards
