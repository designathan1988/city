# Data Model: Bring the reference game's features, correctly and fast

## Feature (the inventory, tools/atlas/inventory*.py)

| Field | Meaning | Rule |
|---|---|---|
| game | `reference` or `city` | from the tree read |
| area | one of the shared areas, or `unclassified` | from the folder, file or key words (inventory_rules.py), never a hand list of features |
| kind | `action`, `query`, `text`, `data`, `system`, `test` | from the form of its declaration |
| name | its key, id or module path | as declared |
| label | the player's text, when one | the text table's entry for its key |
| description | the file's first comment, or the first use of a text's key | read, never written by hand |
| source | `path:line` | the declaration's line |

## Pair

| Field | Meaning | Rule |
|---|---|---|
| reference, city | the two features | one each |
| evidence | `text:en`, `text:pt`, `key`, `id`, `differs:<lang>` | only by these; a pair by area alone is no pair |

## Comparison sheet (contracts/comparison-sheet.md)

One feature: the reference's outputs, conditions, cases, values (file:line), state, costs and faults; this game's same; the
differences; the design here; the files read whole with their hour; the Context7 answers; the checks that prove it.
State: `drafted` → `read` (every file the trace reached read whole) → `designed` → `built` (code, scene, photos pass) → `paired`
(the inventory shows it paired). A feature not brought is `justified` (a fault, a lab tool, a superseded design), with why.

## Item (doc/work/<block>.md)

One problem or one feature. Sections: Read (path: 1-N of N, hour, what it does, faults), Research (Context7 source, question,
answer), Trace (the sheet), Look (the one run before the code), Plan, Changed, Checks (scene, seconds, verdict), Photos (claim,
file, analysis), Skills (which, when). The block's close refuses an item missing Read, Research or Skills (contracts/block-close.md).

## Camera solids (native/src/api/city_core_sight.cpp, contracts/camera-solids.md)

| Entity | Meaning |
|---|---|
| Solid | a building's volume as it stands: lot frame, plan ring, roof shape, eaves height over the lot's floor |
| roof at (x, z) | the top of the solid whose plan holds the point; none outside |
| floor at (x, z) | the lowest the eye may stand: inside a plan its roof plus `eye_roof_clearance_m`; within `eye_wall_reach_m` round it, easing to the foot by a smoothstep of the distance; none farther |
| eye free | the eye out of every solid, the reference's rules 1-4 (research R1) |
| highest roof | the highest top within a radius, over the middle of its volume |

New values (camera.json, read by DataTables): `eye_roof_clearance_m` 1, `eye_wall_reach_m` 2, `eye_step_back_m` 4,
`near_gap_share` 0.5 (the reference's behaviour, research R1). The pull's (`pull_*`) and the near plane's (`near_per_m`, `near_min`)
values already stand in camera.json and stay; their differences from the reference are recorded in the camera's sheet.
