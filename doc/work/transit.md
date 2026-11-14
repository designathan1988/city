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
  Since then the two rules are in the tools (commit fb0835c2; CLAUDE.md, the skill `port` rules 4 and 10): a run
  carries `--seconds <n>` (`python tools/tasks/cli.py run <tasks> --seconds 180`, `tools/build/build.sh core-windows
  --seconds 300`), anything else in the background starts as `timeout <n> <command>`; a block is committed and closed
  only on its own photo plan, each photo opened after the run and sent to the owner.
- The owner, 2026-11-14 (their words): "Cada funcionalidade exige fotos próprias ... você tem que verificar o que é
  relevante ser fotografado e fotografar!"; "quando encontrar um problema de comportamento seu, jamais registre apenas
  na memoria, mas garanta que todos os mecanismos impeçam de acontecer novamente"; "Sempre pergunte a você mesmo: Qual
  o objetivo do que estou fazendo?; O mecanismo atual cumpre isso ou não?; Caso não, explique o por quê.; Como devo
  resolver o problema na causa?".
- STATE (2026-11-14, 16:00): NO BLOCK OPEN. The blocks before closed and pushed: zones-lots-growth (aa8dd236) and
  zones-photos (fa3adcb3, record 72ee3d0e: the zones' own photo plan, and four defects it showed, fixed). The
  trace ran in 26 s (`python tools/tracer/port.py sheet transit`: the reference whole, 14 files; this game
  partial at its share of the time; report dist/port/transit.json, thin). The sheet
  doc/specs/003-roadcraft-features/sheets/transit.md read whole.
- TO BE PHOTOGRAPHED (the block's own plan, project/tests/visual/transit_plan.json, written with the code; a first
  list, to be made exact when each case is designed): the Transit tool in hand with its help for each kind (bus stop,
  bus line, rail, station, metro); a bus stop placed on a street, before and after; a bus line drawn through stops,
  its buses running, one at its stop with its doors and the people boarding; a city bus of the traffic's own mix
  stopped at a kerb to board (case 1); a rail track and a station placed, a train on it, a level crossing closed with
  the cars held; a line's vehicles changed in its panel; a stop and a track removed; the same line seen from afar
  (the cars culled by the view, case 7).
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
- READ BEFORE A COMPACTION AND WRITTEN HERE FROM THE SESSION'S RECORD (2026-11-14): each line below is read again in
  the code before it is built on.
  - native/src/sim/transit/transit_plan.cpp (182 lines): lane_beside :19-32 (the kerb lane beside a stop: the
    direction of its road with the stop's footway on its right, lane 0); place_buses :35-64 (a line's buses kept,
    more waiting at its terminal `bus_spacing_s` apart; a line with fewer than 2 served stops gets none);
    place_all_buses :68-87 (buses whose line went leave the road); rebuild :90-122 (stop_index, stops_on_seg,
    lines_at_stop, stop_lanes, place_all_buses, place_rails; plan_seen, roads_seen); follow_roads :129-161 (after
    an edit of the roads alone: only the stops on changed segments, only the lines calling at a stop whose lane
    moved, refresh_crossings); stop_of :165-168; line_of :171-173; read_plan :175-179.
  - THE REFERENCE, sim/transit/transit.ts (lines 60-389): rebuild :122-158 (bus stops filed by place in a
    CellFiling :118, :131); placeBuses :161-182; placeRails :185-240: a line's trains are kept only when
    `was.trains.length === line.vehicles && samePoints(was.points, points)` and every station's stop and distance
    are the same (:223-224) (case 6: this game keeps them by point count and length alone, rail_run.cpp:132);
    crossingsByLane :243; followLanes :246-293; takeOff :296-312; step :315-324; stepBus :331-370: a bus not on the
    road waits its time, a stop out of the focus reach is passed by the timetable (:341), a bus on the road is
    taken off only by `awayFromView` when `bus.dwell === 0 && !v.kerbStop` and every door is shut (:357);
    awayFromView :373-382: off the road only when its pose is outside `w.inFocusReach(pose.p.x, pose.p.y,
    AWAY_MARGIN)` (case 5: out of sight with a margin; here transit_sim.cpp:96-99 takes it off as it leaves the
    near window and traffic_params.h:63 bus_away_margin_m is read by nothing); stopPoint :385 on.
  - STILL TO READ: render/transit/transit_parts.* (322 lines), project/ui/transit_strip.gd (58), the texts
    (strings.csv:361, strings_c.csv:344-349), the rest of project/data/render/transit.json (its head read: radius_m
    900, items_per_frame 8, run_m 256, car_m [17.5, 3.6, 2.9]); the reference's world/transit.ts (368),
    editor/transitTools.ts (249).
- THE REFERENCE, editor/transitTools.ts (249 lines), read whole on 2026-11-14: the kinds are stop, terminal, track,
  station and line, the rail train or metro (:16, :34). A click (:86-137): Shift+click REMOVES the stop under it, or
  else the track, and says so (`hint.transit.removed`, :88-93); stop: the footway nearest within STOP_REACH 18 m
  (:97; "DO NOT build the walking network of the whole map for a click", :63), refused with its reason
  (`noStreet`, `taken`); terminal: a click on a bus stop turns its terminal on or off (:103-109; `noStop`); station:
  the track nearest within PICK x 1.5 (PICK 8 m, :28, :111; `noTrack`, `taken`), a metro's way down opened on the
  nearest footway clear of buildings within 120 m (:115, `built` :54-61 asks the buildings near, never all); track: a
  point snapped to a track point or a track within JOIN x 2 (:77-83: "the track points round the pointer only"), a
  double click ends it; line: only stops of the first one's mode (:128-129), a double click ends it. Keys (:142-149):
  Enter ends a run or a line, Backspace takes the last point back AND redraws at once (:146), Escape drops it.
  finishTrack :151-164: points a metre apart at least, two at least (`trackShort`), a train's track may not run down
  a road for more than ALONG_ROAD (`trackOnRoad`, :158). finishLine :166-173 (`lineShort`, `lineMade`). The plan
  drawn (:188-243): the stops and stations ALWAYS, tool on or off (:209-225: a letter B, M or T, a terminal square),
  the lines only with the tool on (:193-208), the line being made through its stops on to the pointer (:234-241);
  the run being laid is built in the scene as real track (`preview` :176-179). Case 2 here: transit_tool.gd shows
  one hint whatever the kind and the stops' marks only while the tool is on, and Backspace shows nothing until the
  next move: the reference tells each refusal by its own text, keeps the stops marked and redraws at once.
- THE REFERENCE, world/transit.ts (368 lines), read whole on 2026-11-14: normalizeTransit :63-101 drops what is
  malformed, keeps of a line only the stops that exist and are of its mode (under two: the line goes), a colour only
  when it is a hex colour, and its vehicles `clamp(1, 20, round)` or 2 when the save has none (:95: case 6; here
  transit_data.cpp:107-117 leaves `vehicles` as read). addLine :134-140: a bus line starts with 3 vehicles, a rail
  with 2, a colour of eight in turn, a name by mode and count; setLineVehicles :146-148 clamps 1 to 20; removeStop
  :113-116 takes the stop out of every line and a line left with fewer than two goes; removeTrack :128-132 takes its
  stations. The picks are filed (case 3): the tracks' stretches by 25 m cell, made once a version of the data
  (`trackCells` :209-228), read by nearestTrack :231-242, stretchesNear :245-254 and trackPointNear :257-263 ("DO
  NOT walk every point of every track for each move of the pointer"); the stops by id and by 25 m cell (`stopIndex`
  :266-278; stopById :281, stopNear :284-299). railGraph :164-204 joins points within JOIN 3 m by cells. railPath
  :302-349: `onEdge` walks EVERY edge of the graph for each end of a leg (:303-314, the reference's own cost; the
  same walk as transit_data.cpp:32-45 here: case 4 takes it off both), then Dijkstra with a heap. ALONG_ROAD 12 m
  and longestOnRoad :355-368 (a sample each metre).
- THE TEXTS (2026-11-14): project/data/i18n/strings.csv:364 `game.hint.transit` says "Clique na calçada para pôr
  paradas na linha; clique numa parada para tirar; Shift+clique torna a parada terminal": the tool does neither
  (transit_tool.gd:34-39: Shift+click REMOVES; a terminal is its own kind). strings_c.csv:344-349 hold a help for
  each kind (`ui.transit.help.stop`, `.terminal`, `.trainTrack`, `.trainStation`, `.metroTrack`, `.metroStation`)
  that the tool never shows (it shows `game.hint.transit` whatever the kind, transit_tool.gd:18-25); `.help.stop`
  also says a click on a stop removes it; there is no help for the kind `line` (`transit.tool.line`,
  strings_c.csv:113). The refusals' texts exist, one each (strings_c.csv:488-496: `hint.transit.noStreet`, `taken`,
  `noStop`, `noTrack`, `trackShort`, `trackOnRoad`, `lineShort`, `removed`, `lineMade`) and the marks
  (`transit.mark.bus`, `.train`, `.metro`, :498-500).
