// Copies of an object's properties by name (heap/copies.py; log/trace-research.md F34; ECMA-262 CopyDataProperties
// for a literal's spread, a declared intersection for one object that is both): each own property of the source into
// the target's property of the same name — those made so far and those made later (members::field hands each new
// property on) — and the source's dictionary part into the target's. A declared object's properties are what its
// declared types give; a string's characters are its indexed properties; the unknown object's properties are unknown.
#include <algorithm>

#include "semantics.h"

namespace ftengine::copies {

namespace {

std::vector<Value> declared_names(Analysis& an, Obj obj) {
    const Vocab& V = an.V;
    std::vector<Value> names;
    for (Value key : typesys::own_types(an, obj)) {
        for (Value k : typesys::ancestors(an, key)) {
            for (Value g : typesys::group(an, k)) {
                for (const auto& [name, gids] : an.in.members_of(g)) {
                    if (name == V.CALL_SIGNATURE || name == V.CONSTRUCT_SIGNATURE || name == V.INDEX_SIGNATURE) continue;
                    bool property = false;                 // a method signature: a prototype's, no own property
                    for (int64_t g2 : gids) property = property || an.is_state(g2);
                    if (!property) continue;
                    if (std::find(names.begin(), names.end(), name) == names.end()) names.push_back(name);
                }
            }
        }
    }
    return names;
}

}  // namespace

void copy(Analysis& an, Obj source, Obj target) {
    const Vocab& V = an.V;
    Value kind = an.okind(source);
    if (source == target) return;
    if (kind == V.SITE_UNKNOWN) {
        an.add(an.any_field(target), source);
        return;
    }
    if (kind == V.SITE_PRIMITIVE || kind == V.SITE_TEXT || kind == V.SITE_KEY) {
        if (verdicts::has_typeof(an, source, V.TYPEOF_STRING)) {
            an.add(members::field(an, target, V.ELEMENTS_FIELD), an.primitive(V.GD_STRING));
        }
        return;
    }
    uint64_t pair = static_cast<uint64_t>(source) << 32 | target;
    if (!an.copied.insert(pair).second) return;
    an.copies[source].push_back(target);
    std::vector<Value> names = an.names_of[source];
    for (Value name : names) an.follow(members::field(an, source, name), members::field(an, target, name));
    an.follow(an.any_field(source), an.any_field(target));
    if (kind != V.SITE_DECLARED) return;
    for (Value name : declared_names(an, source)) members::read(an, members::field(an, target, name), source, name);
}

}  // namespace ftengine::copies
