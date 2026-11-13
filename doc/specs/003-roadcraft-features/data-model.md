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
| Camera floor tile | `floor_tile_m` square of `floor_cell_m` cells: each the highest roof top plus `eye_roof_clearance_m` over it (the ground where no building stands), then slope-limited to `floor_slope` by a forward and a backward sweep over the tile and an apron of `max roof height / floor_slope`; built on first ask, kept in an LRU of `floor_tiles_kept`, dropped when ChangeLog names a building in it (derived data: never saved, never in the city's digest) |
| eye clear | the eye's sphere pushed out of the solids it overlaps toward the point (research R1b) |
| highest roof | the highest top within a radius, over the middle of its volume |

Validation: `floor_slope` > 0; `floor_cell_m` divides `floor_tile_m`; a floor read is the bilinear mix of its four cells (continuous);
memory bounded by `floor_tiles_kept` whatever the area explored.
