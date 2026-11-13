# Saves and the interface: the area's index (specification 003, T021, T024)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present: autosave (0.7 s quiet, 3 s at most, every 120 s, on focus-out and close); slots and Load last (beyond the reference);
  versions and quarantine; undo and redo with groups and labels (500 steps, 512 MiB); title screen and new-city form; the top bar;
  dock, drawers, menu and popovers; overlays (grid, zones, congestion, interior); hints and toasts; settings (graphics, language,
  road keys). Neither game has a tutorial, sound settings or a land-value overlay.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | The player told when a city cannot be read or a save fails | PARTIAL, medium | the last city set aside unsaid (play.gd:49,133-136); `save.damaged`, `save.fallback` exist unused; quitting or replacing goes on whatever the save's outcome; a short write replaces the save (audit.md) | strings_b.csv:429,438, game_flow.gd:50-51, city_core_persist.cpp:37-49; persistence.ts:363-391, main.ts:2528 |
| 2 | Autosave silent | PARTIAL, medium | "Saved automatically." after nearly every edit; the reference speaks only on a failure, at most once a minute | hud_actions.gd:131-136; main.ts:3966-3973 |
| 3 | The inspector's parking and street numbers | ABSENT, medium | parking left/right and street numbers nowhere in project/ui | inspector.ts:241,250 |
| 4 | A new map undoable, or the old city kept | PARTIAL, medium | a new map empties the history (the old city saved first instead) | city_gen.cpp:83, edit_journal.h:145; main.ts:2067-2073 |
| 5 | The session with the city | PARTIAL, low | camera, speed, levels and paint in `<slot>.cfg`, written only after a city save; nothing written on close if only the camera moved; an exported copy loses its .cfg | session.gd:16-29,53-55, city_saves.gd:109-111,200-206; persistence.ts:28-50, mapExport.ts:8 |
| 6 | The help and small defects | PARTIAL, low | no Builder help section; PgUp/PgDn in Inspect taken by the camera; the counts tooltip always "— km/h" | pop_bodies.gd:65-76, help.json, hud_top_bar.gd:181; shell.ts:675,795-808 |

## Reference faults never brought

The whole map saved as one text into localStorage (persistence.ts:26,131); gzip per IndexedDB frame in batches
(saveStore.ts:37-48); a bytes getter looping over both undo stacks each trim pass (history.ts:25-30,157-158; here a running total);
a full map copy kept to redo whole-map steps (history.ts:73-78); a 250 ms timer polling the readouts (shell.ts:718); layout
measured on every drawer redraw (shell.ts:1926-1950); a blocking window.confirm (main.ts:2067).
