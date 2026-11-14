# look: the game's look from far, worse than on 7 November; the photos' wait; the guard

## Now
- The owner's words (2026-11-13, night): "O JOGO NÃO ESTAVA ASSIM!!! CADE OS DETALHES DE TUDO? OS PRÉDIOS ESTAVAM SUPER
  BEM FEITOS"; "OLHA COMO ERA O JOGO ANTES EM DISTANCIA ATÉ MAIOR"; "NÃO QUERO SABER DE PIXEL DIFERENTE, SERÁ QUE VC
  CONSEGUE OLHAR A IMAGEM E COMPREENDER"; "É SÓ OLHAR NO CODIGO. VOCE TIROU FOTO QUANDO A CIDADE JÁ TINHA CARREGADO OU
  NÃO? ... CONFIRA SE FOI ISSO. SE FOI CONSERTE E GARANTA QUE NÃO ACONTEÇA MAIS NO SCRIPT DE FOTOS. E JÁ É ALGO A
  VERIFICAR, PORQUE SE DEMOROU É PQ O DESEMPENHO ESTÁ PIOR"; "OLHA COM ATENÇÃO AS IMAGENS A PARTIR DE AGORA! ... EU MANDEI
  VOCÊ FAZER AS IMPLEMENTAÇÕES SEMPRE OLHANDO AS FOTOS PARA NÃO QUEBRAR NADA"; "CONTINUA OS TRABALHOS, SEM MAIS PERDA DE
  TEMPO". Standing: Portuguese; no stopping to ask (the /goal); no pixel counts to the owner; whole photos, never a
  magnified crop alone; never "it is the same" without the same view looked at; no old tree run to photograph (the
  owner stopped it): the code is read.
- Answered, from the runs' own logs: the photos were NOT taken before the game loaded: at every shot `pending=0 want=0
  unmet=0 asked=0 done=0` (zoom_ladder: ...\tasks\baw48367z.output:2119-2148; capture `city`: ...\tasks\be9itmde0.output
  :2059-2171: the town drawn in 1133 ms at the opening, each view in 33 ms). No slow loading in those runs.
- A hole found on the way, NOT fixed yet: the capture `city` marked "12_square came out without the building's near
  level" (project/tests/visual/capture.gd:216) with pending at 0: the game says done and a photo lacks the level it
  should show. To read: capture.gd:160-220 (JUMP_FRAMES, the civic shots), capture_wait.gd until_drawn (settle frames,
  then render_pending 0), near_reach() (building_renderer_reach.cpp) against where those shots stand; then the fix in
  the photos' wait (and in look.gd's shot), so no photo leaves with a level of its view missing.
- The far look, open, registered (the register's newest render problem: "from far the buildings are pale flat blocks"):
  past building_mid_end_m (1100 m, measured with the eye's height: building_renderer_view.cpp:51-52) the whole town is
  drawn by the far level: towers with no window grid, washed colours. Same switch, same distances, same far shader and
  window filter (building_lamps.gdshaderinc:62-71 pulse) as at the revert dcfd4c7c. After the revert the far level was
  changed by 71b78d05 (its message: no coping, a pitched roof as its planes, a roof's pieces as boxes kept by size, no
  small four-cornered face; "far 31.9 M -> 15.7 M triangles"), then 2ca388ac, fe8811b7, eca6c6de, e8174734. Which took
  the look: not found by reading. References kept: doc/look/far_town_2026-11-07.png, mid_town_2026-11-07.png,
  doc/look/zoom_ladder/00..02 (near and middle as they are now, good).
- The guard, written, tests pass (12), NOT committed yet: .claude/hooks/port_gate.py refuses a block's commit unless the
  verification's run photographed the plan `zoom_ladder` (LOOK, LOOKED), and says to open its six photos whole and tell
  what buildings, roads, vehicles, ground and sky show against doc/look; tools/tasks/tests/test_port_gate.py.
  To write beside it: the same rule in CLAUDE.md and the skill `port` (step 8b), doc/map.md for doc/look.
- Next, in order: (1) CLAUDE.md, the skill, doc/map.md; commit and push the guard, doc/look, the register, this note;
  (2) the photos' wait: read as above, a block `look`, fixed, one run (city, zoom_ladder, e2e), its photos looked at;
  (3) the far level: far_mass.cpp, shell.cpp, roof_pitched.cpp, roof_plant.cpp read whole against `git show 71b78d05`
  and the four commits after it, the look given back in the code, the same views looked at against doc/look; (4) the
  board's next line, roads (doc/work/t015-roads.md, sheets/roads.md; its note doc/work/roads.md before its code).
- The worktree dist/before/dcfd4c7c stands unused: `git worktree remove dist/before/dcfd4c7c` is a deletion, left for
  the owner's word.

## The record of this evening (2026-11-13)
- The far cars' proof picture sent to the owner was a crop magnified 4 times of a view from 2100 m (0.8 px a metre, past
  the medium tier's detail cut-off 0.85): the owner took it for the game's look. The plan zoom_ladder
  (project/tests/visual/zoom_ladder_plan.json, commit 0e189a7a) photographs one place at 6, 3, 1.5, 1, 0.8 and 0.5 px a
  metre: full detail to 1.5; at 1 and under, the towers are flat pale blocks.
- Same capture scene `city` (tests/visual/capture.tscn, 1600x900, tier `medium` then and now), 7 November 20:27
  (dist/captures/antes/city_1107) against now (dist/captures/city): up to the middle distance the towers keep their
  window grids and colours, and the town's buildings are not the same ones (the generation changed); past 1100 m the
  towers had dark window grids and strong colours then and are pale flat blocks now.
- Not the cause: the traffic's work of this session (no building or atmosphere file changed, git diff ef7e16f9..HEAD;
  the plan `roofs` at 22:20 looks as its photos of 03:38). Between the revert dcfd4c7c (2026-11-10 08:07, "back to
  a1a771c4 (2026-11-07 20:29), by the owner's order") and 03:38: 378 commits, about 110 on the render, its data, the
  town's generation or the buildings.
- Facts: look runs use settings.json:4 `tests_preset` "medium" (the same at dcfd4c7c); detail_cutoff_zoom 0.85 for it
  (low 1.25, high 0.65, ultra 0.5; the same since 11-04): under it the camera draws no furniture, people, vehicles nor
  road paint (project/game/settings/render_switches.gd:61-64, :100, :113-121). RenderWorld::pending
  (render_world.h:185-189) sums every renderer's; the buildings' (building_renderer_probe.cpp:65-68) counts what the
  view wants and has not got, the far meshes waiting on their clock too (building_renderer_view.cpp:93-103); a chunk's
  mid level is drawn at any distance while its superchunk has no far mesh (:67). The far walls carry their grid and
  opening (far_mass.cpp:77-84, :203-214; shell.cpp:265-288; building_renderer_build.cpp:265-270, CUSTOM0).
