# zones-lots-growth: zones, lots and growth, ported in one block (doc/port.md, section 4)

## Now
- The owner's standing orders (2026-11-13/14, their words): "FAÇA O PRÓXIMO, FAÇA O QUE É NECESSÁRIO PARA ESSE JOGO
  SER FEITO DE UMA VEZ POR TODAS, SEM MAIS ENROLAÇÃO OU DESVIOS"; a functionality is whole ("É A PORRA DA
  FUNCIONALDIADE INTEITA"); "RESOLVA NO CODIGO, NADA DE FICAR INVESTIGANDO COM TESTE, MEDIÇÃO"; "OLHA COM ATENÇÃO AS
  IMAGENS A PARTIR DE AGORA"; "CHEGA DE DEMORA! CHEGA DE TRABALHO MAL FEITO!". The /goal of 2026-11-14 (their
  words): "Depois de fechar, commit e push, siga na hora para a próxima linha desmarcada de doc/port.md, e assim por
  diante, até portar tudo ... ler os dois jogos, fontes, nota, `block open`, codificar tudo sem rodar nada, ler o
  diff, `block review`, `block verify`, build, UMA execução pequena com `zoom_ladder`, fotos abertas e olhadas
  contra doc/look, UMA medida, commit, push, `block close` ... NÃO PARE PARA ME PERGUNTAR NADA ... Só pare se uma
  terceira verificação de um bloco falhar". Standing: Portuguese to the owner; no agents; defects found by reading;
  one small verification; whole photos.
- THE OWNER'S ADDENDUM (2026-11-14, their words): "NADA FICA DE FORA POR CUSTO; O MELHOR VENCE; TUDO SE DECIDE
  PELO CÓDIGO ... QUAL É MELHOR SE DECIDE LENDO O CÓDIGO DOS DOIS, NUNCA RODANDO ... Escreva na nota a comparação
  com file:line dos dois lados e o porquê da escolha ... ANTES de `block review`, em todo bloco: releia cada caso,
  item por item, e escreva na nota o file:line onde cada item está feito".
- THE OWNER'S ORDER OF 2026-11-14, LATEST AND FIRST (their words): "TODO PROCESSO QUE ENVIAR DEVE DURAR NO MÁXIMO
  SEGUNDOS, COM MONITORAMENTO, REAL E SAÍDA. JAMAIS UM PROCESSO DEVE FICAR PERPETUAMENTE ... AO CODIFICAR, NÃO FAÇA
  MAIS REVISÃO, RE-REVISÃO, ETC. NEM TIRE MAIS FOTOS REPETITIVAS DA MESMA VISTA DE LONGE. CADA ETAPA TEM AS SUAS
  FOTOS PROPRIAS DE SEGUNDOS E TESTES DE SEGUNDOS. CODIFIQUE, RODE UM SCRIPT DE SEGUNDOS PARA TESTAR SE DEU CERTO,
  RODE O SCRIPT DE SEGUNDOS PARA TIRAR FOTOS E ANALISE. SE TUDO DEU CERTO VÁ PRA A PROXIMA FUNCIONALIDADE ... O
  PROCESSO É: RASTREAMENTO DE FUNCIONADADE DOS DOIS JOGOS EM SEGUNDOS, ANALISE E COMPREENSÃO, CODIFICAÇÃO, TESTE E
  FOTOS COM PROCESSOS DE SEGUNDOS E TUDO COM MONITORAMENTO E JAMAIS PROCESSOS QUE FIQUEM PERPETUAMENTE, ANALISE,
  CORREÇÃO SE PRECISAR NO CODIGO, PROXIMA FUNCIONALIDADE ... NADA DEVE FICAR ABERTO OU SER IGNORADO". Why: the
  tracer below sat 7 hours with no deadline. Every process from here: a deadline of seconds, its output read
  when it ends, stopped by its ID when it passes the deadline.
