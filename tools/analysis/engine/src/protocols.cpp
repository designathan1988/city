// The language's protocols over objects (heap/protocols.py; ECMA-262 GetIterator, IteratorNext, IteratorValue, Await
// and the promise resolve functions): iterating calls @@iterator, `next` and reads `value`, besides an array's
// elements; awaiting a thenable calls its `then` with a resolving function.
#include "semantics.h"

namespace ftengine::protocols {

void iterate(Analysis& an, Value node, Value source) {
    const Vocab& V = an.V;
    auto rest = an.at(node).subspan(1);
    std::vector<Value> first{V.ITERATOR_CALL}, second{V.NEXT_CALL};
    first.insert(first.end(), rest.begin(), rest.end());
    second.insert(second.end(), rest.begin(), rest.end());
    Value a = an.tuple(first), b = an.tuple(second);
    an.subscribe(source, node, an.tuple({V.ELEMENTS_FIELD}), pack(an, kIterElements, {}));
    an.subscribe(source, a, an.tuple({V.ITERATOR}), pack(an, kIterIterator, {}));
    an.subscribe(a, b, an.tuple({V.NEXT}), pack(an, kIterNext, {}));
    an.subscribe(b, node, an.tuple({V.VALUE}), pack(an, kIterValue, {}));
}

void method(Analysis& an, Obj obj, Value name, Value call, Value args) {
    Value probe = an.tuple({an.V.PROBE, I(obj), name, call});
    members::read(an, probe, obj, name);
    an.subscribe(probe, call, an.tuple({an.V.CALL, I(obj)}), pack(an, kMethodCall, {I(obj), args}));
}

void settle(Analysis& an, Value node, Value source) {
    an.subscribe(source, an.tuple({an.V.SETTLE, node}), an.tuple({an.V.SETTLE, source}), pack(an, kSettle, {node}));
}

void one(Analysis& an, Value node, Obj obj) {
    const Vocab& V = an.V;
    if (an.okind(obj) == V.SITE_PROMISE) {
        an.follow(members::field(an, obj, V.RESULT_FIELD), node);
        return;
    }
    if (!typesys::has_member(an, obj, V.THEN)) {
        an.add(node, obj);
        return;
    }
    Value resolving = an.tuple({V.RESOLVING, node});
    an.add(resolving, an.make({V.SITE_RESOLVER, node}));
    std::vector<Value> call{V.THEN_CALL};
    auto rest = an.at(node).subspan(1);
    call.insert(call.end(), rest.begin(), rest.end());
    method(an, obj, V.THEN, an.tuple(call), an.tuple({resolving}));
}

}  // namespace ftengine::protocols
