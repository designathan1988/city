# Difference propagation on every value node: plan (2026-11-10)

**Goal:** the tracer reaches its fixpoint on large programs; same links as before. **Spec:** log/trace-rules.md,
"Difference propagation on every value". **Cause:** typer.compute recomputes a node in full on every growth of what it
read, and its readers in full: quadratic on large unions.

1. **Solver** (solver.py): value nodes with a set (`value`, `known`), subscriptions `subs[source] = {(target, transfer)}`,
   `add(node, values)` keeps only new values (canonical form, identity) and queues the delta; `propagate()` runs each
   delta through its subscriptions until none is left; `subscribe` catches up on the source's current values. Consumers
   (references, non-distributive nodes) keep `read`/`grew`/`dirty`/`work`.
2. **Transfers** (transfer.py): the per-value rules of member, call, step, operator and substitution, each adding
   constants and subscribing to the nodes it reads (held values, returns).
3. **Typer** (typer.py): `node_of(tree, unit)` wires a node once by its shape; named, local, held (`#hold`), returns
   (`#ret`) are nodes; `evaluate` = read + node_of + its values; `receiver`, `type_class`, `canon` unchanged.
4. **Flows** (flows.py): places `#arg`, `#held`, `#engine` are value nodes; a link subscribes its argument trees to the
   parameters, a write its value to the variable, a signal its carried values to the handler's parameters.
5. **Linker** (link.py): the loop drains the deltas before each consumer; FLOW nodes are gone.
6. Review; then tests (unit, city, lab), log, gates, commit, push.