- project/ui/transit_strip.gd (58 lines), whole: the Make a line tab's list: each line with its colour, name, mode
  and stops, its vehicles with fewer and more (`transit_vehicles`), and its removal (`transit_drop_line`); filled
  again when the plan changes (`plan_changed`, one shot). Nothing to change but what case 6's clamp shows.
- native/src/sim/transit/rail_net.h (40) and .cpp (53), whole (2026-11-14): one mode's tracks as a graph, made again
  only when the mode's tracks (point for point, `same_tracks` :12-23, a compare of every point of the mode at each
  making of the loops) or the joining reach change (`ready_net` :27-37); the legs found on it kept by their two end
  points and let go when a making did not ask for them (`leg_of` :39-47, `drop_unused_legs` :49-51). Nothing to
  change for case 4 here: the search is made once a leg.
- native/src/sim/transit/transit_ground.h (25) and .cpp (78), whole: `transit_paved` :22-38 asks the segments near a
  point (the spatial file, no whole walk); `transit_stop_frame` :40-61 walks EVERY track of the plan to find the stop's
  own by its id (:42-43), and every point of it for the nearest piece (case 3: a walk of the plan for each stop's
  frame, asked by the renderer's signature and the commands); `transit_stop_by_road` :63-69; `transit_built` :71-75.
- THE REFERENCE, sim/transit/transit.ts :385-653, read whole on 2026-11-14 (after the zones-photos block):
  stopPoint :385-396 (a stop's point on its lane; a stop out of the focus reach by its own place and its road's
  limit); passStopAway :399-405 (the timetable: dwell, turn at the ends, the time to drive there); sendBus
  :408-424 (the way planned ONCE, a stop it cannot reach skipped; "DO NOT search the way there twice");
  leaveTowards :427-438 (a stop beyond the window: the bus drives out to the hand-off lane nearest it and the
  timetable takes it there); spawnBus :440-463 (a way back from the stop, APPROACH, only with the lane clear from
  there to the stop; the line's colour). Trains: stepTrain :467-497: the gap to the train ahead is a walk of every
  train of the line (:487: the reference's own cost, the same as rail_run.cpp:200-205 here); the headway
  `min(TRAIN_GAP, max(length + 10, 0.6 * loop / trains))` (:491); at an end of the line the last car leads (:476);
  pointAt :500-515 a binary search by distance along the loop; crossingNearEnd :518-524, crossingNear :527-530,
  closedOn :533-544 read `crossingsByLane` (a lane's own crossings; closed while a train is within CROSSING_WARN
  ahead or still across); trainZones :547-585 makes the walkers' discs anew on each call, a map and arrays (an
  allocation a call), left out when a train cannot reach the walkers' box (:551-555); trains() :588-600 hands
  EVERY train's cars to the drawing, no cull (case 7: the reference's cost); levelCrossings :614-634: each leg of
  the loop against the lanes in its box (`lanesInBox`), each lane's own centre line: a crossing a LANE at that
  lane's own arc (:627-628). This game crosses the road's MIDDLE line and files one arc for every lane of both
  directions (rail_run.cpp:55-83): on a skewed crossing or a wide road each lane's stop line is off by the skew;
  the reference's way is the better (case 4), built here off the road's lanes with no whole-map walk.
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
