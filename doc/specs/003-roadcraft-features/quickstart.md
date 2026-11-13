# Quickstart: the per-feature loop and the validations

## 1. Know what exists (seconds)

```bash
python tools/atlas/cli.py inventory --area people --unpaired
```

Read `dist/inventory/inventory.md` for the area: what the reference has unpaired, with sources.

## 2. One feature (the owner's order: one look before, research, code whole, test at the end)

1. One look (once): the scene, measure or photo that shows the feature's state now (`python tools/tasks/cli.py run <task>`).
2. Trace and read: `python tools/tracer/trace.py --feature <output> --root <files>` on both games; the files it reaches read; the
   sheet (contracts/comparison-sheet.md).
3. Context7 for each API the code will call; the internet for the design decisions.
4. `python tools/tasks/cli.py block open <feature>`; `doc/work/<feature>.md` short (goal, sheet, plan, state, next); the whole
   feature coded, every case of the sheet; nothing runs.
5. `python tools/tasks/cli.py block verify`; both DLLs (`tools/build/build.sh core-windows`); its scenes, measure and photo plan:
   `python tools/tasks/cli.py run <scene> <scene> <plan>`; each photo opened and looked at.
6. A failure: `superpowers:systematic-debugging`, the cause fixed in the code (`block open`), only the failed check again. A pass:
   commit, push, `block close`, the next feature. A defect outside the feature: one line in the register, not worked now.

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