- State (2026-11-14): NO BLOCK OPEN. The block before (junctions) closed: commit 49500272, pushed
  (doc/work/junctions.md). The tracer's sheet (`python tools/tracer/port.py sheet zones-lots-growth`, task
  b42jrh624) printed "reference: 3 folder(s)" and nothing more for 7 hours: STOPPED by its ID. Its cause, read:
  tools/tracer/port.py (was :126) called `analyse` with no ceiling (tracer/limits.py:13-15: no limit unless
  asked) and no `say` (no step printed), and `roots_of` (was :118-121) took the whole folder of every cited file:
  the sheet cites main.ts, whose folder is the reference's whole `src`. FIXED in port.py: a `Watch` (a deadline of
  60 s for the whole run, `--seconds`; each step said with the time; the run ended with exit 3 past the deadline,
  saying its step), each side's analysis a third of the time (partial is said), a file whose folder holds another
  root is a root alone; tests test_limits.PortDriver 3/3 in 0.4 s. THE TRACE RAN IN 33 s: the reference whole in
  9.1 s (49 files), this game partial (clang: 61 files left at its share); report dist/port/zones-lots-growth.json
  (thin: the sheet's evidence names files of the same name in several folders, said "not found"). The reading of
  both games' code is the source. The one look with no block open is not spent.
- The cases (doc/port.md; doc/specs/003-roadcraft-features/sheets/zones-lots-growth.md has each with its
  evidence): 1. growth's order and pace; 2. the 24 km city opens fast (research.md R4; `window_traffic_test`
  under 60 s with the same check); 3. rezoning keeps the building; 4. lots cut to the land and kept;
  5. buildings that age into decay; 6. a polygon lot's building fits its outline, set-backs and lower densities
  tried, eras by quarter; 7. industrial densities, the generator's parks.
