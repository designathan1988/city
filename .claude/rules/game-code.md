---
paths:
  - "native/src/**"
  - "project/game/**"
  - "project/ui/**"
  - "project/render/**"
  - "project/data/**"
  - "project/tests/**"
---

# The game's code: what holds whenever a file here is read or changed

- This code is changed only with its block open (`python tools/tasks/cli.py block open <area>`) and the skill `port`
  invoked in the session. With the block open nothing is built, run, measured or photographed.
- After `block verify` the code does not change: a failed check's cause is read to a `file:line`
  (`superpowers:systematic-debugging`), then `block open --cause "<check>: <file:line>: <why>"`.
- Before a line that calls an engine, library or language API: Context7 (`resolve-library-id`, `query-docs`). Before a value
  of the real world or a design decision: the internet (WebSearch, WebFetch), the source cited in the data's `_doc`.
- One source of truth, `CityState`: only `sim/commands` changes it, only `sim/simulation` advances time; `core` -> `sim/*`
  (pure C++, no Godot) -> `render` (only reads) -> `api`.
- Every tunable value in `project/data/**` (JSON with its schema in `tools/gates/schemas`), read by `DataTables`; no number
  in `native/src`, `project/game`, `project/ui`.
- Nothing of the reference: no line, translation, structure, name or data key (`python tools/gates/lab_scan.py`).
- A file starts with a comment on its responsibility; a file created or deleted enters `doc/map.md` in the same commit.
