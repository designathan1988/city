// The engine's vocabulary (featuretrace/engine_input.py `vocabulary`): every constant the semantics compare with, read
// from the input file by its owner's name, so each has one definition, the Python one. FT_VOCAB lists them.
#pragma once

#include "input.h"

namespace ftengine {

#define FT_VOCAB(X)                                                                                                  \
    X(NAME, "vocab.NAME") X(MEMBER, "vocab.MEMBER") X(INDEX, "vocab.INDEX") X(CALL, "vocab.CALL")                    \
    X(CONSTRUCT, "vocab.CONSTRUCT") X(CALLABLE, "vocab.CALLABLE") X(CAST, "vocab.CAST")                              \
    X(COLLECTION, "vocab.COLLECTION") X(ELEMENTS, "vocab.ELEMENTS") X(EACH, "vocab.EACH") X(UNION, "vocab.UNION")    \
    X(AWAIT, "vocab.AWAIT") X(NARROW, "vocab.NARROW") X(SELF, "vocab.SELF") X(LITERAL, "vocab.LITERAL")              \
    X(STRNAME, "vocab.STRNAME") X(OP, "vocab.OP") X(UNARY, "vocab.UNARY") X(JSX, "vocab.JSX") X(KEYS, "vocab.KEYS")  \
    X(SUPER, "vocab.SUPER") X(NEW, "vocab.NEW") X(INVOKE, "vocab.INVOKE") X(WRITE, "vocab.WRITE")                    \
    X(STORE, "vocab.STORE") X(READ, "vocab.READ") X(LAMBDA, "vocab.LAMBDA") X(IMPORT, "vocab.IMPORT")                \
    X(HANDED, "vocab.HANDED") X(ANY_FIELD, "vocab.ANY_FIELD") X(ELEMENTS_FIELD, "vocab.ELEMENTS_FIELD")              \
    X(RESULT_FIELD, "vocab.RESULT_FIELD") X(PLATFORM_FIELD, "vocab.PLATFORM_FIELD")                                 \
    X(JSX_CHILDREN, "vocab.JSX_CHILDREN") X(CLASS_UNIT, "vocab.CLASS_UNIT")                                          \
    X(SITE_ARRAY, "vocab.SITE_ARRAY") X(SITE_CLASS, "vocab.SITE_CLASS") X(SITE_DECLARED, "vocab.SITE_DECLARED")      \
    X(SITE_DECLARED_FN, "vocab.SITE_DECLARED_FN") X(SITE_FUNCTION, "vocab.SITE_FUNCTION")                            \
    X(SITE_GENERATOR, "vocab.SITE_GENERATOR") X(SITE_KEY, "vocab.SITE_KEY") X(SITE_LITERAL, "vocab.SITE_LITERAL")    \
    X(SITE_MODULE, "vocab.SITE_MODULE") X(SITE_NEW, "vocab.SITE_NEW") X(SITE_PRIMITIVE, "vocab.SITE_PRIMITIVE")      \
    X(SITE_PROMISE, "vocab.SITE_PROMISE") X(SITE_PROTOTYPE, "vocab.SITE_PROTOTYPE")                                  \
    X(SITE_RESOLVER, "vocab.SITE_RESOLVER") X(SITE_TEXT, "vocab.SITE_TEXT") X(SITE_UNKNOWN, "vocab.SITE_UNKNOWN")    \
    X(T_BOTH, "vocab.T_BOTH") X(T_FN, "vocab.T_FN") X(T_INDEX, "vocab.T_INDEX") X(T_NAMED, "vocab.T_NAMED")          \
    X(T_OBJECT, "vocab.T_OBJECT") X(T_OPEN, "vocab.T_OPEN") X(T_PARAM, "vocab.T_PARAM") X(T_QUERY, "vocab.T_QUERY")  \
    X(T_THIS, "vocab.T_THIS") X(T_TUPLE, "vocab.T_TUPLE") X(T_UNION, "vocab.T_UNION") X(T_VALUES, "vocab.T_VALUES")  \
    X(LEVEL_ALIAS, "vocab.LEVEL_ALIAS") X(LEVEL_OWN, "vocab.LEVEL_OWN") X(LEVEL_TYPE, "vocab.LEVEL_TYPE")            \
    X(ARRAY_TYPE, "languages.ARRAY_TYPE") X(CALL_SIGNATURE, "languages.CALL_SIGNATURE")                              \
    X(CONSTRUCT_SIGNATURE, "languages.CONSTRUCT_SIGNATURE") X(GETTER, "languages.GETTER")                            \
    X(SETTER, "languages.SETTER") X(INDEX_SIGNATURE, "languages.INDEX_SIGNATURE")                                    \
    X(INTRINSICS, "languages.INTRINSICS") X(OPEN_INTRINSICS, "languages.OPEN_INTRINSICS") X(SLOTS, "languages.SLOTS") \
    X(ITERATOR, "languages.ITERATOR") X(NEXT, "languages.NEXT") X(VALUE, "languages.VALUE")                          \
    X(THEN, "languages.THEN") X(PROTOTYPE, "languages.PROTOTYPE")                                                    \
    X(PROTOTYPE_CONSTRUCTOR, "languages.PROTOTYPE_CONSTRUCTOR") X(JSX_AUTOMATIC, "languages.JSX_AUTOMATIC")          \
    X(JSX_CLASSIC, "languages.JSX_CLASSIC") X(JSX_FACTORY_DEFAULT, "languages.JSX_FACTORY_DEFAULT")                  \
    X(JSX_FACTORY_OPTION, "languages.JSX_FACTORY_OPTION") X(JSX_MODE_OPTION, "languages.JSX_MODE_OPTION")            \
    X(CLASS_VALUE, "ir_js.CLASS_VALUE") X(MADE_CLASS, "js_class.MADE_CLASS") X(MADE_LITERAL, "js_class.MADE_LITERAL") \
    X(ARROW, "js_functions.ARROW") X(MODULE_UNIT, "js_read.MODULE_UNIT") X(LIB_ENGINE, "library.ENGINE")             \
    X(FUNCTION_KINDS, "model.FUNCTION_KINDS") X(STATE_KINDS, "model.STATE_KINDS") X(CONSTRUCTOR, "rules.CONSTRUCTOR") \
    X(ENGINE, "solver.ENGINE") X(LANG, "solver.LANG") X(NUMBER, "solver.NUMBER") X(OBJECT, "solver.OBJECT")          \
    X(SYMBOL, "solver.SYMBOL") X(TEXT, "solver.TEXT")                       \
    X(NO_ENV, "heap.values.NO_ENV") X(MADE, "heap.objects.MADE") X(PROGRAM_MADE, "heap.declared.PROGRAM_MADE")       \
    X(ONCE, "heap.contexts.ONCE") X(TREE, "heap.analysis.TREE") X(LOCAL, "heap.analysis.LOCAL") X(PARAM, "heap.analysis.PARAM")                   \
    X(THIS, "heap.analysis.THIS") X(RET, "heap.analysis.RET") X(HOLD, "heap.analysis.HOLD")                         \
    X(FIELD, "heap.analysis.FIELD") X(INITIAL, "heap.analysis.INITIAL") X(INSTANCES, "heap.analysis.INSTANCES")      \
    X(MADE_BY_CALL, "heap.analysis.MADE_BY_CALL") X(CONSTRUCTED, "heap.trees.CONSTRUCTED") X(SPREAD, "heap.trees.SPREAD")                           \
    X(WRITTEN, "heap.refs.WRITTEN") X(ESCAPE, "heap.refs.ESCAPE") X(ALL, "heap.members.ALL") X(ACCESSOR, "heap.members.ACCESSOR")                  \
    X(PROTOTYPES, "heap.members.PROTOTYPES") X(PROPERTY, "heap.members.PROPERTY") X(INDEX_KINDS, "heap.members.INDEX_KINDS")                                    \
    X(PRODUCED, "heap.values.PRODUCED") X(CONSUMED, "heap.values.CONSUMED") X(OWN_PLACE, "heap.values.OWN_PLACE")    \
    X(RESULT, "heap.values.RESULT") X(V_MEMBER, "heap.values.MEMBER") X(BOUND, "heap.values.BOUND")                  \
    X(CALLBACK, "heap.declared.CALLBACK") X(CALLBACK_SITE, "heap.declared.CALLBACK_SITE")                            \
    X(READ_MEMBER, "heap.declared.READ_MEMBER") X(D_THIS, "heap.declared.THIS")                                      \
    X(RECEIVERS, "heap.declared.RECEIVERS") X(CALLBACK_ARG, "heap.declared.CALLBACK_ARG")                            \
    X(STRUCTURAL, "heap.declared.STRUCTURAL") X(POOL, "heap.summaries.POOL") X(RECEIVE, "heap.summaries.RECEIVE")    \
    X(FIELDS, "heap.summaries.FIELDS") X(CALLED, "heap.summaries.CALLED") X(REFLECT, "heap.summaries.REFLECT")       \
    X(MEMBER_OF, "heap.summaries.MEMBER_OF") X(SUMMARY_SITE, "heap.summaries.SUMMARY_SITE")                          \
    X(KEPT, "heap.summaries.KEPT") X(KEPT_SITE, "heap.summaries.KEPT_SITE") X(KEPT_ENTRY, "heap.summaries.KEPT_ENTRY") \
    X(GIVEN, "heap.summaries.GIVEN") X(UNKNOWN_ARG, "heap.summaries.UNKNOWN_ARG")                                    \
    X(S_TEXTS, "heap.summaries.TEXTS") X(KEYED, "heap.summaries.KEYED") X(D_REST, "heap.declared.REST")             \
    X(KEYPUT, "heap.copies.KEYPUT") X(CORRELATED, "heap.correlation.CORRELATED")                                                                                \
    X(ITERATOR_CALL, "heap.protocols.ITERATOR_CALL") X(NEXT_CALL, "heap.protocols.NEXT_CALL")                        \
    X(THEN_CALL, "heap.protocols.THEN_CALL") X(PROBE, "heap.protocols.PROBE") X(RESOLVING, "heap.protocols.RESOLVING") \
    X(SETTLE, "heap.protocols.SETTLE") X(JSX_CALL, "heap.jsx.JSX_CALL") X(FACTORY, "heap.jsx.FACTORY")              \
    X(CHILDREN, "heap.jsx.CHILDREN") X(ALIAS, "heap.typesys.ALIAS") X(CLASS, "heap.typesys.CLASS")                   \
    X(G_TRUTHY, "vocab.G_TRUTHY") X(G_MEMBER, "vocab.G_MEMBER") X(G_TYPEOF, "vocab.G_TYPEOF")                        \
    X(G_INSTANCE, "vocab.G_INSTANCE")                          \
    X(G_HAS, "vocab.G_HAS") X(G_EQUALS, "vocab.G_EQUALS")                              \
    X(G_NULLISH, "vocab.G_NULLISH") X(G_MEMBER_EQUALS, "vocab.G_MEMBER_EQUALS") X(G_PREDICATE, "vocab.G_PREDICATE")  \
    X(G_ASSERTS, "vocab.G_ASSERTS") X(TYPEOF_UNDEFINED, "languages.TYPEOF_UNDEFINED")                                \
    X(TYPEOF_OBJECT, "languages.TYPEOF_OBJECT") X(TYPEOF_BOOLEAN, "languages.TYPEOF_BOOLEAN")                        \
    X(TYPEOF_NUMBER, "languages.TYPEOF_NUMBER") X(TYPEOF_BIGINT, "languages.TYPEOF_BIGINT")                          \
    X(TYPEOF_STRING, "languages.TYPEOF_STRING") X(TYPEOF_SYMBOL, "languages.TYPEOF_SYMBOL")                          \
    X(TYPEOF_FUNCTION, "languages.TYPEOF_FUNCTION") X(HAS_INSTANCE, "languages.HAS_INSTANCE")                        \
    X(PROTO_PROPERTY, "languages.PROTO_PROPERTY") X(S_UNDEFINED, "solver.UNDEFINED") X(S_STRING, "solver.STRING")    \
    X(S_FUNCTION, "solver.FUNCTION") X(GD_MEMBER, "heap.guards.GUARD_MEMBER")                                        \
    X(GD_UNDEFINED, "heap.verdicts.UNDEFINED_K") X(GD_NULL, "heap.verdicts.NULL_K")                                  \
    X(GD_TRUE, "heap.verdicts.TRUE_K") X(GD_FALSE, "heap.verdicts.FALSE_K") X(GD_NUMBER, "heap.verdicts.NUMBER_K")   \
    X(GD_BIGINT, "heap.verdicts.BIGINT_K") X(GD_STRING, "heap.verdicts.STRING_K")                                    \
    X(GD_SYMBOL, "heap.verdicts.SYMBOL_K") X(GD_OBJECT, "heap.verdicts.OBJECT_K")                                    \
    X(GD_FUNCTION, "heap.verdicts.FUNCTION_K") X(GD_EVERY_TYPEOF, "heap.verdicts.EVERY_TYPEOF")                      \
    X(GD_FALSY_KINDS, "heap.verdicts.FALSY_KINDS") X(GD_EITHER_KINDS, "heap.verdicts.EITHER_KINDS")                  \
    X(GD_OBJECT_KINDS, "heap.verdicts.OBJECT_KINDS") X(GD_CALLABLE, "heap.verdicts.CALLABLE")                        \
    X(GD_ORDINARY, "heap.verdicts.ORDINARY") X(GD_NOT_INSTANCES, "heap.verdicts.NOT_INSTANCES")

struct Vocab {
#define FT_FIELD(name, owner) Value name;
    FT_VOCAB(FT_FIELD)
#undef FT_FIELD
    explicit Vocab(const Input& in) {
#define FT_READ(name, owner) name = in.vocab(owner);
        FT_VOCAB(FT_READ)
#undef FT_READ
    }
};

}  // namespace ftengine
