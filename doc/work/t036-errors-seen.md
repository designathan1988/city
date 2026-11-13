# t036-errors-seen: every failure told in one place, the city never lost on a failed save (implementation plan)

> **For agentic workers:** REQUIRED SUB-SKILL: superpowers:subagent-driven-development. The owner's order replaces the TDD
> run cycle: each task writes its test and its code with nothing run (no test, build, gate, Godot); every check runs once at
> the block's end (`block verify`). Steps use checkbox (`- [ ]`) syntax.

**Goal:** a refused command reaches the player once, from one place, with its reason; a save that fails never replaces the
previous one and never lets the game quit or open a new map unsaid; every key the core returns has its text; an engine warning
fails a test.

**Architecture:** the core answers every command `{ok, error, ...}` (no empty answer, no bare int); `CityStore.dispatch` is
the one place a refusal is announced (`command_failed(command, result)`), `Refusals` the one formatter, the HUD the one
listener; the save path checks each file step in the core, and GameFlow/HudActions read the save's outcome before leaving.

**Tech stack:** C++20 GDExtension (godot-cpp, Godot 4.7.2), GDScript, Python 3.12 (tools/gates, tools/tasks), JSON data.

**Spec:** doc/specs/003-roadcraft-features/sheets/backbone-errors-seen.md (behaviour 1-10, cases); tasks.md T036;
audit.md "The interface", "The core's surface".

## Global constraints

- Files start with a comment stating their responsibility; repository text in English; UI strings via `tr("KEY")` with rows
  in project/data/i18n (pt_BR, en); bytes written with LF.
