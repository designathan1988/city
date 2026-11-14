# look: the game's look from far, worse than on 7 November; the photos' wait; the guards

## Now
- The owner's words (2026-11-13/14): "O JOGO NÃO ESTAVA ASSIM!!! CADE OS DETALHES DE TUDO? OS PRÉDIOS ESTAVAM SUPER
  BEM FEITOS"; "OLHA COMO ERA O JOGO ANTES EM DISTANCIA ATÉ MAIOR"; "NÃO QUERO SABER DE PIXEL DIFERENTE, SERÁ QUE VC
  CONSEGUE OLHAR A IMAGEM E COMPREENDER"; "VÁ PARA O CODIGO E RESOLVA ... CHEGA DE TIRAR FOTOS"; "É SÓ OLHAR NO
  CODIGO"; "OLHA COM ATENÇÃO AS IMAGENS A PARTIR DE AGORA!"; "VOCÊ ESTÁ FAZENDO TRAVAS, HOOKS, REGRAS E UM MONTE DE
  COISAS QUE SEQUER MOSTRARAM FUNCIONAR UMA ÚNICA VEZ NESSSA SESSÃO. VERIFICA ISSO, PORRA! E CORRIJA"; "SEQUER VOCE
  TESTA DE PONTA A PONTA PARA GARANTIR QUE FUNCIONA"; "CHEGA DE DEMORA! CHEGA DE TRABALHO MAL FEITO! FAÇA O SEU
  TRABALHO DIREITO!". Standing: Portuguese; no stopping to ask (the /goal); no pixel counts to the owner; whole
  photos, never a magnified crop alone; never "it is the same" without the same view looked at; no old tree run to
  photograph; the code is read; no more time lost.
- No block is open. The guards were checked and mended (2026-11-14, the section "The guards, checked" below): to
  commit and push now (the hooks, tools/tasks, CLAUDE.md, the skill, doc/map.md, this note).
- The far look, open, registered ("from far the buildings are pale flat blocks"): past building_mid_end_m (1100 m
  from the eye, the eye's height counted: building_renderer_view.cpp:51-52) the whole town is drawn by the far level:
  towers with no window grid, washed colours; on 7 November (same capture `city`, same tier) the far towers had dark
  window grids and strong colours. The same at the revert dcfd4c7c and now (read): the level switch and distances,
  the far shader by day (building_far.gdshader:43-91), the window filter (building_lamps.gdshaderinc:62-76 pulse), the
  far material's values, the far tones (buildings.json finish_far), the tier and the cut-off. Changed since, by their
  diffs: the far mesh (71b78d05: no coping, a pitched roof as its planes, roof pieces as boxes, quad_least/box_least;
  then 2ca388ac, fe8811b7, eca6c6de, e8174734), the levels' hand-over (9419745b, 8bfeea46), the daylight (cascades to
  project/render/sun_cascades.gd, values through project/render/sent_params.gd, `_sun.shadow_enabled = shadows_on`
  with the moon's `shadow_opacity` lines gone, `horizon_linear`), render_method.gd (ea76da80), palette.json, the
  town's generation. The air by the eye's height: daylight.gd:149-162 (`density = fog_density * air_height_m / y *
  (1 - exp(-y / air_height_m))`, `fog_light_color = air`), city_world.gd:170-172 (fog on), sky_view.gd:73-74,
  :150-155. sun_cascades.gd read whole (the cascades end where the picture ends on the ground: no shadow past it).
  Not read yet: sent_params.gd, render_method.gd, the sky shader's diff, whether the air's values changed, and
  far_mass.cpp, shell.cpp, roof_pitched.cpp, roof_plant.cpp against `git show 71b78d05` and the four after it.
- Next, in order: (1) commit and push the guards; (2) the far look: the files above read until the cause is a
  file:line (superpowers:systematic-debugging), Context7 and the internet, `block open look`, the fix in the code,
  `block review`, `block verify`, one run (`city,zoom_ladder,e2e`), each photo opened whole and compared by eye with
  doc/look (far_town_2026-11-07.png, mid_town_2026-11-07.png, zoom_ladder/00..02), the owner shown the same far view
  before and after; (3) the board's next line, roads (doc/work/t015-roads.md, sheets/roads.md; its note
  doc/work/roads.md before its code).
- The one look with no block open is NOT spent yet (dist/block/looks.json absent). The worktree
  dist/before/dcfd4c7c stands unused: removing it is a deletion, left for the owner's word.

