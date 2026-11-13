# Contract: the inventory command

`python tools/atlas/cli.py inventory [--area A] [--game reference|city] [--unpaired] [--json]`

- Reads the reference (`tools/gates/rules.json` `lab_reference`, its place from `tools/tasks/locations.py`) and this repository each
  run; prints its progress per game (files read, seconds), the per-area table (reference, paired, only; city, paired, only), the
  evidence counts, what it could not extract and the unclassified places, and the reports' paths.
- Writes `dist/inventory/inventory.json` (`generated`, `roots`, `languages`, `files`, `summary`, `evidence`, `unclassified`,
  `unextracted`, `seconds`, `features`, `pairs`) and `dist/inventory/inventory.md` (per area: pairs side by side, then each game's
  unpaired features, each with its source).
- Exit 0 on success; 1 when the reference is not on the machine or the reports pass the folder's bound (suites.json lifecycle);
  2 for an unknown area.
- Ends in under 5 s on both trees (1.27 s measured on 2026-11-13).
