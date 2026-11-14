# transit: public transport, ported in one block (doc/port.md, section 5)

## Now
- The owner's standing orders (2026-11-13/14, their words): "FAÇA O PRÓXIMO, FAÇA O QUE É NECESSÁRIO PARA ESSE JOGO
  SER FEITO DE UMA VEZ POR TODAS, SEM MAIS ENROLAÇÃO OU DESVIOS"; "É A PORRA DA FUNCIONALDIADE INTEITA"; "RESOLVA NO
  CODIGO, NADA DE FICAR INVESTIGANDO COM TESTE, MEDIÇÃO"; "OLHA COM ATENÇÃO AS IMAGENS A PARTIR DE AGORA". The /goal
  (their words): "siga na hora para a próxima linha desmarcada de doc/port.md, e assim por diante, até portar tudo
  ... NÃO PARE PARA ME PERGUNTAR NADA ... Só pare se uma terceira verificação de um bloco falhar". The addendum
  (their words): "NADA FICA DE FORA POR CUSTO; O MELHOR VENCE; TUDO SE DECIDE PELO CÓDIGO ... Escreva na nota a
  comparação com file:line dos dois lados e o porquê da escolha".
- THE OWNER'S ORDER OF 2026-11-14, LATEST AND FIRST (their words): "TODO PROCESSO QUE ENVIAR DEVE DURAR NO MÁXIMO
  SEGUNDOS, COM MONITORAMENTO, REAL E SAÍDA. JAMAIS UM PROCESSO DEVE FICAR PERPETUAMENTE ... AO CODIFICAR, NÃO FAÇA
  MAIS REVISÃO, RE-REVISÃO, ETC. NEM TIRE MAIS FOTOS REPETITIVAS DA MESMA VISTA DE LONGE. CADA ETAPA TEM AS SUAS
  FOTOS PROPRIAS DE SEGUNDOS E TESTES DE SEGUNDOS ... O PROCESSO É: RASTREAMENTO DE FUNCIONADADE DOS DOIS JOGOS EM
  SEGUNDOS, ANALISE E COMPREENSÃO, CODIFICAÇÃO, TESTE E FOTOS COM PROCESSOS DE SEGUNDOS ... ANALISE, CORREÇÃO SE
  PRECISAR NO CODIGO, PROXIMA FUNCIONALIDADE ... NADA DEVE FICAR ABERTO OU SER IGNORADO". Every process: a deadline
  of seconds (the Bash tool's timeout, the tool's own), its output read when it ends.
- STATE (2026-11-14): NO BLOCK OPEN. The block before (zones-lots-growth) closed: commit aa8dd236, pushed. The
  trace ran in 26 s (`python tools/tracer/port.py sheet transit`: the reference whole, 14 files; this game
  partial at its share of the time; report dist/port/transit.json, thin). The sheet
  doc/specs/003-roadcraft-features/sheets/transit.md read whole.
- The cases (doc/port.md, with the sheet's evidence): 1. random city buses stop to board (demand.cpp:63,
  kerb_stops.cpp:326-327; reference kerbStops.ts:59-141,365-398); 2. the transit tools' help (strings.csv:361,
  transit_tool.gd:22-39,105-107, strings_c.csv:344-349); 3. picking and previewing without whole-plan walks
  (transit_data.cpp:220-247, transit_commands.cpp:193-202, transit_renderer.cpp:235-248); 4. trains redesigned
  off the reference's costs (transit_data.cpp:32-45, rail_run.cpp:55-83,200-205,250-282); 5. buses leave only
  out of sight (traffic_params.h:63, transit_sim.cpp:96); 6. a save's lines clamped on load, trains kept through
  an edit only when their track is the same (transit_data.cpp:107-117, rail_run.cpp:132); 7. transit vehicles
  culled by the view (transit.json:3); 8. bus lanes (done with roads 7: to confirm in the code).
- READ SO FAR (below, "Read"): sim/transit/transit_data.h/.cpp, rail_run.cpp, transit_sim.h/.cpp,
  commands/transit_commands.cpp, project/game/tools/transit_tool.gd, render/transit/transit_renderer.cpp.
  STILL TO READ: sim/traffic/kerb_stops.cpp and demand.cpp:63 (case 1), transit_plan.cpp, rail_net.*,
  render/transit/transit_parts.cpp, transit_overlay.gd, the texts (strings.csv:361, strings_c.csv:344-349),
  transit.json; the reference's sim/vehicles/kerbStops.ts:59-141,365-398, sim/transit/transit.ts,
  world/transit.ts, editor/transitTools.ts.
