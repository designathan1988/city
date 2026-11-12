"""What the language's tests can give of one abstract value (log/trace-research.md F31, F38; `guards` wires them where
a test holds): `typeof` (ECMA-262 13.5.3.1, with B.3.6's [[IsHTMLDDA]]), ToBoolean (7.1.2), IsStrictlyEqual with a
constant, `== null` (IsLooselyEqual), `instanceof` (13.10.2, OrdinaryHasInstance 7.3.21: a class's own @@hasInstance
decides alone, a primitive is no instance, an object by its prototype chain); and, for a declared type predicate,
whether a value can be of a declared type and whether it surely is one (TypeScript handbook, Narrowing; Type
Compatibility: a class of the program by its chain, an interface or a declared class by its members, the structure
TypeScript compares). A value the analysis cannot decide can give either: the unknown object, a host's declared
object (it may be [[IsHTMLDDA]] or of any class), an object whose chain reaches what the program does not write.
"""
from __future__ import annotations

from ..languages import (CALL_SIGNATURE, CONSTRUCT_SIGNATURE, HAS_INSTANCE, INDEX_SIGNATURE, PROTO_PROPERTY,
                         TYPEOF_BIGINT, TYPEOF_BOOLEAN, TYPEOF_FUNCTION, TYPEOF_NUMBER,
                         TYPEOF_OBJECT, TYPEOF_STRING, TYPEOF_SYMBOL, TYPEOF_UNDEFINED)
from ..solver import BIGINT, ENGINE, FALSE, FUNCTION, LANG, NULL, NUMBER, OBJECT, STRING, SYMBOL, TEXT, TRUE, UNDEFINED
from ..vocab import (SITE_ARRAY, SITE_CLASS, SITE_DECLARED, SITE_DECLARED_FN, SITE_FUNCTION, SITE_GENERATOR, SITE_KEY,
                     SITE_LITERAL, SITE_MODULE, SITE_NEW, SITE_PRIMITIVE, SITE_PROMISE, SITE_PROTOTYPE, SITE_RESOLVER,
                     SITE_TEXT, SITE_UNKNOWN, T_BOTH, T_FN, T_NAMED, T_OBJECT, T_TUPLE, T_UNION, T_VALUES)
from . import typesys

UNDEFINED_K, NULL_K, TRUE_K, FALSE_K, NUMBER_K, BIGINT_K, STRING_K, SYMBOL_K, OBJECT_K, FUNCTION_K = (
    c[len(LANG):] for c in (UNDEFINED, NULL, TRUE, FALSE, NUMBER, BIGINT, STRING, SYMBOL, OBJECT, FUNCTION))
EVERY_TYPEOF = frozenset({TYPEOF_UNDEFINED, TYPEOF_OBJECT, TYPEOF_BOOLEAN, TYPEOF_NUMBER, TYPEOF_BIGINT, TYPEOF_STRING,
                          TYPEOF_SYMBOL, TYPEOF_FUNCTION})
PRIMITIVE_TYPEOF = {UNDEFINED_K: {TYPEOF_UNDEFINED}, NULL_K: {TYPEOF_OBJECT}, TRUE_K: {TYPEOF_BOOLEAN},
                    FALSE_K: {TYPEOF_BOOLEAN}, NUMBER_K: {TYPEOF_NUMBER}, BIGINT_K: {TYPEOF_BIGINT},
                    STRING_K: {TYPEOF_STRING}, SYMBOL_K: {TYPEOF_SYMBOL}, OBJECT_K: {TYPEOF_OBJECT, TYPEOF_FUNCTION},
                    FUNCTION_K: {TYPEOF_FUNCTION}}
FALSY_KINDS, EITHER_KINDS = (UNDEFINED_K, NULL_K, FALSE_K), (NUMBER_K, BIGINT_K, STRING_K)
OBJECT_KINDS = (OBJECT_K, FUNCTION_K)      # a primitive of these kinds stands for an object the program does not name
CALLABLE = (SITE_FUNCTION, SITE_CLASS, SITE_DECLARED_FN, SITE_RESOLVER)
ORDINARY = (SITE_LITERAL, SITE_ARRAY, SITE_PROTOTYPE, SITE_MODULE, SITE_GENERATOR, SITE_PROMISE)
NOT_INSTANCES = (SITE_ARRAY, SITE_FUNCTION, SITE_CLASS, SITE_DECLARED_FN, SITE_RESOLVER, SITE_MODULE, SITE_GENERATOR,
                 SITE_PROMISE)             # their chains are the language's intrinsics: no class of the program's on them
YES, NO, EITHER, NEITHER = frozenset({True}), frozenset({False}), frozenset({True, False}), frozenset()
SIGNATURES = (CALL_SIGNATURE, CONSTRUCT_SIGNATURE, INDEX_SIGNATURE)    # a type's members that are no property


