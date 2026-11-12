// A write through a key whose value is read with that same key (heap/correlation.py; Sridharan et al., ECOOP 2012):
// the written value's tree, with the locals of its function whose values read the key put in place, evaluated once per
// name with the key read as that name's text; a value that reads no key is written as it is.
#include "semantics.h"

namespace ftengine::correlation {

namespace {

bool holds(Analysis& an, Value tree) {             // a declared type's cast is no value a local holds (trees.cpp)
    return an.truthy(tree) && (an.item(tree, 0) != an.V.CAST || trees::is_made(an, an.item(tree, 1)));
}

std::pair<Value, bool> expand(Analysis& an, const UnitRec& unit, Value tree, Value key_tree,
                              std::unordered_set<uint64_t>& seen) {
    if (tree == key_tree) return {tree, true};
    if (!an.is_tuple(tree) || !an.truthy(tree)) return {tree, false};
    auto items = an.at(tree);
    if (items[0] == an.V.NAME && items.size() == 2) {
        auto found = unit.types.find(items[1].bits());
        if (found != unit.types.end() && !seen.count(items[1].bits())) {
            seen.insert(items[1].bits());
            std::vector<Value> parts;
            bool reads = false;
            for (Value value : found->second) {
                if (!holds(an, value)) continue;
                auto [part, read] = expand(an, unit, value, key_tree, seen);
                parts.push_back(part);
                reads = reads || read;
            }
            seen.erase(items[1].bits());
            if (!reads) return {tree, false};
            Value joined = parts[0];
            for (size_t i = 1; i < parts.size(); ++i) joined = an.tuple({an.V.UNION, joined, parts[i]});
            return {joined, true};
        }
        return {tree, false};
    }
    std::vector<Value> out(items.begin(), items.end());
    bool reads = false;
    for (Value& part : out) {
        if (!an.is_tuple(part)) continue;
        auto [expanded, read] = expand(an, unit, part, key_tree, seen);
        part = expanded;
        reads = reads || read;
    }
    return {reads ? an.tuple(out) : tree, reads};
}

Value substitute(Analysis& an, Value tree, Value key_tree, Value replacement) {
    if (tree == key_tree) return replacement;
    if (!an.is_tuple(tree)) return tree;
    auto items = an.at(tree);
    std::vector<Value> out(items.begin(), items.end());
    for (Value& part : out) part = substitute(an, part, key_tree, replacement);
    return an.tuple(out);
}

}  // namespace

Value describe(Analysis& an, const UnitRec& unit, Value tree, Value key_tree, Value ctx) {
    std::unordered_set<uint64_t> seen;
    auto [expanded, reads] = expand(an, unit, tree, key_tree, seen);
    return reads ? an.tuple({an.V.CORRELATED, I(unit.gid), expanded, key_tree, ctx}) : NONE;
}

Value source(Analysis& an, Value given, Value name) {
    if (!an.is_tuple(given) || an.item(given, 0) != an.V.CORRELATED) return given;
    const UnitRec& unit = an.unit(an.item(given, 1).data());
    Value tree = an.item(given, 2), key_tree = an.item(given, 3), ctx = an.item(given, 4);
    if (name.kind() != Kind::None) tree = substitute(an, tree, key_tree, an.tuple({an.V.STRNAME, name}));
    return an.node(tree, unit, ctx);
}

}  // namespace ftengine::correlation
