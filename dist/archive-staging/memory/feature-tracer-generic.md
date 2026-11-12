---
name: feature-tracer-generic
description: "BINDING (owner, repeated hundreds of times): the feature tracer is GENERIC — never a rule, table, list or special case for a program, library, framework, API member or code pattern"
metadata:
  node_type: memory
  type: feedback
  originSessionId: bceccebc-2abc-4510-8d0b-e2ad9391ef75
  modified: 2026-11-11T18:56:57.252Z
---

The feature tracer (`tools/analysis/featuretrace`, CLI `tools/analysis/trace_feature.py`) must trace ANY feature of ANY
program in its languages (GDScript, C++, JavaScript/TypeScript). It is generic, never "engessado":

- **No case for a program, library, framework or API.** No rule, table, list, name set or branch that names a program's,
  a library's or a platform's members (`addEventListener`, `forEach`, `CityStore`, three.js, `identityMemo`, Godot
  method names...). The languages' semantics come from their specifications (ECMA-262, ISO C++, the GDScript reference);
  what a platform does comes only from its official declarations, read as data (TypeScript `lib.*.d.ts`, packages'
  `.d.ts`, Godot `extension_api.json`, headers the program includes); what no declaration says gets a generic summary
  derived from the declarations, never a hand list.
- **A gap is never closed by adding a case.** A wrong or missing link means the general mechanism is wrong or missing:
  fix the mechanism (the IR, the analysis, the declarations' reading) so every program with that shape is covered.
- **A measurement is acceptance, not the specification.** Never write "the narrowest rule that explains this symptom";
  a rule with a prime (J2', J2'') is a sign that the mechanism, not the case, needed the fix.
- **Plans and records follow the same rule.** A plan task that lists built-ins or names by hand is wrong; reject it.

**Why:** the owner ordered a generic tool from the start and warned me hundreds of times. I kept adding cases: 88 rule
ids with up to four primes, 51 hand tables, an opaque "engine" value hiding gaps (diagnosis in `log/trace-rules.md`,
"Why the tracer never converges", 2026-11-11). On 2026-11-11 the owner said: "NÃO TOLERO MAIS QUE ENGESSE A FERRAMENTA".

**How to apply:** before writing any tracer code, plan or rule, ask "does this name a specific program, library, API,
name or pattern?" — if yes, stop and find the generic mechanism. The gate `tests/test_generic.py` fails on a hand table
of names in the new core (`featuretrace/heap`); the old core's tables are debts listed there, removed by
`log/trace-core-plan.md`. Related: [[fix-cause-effort-neutral]], [[request-is-one-node]], [[study-before-coding]].
