// A property read or written through a key (heap/by_key.py; ECMA-262 ToPropertyKey), for one value of the key: a string
// of a known text is the property it names, an index the elements, a key enumerating an object a property that object
// has (copies.cpp), any other key the object as a dictionary: read through a key, not by a name (F40).
#include "semantics.h"

namespace ftengine::members {

void store_by_key(Analysis& an, Value source, Obj obj, Obj key) {
    if (an.okind(key) == an.V.SITE_TEXT) {           // a known text: the property it names (ToPropertyKey)
        write(an, source, obj, an.item(an.okey(key), 1));
    } else if (an.okind(key) == an.V.SITE_KEY) {     // a key enumerating an object: a property that object has
        copies::put(an, source, static_cast<Obj>(an.item(an.okey(key), 1).data()), obj);
    } else if (is_index(an, key)) {
        if (!keeps_nothing(an, obj)) an.follow(source, field(an, obj, an.V.ELEMENTS_FIELD));
    } else {
        put_dictionary(an, source, obj);
    }
}

void load_by_key(Analysis& an, Value target, Obj obj, Obj key) {
    if (an.okind(key) == an.V.SITE_TEXT) {           // a known text: the property it names, as `obj.text`
        read(an, target, obj, an.item(an.okey(key), 1));
        return;
    }
    if (keeps_nothing(an, obj)) return;
    if (is_index(an, key)) {
        an.follow(field(an, obj, an.V.ELEMENTS_FIELD), target);
        return;
    }
    if (an.okind(key) == an.V.SITE_KEY && an.item(an.okey(key), 1) == I(obj)) an.follow(every(an, obj), target);
    read_all(an, target, obj);
}

}  // namespace ftengine::members
