# roads: the complete roads, ported in one block (doc/port.md, section 1)

## Now
- The owner's standing orders (2026-11-13/14, their words): "FAÇA O PRÓXIMO, FAÇA O QUE É NECESSÁRIO PARA ESSE JOGO
  SER FEITO DE UMA VEZ POR TODAS, SEM MAIS ENROLAÇÃO OU DESVIOS"; a functionality is whole ("É A PORRA DA
  FUNCIONALDIADE INTEITA"); the /goal: "Continue os trabalhos até concluir tudo ... NÃO PARE PARA ME PERGUNTAR NADA,
  NEM PARA PEDIR AUTORIZAÇÃO ... ESCOLHA SEMPRE A MELHOR OPÇÃO"; "RESOLVA NO CODIGO, NADA DE FICAR INVESTIGANDO COM
  TESTE, MEDIÇÃO"; "OLHA COM ATENÇÃO AS IMAGENS A PARTIR DE AGORA"; "CHEGA DE DEMORA! CHEGA DE TRABALHO MAL FEITO!".
  Standing: Portuguese to the owner; no agents; defects found by reading; one small verification; whole photos.
- Before it: the block `look` closed (9989db18: the far level lays each lot; the guards 6cee0a3e). Its note:
  doc/work/look.md.
- State (2026-11-14): no block open; nothing coded. The tracer's sheet ran (dist/port/roads.json: differences
  named for case 1 only; the other rows carry no file:line it can start from, so each was read from its files).
  READ AND WRITTEN BELOW, both games: case 1 (the five road_grade files here; the reference's elevation.ts to :660
  and its functions' list), case 2 (road_plan.cpp, road_economy.cpp/.h, road_draft.gd; the reference's buildMode.ts,
  economy.ts, structures.ts, main.ts:3360-3547), case 4 (road_markings.cpp; the reference's markings.ts whole,
  markingStyle.ts), case 5 (road_profile_panel.gd, road_spec_ops.gd; the reference's profileEditor.ts by its
  controls), case 6 (street_names.h, inspector_panel.gd; the reference's streetNames.ts, inspector.ts:210-254,
  vehicles/spawn.ts:80-230), case 7 (profile_types.h, lane_graph.cpp, lane_graph_types.h, lane_change.cpp; the
  reference's rules at their places), the reference's tuning.ts.
- STILL TO READ before the block: the consumers of the grade here (road_network.cpp:245, road_network_deck.cpp,
  road_network_cut.cpp, road_grade_sync.cpp, the probes) and road_grade_test; this game's tuning.json `grade` and
  `economy` keys against the reference's; city_core_commands.cpp:60-90 (the preview's heights) and road_bend.cpp,
  road_snap.cpp:127 (a start on a raised road); the far lines (project/render/shaders/road_far.gdshader, the
  reference's render/roadPaint.ts); the basic furniture set (street_furniture*.cpp here; the reference's
  world/roads/furnitureSets.ts, world/streetFurniture.ts); where this game's demand could take cars in at the map's
  edge (lot_demand.cpp, street_flows.h, flow_step.cpp); way_search.cpp (does a way know the lanes); road_spec.cpp
  and city_core_road_spec.cpp (the editor's core: limits, issues, the 256 customs), RoadModels (saved roads).
- THEN: Context7 for each API the code calls (Godot's UI nodes of the editor, ImmediateMesh or the draft's
  overlay, shader derivatives for the far dashes); the internet for each design decision and value (the smoother
  and the envelopes of case 1; the legends' sizes and the arrows by the signage manual; what a general vehicle may
  do on a bus lane; the entry rates at a town's edge); each case's design written under "Design".
- The cases (doc/port.md; doc/specs/003-roadcraft-features/sheets/roads.md has each with its evidence):
  1. the heights of the roads, solved in this game's design (Proads-8, -10, -15, -33, -35; read and researched before:
     doc/work/t015-roads.md: Whittaker-Eilers smoothing, Eilers 2003; exact two-pass envelopes, Felzenszwalb and
     Huttenlocher; vertical alignment as constrained least squares, UBC thesis 1.0400121);
  2. laying a road: bridge, fill, cut, tunnel stretches and their price in the draft; a tunnel for a deep cut; the
     grade-limited end told; a start on a raised road takes its height;
  3. links and bans kept through edits: done (T037, 90b67e27);
  4. markings: turn arrows, bus legends, words, plate marks; far dashes and colours; the line before a signal;
  5. the profile editor: class, carriageway, paving, centre line; each footway its own; past 256 customs; Save's
     problems; deleting a saved road;
  6. streets and their life: traffic from the map's edge, street names and house numbers, the basic furniture set;
  7. lanes that obey the road: bus lanes, solid lines, per-lane turns read by the traffic.
- Next, in order: the tracer's report read; both games' code of each case read whole (facts with file:line written
  here, case by case, as they are read); Context7 for each API and the internet for each design decision and value;
  each case's design written here; then `block open roads`, every case coded with nothing run; the whole diff read;
  `block review`; `block verify`; the build; one run (roads' own tests, a roads photo plan, zoom_ladder, e2e);
  lab_test; commit; the board's line checked.
- The one look with no block open is not spent since the block `look` closed.

## Design, case by case (written as each is settled; nothing coded yet)
- Case 1, the heights, the behaviour to keep (what the player meets): a road follows the ground it is laid on,
  cut balancing fill, never steeper than its grade; it is level over each junction's plate at the height its node
  settles on; a raised road is a straight deck that clears what is under it and comes down by ramps with a sag and a
  crest curve; a tunnel goes down by ramps to its depth; an authored height is met by an eased line; nodes settle
  where their roads at grade want them, an aloft node where its decks want it within what a ramp can climb; only
  what an edit reaches is solved again; the same map gives the same heights.
- Case 1, this game's own design (to be detailed with its sources before the block): (a) one float line a segment
  kept for good (the heights), the ground lines (ceiling, mean) made when a segment is solved and held in a cache
  bounded by memory (Proads-8); (b) the grade line as a penalised least-squares smoother solved exactly as a banded
  system (Whittaker; Eilers 2003), no moving-mean passes; (c) the grade limit as the exact two-sweep envelope of
  cones (the lower and upper envelopes of |slope| <= g are exact in one forward and one backward sweep:
  Felzenszwalb and Huttenlocher's distance transform of sampled functions), ended by convergence, never by a pass
  count (Proads-33); (d) heights read by arc length everywhere (Proads-35); (e) every reach and share in
  project/data/catalog/tuning.json `grade` with its schema and source, none in the code; (f) names and structure of
  this game (no solveGround/solveRaised/... one to one: LAB_COPY).

- Case 1, the design in detail (this game's own building blocks; the contract others read stays: prof[s].{valid,
  structure, manual, walls, a, b, len, step, half, plate_a, plate_b, h, ylo, yhi}, node[n], nodes[n].y, moved,
  gone, at, has, pending, the four marks):
  A. Two exact operations on a line of stations, no pass count in either (Proads-33):
     - `within_grade(z, step, g)`: the greatest line under z and the least line over z whose slope never passes g
       (each exact in one sweep forward and one back: the lower envelope of cones z_j + g d and the upper of
       z_j - g d; Felzenszwalb and Huttenlocher's two-pass distance transform of a sampled function, with the
       L1 cone), and their mean: within the grade, equal to z wherever z already is, cut balancing fill elsewhere.
     - `smoothed(y, w, lambda)`: the weighted penalised least-squares line (Whittaker; Eilers 2003): it minimises
       sum w_i (z_i - y_i)^2 + lambda sum (second differences of z)^2; its system (W + lambda D'D) z = W y is
       symmetric, positive and five-banded, solved exactly by a banded LDL' in O(n), in double. lambda from a
       wavelength in metres: the wave the smoother halves, lambda = 1 / (2 - 2 cos(2 pi step / wavelength))^2.
       A station held (a plate, an end) takes a weight large against 1: the line passes through it and bends
       smoothly into it.
  B. The design line of a road (its "ground line"): the mean ground across the casing at each station, smoothed
     (weights 1, wavelength grade.smooth_wavelength_m: the 36 m moving mean's half-power wave, 1.66 x 72 m, about
     120 m), then within_grade(grade.ground). A road laid in pieces through joints that carry it on: one line
     along all of it, as now.
  C. A span at grade between its two plates at heights ha and hb: target = the design line tied to the two heights
     (each end's shift carried on over a tie length and eased out; an authored span: the two lifts eased across the
     whole span); held between the two cones of slope G from its ends (G = the span's grade: grade.ground, or
     grade.ramp when authored, never under what its two ends need times grade.span_slack); within_grade(G); then
     smoothed with the plates' stations held (the vertical curve's wavelength: grade.curve_wavelength_m, from the
     4.8 m reach), which rounds every change of grade and the joins into the level plates in one solve; the plates
     set to ha and hb exactly.
  D. A raised span: the deck's floor = the higher of the straight line between what its two ends want and the
     ground's top plus the room under a deck; the least line over it within the deck's grade (one exact envelope);
     from an end on land, never over what a ramp climbs from that node (a sag parabola over grade.ramp_sag_m, then
     grade.ramp), the two met by a rounded crest; the ground stays a floor, relaxed towards each junction; a capped
     end holds the ramp's line; the plates level at the highest reached. A sunken span: eased ramps from each end
     down to its depth, the greatest line under them within grade.tunnel, the plates level.
  E. The nodes as now (a node wants the mean of its at-grade roads' design lines at their plate edges plus its
     lift; the mean ground over its disc when none; aloft when every one of two or more is raised; an aloft node
     starts at what its decks want within what a ramp from land climbs), settled in rounds until none rises; the
     round count is no rule: a guard (grade.rounds_most) that is counted and shown by the probe when met.
  F. Memory (Proads-8): a profile keeps its heights alone; the ground read under a road (its mean and top) and its
     design line live in a cache bounded by grade.ground_cache_mb, made again from the ground when missed (for a
     piece of a chained road: the whole chain's line again, so a miss never changes a height).
  G. Heights by arc length everywhere (Proads-35: the readers that took the curve's parameter, to find by grep
     of `q.at(` beside `seg_y` and of the lanes' poses) and every reach, share and wavelength in tuning.json
     `grade` with its schema and its `_doc` (the vertical curve and ramp values need their sources: to search).
  The files: sim/roads/grade_line.h/.cpp (A), grade_ground.cpp (the ground read, the cache, B), grade_span.cpp
  (C, D), road_grade.cpp (E and the solve), road_grade.h (the contract); the three translated files go.
- Case 2, the design's direction: (a) the plan prices what is in hand: plan_road takes the structure the tool
  holds and prices RoadBuild{that structure, lift_a, lift_b} (road_plan.cpp:213), the one function the commit's
  price already is: what the draft shows is what the click costs; (b) the draft's stretches: a pure reading of the
  plan (no state touched): stations along the planned curve, the ground under each (ground_of, as the solver reads
  it), the deck there (the solver's own line between the two ends' heights for a road alone: the functions of case
  1 on a profile made for the plan), each station's way of building by its lift (a tunnel under the bore's cover,
  a bridge when raised or past the piers' lift, an embankment or a cutting past fill_from_m, else ground), merged
  into runs; the plan carries them and their lengths; (c) the draft's words: length, price, then each stretch that
  is not ground and is 5 m or more, the longest first, in tens of metres, then the refusal (road_draft.gd measure;
  the stretches' names as i18n keys); the overlay paints the fill, the cutting, the piers and the buried stretch;
  (d) a grade-limited end: a held connection refused with its own reason (a new key, not "too short"), a free end
  told by the height label in the warning colour; (e) a start on a raised road takes its height (road_snap.cpp:127);
  (f) a deep cut becomes a tunnel: where the solved cover over a road at grade passes tunnel.auto_cover_m (the
  reference's 18 m; a source to be found) the stretch is bored.
- Case 4, the design's direction (MBST vol. IV, the Sources below): (a) lane arrows (PEM): for each arriving lane
  whose movements are not all of the approach's (its own turn mask, or the rule by index where the approach has
  lanes that differ), white, at the lane's centre, two in sequence: the first d before the end of lane changing
  (the stop line's back), the second d1 before it, d, d1 and the arrow's length by the road's speed (10/30/5.00
  under 60 km/h; 15/40/5.00 to 80; 15/50/7.50 over 80), only where the road is long enough for the first; the
  shapes drawn from the manual's own dimensions (the straight arrow's a to f; the turn and combined ones from their
  tables), this game's own outlines in project/data/render/streets.json; (b) the stop word before a stop line where
  the approach must stop (DirSeg.must_stop), in every lane, 1.60 m before the line, letters 1.60 m high (2.40 over
  80 km/h); the give-way triangle where it yields (rank 1), pointing against the traffic, 3.60 x 1.20 (6.00 x 2.00
  over 60 km/h), within 1.5 to 15 m of the junction; (c) a bus lane: its line continuous, white, 0.20 to 0.30 m
  wide, broken (the continuity line) along the stretch before a junction where a turn crosses it, and its legend
  along the lane (the style's word); (d) the words by the city's regional style (the stop word, the bus word) as
  data; letters drawn as this game's own strokes (a table of its own in the data, not the reference's); (e) the far
  lines: each line's colour and whether it is dashed carried to the far level (the lane strip's vertex), filtered
  by the pixel's footprint along the road too, so a dashed line is its duty and a solid one whole, a yellow one
  yellow; the three numbers out of the shader into the material's data; (f) all of it as M_PAINT geometry of the
  road's mesh, under the tier's detail cut-off like the rest of the paint.
- Case 5, the design's direction: the editor gains the road's class, its carriageway's surface (Surface: asphalt,
  concrete, cobble), each footway's and median's paving (Paving) and the centre line (auto, dashed, solid, double:
  RoadSpec.centre already carries it); widening changes the picked element alone, or every lane when it is a lane
  (road_spec_ops.gd with_width compares by index, not by value); the limits of speed from the core; Apply past the
  core's count of custom roads is told (the refusal's reason shown); Save is refused while a problem stands; a
  saved road can be deleted (RoadModels); the numbers of the panel in project/data/ui.
- Case 6, the design's direction: (a) the furniture levels as written under "Read" (street_furniture.cpp:322-333);
  (b) the inspector: put_road adds the street's name (the first name item of works.names.by_seg[seg]: its given
  keys or its own text) and its numbers (metres from the street's end nearest the map's origin to the segment's
  two ends along street_chain, and which side is even); inspector_panel.gd shows "<the street's name> · <the
  class>" and a line of the numbers (new i18n keys); (c) traffic from the map's edge, in this game's own way (the
  far traffic is flows a street: no entry queue of cars as the reference's): a road's end is on the map's edge when
  its node has one arm and stands within traffic.json edge_reach_m of the map's side (CityState::map_half,
  city_state.h:84); in flow_rates (flow_rates.cpp:17-29) the directed segment leaving such a node starts, besides
  what its sides start, the flow a street of its lanes and speed carries at the demand's density of the hour
  (per_lane_m x lanes x speed) times edge_share, and the one arriving at it ends every trip that reaches its end
  (end_share 1); the detailed vehicles the window makes take such an end as a place to go by the same weight
  (demand.cpp side_weight and what make_vehicle draws a goal from: to read at :118-233); the two values and their
  source (a share of through traffic at a town's edge) in the data with its schema.
- Case 7, the design's direction (sources to be searched before the block: what a general vehicle may do on a bus
  lane to reach a turn or a lot, by the traffic code): native/src/sim/traffic/lane_graph_types.h DirSeg gains, a
  lane: its bus-only flag, whether the line to the next lane bars a change, its turn mask; LaneInfo
  (profile_types.h) gains the bar to the next lane, filled where profiles.cpp:29 already decides the line;
  lane_graph.cpp build_dir fills them; link_pair gives a lane with a turn mask only its own turns (never a U-turn),
  the other lanes the rule by index, and never leads a general lane's movement into a bus-only lane where a general
  one stands; lane_change.cpp choose: no change by choice across a barred line nor into a bus-only lane for a
  vehicle that is no bus (v.bus < 0), a mandatory one waits for the end of the bar or its forced gap (its wait at
  the line: FORCE_AFTER_S, so no vehicle is held for ever); the far flows keep counting cars a street, not lanes
  (flow_step.cpp), so nothing changes there.

