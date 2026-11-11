# SDD ledger — plan: log/trace-rules-bounds-plan.md
Ruling: execute on main, no worktree — CLAUDE.md (work directly on main) — cost if wrong: a broken commit on main (each task commits only after its tests pass).
Pre-flight: Task 2 consumes Task 1 (js_bounds.of tokens, Unit.bounds/Unit.bound, bounds.flow) — names match across the blocks.
Task 1: Ruling: TypeScript classes and interfaces are structural (handbook, Type Compatibility): a class bound admits an object of a class that declares all its members besides its chain; a host type the tracer did not read admits everything (H1: it may be an alias of a primitive, DOMHighResTimeStamp) — cost if wrong: a host-typed place keeps values its host type would refuse.
Task 1: Ruling: the bound is a filter subscription from each source into the place (no intermediate node): a place first wired after the solve keeps getting its values in the same step, as an identity did — cost if wrong: none.
Task 1: Ruling: a 'named' token is resolved before a null or a primitive is refused (an alias can admit null, a host type everything) — found by test_narrowing.test_alias_lookup_and_conditional_types — cost if wrong: none.
