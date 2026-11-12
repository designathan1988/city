# Contract: the game's declarations (core X-macros)

Three registries, each in the header of its owner, each expanded by the compiler (the code uses them) and dumped as JSON by the
engine-free test executable (`city_tests --dump-declarations dist/atlas/declared.json`).

## `CITY_STATE_PARTS(X)` — `native/src/sim/state/city_state.h`

`X(name, owner_symbol, controller, writers)`

- `name`: the `CityState` member or group (e.g. `roads`, `lots`, `buildings`, `terrain`, `traffic`, `clock`).
- `owner_symbol`: the one type or function that defines it.
- `controller`: the setter and its transition table for a stateful thing, or `NONE("why")`.
- `writers`: the allowed writer kinds (`COMMAND(area)`, `STEP(system)`, `LOAD`, `GENERATE`).

Debug and profile builds keep one write revision per part; every writer runs inside a scope that names what it declared, and a
part whose revision moved outside a declared scope is a violation in the run report (rule S2 of the world rules).

## `CITY_COMMANDS(X)` — `native/src/sim/commands/`

`X(name, area, writes, announces, undoable)`

- Generates the script side's `StoreApi.COMMANDS` (or is checked against it by `atlas check declared`).
- `writes`: state parts; `announces`: change kinds.

## `CITY_CHANGE_KINDS(X)` — `native/src/sim/state/change_log.h`

`X(name, payload, producers, consumers, touches)`

- `name`: one list of `ChangeLog` (`road_chunks`, `lot_cells`, `net_nodes`, ...).
- `consumers`: who drains it (render adapters, traffic, transit, lot cache).
- `touches`: the area a consumer may rebuild for one entry; work outside it is "without need".

Debug and profile builds count, per kind and consumer: entries drained, units rebuilt, units whose output hash equals the one before
(needless). Counts go to the run report.

## `dist/atlas/declared.json`

```json
{"commit": "<sha>", "state_parts": [{"name": "...", "owner": "...", "controller": "...", "writers": ["..."]}],
 "commands": [{"name": "...", "area": "...", "writes": ["..."], "announces": ["..."], "undoable": true}],
 "change_kinds": [{"name": "...", "payload": "...", "producers": ["..."], "consumers": ["..."], "touches": "..."}]}
```