## Sources (the source's own words, for the data's `_doc`)
- CONTRAN, Manual Brasileiro de Sinalização de Trânsito, volume IV, Sinalização Horizontal (Resolução 236/2007),
  read from the Ministry of Transport's own PDF (gov.br, manual_vol_iv_2.pdf, its text extracted page by page):
  - 5.5.1 MFE, the exclusive lane: "Branca para Faixas exclusivas no fluxo."; "A marcação de Faixa exclusiva no
    fluxo é constituída por uma linha contínua, com largura (l) que varia entre 0,20 m e 0,30 m."; "Deve ser
    contínua em toda a extensão, exceto nos trechos onde for permitida a entrada ou saída da Faixa exclusiva, ou
    onde houver interseção ou movimento de conversão, onde deve ser utilizada linha de continuidade."; "Pode ser
    aplicada legenda ao longo de toda a extensão da faixa exclusiva de forma a identificar o seu uso."
  - 9.1.1 PEM, the lane arrows: "Cor Branca."; "Deve existir uma seta para cada faixa de trânsito, posicionada no
    centro da mesma, com a conformação adequada ao movimento nela permitido. Recomenda-se implantar pelo menos
    duas em seqüência na mesma faixa, sendo opcional a colocação de uma terceira."; seven shapes ("Siga em
    Frente; Vire à Esquerda; Vire à Direita; Siga em Frente ou Vire à Esquerda; Siga em Frente ou Vire à Direita;
    Retorne à Esquerda; Retorne à Direita"); urban roads, by the regulated speed: v < 60: d 10 m, d1 30 m, d2
    45 m, the arrow 5.00 m long; 60 <= v <= 80: 15, 40, 60, 5.00; v > 80: 15, 50, 75, 7.50; "d = distância
    considerada a partir do ponto de saída da faixa de trânsito, onde não pode mais haver transposição de faixa
    (início da linha simples contínua de aproximação). d1 = distância entre a primeira e a segunda fileira."; the
    straight arrow's table: a 5.00, b 0.75, c 1.50, d 3.50, e 0.15, f 0.30 (7.50: 0.75, 2.25, 5.25, 0.15, 0.30).
  - 9.2.1 SIP, give way: "A SIP é utilizada como reforço ao sinal de regulamentação R-2"; "Cor Branca."; v <= 60:
    a 3.60, b 1.20, c 0.20, d 0.55; v > 60: 6.00, 2.00, 0.30, 1.00; "O triângulo deve ser colocado de forma que
    aponte contra o sentido de circulação, inscrito entre 1,50 m a 15,00 m de distância da interseção, a partir
    do prolongamento do meio fio da via transversal, no centro da faixa onde estiver inserido."
  - 9.3 Legendas: "Cor Branca."; urban roads: v <= 80: 1.60 m; v > 80: 2.40 m; "As legendas devem conter
    mensagens simples e curtas."; "A legenda “PARE” deve ser posicionada, no mínimo, a 1,60 m antes da linha de
    retenção, centralizada na faixa de circulação em que está inscrita. Deve ser utilizada como reforço ao sinal
    de regulamentação R-1 – “Parada obrigatória”."
