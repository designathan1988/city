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
- State: no block open. The tracer's sheet ran (report dist/port/roads.json; its output names differences for
  feature 1 only: the other features' rows in the sheet carry no file:line the tracer can start from, so each is
  read from its files directly). This game's grade code was read whole (the five road_grade files: the section Read
  below). In hand: the reference's world/elevation.ts (1,566 lines), world/rampRise.ts (100), then the consumers of
  the grade here; then cases 2, 4, 5, 6, 7 in both games (the reference: world/structures.ts, world/economy.ts 169,
  editor/roadTool.ts, editor/roadPath.ts, world/markings.ts 547, ui/roads/profileEditor.ts and profilePanel.ts,
  world/streetFurniture.ts, world/roads, sim/roads).
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
- What reads the grade (from doc/work/t015-roads.md, to be read again): at(), ylo/yhi, structure, plates,
  len/step/half: road_network.cpp:245, road_network_deck.cpp:30, road_network_cut.cpp:14, road_grade_sync.cpp:29,
  city_core_state_probe.cpp:101, city_core_grade_probe.cpp.
