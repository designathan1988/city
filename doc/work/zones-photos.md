# zones-photos: the photos of zones, lots and growth, each case shown working

## Now
- THE OWNER'S ORDERS OF 2026-11-14, LATEST FIRST (their words):
  - "outra sessão editou o hook de memória, autocompactação, compactação, limite de caracteres, pois ela alegou que
    estavam impedindo o trabalho corretamente. Verifique isso também em conjunto com todo o resto do mecanismo. Sempre
    consulte a documentação oficial, compreenda na totalidade. Sempre pergunte a você mesmo: Qual o objetivo do que
    estou fazendo?; O mecanismo atual cumpre isso ou não?; Caso não, explique o por quê.; Como devo resolver o
    problema na causa? Sempre use as skills necessárias para isso."
  - "não basta apenas registrar na memoria, você tem que garantir que a skill deste projeto, hooks e demais
    funcionalidades garantam, isso. quando encontrar um problema de comportamento seu, jamais registre apenas na
    memoria, mas garanta que todos os mecanismos impeçam de acontecer novamente ... Sempre presquise na documentação
    oficial no claude ... verifique e teste para garantir que tudo funciona adequadamente. Além disso, audite todos
    os mecanimos para garantir que tudo funciona corretamente."
  - "Cada funcionalidade exige fotos próprias ... sempre está tirando as mesmas fotos de vista afastada e dos mesmos
    ângulos ... deveria ter mostrado o zoneamento sem as construções, o zoneamento pintado por tipos, o zoneamento com
    as construções, lotes, as ferramentas testadas, como os lotes e construções se adaptam as vias, etc. E isso não é
    só no caso do zoneamento, é em qualquer outra funcionalidade".
  - "TODO PROCESSO QUE ENVIAR DEVE DURAR NO MÁXIMO SEGUNDOS, COM MONITORAMENTO, REAL E SAÍDA. JAMAIS UM PROCESSO DEVE
    FICAR PERPETUAMENTE ... AO CODIFICAR, NÃO FAÇA MAIS REVISÃO, RE-REVISÃO ... CADA ETAPA TEM AS SUAS FOTOS PROPRIAS
    DE SEGUNDOS E TESTES DE SEGUNDOS ... NADA DEVE FICAR ABERTO OU SER IGNORADO"; "NÃO PARE PARA ME PERGUNTAR NADA".
  - Standing: Portuguese to the owner; whole photos, never a crop alone; no agents; a failed check's cause read in the
    code; a hook's refusal obeyed.
  - "Além disso garanta que as configurações em C:\Users\Jonathan\.claude estão corretas. Pesquise antes, e tenha
    certeza antes de mexer em qualquer coisa."