- A general vehicle on a bus lane (searched; the Curitiba page refused the fetch, 403; to be read from an official
  text before the data is written): the enforcement's rule reported by several sources: no infraction for who
  enters the exclusive lane on the right to turn right or to reach a property beside it, on the stretch marked
  with the continuity line; which is what MBST 5.5.1 above says of the marking itself.
- P. H. C. Eilers, "A Perfect Smoother", Analytical Chemistry 75 (14), 2003, 3631-3636, doi 10.1021/ac034173t:
  a smoother by penalised least squares after Whittaker (1923), its system sparse and banded, solved by a
  Cholesky decomposition (the paper's full text still to be read for the equations and the choice of lambda).

## Read (facts with file:line)
- native/src/sim/roads/road_grade.h:1-114: GradeProfile keeps four float lines a segment for good (h, ceil, base,
  raw: :61) besides q and read_half (:64-65): Proads-8; Structure {GROUND, ELEVATED, BRIDGE, TUNNEL} (:24);
  constants STATION_M 1.6, MAX_STATIONS 400, PLATE_MARGIN_M 2, CASING_BAND_M 1, FOOTWAY_RISE_M 0.15,
  UNDER_DECK_ROOM_M 0.4 (:43-48); RoadGrade: prof and node by id, seg_changed/node_changed/ground_changed/everything,
  solve(net, ground, moved, gone, jobs), at(s, arc), Scratch (:77-112).
- native/src/sim/roads/road_grade_internal.h:1-56: GradeTune (ground, ramp, deck, deck_tie, tunnel, clearance,
  structure specs), grade_chains, grade_inputs, station_index, ramp_rise, smooth_profile, slope_limit, dilate,
  RaisedEnds, solve_ground, solve_variable, solve_raised, solve_sunken, pin_plates: the reference's solver names one
  to one (Proads-10, Proads-15).
- native/src/sim/roads/road_grade.cpp (read whole, 219 lines): the ground read at 7 places across the casing
  (LATERAL :19), ceil = the highest + clearance, dilated 2 stations, raw = the mean + clearance (read_stations
  :85-105); grade_inputs :110-138: stations = clamp(ceil(len / 1.6) + 1, 2, 400), the ground read kept while curve,
  casing and ground stand, base = raw under a 36 m moving mean (SMOOTH_REACH_M :22) then slope_limit(grade.ground),
  plates = min(len x MOUTH_SHARE, plate_reach) (plate_reach :64-76: none at a joint that carries one road on, the
  margin 2 m at a transition, retreat + margin at a plain dead end, retreat + footway and casing + margin at a
  junction), manual when a node has a lift; grade_chains :140-209: a road laid in pieces through such joints gets one
  design line along all its pieces; RoadGrade::at :211-217: linear between stations, by arc.
- native/src/sim/roads/road_grade_profile.cpp (read whole, 352 lines): its shares and reaches :18-32 (RAMP_CURVE_M 8,
  VERTICAL_CURVE_M 4.8, PLATE_TANGENT_M 16, PLATE_TANGENT_SHARE 0.55, CREST_ROUND_RUN 3.2, CURVE_PEAK 1.5,
  TIE_REACH_M 28, TIE_MIN_M 0.4, TIE_ROAD_SHARE 0.4, PLATE_FADE_OUT 16, RAMP_RELAX 0.8, STEEP_SHARE 0.75,
  GRADE_SLACK 1.1) and the pass counts that decide results (SLOPE_PASSES 6, ENVELOPE_PASSES 4, PIN_ROUNDS 3:
  Proads-33); envelope :67-74 and slope_limit :179-186 are forward-backward sweeps repeated a fixed number of times;
  smooth_profile :167-177 sums each window afresh, O(n x radius); solve_ground :207-239 (the base tied to the nodes'
  heights over a tie length, pinned and slope-limited 3 rounds, grade breaks rounded by moving means, the plate
  tangent taken off, plate joins rounded); solve_variable :241-261 (an authored alignment: eased lifts);
  solve_raised :263-309 (the straight deck over ceil + deck + footway rise + 0.4 m, envelopes at grade.deck or
  grade.ramp, ramps from land ends by ramp_rise with a smooth-min crest, the ground floor relaxed towards a junction,
  caps, plates flat at the highest); solve_sunken :311-326 (eased ramps down to the wanted depth, envelope at
  grade.tunnel); ramp_rise :161-165 (a sag parabola over 8 m, then straight at grade.ramp); grade_tune and
  structure_spec :328-350 read the tuning (grade_*).
- native/src/sim/roads/road_grade_solve.cpp (read whole, 373 lines): MAX_SOLVE_PASSES 64 (:20); a node's grade =
  the mean of its at-grade roads' base at their plate edges + its lift, or the mean ground over a disc of 2 rings x 8
  points (balanced_at :66-85, grade_node :89-104); aloft when two or more roads meet and all are raised; an aloft
  node starts at the deck its spans want, capped by what a ramp from a land end climbs in its span (start_node
  :116-136); a sunken span wants the tunnel's depth under the lowest of its ground and ends (wanted_of :139-142);
  rounds :184-195: every reached profile solved, each end node raised to its highest end (capped), its roads solved
  again, up to 64 passes, then the plates pinned; reach_nodes :200-218: the changed ends, one ring further, and on
  along roads a node was carried up by; RoadGrade::solve :261-371: only what was marked (touched), the ground read
  over the job pool from 64 segments, `moved` = a touched profile whose shape differs bit for bit or one carried by
  a node past 1e-4 m (unshown_ keeps the rest), net.nodes[n].y written, the scratch put back.
- The reference, world/elevation.ts (1,566 lines; :1-660 read, the rest by its functions' list): the same steps in
  the same order as this game's three files: constants :44-89 (STATION 1.6, MAX_STATIONS 400, PLATE_MARGIN,
  LATERAL, DILATE, SMOOTH_REACH 36, TIE_REACH 28, PLATE_BLEND 16, RAMP_RELAX 0.8, RAMP_CURVE 8,
  VERTICAL_CURVE_REACH 4.8, PLATE_TANGENT_REACH 16, PLATE_TANGENT_SHARE 0.55, RAMP_CREST, CURVE_PEAK 1.5,
  MAX_SOLVE_PASSES 64); stations and plates :356-445 (reachOf :386-403, plates min(length x 0.45, reach));
  node heights :447-562 (balancedAtNode :523-544, gradeHeight :546-562); aloft :564-569; raised wants and caps
  :574-633; the rounds :635-680; readStations :1154, solveVariable :1189, solveGround :1219, roundPlateJoins :1283,
  roundGradeBreaks :1309, solveRaised :1331, rampRise :1408, smoothMin :1420, solveSunken :1426, flattenPlates
  :1469, gradeEnvelope :1486, descentEnvelope :1495, smoothProfile :1504, slopeLimit :1520, pinPlates :1535,
  dilate :1544. So Proads-10 and Proads-15 stand: this game's solver is that file, function for function. What the
  reference also has there and this game has elsewhere: the ground shaped by the roads (shapeAt :867-914: SHAPE_INNER
  1.2, SHAPE_SHOULDER 18, BATTER 2.5, SHAPE_SHOULDER_MAX 36, CUT_SHOULDER 10, SHAPE_DROP 0.6 :216-228), a solve by
  scope (SolveScope :173-209: the roads round the view first), summaries and digests.
- Case 2, this game (read whole): native/src/sim/commands/road_plan.cpp:185-216 plan_road: the curve, its length,
  the refusals (tool.failed, refuse.road.tooShort, refuse.road.selfLoop, hint.road.clearance,
  refuse.road.sharpAngle, road.reason.overlap, road.reason.fullNode, refuse.road.junctionsClose); the end's lift
  held to len x grade_authored (:194-198) and, when a connection cannot move, refused as "refuse.road.tooShort"
  (:199: the wrong word for it, and a free end clamped is never told); the price :213 =
  build_value(profile, len, RoadBuild{GROUND, lift_a, lift_b}): by the lifts alone, never by the ground under the
  road nor by the structure in hand. native/src/sim/economy/road_economy.cpp:30-52 build_lengths: a tunnel and a
  raised structure whole; at grade the shares of the length whose authored height lies under -fill_from (trench),
  within it (ground), up to on_piers_from (embankment), past it (bridge), on the straight line between the two
  lifts; build_value :54-59 = cost_per_m x the lengths x the structure's factors; charge_for :63-72 (refund share);
  edit_value_change :78-91 from the journal's ops. project/game/tools/road_draft.gd:37-40 measure: "120 m · R$
  150.000" only: no stretch named.
- Case 2, the reference (read): world/roads/buildMode.ts:13-21 buildModeAt(structure, lift = deck - natural
  ground): tunnel when lift < -TUNNEL_BORE, bridge when the structure is raised or lift > RAISED_LIFT (2 m),
  embankment at lift >= fillFrom, cutting at lift <= -fillFrom, else ground; stationsAlong :82-107 (the road in hand
  laid in a trial copy, its SOLVED deck read every step against the natural ground); buildRuns :110-124 (a stretch
  changes half way between two stations built different ways). main.ts:3397-3404: the label "120 m · R$ ... · ponte
  120 m · aterro 40 m · <the refusal's short reason>"; buildSummary :3539-3547 (stretches of 5 m or more, the longest
  first, in tens of metres, "cutting with walls" when chosen); drawBuildModes :3482-3520 (embankment and cutting
  filled between deck and ground, the cut's top line, a pier every 14 stations of a bridge); drawTunnelStretches
  :3522-3536 (the buried stretch dimmed); the height label "+3.0 m · 4%" with a small sketch of the line, in the
  warning colour when the grade limited it (:3418-3468, `limited` :3288-3297). world/structures.ts:29-53:
  TUNNEL_HEADROOM 4.4, TUNNEL_ARCH 0.6, TUNNEL_DEPTH = headroom + arch + 2, TUNNEL_CLOSE = headroom + arch + 0.4,
  TUNNEL_ROOF/BORE = close -/+ 0.16, TUNNEL_PORTAL_COVER 0.6, RAISED_LIFT 2, ROAD_GROUND_CLEARANCE 0.12; decks
  0.22 / 1.04 / 0.96 / 0.24. world/economy.ts:44-60 buildLengths and :160-169 chargeFor: this game's
  road_economy.cpp is these, line for line (the lab scan's 45%): to be this game's own in the block.
- Case 2, what is wrong here, by the reading: the preview prices the road as Structure::GROUND whatever is in hand
  (road_plan.cpp:213) while the commit prices the recorded structure (road_economy.cpp:21-27): a bridge shown
  affordable is refused at the click; no stretch is shown; a grade-limited free end is not told and a held
  connection is told "too short".
- Case 4, this game (read whole): native/src/render/roads/road_markings.cpp (176 lines): lines :42-69 (each
  MarkLine of the profile: colour, width by fast/centre/exclusive, the dash by the speed band, a solid centre drawn
  double on a multi-lane road by the style), crossings :96-133 (the zebra's bars centred on each carriageway, their
  mean colour at the far level, the stop line across the arriving lanes at stop_line_back), parking_paint :142-160
  (bay divisions, the band's dashed edge, the yellow no-parking line), emit_markings :163-174 (no lines at the far
  level: "filtered by the road's shader there"). No turn arrow, no legend, no word, no symbol anywhere in it.
- Case 4, the reference, world/markings.ts (547 lines; :1-280 read): the lines along a road are drawn by the
  carriageway's shader, not as polygons (AlongPaint :83-151: each line's offset, width, colour, dashed or not, the
  dash's frame so a split does not reflow it); a divider between two lanes of one way is solid, and wider (0.248
  against 0.128), where the lanes' uses differ (a bus lane) or the section says solid (:121-126); the centre line
  dashed, solid or double as the section says (:110-118); segmentMarkings :154-164 = the lanes' turn arrows and the
  bus lanes' legends; busLegends :166-189 (the word of the style along each bus lane, first 12 m from each end of
  the painted stretch, then every 70 m, letter = min(0.78 of the lane over the word's width, half the height));
  paintInLane :192-208 (strokes placed along the lane as the arriving driver reads them); approachLegends :213-254
  (at a junction of three legs or more, on each leg that arrives with a stop or a yield rule, except highways and
  ramps: the style's stop word or the yield symbol in every lane, 1.5 m clear of the stop line, its foot at least
  2 m into the road; the yield symbol 3 m long); laneArrowMarkings :257-299 (only on a road of 19.2 m or more, for
  each lane whose turn rule is not "all", its station 8.8 m before the end; a shaft 2 m back, the straight-on
  arrow 2 m ahead with its head, a hook to each side the rule allows, strokes 0.12 m wide following the curve);
  stopBar :313-335 (across the arriving half only); zebraBars :369-401 (bars 0.5 on a pitch of 1, 0.3 clear of each
  kerb, each side of a median; one quad a stretch whose bars the shader cuts); junctionDetailAt :416-437 (no paint
  at a junction with a highway or a ramp; a stop bar only where three legs or more meet and the leg arrives);
  midBlockDetailAt :440-469; transitionMarkings :472-522 (across a taper: the two directions parting round a
  median's nose, or one solid line in each road's colour on its half, and the lanes both roads have run on dashed);
  miniRoundaboutIsland :537-546 (one painted disc of 40 sides, radius 2 to 4 m, 0.4 of the nearest mouth). Not read
  yet: world/roads/markingStyle.ts (the words' strokes, the yield symbol, the styles), render/roadPaint.ts (the far
  filter of the lines).
- Case 4, the reference's styles, world/roads/markingStyle.ts (read whole, 86 lines): four regional standards
  (classic, br, us, eu): the centre line's colour (yellow, or white in eu; classic the class's own), the dash and gap
  by speed (br: 3+6 under 60 km/h, 3+9 under 80, 4+12 from 80; us and eu 3+9; classic 3.2+3.2), the bus lane's word
  ("ÔNIBUS" or "BUS"), the stop word ("PARE" or "STOP"), the legend's height (1.6 m, 2.4 m over 80 km/h); the
  letters are polylines in a unit box (its own drawings: not to be brought; this game draws its own letters), a
  gap of 0.35 of a letter between letters; the give-way symbol a triangle in a triangle.
- Case 6, the reference's names and numbers, world/roads/streetNames.ts (read whole, 141 lines): a street is a chain
  of roads (streetChain); its name is the text of the first name plate (by id) within 30 m of one of its roads
  (:57-66); a street with no plate is given "Rua", "Avenida" (avenue, boulevard) or "Rodovia" (highway, ramp) and a
  name from a list of 57 Brazilian street names picked by a hash of its id, one not in use (:69-96), the plate set at
  the middle of the road; numbers (:107-140): metres from the street's end nearest the map's zero, a segment's from
  and to, even on its right when it runs away from the start. Its quadratic walks (:131) are not brought.
- The reference's tuning, world/roads/tuning.ts (read whole, 85 lines): grade ground 0.12, authored 0.12, ramp 0.16,
  deck 0.05, deckTie 0.05, tunnel 0.13, refuse 0.35; clearance elevated 5.6, bridge 3; structure liftOn 0.6,
  pierFrom 2.8; tunnel autoCover 18; piers every 29.6 (elevated) and 38.4 (bridge); economy: start 20,000,000,
  per m2 carriageway 150, median 40, footway 80, structure x1 / 1.4 / 1.8 / 2.5 / 6, fillFrom 1 m, refund 0.25
  (this game's tuning.json holds its own of these already: to be compared key by key before the block).
- Case 6, the inspector: the reference, ui/inspector.ts:226-241: a road's title is "<its street's name, or Road> ·
  <the class's name>", and under its numbers a line "No. <from> to <to>" (streetNumbers). This game,
  project/ui/inspector_panel.gd:82-104 _road: the title is "Road · <profile>" (:83), no street name, no numbers;
  the names themselves exist here (native/src/sim/street/street_names.h: names_for, name_segment,
  settle_name_plates; the list and forms in street_furniture.json street_names, furniture_loader.cpp:149-157).
- Case 6, traffic from the map's edge: the reference, sim/vehicles/spawn.ts:86-158: an entry is a link of 12 m or
  more that starts at a node on the map's edge (doc.mapEdge); each entry has a Poisson stream (ENTRY_RATE x the
  traffic's intensity x the demand's multiplier, or a fill rate towards a chosen number of cars), its arrivals wait
  in a queue of ENTRY_QUEUE at most (the rest counted lost), the oldest waiting arrival anywhere is admitted first,
  ADMIT_PER_TICK a tick, while the fleet is under its target; spawnAt :167-226: at the lane's start when there is
  room behind what is on it and nobody on foot there, a bus never on a lane it cannot use nor at a junction none of
  whose movements its body takes; its trip ends at a map exit (:219, :228); none of it while the city's residents
  drive their own cars (:99-100) or the meso traffic feeds the entries (:101-102). Cars leave at a node on the edge
  (:296-318). This game: to be read (the demand is the lots': lot_demand.cpp; the far flows: street_flows.h).
- Case 5, this game (read whole): project/ui/road_profile_panel.gd (164 lines): the elements shown and picked
  (RoadSectionView), added from a palette, moved, widened by 0.5 m within the core's limits (road_spec_limits),
  removed; a lane's direction, bus use and solid line (:124-127), a footway's or a median's flush (:128-129); the
  road's speed by 10 km/h between 10 and 130 (:133-135, numbers in the interface: to the data); the problems asked
  of the core (road_spec_issues :97) and "use" disabled on any; _apply :146-153 = road_spec_make (refused past the
  core's count of custom roads: told only by command_failed); _save :162-164 = RoadModels.save whatever the
  problems. project/ui/road_spec_ops.gd (113 lines): with_width :82-89 widens every element EQUAL to the target
  (`e == target` compares two dictionaries by value: a footway and its twin of the same width are both widened)
  and every lane with a lane; insertion_index :41-78; no class, no carriageway material, no paving of a footway or
  a median, no centre line anywhere; no way to delete a saved road.
- Case 5, the reference, ui/roads/profileEditor.ts (754 lines; its controls listed by their labels): the saved
  roads as cards, a user's card with a delete button (:407, :422, deleteTemplate) and a blank card (:424); the
  picked element moved left or right (:483-485), removed (:487), its width by a stepper ("all lanes" for a lane
  :499; a fixed width told :505), a lane's direction (:509), use all or bus (:513), the line on its right dashed
  or solid, solid by rule beside a bus lane (:522-530), a footway's paving (FOOTWAY_MATERIALS) and flush (:538-540),
  a median's paving (MEDIAN_MATERIALS) and flush (:546-548); the road's class (:560), carriageway material (:562),
  speed (:565), centre line auto, dashed, solid or double (:572); each problem a line with a link to its element
  (:456); save under a name (:612-616); draw with it (:628) or apply it to the road (:639).
- Case 7, this game (read): native/src/sim/catalog/profile_types.h:22-29 Strip holds `bus`, `solid` and `turns`
  (left 1, straight 2, right 4; 0 automatic), :44-48 LaneInfo holds `bus`, `rail`, `turns`, :36-42 MarkLine
  `exclusive`; sim/roads/profiles.cpp:29 makes the line between two lanes solid where either says so or their bus
  use differs, :75 fills LaneInfo; the paint draws them (road_markings.cpp). The traffic reads NONE of it:
  native/src/sim/traffic/lane_graph.cpp:191-230 build_dir takes each lane's place and width alone (:210-214: no
  bus, no turns into DirSeg); link_pair :154-165 gives a turn class to the lanes lane_allows(l, lanes, turn) picks
  by the lane's index alone, unless the player linked the lane by hand (lane_by_hand :147-151); no reader of
  `.solid` or `.turns` anywhere under sim/traffic (grep). So a bus lane takes any car, a solid line stops no lane
  change and a lane's painted turns bind nobody. To read next: lane_allows, the lane-change rule (the model's
  files), how a vehicle picks its lane, what a bus is (v.bus), the reference's rules (world/roadSection.ts
  laneTurnAllowed, world/roads/rules.ts, sim graph laneUsable).