- NEXT, in order: the tracer's report read; this game's code read whole (below, "Read": what is still to
  read); the reference's (world/zoning.ts, world/lots.ts, world/growth.ts, decayWatch.ts, plan.ts, zoneGrid.ts,
  growthFeed.ts, main.ts:2656-2667); research.md R4 read; the sources (the internet) for each real value;
  each case's design written here with the comparison of both sides by file:line; then `block open
  zones-lots-growth` and the coding.

## Read (facts with file:line)
- native/src/sim/simulation/city_growth.cpp (read whole, 116 lines; its header names the reference's
  functions: to be this game's own where the block changes it): nearest_free :34-56 walks EVERY lot of EVERY
  queued cell, a building query a lot (lot_occupied), and takes the free lot nearest the last one grown: no
  reach, no random jump, no lot refused and kept waiting (case 1; the cost the sheet names: every building
  scans the whole queue); decide :59-69 (record_for, `built` = the clock's minutes); grow_step :82-114: owes
  dt x tune sim_growth_per_s, at most MAX_OWED 50, sim_growth_tries a step; decides under the shared map lock,
  adds under the writer's (void when the revision moved); settle_sites after each; a blow's rubble holds
  growth (damage.growth_hold_until :90).
- native/src/sim/buildings/growth.cpp (read whole, 200 lines; "Port of Roadcraft src/world/growth.ts"):
  base_zone / zone_density :34-35 (zones 5, 6 residential low and high; 7, 8 industrial low and high);
  civic_for :54-69 (the cell's biggest lot, by a hash of the cell); generated_age :71-75 (a hash of the seed
  over decay_span_min / 10: the generated city's ages; nothing reads `built` to decay: case 5);
  lot_occupied :77-94 (a ruin; a building's middle within 0.8 of the half sizes; a square over the lot);
  best_candidate :134-149: with neighbours within LOOK_REACH_M, LOOK_CANDIDATES complete bodies drawn in
  parallel (jobs->parallel_for) and the least like the neighbours kept (research R4: a candidate's look from
  its massing instead); record_for :152-180 (civic, the tallest within organic_reach_m gives the cap, the
  lot's edges read, the candidate's seed and storeys); fill_all :182-198: every cell's lots in key order,
  SERIAL, each lot's candidates compared against what the lots before it built (case 2: the 24 km opening).
- NOTE: growth.cpp is native/src/sim/simulation/growth.cpp (not sim/buildings); its header sim/buildings/growth.h.
- native/src/sim/commands/zone_commands.cpp (117 lines, whole): paint_zone :65-94 sets a side's zone and, where the
  side had one, clear_side :44-62 knocks down every building facing it whose BASE zone differs (case 3: the
  reference keeps them); paint_zone_path :96-115 zones a player's lot under the dab (zone_lot) else the sides.
- native/src/sim/commands/lot_commands.cpp (253, whole): zone_lot :234-251 razes what stands on the lot when it
  had a zone (case 3); free_of_lots :51-55 walks every lot of the book (the sheet's fault: LotIndex has the
  place hash, lot_book.h:45-64); add_lot :80-91 (lot_on_land cuts the drawn ring to the land).
- native/src/sim/lots/lot_cache.cpp (102, whole) and .h: by_cell recomputed for each dirty cell from the streets
  (lots_of_cell :42-72, build_lots) and never saved (lot_cache.h:1-2); with_player_lots :23-38: a zoned lot
  of the player's joins the cell as its STAND, a rectangle (LotStand, lot_book.h:24-26): case 6's "the biggest
  rectangle only"; lot_gen.h:32-36 `Lot` is x, z, w, d, rot, zone, profile, seed: a rectangle (case 4).
- project/data/catalog/zones.json: codes 0-8; 5, 6 residential low/high, 7, 8 COMMERCIAL low/high in the data
  while growth.cpp:23,34-35 reads 7, 8 as INDUSTRIAL low/high ("base 2"): the two disagree (case 7; to settle
  by city_state's ZONE_ names). growth.json: plan (no parks key), civic, organic, decay_grace_min 7200,
  decay_span_min 43200 (5 and 30 days, the reference's types.ts:454-456).
- native/src/sim/citygen/city_plan.cpp (57, whole): zone_at :37-55: industry a sector past industry_from,
  ONE code (ZONE_IND) whatever the distance; no park branch (case 7).
- THE REFERENCE, read: world/growth/zoning.ts (618, whole): pickLot :243-253 = the open lot nearest the last
  grown within GROW_ON_REACH 70 m (:88, nearestLot :232-240, nearer by a millimetre at least, ties by the
  list's order), else a lot drawn at random by a stream seeded by the next building's id (lotStream
  :226-229); growOnLot :461-468 sets a lot nothing fits aside (`refused`); designOnLot :406-450 = for each
  density (the lot's own, then each lower, densitiesOf :403) x 6 TRIES (:159-162: front 0/1.5/3/3/4/4 m, side
  0/0/1/1/1.5/1.5 m, shrink 1/1/0.9/0.75/0.6/0.45) a body made and checked on the site; 4 CANDIDATES a try
  (:82, :317-328); quarterOf :143-156: `character` one hash per 60 m cell and facing, `era` one hash per 220 m
  cell: within 450 m of the map's middle old 0.5 / middle 0.3 / new 0.2, outside 0.15 / 0.35 / 0.5;
  fitToLot :530-618: a lot that is not a rectangle (4 corners filling 97% of its box) gets a lawn of the lot's
  own outline, the plan's surfaces clipped to the lot (the largest piece of the intersection), the boundary
  run along the lot's own sides but the front in pieces of 20 m at most, what falls outside left out, a
  building's block kept where a corner or its middle is inside. main.ts:2656-2668: one building each 0.5
  simulated seconds, none while dragging nor under the rubble hold. world/buildings/types.ts:453-469:
  DECAY_GRACE 5 days, DECAY_SPAN 30 days, generatedBuiltAt (a hash of the id over a tenth of the span), decayOf
  = round(clamp((now - builtAt - grace) / span, 0, 1) x 10) / 10. decayWatch.ts (65, whole): a heap of each
  building's next tenth (:19-24: builtAt + grace + span x (decay x 10 + 0.5) / 10), fed by the journal's
  changed ids (:49), popped while due (:52-62): `wear(id, decay)`; never a walk of the town but after a load.
  world/cityGen/plan.ts:137-154: parks = a coarse noise (140 m cells, salted by the seed) over 1 - parks x 9
  where k > 0.15 (parks 0 to 0.2, default 0); industry past 0.62 in a sector of 0.55 rad: HIGH past k 0.85,
  else MEDIUM. world/lots.ts:1254-1272 zoneLots: a new zone keeps the lot's building; the zone taken off
  removes it.
- More of this game, read: sim/simulation/growth.cpp:23,34-35: the constants named IND_LOW/IND_HIGH are the
  codes 7, 8 = commercial low/high (base 2: zones.json, road_gen.h:16-17): misnamed, not wrong; there is no
  industrial density. lot_gen.cpp (160) + lot_place.cpp (116): a cell's lots are rectangles off a 2 m grid
  (mark_roads, place_blocks, place_strips), each refused where its ground is wet or its relief over 10 m
  (unfit :128-140): never cut to the land, a lot is whole or absent; WORKS depth 50 / frontage 42 only for
  `zone == ZONE_IND` (lot_place.cpp:82). b40/massing_core.cpp:163-191 make_body: plan_lot, the massing,
  finish_body (facades, dress), lay_out_lot; env.character = seed & 0xffff, env.era = (seed >> 16) % 3
  (:176-177): the era is the SEED's, never the quarter's (case 6); kind_of :24-34 by zone code.
  likeness.cpp body_look :34-61 reads only the volumes (place, base, storeys, roof kind), the main block's
  wall paint and roof paint: all made by the massing, before finish_body's facades and the lot's layout.
  b40/massing_works.cpp:16-56: one hall height range whatever the density. commands/road_edit.cpp:159-163:
  after an edit the dirty lot cells are cut again (process_dirty) and pushed to the growth queue;
  journal_damage.cpp:17-27 pushes a ruin's cells. Shaders: building_common.gdshaderinc:40-46 `weather(n)` is
  one fixed stain for every building (building_body.gdshaderinc:105, building_parts.gdshader:129): nothing
  of `built` reaches the picture (case 5); strike_buildings.cpp:103-108 ages a struck building by moving
  `built` back.
- The reference's lot planner (world/lots.ts:691-873): fitFree :692-707 cuts a new lot round the lots already
  there (the largest piece of the difference, kept when over 60% of it is left); blockPlan :710-781 and
  streetPlan :784-859 cut rows and strips in equal spans ROUND the kept lots (freeSpans, cutSpans :876-897)
  and each lot back to the land (landLot :866-873: the paving taken off, refused under half its area); the
  plan is stored with the land's key so a street change plans only land with no key (applyLots :900-908).
  world/buildings/procedural.ts:633-672 works by density: low a warehouse, hall 6-7.5 m; medium 7-9 m; high
  8-10 m; factory or warehouse by a coin. render: the decay goes to the shader with the finish
  (cellArrays.ts:105-111 finishWear) and darkens the stain under a sill (buildingMesh.ts:1793).

## Design (each case: both sides by file:line, the choice, why)
1. ORDER AND PACE. Reference: zoning.ts:232-253 (nearest open lot within 70 m of the last grown, else one
   drawn at random; a lot nothing fits set aside), main.ts:2656 (one each 0.5 s). Here: city_growth.cpp:34-56
   (nearest with no reach: one front marches across the whole queue; every building scans every lot of every
   queued cell with a building query a lot). THE REFERENCE'S ORDER WINS (the street the player zoned fills
   along and other zoned land starts too), built without either cost: GrowthQueue keeps each queued cell's
   OPEN lots (worked out once when the cell is queued or cut again, one building query a lot), the nearest
   is searched only in the cells the reach touches, the random lot is drawn by the cells' counts with a
   stream seeded by the city's seed and the count grown (the same city grows the same way); the lot picked
   is checked once more (a building may have come by another hand) and a lot nothing fits leaves the open
   list until its cell is cut again. THIS GAME'S PACE WINS (tuning sim.growth_per_s 25 against 2 a second):
   judged at the large city, 2 a second fills the 24 km map's 473,839 lots in 66 hours of city time.
2. THE 24 KM OPENING. Research R4 (doc/specs/003-roadcraft-features/research.md:142-166), decided there:
   a candidate's look from its massing alone (make_body stops before finish_body's facades and before the
   lot's layout: body_look reads nothing they make), so a lot costs one whole body; fill_all in a 2 x 2
   checkerboard of cells over the job pool (no two cells of a pass are neighbours; a read reaches 100 m, a
   cell is 256 m), each cell's lots in order, the records added in the cells' key order after each pass:
   the same city for any pool. The reference has no such opening (it grows over time): nothing to compare.
3. REZONING. Reference lots.ts:1254-1272 keeps the building under a new zone and removes it when the zone
   is taken off; here zone_commands.cpp:83 and lot_commands.cpp:245 knock down on any change. THE REFERENCE
   WINS (a player who repaints a district does not lose it): clear_side and raze only where the new zone
   is none.
4. LOTS CUT TO THE LAND AND KEPT. Reference lots.ts:691-908 (above). Here lot_cache.cpp:82-100 cuts a cell
   again whole on any change and with_player_lots :23-38 drops a proposed lot only where its MIDDLE is in a
   player's lot. This game's design keeps its better part (lots a pure function of the streets, a cell at a
   time, in parallel, nothing stored that can go stale) and brings what the reference has and it lacks:
   KEPT = a standing building's rectangle is its lot: a cell cut again gives way to every standing
   building's lot by overlap (never by the middle alone) and its rows are cut in equal spans round them, so
   an edit never cuts a lot under a building; saved with the buildings (their records). FITTED ROUND the
   player's lots the same way (overlap, the free span cut again). CUT TO THE LAND: a lot whose rectangle
   meets wet or steep ground is narrowed or made shallower to its dry, fit part instead of refused whole
   (kept when over half is left, the reference's share, lots.ts:869).
5. DECAY. Reference types.ts:453-469 + decayWatch.ts (tenths, a heap, a record written and its tile drawn
   again each tenth). Here nothing shows. A THIRD DESIGN, better than both: the record's `built` minute goes
   to the mesh once (a vertex value of the shell) and the shader works the wear out from the city's clock (a
   global uniform): no heap, no record written, no chunk drawn again, the wear continuous instead of in
   ten steps; the decay itself (grace, span) is one function, `decay_of`, read by the inspector too.
6. POLYGON LOTS, TRIES, ERAS. Reference zoning.ts:143-162,403-449,529-618. Here lot_book's stand is the
   biggest rectangle and the seed gives the era. ERAS: record_for writes the quarter's era and the
   frontage's character into the seed's own bits (the body reads them there: massing_core.cpp:176-177), so
   no key and no save changes. TRIES: a body whose blocks stand over a neighbour's lot or a street's paving
   is tried again set back and narrower, then a density lower (the reference's six steps and densities), the
   lot set aside when nothing fits. OUTLINE: a player's lot gives its building its own ring: the record
   names its lot, the lot's layout runs its boundary along the lot's sides, lays the lawn to the outline
   and leaves out what falls outside.
7. INDUSTRY AND PARKS. Reference plan.ts:137-154, procedural.ts:633-672. Here one industrial code and no
   park. Zones 9, 10 = industrial low, high (ZONE_CODES 11, zones.json, texts), the hall's height by the
   density, the generator's sector high past 0.85 of the radius and medium inside; parks = the plan's
   coarse noise (growth.json plan.parks): a block it marks is laid as a green (squares.cpp's own body).
- STILL TO READ (this game): sim/commands/zone_commands.cpp, lot_commands.cpp, lot_sites.*, sim/lots/lot_cache.cpp,
  lot_book.*, sim/citygen/lot_gen.*, lot_land.*, lot_place.cpp, block_lots.cpp, city_plan.cpp, squares.*,
  sim/buildings/growth.h, city_growth.h, the save of lots and zones, project/data/catalog/growth.json,
  zones.json, tuning.json's growth keys, project/ui drawer_strips.gd, lots_tool.gd, zone_tool.gd, the area's
  tests (`run tests --list`: growth, lots, zone, city_mass, window_traffic), doc/research.md R4.
