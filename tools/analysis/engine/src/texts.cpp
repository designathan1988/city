// The texts a key position can have, read from the code around it (heap/texts.py): a string literal its text; a
// union, a narrowing either side; a name, the values its declarations give when every one is such a text; anything
// else is no known text (NONE).
#include "semantics.h"

namespace ftengine::texts {

namespace {

using Seen = std::vector<std::pair<int64_t, uint64_t>>;
bool of_into(Analysis& an, Value tree, const UnitRec& unit, Seen& seen, std::vector<Value>& out);

bool every(Analysis& an, std::span<const Value> trees, const UnitRec& unit, Seen& seen, std::vector<Value>& out) {
    for (Value tree : trees) {
        if (!of_into(an, tree, unit, seen, out)) return false;
    }
    return true;
}

bool named(Analysis& an, Value name, const UnitRec& unit, Seen& seen, std::vector<Value>& out) {
    std::pair<int64_t, uint64_t> key{unit.gid, name.bits()};
    if (std::find(seen.begin(), seen.end(), key) != seen.end()) return true;
    seen.push_back(key);
    struct Pop {
        Seen& s;
        ~Pop() { s.pop_back(); }
    } pop{seen};
    if (unit.param_index(name) >= 0) return false;
    auto types = unit.types.find(name.bits());
    if (types != unit.types.end()) {
        return !types->second.empty() && every(an, types->second, unit, seen, out);
    }
    auto [owner, ctx] = names::enclosing(an, unit, name, an.ROOT);
    if (owner != nullptr) return named(an, name, *owner, seen, out);
    std::vector<int64_t> found = names::definitions(an, unit, name);
    if (found.empty()) return false;
    for (int64_t gid : found) {
        if (!an.is_state(gid)) return false;
    }
    for (int64_t gid : found) {
        if (!every(an, an.unit(gid).values, an.unit(gid), seen, out)) return false;
    }
    return true;
}

bool of_into(Analysis& an, Value tree, const UnitRec& unit, Seen& seen, std::vector<Value>& out) {
    if (!an.is_tuple(tree) || !an.truthy(tree)) return false;
    Value kind = an.item(tree, 0);
    if (kind == an.V.STRNAME) {
        out.push_back(an.item(tree, 1));
        return true;
    }
    if (kind == an.V.UNION) {
        return of_into(an, an.item(tree, 1), unit, seen, out) && of_into(an, an.item(tree, 2), unit, seen, out);
    }
    if (kind == an.V.NARROW) return of_into(an, an.item(tree, 1), unit, seen, out);
    if (kind == an.V.NAME) return named(an, an.item(tree, 1), unit, seen, out);
    return false;
}

}  // namespace

Value of(Analysis& an, Value tree, const UnitRec& unit) {
    Seen seen;
    std::vector<Value> out;
    if (!of_into(an, tree, unit, seen, out)) return NONE;
    return TV(an.s.term(Shape::FrozenSet, 0, out));
}

Value argument(Analysis& an, Value node, size_t index) {
    auto items = an.at(node);
    if (!an.is_tuple(node) || items.size() < 4 || !an.is_tuple(items[2]) || !an.truthy(items[2])) return NONE;
    Value tree = items[2];
    if (items[1].kind() != Kind::Int) return NONE;
    const UnitRec& unit = an.unit(items[1].data());
    Value args = an.item(tree, 0) == an.V.CALL ? an.item(tree, 2)
                 : an.item(tree, 0) == an.V.CONSTRUCT && an.at(tree).size() > 3 ? an.item(tree, 3) : NONE;
    if (args.kind() == Kind::None || index >= an.at(args).size()) return NONE;
    return of(an, an.at(args)[index], unit);
}

}  // namespace ftengine::texts