- Case 7, the reference's rules (read at their places): world/lanelets.ts:48-52 a lane may be `use: 'bus'` (buses
  only) and carry solidInner / solidOuter (no lane change across the line to the lane on that side: a solid line,
  or a bus lane's); :861-864 laneUsable(lane, shape) = not a bus lane, or the vehicle is a bus; changeTargets: the
  lanes of the same carriageway a vehicle may change into, across dashed lines only; the rule is asked when a car
  is put on the map (spawn.ts:172), when a way is searched (routing/router.ts:150, :156, :249: a movement whose
  arrival lane the vehicle may not use is no movement) and when a lane change is weighed (drive/tactical.ts:31);
  world/roadSection.ts:238-243 laneTurnAllowed(rule, turn): no rule or "all" allows everything, a rule never a
  U-turn, else the rule's own turns (left, through, right, leftThrough, throughRight); world/junctionBuild.ts:172:
  a movement from a lane its rule does not allow is not made. This game's lane_allows
  (native/src/sim/traffic/lane_graph.cpp:38-42): a U-turn from the last lane, straight on from any, right from lane
  0, left from the last: by the index alone.
- Case 7, this game's lane change and graph (read whole): native/src/sim/traffic/lane_graph_types.h:25-43 DirSeg
  (lanes, u[MAX_LANES 4], len, speed, stage, rank, must_stop, stop_back, cycle_shift, edge_shift, bikes, bend:
  nothing of a lane's use, line or turns), :48-69 Conn (from_ds, from_lane, to_ds, to_lane, turn, max_class,
  conflicts, forks); native/src/sim/traffic/lane_change.cpp (211 lines, MOBIL): choose :131-166 (a mandatory change
  one lane towards lane_toward(next) when the own lane does not reach the route's next road or towards the stop's
  lane, forced after FORCE_AFTER_S 5 s at the line; a change by choice to lane - 1 or lane + 1 when it reaches the
  next road and its incentive passes the driver's threshold, never within lc_free_m of the end nor on a bicycle),
  evaluate :89-114 (gaps, the body out of its lane before its leader, the new follower's braking, the politeness),
  change_lanes :175-208 (only the detailed vehicles, signalled first: lc_want, lc_lead).
- Case 6, the furniture a new road is built with: the reference, world/roads/furnitureSets.ts (read whole, 164
  lines) and furnitureCatalog.ts (28): the player picks a set for the roads drawn: complete, basic or none; basic =
  lamps (every 30 m from 6 m) and bins (every 60 m from 9 m); complete adds hydrants (every 100 m from 20 m, the
  right side only), benches (every 60 m from 24 m) and trees (every 10 m from 12 m; down a planted median every
  10 m instead, :125-136); on both sides in the furnishing zone, the left side's lamps staggered 15 m (:145), no
  lamp column on the side a power line's poles light (:148), nothing where the piece would leave under 1.2 m of
  clear walk (NBR 9050 6.12.3, :17-18, :151), each piece where the landscaping tool would accept it (snapLandscape).
  This game, project/data/catalog/street_furniture.json styles :169-220: the furniture follows the city's regional
  style, not a set picked for a road: roadcraft (columns every 30 m from 6 m, bins every 60 from 9, benches every 60
  from 24, rows ending 4 m short, hydrants by a reach of 100 m, no tree in the generated city), brazil (no bins or
  benches rows, hydrants by 300 m, trees in the generated city); the code: native/src/sim/roads/street_furniture.cpp
  (345 lines), street_furniture_places.cpp (95), street_furniture_posts.cpp (134): to read, with whether the road
  tool offers a set (project/ui/road_options.gd).
- Case 6, this game's furniture levels (read): native/src/sim/roads/street_furniture.h:18-20 (a segment's level in
  its flags: 0 the full kit, 1 basic, 2 none); street_furniture.cpp:322-333: any level but none gets its lamps and,
  with the later kinds asked, its hydrants and trees; only the full kit gets bins and benches (:332). The
  reference's basic is lamps and bins, and hydrants, benches and trees come with the complete set alone: so here a
  basic road gets hydrants and trees it should not and no bin it should. The fix is in that one place: the hydrants
  and the trees with the full kit, the bins with basic and full, the benches with full (bins_and_benches :240-259
  split in two). The road tool already offers the level (project/ui/road_options.gd:153-160, set_road_furniture).
- Case 4, the far lines here (read whole): project/render/shaders/road_body.gdshaderinc (217 lines; road.gdshader
  and road_far.gdshader include it): the paint is geometry (M_PAINT), dropped whole when the tier's detail cut-off
  sets `road_paint` 0 (:11, :123); at the far level alone (ROAD_FAR, far_pull > 0, :141-149) each lane strip paints
  a line along each of its own edges, box-filtered across the pixel (fwidth of the wear's across), in ONE grey
  (FAR_LINE_TONE 0.78 :54: a yellow centre line turns grey from afar) at ONE duty for dashed and solid alike
  (FAR_LINE_DUTY 0.6 :53), none along the carriageway's edge (:146); FAR_LINE_M 0.12 :52: three numbers in the
  shader. Between the near paint's end and the far level the dashes are quads with no filter (road_markings.cpp
  lines :59-62: a quad a dash).
- Case 6, what the inspector can read here (read whole): native/src/api/city_core_inspect.cpp:37-72 put_road (kind,
  id, profile, profile_id, lanes, one_way, speed_ms, width_m, cost_per_m, length_m over pieces_of, ends, node_a/b,
  lift_a/b, bend_m, bend_at, median: no street, no numbers); native/src/sim/street/street_names.cpp: a name item
  names the nearest segment within street_names.reach_m (name_segment :45-48) and its whole street
  (street_chain(net, seg) :116); NamePlates keeps by_item (seg, chain, plates, chunks), by_seg (the name items whose
  street holds a segment :125-127) and by_chunk; a given name is keys (GivenName form, name, ordinal: translated by
  the interface), picked by a hash of the segment's id among the style's names not in use (:30-41, :50-75);
  native/src/sim/roads/road_network_runs.cpp: pieces_of :9-32 (one road the player laid, through its cuts),
  settle_runs :34-85 (StreetRun: the run's first segment, this segment's start along it and the run's length,
  through joints only). So the street's name of a segment = the first name item (by id) of works.names.by_seg[seg],
  and its numbers come from where the segment lies along street_chain (to read: sim/street/street_spots.h
  street_chain: its order and whether it crosses junctions).
- Case 5, the editor's core (read whole): native/src/sim/roads/road_spec.cpp (175 lines): spec_issues :67-94 and
  its problems' names (noLanes, laneWidths, laneBalance, busOnly, median, order, width, footways, noFootways,
  parkingSide, class), spec_of :96-127 (a profile as elements; a kerb counted in what it borders), spec_strips
  :129-161, spec_centre :163-173 (the spec's own centre unless "auto": none with a median or one way, dashed for two
  lanes, solid for more); RoadSpec already carries `centre` and each element's `pave` (profile_types.h:50-66), not
  the carriageway's surface nor the class (the class is the base profile's); native/src/api/city_core_road_spec.cpp
  (120 lines): read_spec / write_spec :20-62 (kind, width, dir, bus, solid, flush, pave; speed, priority, centre),
  road_spec_limits :66-73 (lane, footway, median, band: no speed), road_spec_issues :80-92, road_spec_make :96-118
  ({ok, error, index}: "hint.road.specInvalid", "hint.road.variantsFull" when the catalogue holds no room);
  project/game/store/road_models.gd (32 lines): list, save (:20-27: any spec, problems or not), remove(key) :30-31
  (it exists: no button of the catalogue calls it; project/ui/road_catalog.gd to read).