## The guards, checked (2026-11-14)
- What fired by itself in this session, counted in its own record (the hook's answers only): a script typed into the
  shell 2, a muted run 1, a note left unfit 5, the game's code before the skills 2, after a compaction before the
  records 3, the gates after an edit 86, the note's checkpoint 4, the state at a session's start 7.
- What had never fired, provoked on purpose through the real hook, each refused: the engine started outside the tools
  (`godot --version`), a project file written by the shell (`Add-Content project/...`), a run joined to another
  command, a run in the foreground, the game's code with no block open (Write project/zz_guard_probe.txt), an agent
  while porting; and, when port_gate.py did not load in the middle of its own edit, the hook said so at once.
- Holes found by reading, mended:
  1. tools/tasks/block.py refusal: with no block open every run was free, any number of times (the evening's photos
     one after another went through it). Now .claude/hooks/port_looks.py `held`: one run of six tasks at most and one
     measure between a block's close and the next opening (dist/block/looks.json, by the state file's time); a suite,
     a group, km24.py, hunt.py, quick.sh only on the owner's order. It binds the session, not the owner's own hand.
  2. `--owner "<words>"` took any words. Now `unsaid`: the words are looked for in what the owner wrote in the
     session (queue-operation `.content`, attachment `.attachment.prompt`, a user's entry that is no tool result, no
     summary, no meta); `python tools/tasks/cli.py block order "<words>"` (block_review.py `order`) writes
     dist/block/order.json and one run spends it.
  3. A block's commit asked only that zoom_ladder passed. Now `unseen`: each photo of dist/captures/zoom_ladder Read
     after it was taken, and each reference of doc/look Read, since the session's start or last compaction.
  4. After a compaction with no block open nothing was asked. Now port_gate.py `ungrounded`: the skill `port` and a
     note of doc/work Read whole (no offset, no limit) before any edit or command, whatever the block.
  5. A block could close with no check run (a way to reset the look). Now block.py `enter`: close asks this
     verification's run of checks.
- Proof: tools/tasks/tests: 143 pass (test_port_looks.py WholeLife: a block's whole life asked of the hook's real
  entry in a project of its own, the block moved by the real cli.py block); provoked through the real hook after
  the mend: an order the owner never wrote, km24.py, `run tests` with no block: each refused.
- port_gate.py was over 300 lines: the edit's soundness and the gates after an edit moved to
  .claude/hooks/port_edits.py.

## The record of the evening (2026-11-13)
- Answered, from the runs' own logs: the photos were NOT taken before the game loaded: at every shot `pending=0
  want=0 unmet=0 asked=0 done=0` (zoom_ladder: ...\tasks\baw48367z.output:2119-2148; capture `city`:
  ...\tasks\be9itmde0.output:2059-2171: the town drawn in 1133 ms at the opening, each view in 33 ms).
- "12_square came out without the building's near level" (capture `city`): its chunk's flags were 62 (mid built,
  shown, in detail, far mesh there, near not built: building_renderer_probe.cpp:78-88); a near level is asked only
  within near_reach() of the chunk's middle (building_renderer_view.cpp:54; building_renderer_reach.cpp:127), and
  the civic shot stands at 1.1 lot sizes from the lot's middle (capture.gd:29, :211): past the near reach for the
  square's big lot. The capture's check (capture.gd:215) asks a level that view never gets: registered, not worked.
- The far cars' proof picture sent to the owner was a crop magnified 4 times of a view from 2100 m (0.8 px a metre,
  past the medium tier's detail cut-off 0.85): the owner took it for the game's look. The plan zoom_ladder
  (project/tests/visual/zoom_ladder_plan.json, commit 0e189a7a) photographs one place at 6, 3, 1.5, 1, 0.8 and 0.5 px
  a metre: full detail to 1.5; at 1 and under, the towers are flat pale blocks.
- Same capture scene `city` (tests/visual/capture.tscn, 1600x900, tier `medium` then and now), 7 November 20:27
  (dist/captures/antes/city_1107) against now (dist/captures/city): up to the middle distance the towers keep their
  window grids and colours, and the town's buildings are not the same ones (the generation changed); past 1100 m the
  towers had dark window grids and strong colours then and are pale flat blocks now.
- Not the cause: the traffic's work of this session (no building or atmosphere file changed, git diff
  ef7e16f9..HEAD; the plan `roofs` at 22:20 looks as its photos of 03:38). Between the revert dcfd4c7c (2026-11-10
  08:07, "back to a1a771c4 (2026-11-07 20:29), by the owner's order") and 03:38: 378 commits, about 110 on the
  render, its data, the town's generation or the buildings.
- Facts: look runs use settings.json:4 `tests_preset` "medium" (the same at dcfd4c7c); detail_cutoff_zoom 0.85 for
  it (low 1.25, high 0.65, ultra 0.5; the same since 11-04): under it the camera draws no furniture, people,
  vehicles nor road paint (project/game/settings/render_switches.gd:61-64, :100, :113-121). RenderWorld::pending
  (render_world.h:185-189) sums every renderer's; the buildings' (building_renderer_probe.cpp:65-68) counts what the
  view wants and has not got, the far meshes waiting on their clock too (building_renderer_view.cpp:93-103); a
  chunk's mid level is drawn at any distance while its superchunk has no far mesh (:67). The far walls carry their
  grid and opening (far_mass.cpp:77-84, :203-214; shell.cpp:265-288; building_renderer_build.cpp:265-270, CUSTOM0).
- The guard of the look (78feed43): .claude/hooks/port_gate.py refuses a block's commit unless the verification's
  run photographed the plan `zoom_ladder`; the same rule in CLAUDE.md and the skill `port` (step 8b); doc/look kept.