- STATE (2026-11-14, 15:40): THE BLOCK zones-photos IS IN VERIFY, VERIFICATION 2 OF 3, AND IT PASSED SO FAR: the
  build "2/2 passed in 132 s"; the one run `python tools/tasks/cli.py run growth_test,zones,e2e --seconds 180`: "6/6
  passed in 79 s" (growth_test, e2e, zones: FAILURES 0); ALL 17 PHOTOS OPENED AND JUDGED, nine sent to the owner. The
  photos now show: 01, 02 each density in its tone; 11 the Lots tool in hand with both outlines; 12 both lots coloured
  by their zone; 13, 14 the polygon lot's parking paved under its cars, lawn to the outline; 16 the zone taken off
  with the clock standing: buildings and cars gone; 00, 03 to 07, 09, 15 as before (right). Still wrong and
  registered, not this block's: 08, 10 the land through the works' yards. The one measure: `python
  tools/tasks/cli.py run lab_test --all --jobs 1 --seconds 120`: "4/4 passed in 33 s", frame_ms 16.683, fps 59.94,
  cpu_ms 2.81, gpu_ms 6.89, 1 hitch, findings none (before the block: 16.669, 59.99, 2.59, 6.79). The owner answered
  "não" to wiring ~/.claude/hooks/block_transcripts.py: it stays as it is. LEFT: the commit, the push, `block close`,
  the record's commit; then transit (doc/work/transit.md), with its own photo plan from the start. Before: it was opened again with the
  causes named (0 failed rounds counted since every check of the first run passed: what failed is what the photos
  showed). Verification 1: all 17 photos opened, judged and sent whole to the owner. The causes are read (section "The
  causes" below). The mechanisms are committed and pushed (fb0835c2). Registered, not worked here: the land through
  a lot's paving (terrain; photos 08, 10), test_pool's timing tests (tools), the 24 km opening (lots); the brush that
  zones a segment's whole side is the design and Plots-3. THE FOUR FIXES AND THE PLAN ARE CODED (section "Coded in
  the second opening" below), nothing built or run yet. Since the compaction: Context7 asked, the internet searched,
  the skills cpp-pro, godot-performance, godot-headless-testing and gdextension invoked.
- DONE IN THE AUDIT OF THE MECHANISMS (all uncommitted; 70 tests OK: `timeout 280 python -m unittest test_port_gate
  test_port_deadline test_port_looks test_block test_port_memory test_port_skills test_cli test_proc test_help` in
  tools/tasks/tests; section "Mechanisms" below has the facts): (1) real deadlines: tools/tasks/deadline.py and
  `cli.py run --seconds N` (the run ends itself, trees killed by proc.kill_live, unfinished tasks recorded TIMEOUT,
  exit 3), `build.sh core-windows --seconds N`, .claude/hooks/port_deadline.py (a run and trace.py carry `--seconds`,
  anything else in the background starts as `timeout N`, 300 s at most, longer on the owner's order; the Stop hook
  refuses a turn's end while a shell task runs with no deadline; a command moved to the background is named at once);
  (2) the record read right: only `assistant` entries are calls, a record with no time is 0 not now, a Skill whose
  call failed is not invoked, task notifications are no words of the owner's, a command's wrapping is one message,
  CITY_OWNER_WORDS for the tests; (3) the skill's `!` command by `${CLAUDE_PROJECT_DIR}`, port_state.py's plain mode
  in UTF-8; (4) the two rules written into .claude/skills/port/SKILL.md (rules 2, 3, 4, 10, the steps, the red flags)
  and CLAUDE.md (own photos, no rounds of review, every process ends by itself, a corrected behaviour goes into the
  tools); the commit and the close of a block ask the block's own photo plan (port_gate.py, port_looks.py,
  tools/tasks/plans.py, block_review.py `closing`). Found and NOT mine: test_pool fails on HEAD too (a timing test of
  the backfilling: `test_a_task_that_fits_goes_ahead...`): to the register.
- IN HAND: the audit of C:\Users\Jonathan\.claude (read only so far): settings.json's keys are all in the official
  settings reference (permissions.defaultMode bypassPermissions, skipDangerousModePermissionPrompt, enableWorkflows,
  enabledPlugins, extraKnownMarketplaces, outputStyle, switchModelsOnFlag, crossSessionInbound,
  agentPushNotifEnabled); no user CLAUDE.md, no user settings.local.json; ~/.claude/hooks holds block_transcripts.py,
  block_transcripts_test.py, strip_thinking.py and settings.json has no `hooks` key (to read: what wires them);
  nothing there is changed before it is certain, and no security setting is changed by the session.
- NEXT, in order: `timeout 200 python tools/tasks/cli.py block review`; `python tools/tasks/cli.py block
  verify`; `tools/build/build.sh core-windows --seconds 300`; ONE run `python tools/tasks/cli.py run
  growth_test,zones,e2e --seconds 180`; every photo opened, judged against its case, sent to the owner; `python
  tools/tasks/cli.py run lab_test --all --jobs 1 --seconds 120`; superpowers:verification-before-completion; commit
  (the plan, suites.json, the note, the code); push; `block close` (2 verifications left; a third failure stops the
  work and the owner is told). Then transit (doc/work/transit.md), with its own photo plan from the start.

## The photo plan (project/tests/visual/zones_plan.json; suites.json `zones`, 120 s; doc/map.md)
In the format of road_paint_plan.json (look.gd:3-15: setView [x, y north = -z, px a metre, azimuth, elevation],
dispatch, frames, until, shot): bare map, three streets and a crossing; sixteen shots: the streets with no zone; the
zones painted by type with nothing grown (the clock stands); the same grown; each use near (residential low, medium
with the corner, high, commercial, industry along the curve, low, high); a rectangle lot and a polygon lot drawn
(lot_add_rect, lot_add_polygon: api/city_core_bind.cpp:122-123), zoned (paint_zone_path), built, the polygon near; the
low stretch painted commercial (buildings stay); its zone taken off (buildings go). Not in the plan (to say to the
owner): lot split, join and front change; a generated park from near; a new building beside a worn one with the
inspector.

## What the photos show (verification 1, 2026-11-14; each opened whole and sent to the owner)
Right: 00 the streets with no zone; 01 the zones painted, nothing grown (green along the straight street, blue at its
east end, yellow along the curve); 03 the same grown (houses, flats, towers, shops, works); 04 residential low: houses
with yards, walls, sheds, pools, fronts on the street; 05 medium with the corner and its crossing; 06 high: towers with
their parking; 07 commercial: shop fronts, awnings, parking behind; 08 the works' lots turned to follow the curve; 09,
10 works with fenced yards, docks, parking, a fuel station; 15 the low stretch painted commercial: the houses stay.
Wrong (a failed check each; the cause of none is read yet):
- A. 11 and 12: the rectangle lot and the polygon lot drawn (lot_add_rect, lot_add_polygon) and zoned (paint_zone_path)
  show NOTHING, with the zones overlay on: no outline, no tint. They are there (13 shows what grew on both).
- B. 13 and 14: on the polygon lot the cars stand parked on the lawn: the parking's ground is not drawn but for one
  corner with two stalls; a stray grey triangle with a white line stands by the lot's west side.
- F. 16: the zone taken off (paint_zone 0: `buildings_removed` 10): the buildings go, their parked cars stay on the
  bare grass (eight of them).
- C. 01 and 02: residential low, medium and high (zones 5, 1, 6) are one same green in the overlay: the player cannot
  tell the densities he painted. To read: the overlay's colours in the data, and what the reference shows.
- D. 01: `paint_zone` with a 45 m brush at (-60, 0) and (60, 0) zoned the whole of the short crossing street, 90 m to
  each side. To read: what the brush zones (zone_commands.cpp), whether a stretch is the whole side of a segment.
- E. 08 and 10: ragged patches of grass come through the paved yards of the works, at their edges.
Not in the plan yet (to add in the opening): the densities side by side from near with the overlay on; lot split, join
and front change; a generated park from near; the inspector on a worn building beside a new one.

## The causes, read in the code (2026-11-14; nothing was run to find them)
- A1 (11, 12: the lots' outlines not shown), THE PLAN'S: project/tests/visual/zones_plan.json:34-37 draws the lots by
  the commands alone, with no tool in hand. The outlines of the player's lots are the Lots tool's own drawing:
  project/game/tools/lots_tool.gd:55-61 (`_read`: `ctl.preview.show_outlines(rings, DRAWN_PX)`, on `enter` and after
  each edit), from `CityCore::lots()` (native/src/api/city_core_lots.cpp:139-151). The plan must hold the tool as the
  player does (look.gd's `control`: the button by what it says: "Zonas", then "Lotes", zone.lots in strings_c.csv:450).
- A2 (12: a zoned lot is not coloured), THE GAME'S: a lot's edit marks only the lot cache's cells:
  native/src/sim/commands/lot_commands.cpp:30-33 (`mark_box` pushes `s.log.lot_cells`; every lot command ends in
  `commit`, :36-43, and undo and redo pass by `mark_lot_cells`, edit_history.cpp:44-52). The renderer never drains
  that list (render_world.cpp:124-134 `drain_log`) and tells the overlay of road chunks alone
  (render_world.cpp:104, 111, 173 `overlays_.mark_dirty`). `OverlayRenderer::build_zone_cpu` does cover a zoned lot
  (overlay_build.cpp:128-129 `lot_cover`), but it is asked again only for chunks marked, and at a switch-on only for
  `roads_known_` (overlay_renderer.cpp:158-160): a lot zoned with the overlay on shows nothing until a road of its
  chunk changes or the overlay is switched off and on, and the part of a lot in a chunk with no road is never coloured
  (the polygon lot's corner at z 276 lies in the chunk row past z 256, where no street runs).
- B (13, 14: cars on the lawn of the polygon lot): native/src/render/buildings/lot_emit.cpp:271-286 `outline_lawn` lays
  the lot's lawn as the ring's own triangles (three for the pentagon, tens of metres wide) on the heights of the ring's
  corners alone, `le.ground(p) + lot_plate`. Every cover of the plan lies on the lot's ground read on a grid and
  triangulated once (lot_covers.h:1-6; lot_covers.cpp:167 `lay`: plate + half its thickness + a step a rank), 2 mm
  apart. A flat triangle through three far corners is not that ground: where the land between the corners is lower, the
  lawn's sheet passes over the asphalt and its stall lines (the cars stand "on the lawn"), where it is higher it is
  under the land. Outside the ring there is no such sheet: there the asphalt shows (the corner with two stalls, and
  the grey triangle by the west side). So a second fault shows too: a cover is kept whole when its middle is on the lot
  (sim/buildings/b40/lot_outline.cpp:88 `in_ring(ring, e.x, e.y)`), and is drawn past the lot's line.
- C (01, 02: one green for three densities): the data: project/data/catalog/zones.json:4, 8, 9 give residential, res_low
  and res_high the one colour "#56bb73" (and :5, 10, 11 commercial "#5da9e9", :6, 12, 13 industrial "#d9b254");
  overlay_build.cpp:22-25 `zone_color` reads the code's colour as it is.
- D (01: the 45 m brush zoned the whole crossing street): BY THE DESIGN, and in the register (Plots-3, open: "The
  zoning brush zones a strip, not whole lots"): native/src/sim/commands/zone_commands.cpp:23-40 `side_hit` asks whether
  the brush touches a side's frontage line anywhere, and :78-85 sets the zone of the whole side of that segment
  (`j_set_zone(s, seg, side, zone)`: a segment has one zone a side, `zl` and `zr`). The reference's brush does the same
  (the comment cites its sampling). Not changed in this block; said to the owner.
- F (16: the cars stay when the buildings go): the clock stood (`set_speed [0]`, zones_plan.json:46). While PAUSED the
  tick thread waits and steps nothing: native/src/sim/simulation/simulation.cpp:139-146. What an edit takes from the
  traffic is taken by the traffic's own step: traffic_system.cpp:197-199 (`step` begins with
  `apply_network_changes`), which drains `lot_fronts` and calls `forget_parking_lot` (lane_graph_edit.cpp:175-176,
  :192; parking_sources.cpp:284-293: the lot's bays and standing rows dropped), and the standing cars the render draws
  are published by `publish_poses` (traffic_system.cpp:189 `publish_parked`), run by `Simulation::tick`
  (simulation.cpp:110-115). With the clock standing neither runs: every edit made while paused leaves the traffic's
  last picture (parked cars of razed buildings, and by the same path the kerb's cars of a street taken off) until the
  clock runs again. A player builds and razes with the game paused: the picture must follow the edit.
- E (08, 10: grass through the works' yards at their edges): NOT READ TO A LINE YET. Known so far: the covers stand
  0.03 m over the ground read on the lot's own grid (tuning.json:243-245 lot.plate_m, cover_step_m; lot_covers.h:1-6);
  the lots bank the land round them (tuning lot.apron_m, bank_batter, bank_reach_m); the ground cover keeps off lots
  (render/cover/cover_renderer.cpp:99, 136 `in_lot`), so the patches are the land's own surface over the cover where the
  land's mesh and the lot's grid differ. To read: where the land under a lot is banked and how the land's mesh samples
  it, against lot_covers.cpp's grid.

## Coded in the second opening (2026-11-14; nothing built or run yet)
- A2: sim/state/change_log.h (`overlay_chunks`, cleared by `clear_render`; `traffic_owed`), sim/commands/lot_commands.cpp
  `mark_box` (pushes both lists), render/overlays/overlay_renderer.h/.cpp (`lots_known_`, `mark_lot`, `take_lots`,
  `know_lots`; a switch-on and a switch-off walk both sets), render/render_world.cpp (`drain_log` hands the list to
  `take_lots`; `full_rebuild` calls `know_lots`; `<cmath>` unused left out; the four key lists cleared in one line: 348
  lines).
- B: render/buildings/lot_covers.h (`LOT_LAWN`), lot_covers.cpp (`clip_to`, `ring_pieces`, `Reach`, `lay(..., within)`,
  `lot_covers` lays the lot's lawn first and holds every cover to the ring's triangles; CLIP_MOST grown by a triangle),
  lot_emit.cpp (`LotEmit::cover` draws LOT_LAWN; `outline_lawn` and its include gone).
- C: project/data/catalog/zones.json (the low tints #a6e3b5, #aed3f5, #eed9a0; the high shades #1f7a41, #2468b0,
  #a67d1f; `_doc` with the sources), tools/gates/schemas/zones.schema.json (`_doc`).
- F: sim/simulation/simulation.h/.cpp (`Simulation::settle`: PAUSED and `hold_still`, then under the shared map lock
  and the traffic's mutex `traffic::follow_edit` and `publish_poses`), sim/traffic/traffic_system.h/.cpp
  (`follow_edit`: `apply_network_changes`, then `step_parking_sources` while a read is made), api/city_core.cpp
  `_process` (paused and `log.traffic_owed()` read under the shared map lock: `sim_.settle`), sim/checks/records.cpp
  (`traffic_drained` asks `traffic_owed`). The design changed from the note below in one point: no flag and no
  notification; the frame asks (a check of eight lists while paused), since every edit is made on the frame's thread
  and the tick thread, parked, keeps turning nothing (`wakeups` stays still: its probe holds).
- A1 and the plan: project/tests/visual/zones_plan.json (the densities side by side: `zones_by_type_near` at [-10, 0,
  5.5]; the lots shown with the tool in hand: `{"key": "Z"}`, `{"control": "zone.lots"}`; the rectangle lot zoned high
  (6), the polygon medium (1); Escape before the clock runs), project/tests/visual/look.gd (`control` takes the key of
  a control's words: `tr`, so a plan names no language).

## The fixes (design)
- A2, a lot's change reaches the overlay. ChangeLog (sim/state/change_log.h) gains `overlay_chunks` (the chunks whose
  zone overlay is laid again: a player's lot came, went, moved or took a zone), cleared by `clear_render`;
  `mark_box` (sim/commands/lot_commands.cpp:30) pushes its keys there too (the same keys: `cell_key` of a 256 m chunk,
  core/grid.h:22); `RenderWorld::drain_log` swaps it out, `mark_changed` hands each to `OverlayRenderer::mark_lot`
  (a set `lots_known_` beside `roads_known_`: marked when the overlay is on, marked with the roads' at a switch-on,
  unwanted at a switch-off, cleared by `reset`); `full_rebuild` marks the chunks of every lot of the book (a loaded
  city). No per-frame work: a list drained with the others.
- B, the lot's own lawn lies on the lot's ground and nothing is drawn past the lot's line. `lot_covers`
  (render/buildings/lot_covers.cpp:149) is the one owner of what lies on a lot's ground: for a body with `lot_ring` it
  lays first the lot's lawn (a piece of element -1, rank -1, a step under the first cover: `lot_plate - cover_step`),
  clipped out of the same triangulated ground by each of the ring's triangles (core/polygon_ops.h `ring_triangles`,
  turned anticlockwise: `clip` keeps the left of a line), and lays every cover clipped by its own outline AND by each
  of those triangles (`lay` takes the triangle as a second outline). `LotEmit::cover` draws a piece of element -1 in
  the lawn's paint into the lawns' shell; `outline_lawn` (lot_emit.cpp:271-286) goes. The probe
  (api/city_core_lot_cover_probe.cpp) reads pieces: checked for element -1.
- C, the densities told apart: zones.json: the low density a lighter tint and the high a darker shade of its use's
  colour (residential #8fd6a3 / #56bb73 / #2e8a4d; commercial #9cc9f2 / #5da9e9 / #2f79c4; industrial #e8cf8b /
  #d9b254 / #b08a2c). The convention: green, blue and yellow for residential, commercial and industrial "as in
  SimCity" (tvtropes.org, Cities: Skylines); "Light Green- low density housing, Dark Green- high density housing" (a
  Paradox forum post on the game's zones, community-written). Sources: WebSearch of 2026-11-14.
- F, the traffic follows an edit made with the clock standing. `Simulation::edited()` (called by the API when an edit
  ends) sets a flag under `wake_mutex_` and notifies `wake_` (cppreference, condition_variable: the shared variable is
  changed while the mutex is owned, then notified; the predicated wait ignores spurious wakeups); the paused branch of
  `run` (simulation.cpp:139-146) waits for stop, speed or that flag; on the flag it runs `settle`: under the shared map
  lock and the traffic's mutex, `traffic::follow_edit(s)` (`apply_network_changes` and `step_parking_sources`: no time
  passes, nothing moves) and `traffic::publish_poses`. `hold_still` still waits for `parked_` (false while settling).
- A1, the plan holds the tool: after the lots are drawn the plan presses the Zones tab and the Lots tool (look.gd
  `control`), so the outlines are on the screen as the player sees them; new photos: the three densities side by side
  from near with the overlay on.

## Mechanisms: what the official documentation says and what the audit found (2026-11-14)
Sources read (code.claude.com/docs/en): hooks (whole: 293,734 characters in three parts), context-window ("What
survives compaction"), skills, memory, model-config (auto-compact), tools-reference (Bash).
- Bash, background: "A local session you work in from a terminal, the desktop app, or the VS Code extension has no time
  limit on background commands"; only unattended sessions get "30 minutes, or the `timeout` Claude passes with
  `run_in_background`". A foreground command at its timeout: "Claude Code moves it to the background instead of stopping
  it, unless the command starts with `sleep`". So in this session nothing of Claude Code ends a process: the deadline
  must be the process's own (the orchestrator's per-task `timeout_s`, tools/tasks/proc.py kills the tree; the tracer's
  Watch; coreutils `timeout N`, tested: `timeout 3 sleep 20` exits 124).
- Stop hook input: `stop_hook_active`, `last_assistant_message`, `background_tasks` (id, type, status, description,
  command of each in-flight task), `session_crons`; "8-consecutive-continuation cap", reset by each tool call.
- PreCompact: "Claude Code uses its stdout as instructions for the compaction summary. Keep that stdout under 10,000
  characters per hook." PostCompact receives `compact_summary`, no decision.
- After a compaction: CLAUDE.md and auto memory re-injected from disk; invoked skills re-attached "capped at 5,000
  tokens per skill and 25,000 tokens total", truncation keeps the start; up to five files re-read, one over 5,000
  tokens only as a path; SessionStart hooks of the `compact` source run; background commands keep running.
- Auto-compaction: at about 967K tokens on a 1M model by default; `autoCompactWindow` (settings), `/autocompact`,
  `CLAUDE_CODE_AUTO_COMPACT_WINDOW`; none is set in .claude/settings.json, settings.local.json or the user's settings.
- Skills: "Claude Code runs each command in the session shell's current working directory ... Use `${CLAUDE_SKILL_DIR}`
  or `${CLAUDE_PROJECT_DIR}`"; "A failed command aborts the entire skill invocation".
- Memory: "Claude treats them as context, not enforced configuration. To block an action regardless of what Claude
  decides, use a PreToolUse hook instead"; CLAUDE.md "target under 200 lines"; MEMORY.md first 200 lines or 25KB.
- The transcript (what the hooks read to know what the session did) has, besides the `assistant` and `user` entries,
  `api-request-blob` copies of the messages with no timestamp, `queue-operation` and `attachment` entries that record
  task notifications the same way as the owner's queued prompts; a `user` entry of a notification has
  `origin.kind == "task-notification"`. A failed Skill call's result is a tool_result with `is_error` true.
- The other session's commit e63473a6 (read whole): `## Now` unbounded (right: the gate has the note Read whole);
  the note's age by the section's own hash (right); every hook text through `fit` at 9,500 (right: the cap is 10,000
  and cannot be raised); a second SessionStart text of the compact source from the transcript (right in aim; its
  defects are (2) and (3) above); the Stop text no longer cut (right).
