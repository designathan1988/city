# Backbone: errors seen (T036, architecture.md backbone items 1 and 2)

State: `built` (t036, 2026-11-13). Not a reference feature: the reference shows a refusal per tool and loses work on a failed
write too; this sheet fixes how this game tells every failure, read in this game's code (audit.md, "The interface" and "The
core's surface"), each site re-read on 2026-11-13.

## Behaviour

1. **One place shows a refusal.** Every command sent through `CityStore.dispatch` that comes back `{ok: false, error: key}`
   (previews aside) is told by `CityStore.command_failed(command, result)`; the HUD listens once and shows
   `Refusals.text(result)` on the hint bar in its danger style. No tool, panel or strip formats or shows a refusal of its own
   (today 17 tools do, every panel drops it: city_store.gd:10 has no listener).
2. **One formatter.** `Refusals.text(result)`: a `refuse.*` or `road.reason.*` key reads "Cannot build here: {why}." (
   `tools.toast.cannotBuild`), any other key its own text; `Refusals.reason(result)` the short why the road tool's tip label
   shows. Moved out of ToolBase (tool_base.gd:78-88), so panels and tools share it.
3. **Nothing to do is no refusal.** Zoning a lot or a side that already has the zone changes nothing and returns `ok` with
   the note `tools.toast.zoneSame` (lot_commands.cpp:237, zone_commands.cpp:86): a drag over zoned sides never fills the hint
   bar, a click on a zoned lot says so as a plain hint.
4. **A refused panel edit keeps the panel.** The inspector closes after a road edit, a junction's removal or a demolition only
   when it was done (inspector_panel.gd:127-130, 156-158, 287-289); a choice is lit only once its command answered ok
   (land_strips.gd:140-141, hud_widgets.gd:82-85).
5. **Every key the core returns has its text.** The text-keys gate judges the refusal prefixes too (`refuse.`, `save.`,
   `tool.`, `tools.`, `help.`, `building.`, `road.reason.`, besides `cmd.` and `hint.`), read from tools/gates/rules.json.
   road_blocks shows the core's reason (road_blocks.gd:18 sent a raw key as a success).
6. **No city, no silent success.** A command with no city ready returns `hint.noCity` (edit_result.h:32 returned `{}`, which
   dispatch took for success).
7. **A road profile is made by a command.** `road_spec_make` moves from the queries to the commands and answers
   `{ok, error, index}`: `hint.road.specInvalid` for a spec with a problem, `hint.road.variantsFull` when the table is full
   (city_core_road_spec.cpp:93-101 answered -1 for both).
8. **The city is never lost on a failed save.** `write_atomic` checks every step: the open, the write (`get_error`), the
   closed file read back to its full length, the old copy set aside, the rename; any failure leaves the previous file whole
   and answers `save.writeFailed` (city_core_persist.cpp:30-51 checked only the open and the last rename).
9. **Leaving waits for the save.** Closing the window or opening a new map saves what is unsaved, waits for the outcome, and
   stops on a failure with its reason shown (`save.failed`); closing the window again then quits unsaved, as the hint says
   (`save.closeAgain`) (game_flow.gd:50-51, hud_actions.gd:60-62 went on regardless).
10. **An engine warning fails a test** as an engine error does (t042 made `ERROR:` fail): a scene declares the ones it
    provokes on purpose in `expected_errors`.

## Cases

- A refused tool command, a refused panel command (road panel's join on a node that cannot be joined), a preview refused:
  the first two show once on the hint bar, the preview shows nothing through the channel (the lot tool keeps its own preview
  hint).
- A stroke of the zone brush over sides that already hold the zone: no hint; a lot clicked with its own zone: the plain note.
- The save target's temporary file cannot be written (a folder stands at `<target>.tmp`): `save.writeFailed`, the previous
  save byte-identical, the city still ready with the same edit revision; closing refused once, then allowed.
- A spec with a problem, the same good spec twice (same index), the variants table full.

## Values

No tuning value is added; the keys' texts are rows of project/data/i18n (pt_BR, en).

## Faults never brought

- A refusal formatted per tool or dropped by a panel; a success hint built from a refusal key.
- A save that replaces the previous one before the new one is known whole.
- A "nothing to do" told as a failure.
