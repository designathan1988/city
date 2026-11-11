# The tracer's pending items: plan (2026-11-10)

**Spec:** log/trace-rules.md, "The pending rules found against a real run". **Goal:** the 5 calls the runtime check
found unlinked are linked by the general rules; references are relinked by their difference; override labels do not
depend on order.

1. `scopes.py`: the reader's block-scope stack: `open_scope`, `close_scope`, `local_key`, `visible`, `scoped_tree`;
   `model.declare(r, unit, name, trees, param=False, function_scope=False)` and `model.assign(r, ...)` use it; every
   reader opens a scope per block (C++ compound, loops, catch; GDScript blocks, for, match; JS blocks, loops, catch;
   JS `var` at function scope) and resets it per function; `r.ir` renames, `r.add` renames local references.
2. `Unit.local_types`: a C++ class declared in a function (`cpp_body.local` reads it with `CppReader.klass`, keyed by
   the function); `TypeNames.type_class` and the constructors look there first; `enclose` copies it.
3. `cpp_implicit.declared_object`: a bare-name "parameter" of a block-scope object declaration is a value.
4. `cpp_lifetime.py` (C++ object lifetime, out of link.py): implicit construction (moved), destruction of members,
   owned objects of library owners (`rules.LIBRARY_OWNERS`) and bases; `destroy` of a class with no written
   destructor reaches what its implicit one does.
5. Relinking by difference: `Solver.add` keeps each consumer's per-node deltas; `link.link_node` reruns a reference
   with each grown node's delta in its place (each read position in turn); `Resolver` remembers references that
   reached something (`reached`) and found a fitting overload (`fitted`); notes and fallbacks use them.
6. `link.with_overrides`: no exclusion of the current targets; `Index.overrides` memoized.
7. Review; tests: unit (new fixture cases for 1-4), city old/new pairs, runtime check (`trace_validate check`), lab.