- NEXT, in order: the rest of this game's code read whole (sim/transit/*, sim/traffic/kerb_stops.cpp, demand.cpp,
  commands/transit_commands.cpp, render/transit/*, project/game/tools/transit_tool.gd, project/data transit.json
  and the texts); the reference's (sim/transit/transit.ts, sim/vehicles/kerbStops.ts, world/transit.ts); each
  case's design here with both sides by file:line; `block open transit`; the coding; a test and a photo of
  seconds.

## Read (facts with file:line)
- native/src/sim/transit/transit_data.h (116) and .cpp (249), whole: the header says "the stops and stretches are
  filed by cell for the tools' picks" (:2-3) and no such filing exists: nearest_track :220-233 walks every point
  of every track, stop_near :235-247 every stop, TransitData::stop / line :48-58 walk the lists by id (case 3);
  on_graph :32-45 walks every edge of the rail graph for each end of a leg (case 4); transit_normalize :107-117
  drops unknown modes, short tracks and lines under two stops, and leaves `vehicles` as read: no clamp, no
  default (case 6); transit_set_vehicles :102-105 clamps 1..most on an edit.
- native/src/sim/transit/rail_run.cpp (325), whole: crossings_of :55-83 crosses each stretch of a loop with each
  near road's MIDDLE line sampled every 4 m and files one crossing for every lane of both directions at that
  one arc (case 4: "level crossings at the road's middle line for every lane"); place_rails :117-148 keeps a
  line's trains through an edit when the line's id, its loop's point count and length and its vehicle count are
  the same (:132: case 6: by count and length alone, not the track itself); step_train :182-212: the gap to
  the train ahead is a walk of every train of the line (:200-205: quadratic in a line's trains); crossing_gap
  :214-236 walks every train of the rail for each lane asked; train_zones :250-282 clears and makes the zones
  again every tick, a vector of discs a train (allocation a tick); publish :300-322 culls the cars by a radius
  round a point (case 7).
- native/src/sim/transit/transit_sim.h (107) and .cpp (213), whole: step_bus :74-116: a bus off the road waits
  its time; with its stop's lane in the window it is put on the road (spawn_bus :36-61), else passed by the
  timetable (pass_stop_away :64-72); on the road, `!v.micro && b.dwell == 0` takes it off at once (:96-99: case
  5: it leaves as it leaves the near window, never out of sight; traffic_params.h:63 bus_away_margin_m is read
  by nothing); a bus at its stop dwells and serves its call (traffic::bus_call :109); send_bus :121-142 routes
  it to the next stop it can reach; hold_ahead :158-184 (a closed crossing three lanes ahead, a crossing near
  the stop line, the bus's stop).
- native/src/sim/traffic/kerb_stops.cpp (350, whole; at the file's limit: a bus's errand goes in a file of its
  own): assign_errand :323-329 gives a car DROP or PICK and a truck DELIVER, a bus of the traffic's own mix
  (demand.cpp:59-63 draw_type: bus_share) NOTHING: it never stops (case 1); plan :93-134 plans one stop on the
  kerb lane (lane 0), clear of the ends, with a footway; step_stop :232-320 the phases; a line's bus boards by
  traffic::bus_call (bus_call.h) from transit_sim.cpp:109, a different path.
- THE REFERENCE, sim/vehicles/kerbStops.ts: assignOccupancy :137-142: a city bus "is never empty and always
  calls at stops" (errand 'service', 2 riders or more); planBusStop :365-397: no stop before BUS_HEADWAY 35 s
  of the bus's own age since the last (:59, :366); it stops 0.3 of the way along the free stretch (:367), by a
  footway, with no queue ahead; one or two riders get off by the middle door (:373-378), up to two people
  walking alone within 0.7 x BUS_FETCH 45 m of the front door get on (:379-392); `abandon` keeps a bus's
  service for the next stop (:285-286).
- native/src/sim/commands/transit_commands.cpp (204), whole: every command copies the whole plan and hands it to
  the journal (j_transit); transit_place_stop :81-93, transit_place_station :104-122, transit_remove_at
  :156-168 pick with stop_near / nearest_track (the whole-plan walks of case 3); transit_snap_track :193-202
  walks every point of every track of the mode on EACH pointer move (the tool asks `transit_snap` in hover).
- project/game/tools/transit_tool.gd (124), whole: enter :18-25 shows one hint (`game.hint.transit`) whatever
  the kind, and the plan's overlay only while the tool is on (case 2: stop markers only while the tool is on);
  click :34-39 Shift+click REMOVES (the hint's text says it makes a terminal: strings.csv:361, to read);
  hover :78-81 asks a snap and a preview on every move; key :100-109: Backspace takes the last point or stop
  back and shows nothing until the next move (case 2: no `_show_run` after it).
- native/src/render/transit/transit_renderer.cpp (268), whole: set_preview :235-248 drops the preview's mesh
  and makes it again whole on every call (case 3: on every pointer move); see_plan :138-157 makes again only
  the items whose signature moved; signature_of a stop :45-60 walks every line for its colour; update
  :250-266 draws the train cars within radius_ (transit.json radius_m) of the camera's point, a circle round
  it and not the view (case 7).