def primitive(an, obj: int) -> bool:
    """A value of a primitive kind of the language (not one standing for an object)."""
    kind = an.objects.kind(obj)
    return kind in (SITE_TEXT, SITE_KEY) or kind == SITE_PRIMITIVE and prim(an, obj) not in OBJECT_KINDS


def prim(an, obj: int) -> str:
    return an.objects.keys[obj][1]


def open_kind(an, obj: int) -> bool:
    """A value the analysis cannot decide: the unknown object, a host's declared one, an object the program does not
    name, an instance whose chain reaches a class the program does not write (an exotic base: a function, an array)."""
    kind = an.objects.kind(obj)
    if kind in (SITE_UNKNOWN, SITE_DECLARED):
        return True
    if kind == SITE_PRIMITIVE:
        return prim(an, obj) in OBJECT_KINDS
    if kind != SITE_NEW or an.objects.maker(obj) is not None:
        return False
    _, unresolved, ambient = prototype_chain(an, an.objects.class_of(obj))
    return unresolved or ambient


def prototype_chain(an, key: str) -> tuple[tuple, bool, bool]:
    """A class and the classes its `extends` reaches, in order (its instances' prototype chain): (keys, whether it
    reaches a base the program's text does not resolve, whether it reaches a class only declared)."""
    memo = an.memo.setdefault(prototype_chain, {})
    if key not in memo:
        keys, k, unresolved, ambient = [], key, False, False
        while k is not None and k not in keys:
            cls = an.ix.classes.get(k)
            if cls is None:
                unresolved = True
                break
            keys.append(k)
            ambient = ambient or cls.ambient
            k = an.ix.js.class_of(cls.extends, cls.file, cls.outer) if cls.extends else None
            unresolved = unresolved or bool(cls.extends) and k is None
        memo[key] = (tuple(keys), unresolved, ambient)
    return memo[key]


def typeof(an, obj: int) -> frozenset:
    kind = an.objects.kind(obj)
    if kind == SITE_PRIMITIVE:
        return frozenset(PRIMITIVE_TYPEOF.get(prim(an, obj), EVERY_TYPEOF))
    if kind in (SITE_TEXT, SITE_KEY):
        return frozenset({TYPEOF_STRING})
    if kind in CALLABLE:
        return frozenset({TYPEOF_FUNCTION})
    if kind == SITE_DECLARED:
        return frozenset({TYPEOF_OBJECT, TYPEOF_FUNCTION, TYPEOF_UNDEFINED})
    if kind == SITE_NEW:
        return frozenset({TYPEOF_OBJECT, TYPEOF_FUNCTION} if open_kind(an, obj) else {TYPEOF_OBJECT})
    if kind in ORDINARY:
        return frozenset({TYPEOF_OBJECT})
    return EVERY_TYPEOF


def truthiness(an, obj: int) -> tuple[bool, bool]:
    """(whether it can be truthy, whether it can be falsy)."""
    kind = an.objects.kind(obj)
    if kind == SITE_PRIMITIVE:
        k = prim(an, obj)
        return (k not in FALSY_KINDS, k in FALSY_KINDS or k in EITHER_KINDS or k not in PRIMITIVE_TYPEOF)
    if kind == SITE_TEXT:
        empty = an.objects.text(obj) == ""
        return (not empty, empty)
    if kind in (SITE_KEY, SITE_UNKNOWN, SITE_DECLARED):
        return (True, True)
    return (True, False)


def can_equal(an, obj: int, constant: str) -> bool:
    """Whether the value can be strictly equal to a constant of the language or a string (no object is)."""
    kind = an.objects.kind(obj)
    if kind == SITE_UNKNOWN:
        return True
    if kind == SITE_TEXT:
        return constant == TEXT + an.objects.text(obj)
    if kind == SITE_KEY:
        return constant.startswith(TEXT)
    if kind != SITE_PRIMITIVE or prim(an, obj) in OBJECT_KINDS:
        return False
    k = prim(an, obj)
    return k == STRING_K if constant.startswith(TEXT) else constant == LANG + k


def can_differ(an, obj: int, constant: str) -> bool:
    kind = an.objects.kind(obj)
    if kind == SITE_TEXT:
        return constant != TEXT + an.objects.text(obj)
    if kind == SITE_PRIMITIVE and prim(an, obj) in (UNDEFINED_K, NULL_K, TRUE_K, FALSE_K):
        return constant != LANG + prim(an, obj)
    return True


