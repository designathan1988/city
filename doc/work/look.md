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
- Also: "PORRA, NÃO TÁ VENDO QUE SOME ELEMENTOS NO NIVEL DISTANTE? ... OLHA ESSAS IMAGENS COM ATENÇÃO"; "NÃO DEMORE
  NISSO, RESOLVA LOGO E VÁ PARA A PROXIMA FUNCIONALIDADE ... RESOLVA NO CODIGO, NADA DE FICAR INVESTIGANDO COM TESTE,
  MEDIÇÃO".
- The guards are committed and pushed (6cee0a3e). The block `look`: verification 1 of 3 FAILED (`city` alone),
  opened again with the causes, both fixed (the section "The far lots" below). Verification 2 of 3: the one run
  passed 7 of 7 (far_level_test, city, zoom_ladder, e2e and what they stand on); the town's far level holds 148,172
  triangles (94,682 before the lots; 389,464 at verification 1), 19.1 MB of arrays (12.9 before); the six photos and
  the five references were opened whole: 00, 01, 02 show what their references show (windows, awnings, roof plant,
  lane lines, crossings, cars, park paths); 03, 04, 05 (the far level) now show each lot's walls and paved covers
  beside the houses, the towers their window grids, the roads plain past the detail cut-off as before. lab_test
  passed: frame 16.65 ms (60.05 fps), CPU 2.54 ms, GPU 6.51 ms, 1 hitch in 177 frames (before the lots: CPU 2.38,
  GPU 6.34). Prender-285 and Ptools-132 closed in the register. The far photos 03, 04, 05 are kept as references
  (doc/look/zoom_ladder). To do now: commit, push, `block close`, the owner told; then roads.
- WHAT VANISHES FROM FAR, seen by enlarging the same photos for my own reading (the towers DO keep their window grids
  at the far level: my "no window grid" was wrong, the shrunk picture hid them): past building_mid_end_m every LOT
  loses what lies and stands on it: its paved covers (driveways, paths, patios, parking, courts), its walls and
  hedges, its pools; the lots are bare grass with the houses on it. On 7 November the far view showed them.
- CAUSE: native/src/render/buildings/building_renderer_build.cpp:277 (build_super_cpu): the far level of a superchunk
  is `emit_far(far, pl, true)` alone; the lot is laid by the near level (:176) and the mid level (:194) only, so it
  goes when a chunk is handed to the far mesh (building_renderer_view.cpp:43-76). The same at the revert (dcfd4c7c
  building_renderer_build.cpp:270): the photo of 7 November drew those chunks at their mid level.
- THE FIX (the far level lays the lot, as its nearest distance shows it; nothing run while it is coded):
  1. building_renderer_build.cpp build_super_cpu: after emit_far, when lots are drawn: `pl.coarse = true;
     emit_lot(far, far, pl)`, inside the vertex range whose COLOR.a gets the chunk's bits (so chunk_mask folds the
     lot away where the chunk is drawn in detail). The far shell's own rules hold: a box under box_least is not
     written (a fence's rails, a pool's rim), a face narrower than quad_least neither (a wall's top and ends).
  2. lot_emit.cpp LotEmit::cover: at the far level (pl.lean) a lawn is left to the land's grass under it (the far
     shader has no grass; the lawn IS the land's grass); a paved cover's piece is raised by its rank x
     (shell.quad_least / FAR_COVER_RANKS): covers lie 2 mm apart (tuning lot.cover_step_m), which the depth buffer
     does not hold kilometres away. Godot's docs, 3D rendering limitations, Depth buffer precision: "you may also be
     able to move the Z-fighting objects further apart without the difference being visible to the player."
  3. lot_emit.cpp LotEmit::element: at the far level a wall or hedge on ground level within the far error
     (shell.quad_least) is one piece, not one every 4 m.
  4. shell.cpp Shell::face: a face lying on the lot (SK_GROUND) is a piece of a cover, never left out by its own
     narrowness (a clipped piece's short side is not the cover's width).
  5. building_renderer_commit.cpp set_lots_drawn: the far meshes are made again too (FarDraw dirty, due now).
  6. building_renderer_probe.cpp far_form: thin_quads does not count SK_GROUND faces; far_level_test.gd's words.
- Next, in order: `block verify` (the second of three); the build; one run `far_level_test,city,zoom_ladder,e2e`;
  each photo opened whole and compared by eye with doc/look; lab_test; superpowers:verification-before-completion;
  commit, push, `block close`; the owner shown the same far view before and after, whole, with the cost as it is;
  then the board's next line, roads (doc/work/t015-roads.md; its note doc/work/roads.md first).
- The one look with no block open is NOT spent (dist/block/looks.json absent). The worktree dist/before/dcfd4c7c
  stands unused: removing it is a deletion, left for the owner's word.

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

## The far lots (2026-11-14)
- Verification 1 (far_level_test, zoom_ladder, e2e passed; `city` failed): (a) the far photo shows the lots again
  (walls, driveways, patios), the same kinds of things as on 7 November; (b) `city`: "12_square came out without
  the building's near level": capture.gd:215 asked the near level; the chunk's flags were 62 with nothing pending;
  the near level is asked only when the chunk's middle is within near_reach() of the eye
  (building_renderer_view.cpp:50-54, building_renderer_reach.cpp:127): the check's premise was wrong, it now asks
  the chunk shown in its own detail (CHUNK_SHOWN); (c) the cost: the town's far level went from 94,682 to 389,464
  triangles (12.9 to 53.6 MB of arrays; boxes held 2,447 to 11,295): about 260,000 the covers, clipped cell by cell
  of a 3 m ground grid (lot_covers.cpp:111-135), 35,000 the walls: too heavy for the large city.
- Fixed in the second opening: at the far level a cover is its own outline with each corner on the ground
  (lot_covers.cpp lay; no ground grid read), and a cover narrower than the least width still drawn where the level
  begins (shell.box_least, 0.96 m) is not laid (lot_emit.cpp cover). By the design: about 95,000 + 35,000 + 15,000.
- Not measured: the 24 km city (km24 runs on the owner's order). The far level there held 15.7 M triangles
  (71b78d05's message); the lots add to it in the town's proportion.

- Cause named at `block open` (2026-11-13T23:14 round 1: city: project/tests/visual/capture.gd:215: the civic shot asks the near level of the square's chunk, whose middle stands past near_reach() from the eye (building_renderer_view.cpp:54), so with nothing pending the game rightly draws its mid level (flags 62) and the check calls a whole photo incomplete; and found in far_level_test's own line: native/src/render/buildings/lot_covers.cpp:113: on ground not level to 0.1 mm each cover is clipped cell by cell of a 3 m grid, so the far level went from 94,682 to 389,464 triangles in the town where its distance shows a cover's outline alone)
