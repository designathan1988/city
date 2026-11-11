# SDD ledger — plan: log/trace-rules-delta-plan.md
Spec: log/trace-rules.md, "Difference propagation on every value". Executor: native, this session.
Pre-flight: tasks 1-5 share the solver's node API (add, subscribe, propagate, follow, node_of); one owner, solver.py.
Task 1: complete (commits 1bbf2b4a, tests: python -m unittest discover -s featuretrace/tests -t . -> 85 OK) — solver.py value nodes, subscriptions, propagate
Task 2: complete (commits 1bbf2b4a, tests: same -> 85 OK) — transfer.py per-value rules (member, call, step, operator, substitution)
Task 3: complete (commits 1bbf2b4a, tests: same -> 85 OK) — typer.py wiring by shape; #hold/#ret nodes; types_of.py split by responsibility
Task 4: complete (commits 1bbf2b4a, tests: same -> 85 OK) — flows.py: links subscribe arguments, writes, signals
Task 5: complete (commits 1bbf2b4a, tests: same -> 85 OK) — link.py drains differences before each consumer
Task 6: complete (commits 1bbf2b4a, tests: city old/new solver every link -> 0 pairs lost; lab fixpoint 325 s, 595,715 links)
Task 3: Ruling: Callable(obj, name) and ClassDB.instantiate(name) stay consumers computed whole — their value pairs two growing sets, not distributive — cost if wrong: slower only when a program uses them heavily
Task 3: Ruling: a namespace member with no export adds the engine's value only when neither definitions nor modules answer — same as the old rule's empty case — cost if wrong: a receiver classified engine-free while its export's values are still empty
Final review: self-review (no subagent: the owner forbade agents, 2026-11-10)