def nullish(an, obj: int) -> frozenset:
    """What `x == null` can be: true of null and undefined, either of a host's object ([[IsHTMLDDA]])."""
    kind = an.objects.kind(obj)
    if kind == SITE_PRIMITIVE:
        k = prim(an, obj)
        return YES if k in (UNDEFINED_K, NULL_K) else EITHER if k not in PRIMITIVE_TYPEOF else NO
    return EITHER if kind in (SITE_UNKNOWN, SITE_DECLARED) else NO


def array(an, obj: int) -> frozenset:
    """Whether a value can be an array (IsArray: an Array exotic object, or a proxy of one): a tuple type's value."""
    if an.objects.kind(obj) == SITE_ARRAY:
        return YES
    return EITHER if open_kind(an, obj) else NO


def outcomes(an, obj: int, target: int) -> frozenset:
    """What `obj instanceof target` can give (InstanceofOperator): none when it throws."""
    objects = an.objects
    kind = objects.kind(target)
    if kind == SITE_UNKNOWN or kind == SITE_PRIMITIVE and prim(an, target) in OBJECT_KINDS:
        return EITHER
    if kind in (SITE_PRIMITIVE, SITE_TEXT, SITE_KEY):
        return NEITHER                                   # a target that is no object throws
    if kind == SITE_CLASS:
        key = objects.class_itself(target)
        if key not in an.ix.classes or any(an.ix.units[g].static for g in typesys.lookup(an, key, HAS_INSTANCE)):
            return EITHER                                # its own @@hasInstance decides
        return of_class(an, obj, key)
    if primitive(an, obj):
        return NO if kind in CALLABLE + (SITE_DECLARED,) else EITHER
    if kind == SITE_FUNCTION and objects.kind(obj) == SITE_NEW and objects.maker(obj) == target:
        return YES
    return EITHER


def of_class(an, obj: int, key: str) -> frozenset:
    """OrdinaryHasInstance with a class of the program: whether its prototype object is on the value's chain."""
    objects = an.objects
    if primitive(an, obj):
        return NO
    kind = objects.kind(obj)
    if kind == SITE_NEW:
        if objects.maker(obj) is not None:
            return EITHER
        keys, unresolved, _ = prototype_chain(an, objects.class_of(obj))
        return YES if key in keys else EITHER if unresolved else NO
    if kind == SITE_PROTOTYPE:
        site = objects.keys[obj]
        keys, unresolved, _ = prototype_chain(an, site[1])
        return YES if key in keys[1:] else EITHER if unresolved or len(site) > 2 else NO
    if kind == SITE_LITERAL:
        return EITHER if PROTO_PROPERTY in an.ix.members.get(objects.class_of(obj), {}) else NO
    return NO if kind in NOT_INSTANCES else EITHER


def admits(an, tree: tuple, obj: int, seen: tuple = ()) -> bool:
    """Whether a value can be of a declared type: a primitive of the kinds it admits; of a class of the program, an
    object by its chain; of an interface, an object type or a declared class, a value with every member it requires
    (`structural`)."""
    kind = tree[0] if tree else None
    if kind == T_UNION:
        return any(admits(an, part, obj, seen) for part in tree[1])
    if kind == T_BOTH:
        return all(admits(an, part, obj, seen) for part in tree[1])
    if kind == T_VALUES:
        return any(value_admits(an, v, obj) for v in tree[1])
    if kind == T_NAMED:
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        if found is None or found in seen:
            return True
        if found[0] == typesys.ALIAS:
            return admits(an, an.ix.units[found[1]].result_type, obj, seen + (found,))
        cls = an.ix.classes.get(found[1])
        if cls is None:
            return True
        if cls.shape or cls.ambient:
            return structural(an, found[1], obj)
        return not primitive(an, obj) and True in of_class(an, obj, found[1])
    if kind == T_OBJECT:
        return structural(an, tree[1], obj)
    if kind == T_TUPLE:
        return True in array(an, obj)
    if kind == T_FN:
        return TYPEOF_FUNCTION in typeof(an, obj)
    return True


FILTERING = (T_NAMED, T_OBJECT, T_VALUES, T_UNION, T_BOTH, T_TUPLE, T_FN)   # the types a typed position can refuse by


def assignable(an, tree: tuple, obj: int, seen: tuple = ()) -> bool:
    """Whether a value is assignable to a type the program declares for a position (TypeScript Type Compatibility,
    the rule a typed parameter or return keeps): an object type, a class's included, by structure (`fits`); a union by
    any part, an intersection by every part; the unknown object can be anything."""
    if an.objects.kind(obj) == SITE_UNKNOWN:
        return True
    kind = tree[0] if tree else None
    if kind == T_UNION:
        return any(assignable(an, part, obj, seen) for part in tree[1])
    if kind == T_BOTH:
        return all(assignable(an, part, obj, seen) for part in tree[1])
    if kind == T_VALUES:
        return any(value_admits(an, v, obj) for v in tree[1])
    if kind == T_NAMED:
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        if found is None or found in seen:
            return True
        if found[0] == typesys.ALIAS:
            return assignable(an, an.ix.units[found[1]].result_type, obj, seen + (found,))
        return fits(an, found[1], obj)
    if kind == T_OBJECT:
        return fits(an, tree[1], obj)
    if kind == T_TUPLE:
        return True in array(an, obj)
    if kind == T_FN:
        return TYPEOF_FUNCTION in typeof(an, obj)
    return True