- Case 2, the preview here (read): native/src/api/city_core_commands.cpp:65-88 plan_dict: the points of the preview
  are the ground's height plus the authored lift along the pieces (lift_along), never a solved line; it gives ok,
  error, price, length, crossings, half_width, asphalt_half, affordable (charge_of(price)), points, start, end;
  :60-62: the two ends are found by find_anchor with the lift in hand. native/src/sim/commands/road_snap.cpp
  find_anchor (:119-): a node is taken only when its lift is within grade.join_m of the lift given (`level`), so a
  start over a raised road with another height in hand snaps to nothing of it: the start must take the node's or
  the road's own height (the lift given left out for the first end, the anchor's lift read back).
- Case 6, a street's order: native/src/sim/street/street_spots.cpp:33-61 street_chain: the segment, then the ones
  on from its b end, then the ones back from its a end, each the straightest continuation of the same profile past
  edit.street_cos (one-way kept); the list is not end to end and does not say which way each piece runs: the
  numbers need the same walk giving each piece's start along the street and its direction (a function of
  sim/street beside it).
- Case 6, the demand here: native/src/sim/traffic/lot_demand.cpp (164 lines; :1-60 read): the trips follow each
  kind of place's hours (traffic.json people_working, people_purchasing; flows(kind, hour) :16-31), a bay drawn by
  those shares, then the nearest passing car; take_row :50- sends a parked car out. Nothing brings a car in at a
  road's end on the map's edge. To read whole before the design: lot_demand.cpp, lot_traffic.cpp (337),
  street_flows.h (106), flow_step.cpp (261).