- No magic numbers in project/game, project/ui, native/src: values in project/data/** read through DataTables / the catalogue.
- `core -> sim -> render -> api` stays one way; only sim/commands mutates CityState; a refused command changes nothing.
- No `LAB_COPY`: names and structures designed here, never the reference's.
- Godot/godot-cpp APIs checked with Context7 before use (godot-cpp `FileAccess::store_buffer(const uint8_t*, uint64_t)` returns
  void: check `get_error()`; `DirAccess::rename_absolute` returns `Error`).

## Review focus

1. A preview command's refusal never reaches the hint bar through the channel (the lot and road tools aim every frame).
2. A refusal is shown once: no tool or panel still emits its own refusal hint after dispatch.
3. A save whose write fails leaves the previous file byte-identical and the `.old` copy never deleted before the new file is whole.
4. Closing the window with a failed save does not quit the first time and quits the second time (no trap).
5. An ok answer with nothing changed records no undo step (EditHistory::end with nothing touched).

---

### Task 1: the core answers every command

**Files:** Modify `native/src/api/city_core_persist.cpp:30-51` (write_atomic), `native/src/api/edit_result.h:29-37`
(run_edit), `native/src/api/city_core_road_spec.cpp:93-101`, `native/src/api/city_core.h:86`,
`native/src/api/city_core_bind.cpp:120`, `native/src/sim/commands/lot_commands.cpp:237`,
`native/src/sim/commands/zone_commands.cpp:80-105` · Test `project/tests/core/camera_road_key_cases.gd:59,73`
**Interfaces:** Produces `Dictionary CityCore::road_spec_make(int base, const Dictionary &spec)` → `{ok, error, index}`
(index -1 when refused; errors `hint.road.specInvalid`, `hint.road.variantsFull`); run_edit with no city →
`result_dict(refused("hint.noCity"))`; zoning what already has the zone → `ok = true`, `note = "tools.toast.zoneSame"`, nothing
journaled.

- [ ] camera_road_key_cases.gd:59,73: the two checks read `again.get("ok") and again.get("note") == "tools.toast.zoneSame"`
  (a no-op is no refusal), messages kept.
- [ ] write_atomic: fail with `save.writeFailed` (previous file untouched, tmp removed) when: the compressed open fails;
  `get_error() != OK` after `store_buffer`; after `close()` the tmp reopened with `open_compressed(READ, ZSTD)` is null or its
  `get_length() != bytes.size()`; renaming an existing target to `.old` fails. Only after the tmp→target rename succeeds is
  `.old` removed; a failed final rename puts `.old` back. Context7 (godot-cpp, Godot 4.7 FileAccess) for each call.
- [ ] run_edit: `if (!s.ready) return result_dict(refused("hint.noCity"));` (the refused helper the commands use, or an
  EditResult built the same way).
- [ ] road_spec_make: returns the Dictionary above; the variants table full is told apart from a spec with issues (read
  `city::road_variant`'s refusal; give it a way to say "full" if it only returns -1). Still under the writer lock.
- [ ] zone_lot: unchanged zone → an ok EditResult with the note, before any journal op. paint_zone_path: when every side hit
  already has the zone (zone_commands.cpp:86 sets ZONE_SAME) → ok with the note; keep `tools.toast.zoneNoRoad` a refusal;
  line ~102's test against ZONE_SAME follows the new shape.

### Task 2: one refusal channel, one formatter, one listener

**Files:** Create `project/game/store/refusals.gd`; Modify `project/game/store/city_store.gd:10,96-103`,
`project/ui/roadcraft_hud.gd:45-80`, `project/game/tools/tool_base.gd:78-88`, the tools' post-dispatch refusal hints
(barrier_tool.gd:72, brush_tool.gd:61, building_tool.gd:230, cloud_tool.gd:67, control_tool.gd:19, demolish_tool.gd:57,
lots_tool.gd:67, lot_tool.gd:38 (preview: keeps its hint, via Refusals) and :47, move_tool.gd:48, pole_tool.gd:84,
road_tool.gd:231, roundabout_tool.gd:39, split_tool.gd:24, streetscape_tool.gd:60, strike_tool.gd:11, terrain_tool.gd:78,
upgrade_tool.gd:36, zone_tool.gd:11,70-71), `project/game/tools/road_blocks.gd:18` · Test: create
`project/tests/core/refusal_channel_test.gd` + `.tscn` (form of an existing HUD test such as inspector_test)
**Interfaces:** Produces `class_name Refusals` (extends RefCounted): `static func text(result: Dictionary) -> String`,
`static func reason(result: Dictionary) -> String` (translate with `TranslationServer.translate`; `reason` formats `{max}` from
`DataTables.tuning("road_max_degree")` as today); `signal command_failed(command: StringName, result: Dictionary)` on
CityStore, emitted for every non-preview `{ok: false}` result with a non-empty error.
- [ ] Test refusal_channel_test: with the HUD up and a map open, (a) a refused tool command (e.g. demolish on empty ground
  through the demolish tool's dispatch path) shows `Refusals.text(r)` on the hint bar, danger style, once; (b) a refused
  panel command (road panel `join_roads` on a node that cannot be joined) shows its text on the hint bar; (c) a preview
  command's refusal leaves the hint bar as it was; (d) `command_failed` fires once per refused dispatch.
- [ ] Refusals: the body of ToolBase.refusal/reason moved there; ToolBase keeps no formatter (its callers use Refusals).
- [ ] city_store.gd: the signal's new shape; emitted after `_mark()` for non-preview refusals.
- [ ] roadcraft_hud.gd: connects `CityStore.command_failed` once → `show_hint(Refusals.text(result), false)`.
- [ ] Tools: each post-dispatch `refusal(r)` hint removed; a success hint stays (`ok ? tr(key) : nothing`); lot_tool.gd:38's
  preview hint stays through `Refusals.text`; road_tool's tip label through `Refusals.reason`.
- [ ] zone_tool: the ZONE_SAME filter goes (the core answers ok); a lot click with the note shows `tr(note)` as a plain hint.
- [ ] road_blocks.gd:18: on ok `ctl.hint.emit(tr("tools.toast.blocksBuilt"), true)`; on refusal nothing (the channel tells it).

### Task 3: panels keep what they show and make profiles by command

**Files:** Modify `project/ui/inspector_panel.gd:127-130,156-158,287-289`, `project/ui/land_strips.gd:136-142`,
`project/ui/hud_widgets.gd:80-86`, `project/game/store/store_api.gd:48` (road_spec_make to COMMANDS),
`project/ui/road_catalog.gd:147`, `project/ui/road_panel.gd:68`, `project/ui/road_profile_panel.gd:146,154`,
`project/tests/core/road_edit_test.gd:222-247`, `project/tests/core/road_shape_cases.gd:147`
**Interfaces:** Consumes Task 1's `road_spec_make` answer `{ok, error, index}` through `CityStore.dispatch`.
- [ ] road_edit_test: `dispatch(&"road_spec_make", [base, bad])` → `ok == false` and `error == "hint.road.specInvalid"`;
  the good spec twice → same `index`, `ok`; road_shape_cases reads `.index`.
- [ ] Inspector: `_edit`, the impossible junction's removal and `_demolish` close only when the answer is ok (a refusal is
  told by the channel and the panel stays).
- [ ] land_strips / hud_widgets: the choice is lit after the command answered ok, never before.
- [ ] road_catalog, road_panel, road_profile_panel: `CityStore.dispatch(&"road_spec_make", ...)`, read `.index`, go on only
  when `ok` (a refusal is told by the channel).

### Task 4: leaving waits for the save

**Files:** Modify `project/game/store/city_saves.gd:107-116` (save_on_close), `project/game/flow/game_flow.gd:48-55`,
`project/ui/hud_actions.gd:58-62` · Test: create `project/tests/core/save_fail_test.gd` + `.tscn`
**Interfaces:** Produces `CitySaves.save_on_close() -> Dictionary` `{ok, error}` (`{ok: true}` when nothing was due);
consumes Task 1's checked write_atomic.
- [ ] save_fail_test: open a map paused, make an edit, save once (ok) and read the file's bytes; put a folder at
  `<save path>.tmp`; edit again; `save_on_close()` → `ok == false`, `error == "save.writeFailed"`; the save file's bytes are
  the first save's; the snapshot is still `ready` with the edit revision of the second edit; the `.old` copy, if any, still
  there; remove the folder (the test's own) and `save_on_close()` → ok.
- [ ] save_on_close returns the settled outcome (the result `_finish_save` emits).
- [ ] GameFlow on the close request while PLAYING (not tests_drive): a failed save keeps the game (no quit), flashes nothing
  itself (HudActions already shows `save.failed`) but a hint `save.closeAgain` through the HUD; the next close request quits
  unsaved. A successful or not-due save quits at once.
- [ ] HudActions.open_new: a failed save stops the new map (the save's failure already shown).

### Task 5: warnings fail a test; every key has its text

**Files:** Modify `tools/tasks/verdicts.py:11-17`, `tools/tasks/tests/test_verdicts.py`, `tools/gates/schemas/suites.schema.json`
(expected_errors description), `tools/gates/gate_keys.py:16`, `tools/gates/rules.json`, fixtures
`tools/gates/tests/fixtures/keys__fail`, `keys__pass`, `project/data/i18n/strings.csv`, `strings_e.csv`
- [ ] test_verdicts: `test_a_warning_passes` becomes `test_an_engine_warning_fails` (`WARNING: x` → FAIL); new
  `test_a_declared_warning_passes`; `test_a_colored_warning_fails` (`\x1b[1;33mWARNING:\x1b[0m x`).
- [ ] verdicts.py: GODOT_FAILED also finds `^(?:\x1b\[[\d;]*m)*(?:USER )?WARNING:`; such a line is declarable exactly as an
  engine error; comments say engine error or warning.
- [ ] rules.json: `"text_keys": {"prefixes": ["cmd", "hint", "refuse", "save", "tool", "tools", "help", "building",
  "road.reason"]}`; gate_keys builds KEY from it (`ctx.rules`), a key = a prefix, ".", then `[A-Za-z0-9_.]+`.
- [ ] keys__fail: a `.cpp` returning `"refuse.nowhere"` with no row → TEXT_KEY_MISSING; keys__pass keeps passing.
- [ ] Rows (pt_BR, en): `hint.noCity` "Nenhuma cidade aberta." / "No city is open."; `hint.road.specInvalid` "Esse perfil de
  via tem um problema e não pode ser feito." / "That road profile has a problem and cannot be made.";
  `hint.road.variantsFull` "Não cabem mais perfis de via próprios." / "There is no room for more custom road profiles.";
  `save.closeAgain` "O salvamento falhou: feche de novo para sair sem salvar." / "The save failed: close again to quit
  without saving."

---

## Checks (block verify, once)

- Both DLLs (`tools/build/build.sh core-windows`); `python -m unittest discover -s tools/tasks/tests`, `-s tools/gates/tests`,
  `python tools/gates/run_gates.py --selftest`, the gates of each changed file.
- `python tools/tasks/cli.py run tests` (every scene under the stricter verdict) and the new scenes refusal_channel_test and
  save_fail_test (suites.json entries added by the controller); day/night photos (`run photos`).
- Each new test seen failing without its fix where it costs one revert (save_fail_test against the old write_atomic; the
  warning verdict; the keys fixture).

## Outcome (2026-11-13)

Built by five implementers in parallel on disjoint files (nothing run while coded), each reviewed for spec and quality, one fix
round for Tasks 1-4, a scoped re-review (9 findings addressed, none open), then checked once.

- Core (Task 1): `write_atomic` checks the open, the write (`get_error`), a whole read-back compared slice by slice
  (`reads_back`, native/src/api/city_core_persist.cpp), the rename aside and the final rename; it removes a temporary file only
  when this save made it. `run_edit` answers `hint.noCity` with no city (edit_result.h); `road_spec_make` is a command answering
  `{ok, error, index}` (city_core_road_spec.cpp, store_api.gd COMMANDS); zoning what already has the zone answers ok with the
  note `tools.toast.zoneSame` (lot_commands.cpp, zone_commands.cpp), journaling nothing.
- Channel (Task 2): `CityStore.command_failed(command, result)` for every non-preview answer with an error (a partial refusal
  included); `Refusals.text`/`reason` (project/game/store/refusals.gd) the one formatter; RoadcraftHud the one listener; the
  tools, transit_tool, reference_hand, barrier_tool and land_hands no longer show a store refusal of their own.
- Panels (Task 3): the inspector closes only after an edit was done; a choice is lit only once its command answered ok
  (hud_widgets `choices`, land_strips, approach_panel, hud_popover, road_options); a refused custom road card relights the road
  in hand (road_catalog.gd).
- Leaving (Task 4): `CitySaves.write_before_leaving()` (renamed from the reference's name, LAB_COPY) answers the settled
  outcome; a failed save holds the window's close once (`GameFlow.close_held`, hint `save.closeAgain`), the next close waits for
  any save in flight and quits; a new map stops on a failed save.
- Checks (Task 5): an engine `WARNING:` line fails a Godot task unless declared (tools/tasks/verdicts.py); the text-keys gate
  reads its prefixes from tools/gates/rules.json `text_keys` (refusal prefixes added); rows hint.noCity, hint.road.specInvalid,
  hint.road.variantsFull, save.closeAgain.
- boot_refusal_test now requires the no-city refusal (`hint.noCity`) where it accepted the empty answer the defect gave.

Checks (fresh, 2026-11-13): both DLLs built, no warning; tools/tasks/tests 116 OK; tools/gates/tests 33 OK; gates self-test 97
OK; `run tests` 65/67 — the two failures are camera_feel_test and camera_input_test, Pinterface-142 (T027), no camera file
touched; refusal_channel_test and save_fail_test pass; photos 4/4 (the night street view inside a building is Ptools-110, T027).
The changed files' gates report only registered findings (Psimulation-3, Pinterface-1, -9, -13, -15, -20, -27, -30, -32, -34,
-35, -36, -40) and CLAUDE_MD on the owner's uncommitted CLAUDE.md. The warning verdict's and the key prefixes' tests seen failing
with their fix reverted. The 24 km scale probe does not exist yet: T038 builds it. Registered: Psimulation-107 (.old restore).
