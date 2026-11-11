// The objects' keys (heap/objects.py) and the contexts (heap/contexts.py): an object is the term of its site's
// key, made once; what a key says (its class, maker, function, environment, heap context, site); a context is
// (environment, call), or ROOT for every run when the analysis runs without contexts.
#include "analysis.h"

#include "semantics.h"

namespace ftengine {

Obj Analysis::function_object(int64_t gid, Value env) {
    if (closures[static_cast<uint64_t>(gid)].insert(env.bits()).second) {
        closure_order[static_cast<uint64_t>(gid)].push_back(env);
    }
    return make({V.SITE_FUNCTION, I(gid), env});
}

Obj Analysis::make(Value key) {
    auto found = object_ids_.find(key.bits());
    if (found != object_ids_.end()) return found->second;
    auto id = static_cast<Obj>(objects_.size());
    object_ids_.emplace(key.bits(), id);
    objects_.push_back(key);
    return id;
}

Value Analysis::class_of(Obj o) const {
    Value kind = okind(o);
    return kind == V.SITE_NEW || kind == V.SITE_LITERAL || kind == V.SITE_DECLARED ? item(okey(o), 1) : NONE;
}

int64_t Analysis::maker(Obj o) const {
    if (okind(o) != V.SITE_NEW) return -1;
    Value m = item(okey(o), 4);
    return m.kind() == Kind::Int && m.data() >= 0 ? m.data() : -1;
}

int64_t Analysis::function(Obj o) const {
    return okind(o) == V.SITE_FUNCTION ? item(okey(o), 1).data() : -1;
}

Value Analysis::class_itself(Obj o) const {
    return okind(o) == V.SITE_CLASS ? item(okey(o), 1) : NONE;
}

bool Analysis::class_env(Obj o, Value& env, int64_t& holder) const {
    if (okind(o) != V.SITE_CLASS || at(okey(o)).size() <= 3) return false;
    env = item(okey(o), 2);
    holder = item(okey(o), 3).data();
    return true;
}

Value Analysis::env_of(Obj fn) const {
    auto items = at(okey(fn));
    return okind(fn) == V.SITE_FUNCTION && !items.empty() ? items.back() : ROOT;
}

Value Analysis::object_heap(Obj o) {
    auto items = at(okey(o));
    if (okind(o) == V.SITE_FUNCTION) return contexts::heap(*this, items.back());
    return member_of(okind(o), V.MADE) && !items.empty() ? items.back() : ROOT;
}

Obj Analysis::object_site(Obj o) {
    auto items = at(okey(o));
    if (!member_of(okind(o), V.MADE) || items.empty()) return o;
    return make(tuple(items.first(items.size() - 1)));
}

Value Analysis::declared_env(Obj o) const {
    return okind(o) == V.SITE_DECLARED ? item(okey(o), 3) : NONE;
}

Value Analysis::made_by(Obj obj) {
    Value key = okey(obj);
    if (okind(obj) == V.SITE_CLASS) return in.cls(item(key, 1)) ? item(key, 1) : NONE;
    if (okind(obj) != V.SITE_FUNCTION) return NONE;
    const UnitRec& u = unit(item(key, 1).data());
    return truthy(u.constructs) ? u.constructs : NONE;
}

void Analysis::instance_made(Obj obj) {
    Value kind = okind(obj);
    if (kind != V.SITE_NEW && kind != V.SITE_LITERAL && kind != V.SITE_DECLARED) return;
    for (Value k : typesys::ancestors(*this, item(okey(obj), 1))) add(instances(k), obj);
}

namespace contexts {
Value of_call(Analysis& an, Value env, Value site) {
    return an.contexts ? an.tuple({env, site}) : an.ROOT;
}
Value of_method(Analysis& an, Value env, Value site, Value heap) {
    return an.contexts ? an.tuple({env, an.tuple({site, heap})}) : an.ROOT;
}
Value environment(Analysis& an, Value ctx) {
    return an.truthy(ctx) ? an.item(ctx, 0) : an.ROOT;
}
Value heap(Analysis& an, Value ctx) {
    if (!an.truthy(ctx)) return an.ROOT;
    Value call = an.item(ctx, 1);
    return an.is_tuple(call) ? an.item(call, 0) : call;
}
}  // namespace contexts

}  // namespace ftengine
