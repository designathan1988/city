# Quickstart: the per-item loop and the validations

## 1. Know what exists (seconds)

```bash
python tools/atlas/cli.py inventory --area people --unpaired
```

Read `dist/inventory/inventory.md` for the area: what the reference has unpaired, with sources.

## 2. One item (the owner's order: once before, then code, then seconds)

1. `python tools/tasks/cli.py block open <name>`; `doc/work/<name>.md` with the goal in the owner's words.
2. One look (once): the scene, measure or photo that shows the state now (`python tools/tasks/cli.py run <task>`), done before
   the block opens or at its first verify; recorded under `## Look`.
3. Trace and read: `python tools/tracer/trace.py --feature <output> --root <files>` on both games; every file reached read whole;
   the sheet (contracts/comparison-sheet.md).
4. Context7 for every API the code will call (`## Research`).
5. Code the whole item; nothing runs.
6. `python tools/tasks/cli.py block verify`; its scenes and its plan in one run, each under a minute:
   `python tools/tasks/cli.py run <scene> <scene> <plan>`.
7. Open every photo whole, crop the element, analyse region by region (CLAUDE.md image protocol); write the analysis.
8. A failure: `superpowers:systematic-debugging`, cause read, `block open`, fix, back to 6. A pass:
   `superpowers:verification-before-completion`, commit, push, `block open` for the next item.

## 3. Validations of this feature

| Story | Command | Expected |
|---|---|---|
| US1 | `python tools/tasks/cli.py run camera_feel_test camera_input_test look_view_test` | each under 60 s; eye inside ≤ 1/1000 frames; no jolt from the rig's smoothing; at-once corrections reported |
| US1 | `python tools/tasks/cli.py run roofs photos` | near/mid roof photos show roofs from above; the frozen street view shows a street |
| US2 | `python tools/atlas/cli.py inventory` | under 5 s; sampled rows match their sources |
| US3 | `python tools/tasks/cli.py run tests` (block end only) | every scene under 60 s; no "its turn for lock" waits behind a shared holder |
| US5 | `python tools/tasks/cli.py run window_traffic_test` | under 60 s, same check |
| US5 | `python tools/gates/lab_scan.py` | no text key of this game is the reference's |
| US6 | `python tools/gates/run_gates.py --register doc/problems.jsonl` (owner's order) | 0 NEW |
| US6 | each tool's tests | pass, each under 60 s |