def fits(an, key: str, obj: int) -> bool:
    """Whether a value has every member an object type requires — declared on its own types (a declared object's,
    a primitive's wrapper's) or written on it — or is a dictionary every name reads (a string index signature)."""
    if nullish(an, obj) == YES:
        return False
    if any(NUMBER not in typesys.values(key_type) for own in typesys.own_types(an, obj)
           for gid in typesys.lookup(an, own, INDEX_SIGNATURE) for key_type, _ in an.ix.units[gid].indexes):
        return True
    written = an.names_of.get(obj, ())
    return all(typesys.has_member(an, obj, name) or name in written for name in required(an, key))


def filtering(an, tree: tuple) -> bool:
    """Whether a declared type can refuse a value: written, not open (`summaries.is_open`), of a kind it can tell."""
    from .summaries import is_open
    return bool(tree) and tree[0] in FILTERING and not is_open(an, tree)


def value_admits(an, value: str, obj: int) -> bool:
    if value == ENGINE or an.objects.kind(obj) == SITE_UNKNOWN:
        return True
    if value.startswith(TEXT):
        return can_equal(an, obj, value)
    if value in (OBJECT, FUNCTION):
        return not primitive(an, obj) and (value == OBJECT or TYPEOF_FUNCTION in typeof(an, obj))
    kind = an.objects.kind(obj)
    if kind in (SITE_TEXT, SITE_KEY):
        return value == STRING
    return kind == SITE_PRIMITIVE and value == LANG + prim(an, obj)


def structural(an, key: str, obj: int) -> bool:
    """Whether a value can be of an object type by its structure (TypeScript Type Compatibility): not null nor
    undefined, and having every member the type requires — declared on the value's own types (a primitive's wrapper
    included) or written on it. The unknown object and a host's declared object can be anything."""
    if an.objects.kind(obj) in (SITE_UNKNOWN, SITE_DECLARED):
        return True
    if nullish(an, obj) == YES:
        return False
    written = an.names_of.get(obj, ())
    return all(typesys.has_member(an, obj, name) or name in written for name in required(an, key))


def required(an, key: str) -> tuple:
    """The members an object type requires: each property or method it or its heritage declares, not optional."""
    memo = an.memo.setdefault(required, {})
    if key not in memo:
        names = set()
        for k in typesys.ancestors(an, key):
            for g in typesys.group(an, k):
                for name, gids in an.ix.members.get(g, {}).items():
                    if name not in SIGNATURES and any(not an.ix.units[i].optional for i in gids):
                        names.add(name)
        memo[key] = tuple(sorted(names))
    return memo[key]


def surely(an, tree: tuple, obj: int, seen: tuple = ()) -> bool:
    """Whether a value is surely of a declared type, where a predicate false of it says it is not: a primitive of the
    kind (or the text) the type names; an object whose own types or prototype chain hold the class the type names."""
    kind = tree[0] if tree else None
    if kind == T_UNION:
        return any(surely(an, part, obj, seen) for part in tree[1])
    if kind == T_VALUES:
        return any(surely_value(an, v, obj) for v in tree[1])
    if kind != T_NAMED or primitive(an, obj) or an.objects.kind(obj) in (SITE_UNKNOWN, SITE_DECLARED):
        return False
    found = typesys.resolve(an, tree[1], tree[3], tree[4])
    if found is None or found in seen:
        return False
    if found[0] == typesys.ALIAS:
        return surely(an, an.ix.units[found[1]].result_type, obj, seen + (found,))
    return found[1] in typesys.own_types(an, obj) or YES == of_class(an, obj, found[1])


def surely_value(an, value: str, obj: int) -> bool:
    kind = an.objects.kind(obj)
    if value.startswith(TEXT):
        return kind == SITE_TEXT and value == TEXT + an.objects.text(obj)
    if value == STRING:
        return kind in (SITE_TEXT, SITE_KEY) or kind == SITE_PRIMITIVE and prim(an, obj) == STRING_K
    return kind == SITE_PRIMITIVE and value == LANG + prim(an, obj) and prim(an, obj) not in OBJECT_KINDS
