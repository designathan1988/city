# Cycle Collapsing of Identity Edges Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** The tracer's solver keeps one node for every cycle of `same` subscriptions, so a value set shared by a cycle is held and propagated once instead of once per member.

**Architecture:** Union-find over value nodes in `solver.py` (every node key is read through `find`); a new
`cycles.py` finds the strongly connected components of the `same` edges with an iterative Tarjan sweep, run inside
`propagate` when the `same` edges have doubled since the last sweep, and merges each component exactly (each member's
subscriptions get only what that member lacked; waiting differences keep their member's subscriptions; readers of a
member read the merged node from its start).

**Tech Stack:** Python 3.12, standard library only (`collections.deque`, `defaultdict`).

**Spec:** `log/trace-rules.md`, sections "Collapsing cycles of identical values" and "When the cycles are looked for"
(the second replaces the first's "When to look" row).

## Global Constraints

- Every file under 300 lines and opens with a comment saying its responsibility; all repository text in English.
- A new file goes into `log/mapa.md` in the same commit; gates pass: `python tools/gates/run_gates.py --files-from <list>`.
- Nothing in the tool names a project; only the language-neutral solver changes.
- The owner's order of work: code and review everything first, run tests only at the end, small first, then large.
  The RED half of each test is checked at the end against the base commit's code (`6aaab88f`).
- Processes run in the foreground or under a monitor with a short deadline and live lines; never unwatched.
- Stage only this plan's files (other sessions edit `native/` and `project/`); commit message ends with
  `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`; push to `origin/main`.
- No records deleted: ledgers and logs are appended to.

## Review Focus

1. Two values equal by `identity` but carried with different contexts: a merged node keeps the first found, as each
   node did before; the city and lab link sets must come out equal to the base commit's (Task 3).
2. A sweep while a consumer runs would leave that consumer's read marks pointing into a list that changed under it: the
   sweep runs only when no consumer is on the stack (`test_no_sweep_while_a_consumer_runs`, Task 2).
3. A cycle of tens of thousands of nodes (three.js chains): Tarjan must be iterative
   (`test_a_long_cycle_is_collapsed_without_recursion`, Task 2).
4. Edges added after a merge between two members (now one node) must not become self-subscriptions that resend a set to
   itself (`test_collapsed_values_equal_the_plain_fixpoint` adds edges between merged nodes at random, Task 2).
5. A difference queued for a member before the sweep must still reach that member's subscriptions
   (`test_values_reach_every_member_subscription`, Task 2).

---

### Task 1: The solver reads every node through its representative

**Files:**
- Modify: `tools/analysis/featuretrace/solver.py` (whole class)
- Modify: `tools/analysis/featuretrace/typer.py:31-34,60-61` (`same`, `Typer.follow` move to the solver)
- Modify: `tools/analysis/featuretrace/rules.py` (`HANDLERS["FLOW"]` gains `solver:Solver.follow`)

**Interfaces:**
- Produces: `SAME = ("same",)` (module constant); `Solver.find(node) -> node`; `Solver.follow(source, target) -> None`
  (an identity subscription, transfer `self.add`); `Solver.wake(reader, node) -> None`; `Solver.parent: dict`;
  `Solver.same_edges: int` (identity subscriptions made); `Solver.swept: int` (`same_edges` at the last sweep);
  pending entries become `(node, values, only)` where `only` is `None` (every subscription of `node` when processed) or
  a list of `((target, key), transfer)` items.

- [ ] **Step 1: Write the test** `test_follow_is_an_identity_subscription` in
  `tools/analysis/featuretrace/tests/test_cycles.py`: a bare `Solver`, `follow(a, b)`, `add(a, [("#X", ctx)])`,
  `propagate()`; asserts `[v[0] for v in s.current(b)] == ["#X"]` and `("b", ("same",)) in s.subs[s.find("a")]`.
- [ ] **Step 2: Implement** in `solver.py`: `find` (iterative, path compression); `add`, `subscribe`, `current`, `read`
  apply `find` to the node they are given (`subscribe` to the source only; a `same` subscription whose target already
  finds to the source is not made; a new one counts in `same_edges`); `take_deltas` maps each grown node through
  `find` and reads each representative once; `grew` calls `wake` for each reader; `propagate` processes
  `(node, values, only)`. Remove `same` and `follow` from `typer.py`.

### Task 2: Tarjan sweep and exact merge

**Files:**
- Create: `tools/analysis/featuretrace/cycles.py`
- Modify: `tools/analysis/featuretrace/solver.py` (`propagate` calls the sweep)
- Modify: `tools/analysis/featuretrace/rules.py` (`HANDLERS["FLOW"]` gains `cycles:sweep`, `cycles:collapse`)
- Modify: `log/mapa.md` (the two new files)
- Test: `tools/analysis/featuretrace/tests/test_cycles.py`

**Interfaces:**
- Consumes: Task 1's `find`, `wake`, `parent`, `same_edges`, `swept`, pending triples, `SAME`.
- Produces: `cycles.SWEEP_GROWTH = 2`; `cycles.due(s) -> bool` (`s.same_edges >= SWEEP_GROWTH * s.swept`, more edges
  than at the last sweep, and `not s.stack`); `cycles.sweep(s) -> int` (nodes merged away);
  `cycles.components(s) -> list[list]` (components of two or more representatives, in discovery order);
  `cycles.collapse(s, members: list) -> tuple[node, dict]` (the representative and each member's subscriptions as they
  were, internal `same` edges left out).

- [ ] **Step 1: Write the tests** in `test_cycles.py` (context: a small class with `lang`, `owner`, `file`, `gid`):
  - `test_a_cycle_of_identity_edges_is_one_node`: `a→b→c→a` by `follow`, `add(a, #X)`, `propagate`: one
    representative for the three, and only it is a key of `s.value`.
  - `test_values_reach_every_member_subscription`: `follow(a,b)`, `follow(b,a)` on empty nodes; a recording
    subscription on each; `add(a, #X)`, `add(b, #Y)`; `propagate`: each recorder got `#X` and `#Y` exactly once.
  - `test_a_reader_of_a_merged_node_reads_the_union`: cycle `a↔b` on empty nodes; `add(b, #Y)`; consumer `"c"` reads
    `b`; `add(a, [#X, #Z])`; `propagate`: `"c"` is dirty and `take_deltas("c")[find(b)]` holds `#X`, `#Y`, `#Z`.
  - `test_only_identity_edges_join_a_cycle`: `follow(a, b)` and an element transfer `b → a`: `find(a) != find(b)`.
  - `test_no_sweep_while_a_consumer_runs`: a cycle made with a consumer on `s.stack`, `propagate` called there:
    no merge; after the stack is empty, the next `propagate` merges.
  - `test_a_long_cycle_is_collapsed_without_recursion`: a ring of 20 000 nodes: one representative.
  - `test_collapsed_values_equal_the_plain_fixpoint`: for seeds 0..19, 40 nodes, 100 `follow` edges and 20 element
    edges added in random order with values and `propagate` calls between them, and consumers reading random nodes
    whose deltas are taken as `link.build` takes them; at the end every node's identities equal the closure computed
    in the test, and each consumer has seen every value of what it read.
- [ ] **Step 2: Implement `cycles.py`**: `components` is Tarjan's algorithm with an explicit stack over the
  representatives that have `same` subscriptions, successors `find(target)` of each `same` subscription (self excluded).
  `collapse`: the representative is the member with the most values (first in the component on a tie); the others'
  values are appended to its list (its readers' marks stay valid); queue `(rep, appended, rep's subscriptions as they
  were)` and, for each other member, `(rep, values of the union it lacked, its subscriptions as they were)`; set
  `parent`; move subscriptions (internal `same` edges dropped, the representative's own included); move readers (a
  reader without a mark on the representative gets mark 0) and `wake` every moved reader, and every reader when values
  were appended; drop the members' `value` and `known`. `sweep`: `components`, `collapse` each, then one pass over
  `pending` gives every `only=None` entry of a merged node its member's subscriptions as they were and its
  representative as node; `swept = same_edges`.
- [ ] **Step 3: Wire** `propagate` to call `cycles.sweep(self)` when `cycles.due(self)`, checked before each entry.

### Task 3: Review, tests at the end, records

- [ ] **Step 1: Review** every changed line against the spec and the Review Focus; ledger the result.
- [ ] **Step 2: RED on the base** — copy `tools/analysis` of `6aaab88f` to the scratchpad (`git archive`), run
  `test_cycles.py` there with this test file: Expected: every test fails or errors (no `find`, no `cycles`).
- [ ] **Step 3: Unit tests** — `python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis`:
  Expected: 94 + 8 tests, OK.
- [ ] **Step 4: City** — `dump_edges.py` (scratchpad) with the base copy and with the working tree on the fixed
  snapshot `scratchpad/tree`: Expected: identical link files.
- [ ] **Step 5: Lab** — the same dump on the lab roots (`src`, `tsconfig.json`, `node_modules/three/package.json`,
  `node_modules/three/build`, `node_modules/three/examples/jsm`), base then new, each under a monitor with a 900 s
  deadline: Expected: identical link files (785 524 links), the new run faster than the base's.
- [ ] **Step 6: Gates and records** — gates on the changed files; `log/feature-trace.md` entry; the ledger
  `.superpowers/sdd/trace-rules-cycles-plan/progress.md` and its copy `log/trace-rules-cycles-progress.md`; the
  pending plan's ledger (and copy) gets a line resolving its deferred minor; commit and push.

### Task 4: Exact readers and call sites (added after the lab run, 2026-11-11 04:30)

Spec: `log/trace-rules.md`, "What the lab run showed".

**Files:** `tools/analysis/featuretrace/resolve.py` (`keep_edge`: arguments per call site, `Resolver.carried`),
`tools/analysis/featuretrace/flows.py` (`flows_of(edge)`), `tools/analysis/featuretrace/solver.py`
(`Solver.extra: defaultdict(dict)` consumer -> {node: values it must still read}; `take_deltas` gives extra + tail;
`Typer.recompute` drops a consumer's extra), `tools/analysis/featuretrace/cycles.py` (a moved reader's extra and mark),
`tests/fixture/pending/sites.js`, `tests/test_pending.py`, `tests/test_cycles.py`.

- [ ] **Step 1: Tests**: `test_each_call_site_carries_its_argument` (`sites.js`: `place(new Left()) + place(new Right())`
  on one line: `place`'s `p.side()` reaches `Left.side` and `Right.side`); `test_a_reader_of_a_merged_node_reads_the_union`
  now expects only `#X`, `#Z` (it had read `#Y`); the fixpoint test checks no consumer is given a value twice.
- [ ] **Step 2: Code** as the spec's two rules say.
- [ ] **Step 3: Verify**: unit suite; city base against new (every new link a call site sharing its line with another
  call of the same function); lab with merging against the same code with sweeps off: identical links, and the time.