- Case 6, where a flow starts and ends here (read whole): native/src/sim/traffic/street_flows.h (106 lines):
  StreetFlows by directed segment: start_rate (vehicles a second that want to start a trip on it), end_share (of
  what reaches its end, the share ending there), origin, send, room, owed, turns, curves; flow_rates.cpp (32 lines):
  start_rate = the demand's vehicles a lane metre x length x lanes x speed / trip_decay_m x side_weight(origin),
  end_share = min(1, length / trip_decay_m) x side_weight(end) (:27-28); lot_demand.cpp :97-161: lot_depart sends
  parked cars out while fewer drive than the target, lot_call sends the nearest passing car to a free bay;
  demand.cpp:114-116 side_weight, :118- make_vehicle (a detailed vehicle of the window with a trip of its own).
- What reads the grade (read): RoadNetwork::seg_y (road_network.cpp:240-245: grade.at(s, arc), else the straight
  line between the nodes' y); set_lift :247-252 marks the node and its roads; ray_deck
  (road_network_deck.cpp:28-32: prof.ylo, prof.yhi); RoadNetwork::cut (road_network_cut.cpp:14-31 heights_of: a
  reader's copy takes valid, structure, manual, walls, a, b, len, step, half, plates, h, ylo, yhi: never ceil, base,
  raw); sync_grade (sim/commands/road_grade_sync.cpp:60-82: the ground moved marks its roads, solve, each moved
  road's shape = its stations at their heights with half and kind (shape_of :27-47) put to the terrain, mark_ground,
  mark_heights, settle_sites); ground_of :19-24 (the natural height, the water standing as a floor, the sea);
  the probes (city_core_grade_probe.cpp, city_core_state_probe.cpp:101). So the solver's outputs others hold to are:
  prof[s].{valid, structure, manual, walls, a, b, len, step, half, plate_a, plate_b, h, ylo, yhi}, node[n],
  net.nodes[n].y, `moved`, `gone`, at(), has(), pending() and the four marks.
- The acceptance of case 1: project/tests/core/road_grade_test.gd (226 lines, read whole; the probe grade_probe
  solves a scene of nodes and roads on a named ground): a flat crossroads at the clearance exactly (1e-4); a
  crossroads over a hill: cut under -0.4 m and fill over 0.08 m, the earthwork within 4 m, the road's summed second
  differences under 0.6 of the ground's, the centre over 3.6 m and an end under 1.2 m, no grade past 0.14; an
  elevated span lands exactly on the ground roads it joins, reaches 0.9 of 5.6 m on a long span, never steeper than
  0.165 on a short one; its anatomy: each ramp within 40 m, the deck 0.6 of the span or more and level within
  0.1 m, no grade past 0.162; over rolling ground a straight deck (within 0.12 m of its chord) more than 3.2 m
  clear; landings sunk 0, 1.6 and 3.2 m; a span too short for two ramps lowers and never steepens; a bridge
  crosses a valley level; and RoadGradeSeamCases (to read). A design of this game's own must hold every one.
- The tuning here, project/data/catalog/tuning.json: grade :122-141 (ground 0.12, authored 0.12, join_m 0.3, ramp
  0.16, deck 0.05, deck_tie 0.05, tunnel 0.13, clearance_m 0.12, clear_elevated_m 5.6, clear_bridge_m 3, the decks
  0.22 / 1.04 / 0.96 / 0.24, tunnel headroom 4.4, arch 0.6, floor 2, lift_on_m 0.6), economy :12-17 (start
  20,000,000, refund 0.25, fill_from_m 1, on_piers_from_m 2.8): the reference's figures, with no source in the data.
