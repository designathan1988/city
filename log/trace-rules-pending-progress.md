# SDD ledger — plan: log/trace-rules-pending-plan.md
Spec: log/trace-rules.md, "The pending rules found against a real run". Executor: native, this session.
Pre-flight: tasks 1 and 5 share the reader/solver read path (scopes renaming feeds the trees the relink reads); no conflict.
Task 1: complete (commits 93b903ed..09972b1e, tests: test_pending block scope C++/GDScript/JS, renamed argument, var scope -> RED on 93b903ed then GREEN, suite 94 OK) — scopes.py; declare/assign/add_ref through the reader
Task 2: complete (commits 93b903ed, tests: test_local_class RED -> GREEN) — Unit.local_types, CppReader.local_class, TypeNames.type_class
Task 3: complete (commits 93b903ed, tests: test_direct_initialization_carries_the_arguments) — cpp_implicit.argument
Task 4: complete (commits 93b903ed, tests: test_destruction) — cpp_lifetime.py
Task 5: complete (commits 93b903ed..09972b1e, tests: test_no_stale_unreached_note RED -> GREEN; city pairs identical on a fixed snapshot; lab 368-428 s) — relink by difference
Task 6: complete (commits 93b903ed, tests: city comparison) — with_overrides takes every override; Index.overrides memoized
Task 7: complete (runtime check: new run of road_commands_test on the current DLL, 236/236 calls linked, exit 0; lab fixpoint, 785,524 links, unreached uses 407)
Task 5: Ruling: deltas kept as read positions per (consumer, node) instead of copied lists (the plan said "per-node deltas") — copying to every reader cost 430 s against 368 s for the same links — cost if wrong: none found; same links
Task 5: Ruling: decisions that need the whole set (anything found by lookup, an overload that fits) use what the reference already found (Resolver.exact, fitted) — the full relink had the same order dependence for early fallbacks — cost if wrong: a fallback link an earlier run made stays
Task 5: Ruling: an "unreached" note is dropped when a difference arrives — values now reach the receiver — cost if wrong: none (an EXTERNAL note is made again by the run if it applies)
Task 6: Ruling: every override is labelled as such even when the same call reached it by lookup — labels no longer depend on relink order — cost if wrong: more edges in the report (lab 595k -> 787k before the argument fix)
Final review: self-review (no subagent: the owner forbade agents, 2026-11-10)
Final: Ruling: the duplicated call/override rows were merged in the report (walk.ordered shows an override only for an end no other running link reaches; the graph keeps both) — asked for by 'conclude every pending item' — cost if wrong: an override row hidden where a call row to the same end is shown
Final: minor (deferred): lab run time (368-428 s) is dominated by the initial value drain; collapsing cycles of same-set nodes (PKH/HCD) would cut it
Final: resolved 2026-11-11 05:30 (the deferred minor above): cycles of identity subscriptions are merged (log/trace-rules-cycles-plan.md); measured on the lab, the merge cut the drain 196 s -> 180 s and the whole run not at all (383.6 s -> 382.7 s): the cost is the size of the flow, not cycles (log/pendencias.md).
