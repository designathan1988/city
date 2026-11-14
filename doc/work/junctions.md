# junctions: junctions and signals, ported in one block (doc/port.md, section 2)

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
  item por item, e escreva na nota o file:line onde cada item está feito ... APLIQUE AO QUE JÁ FOI FECHADO, antes
  de seguir adiante". Applied: roads' second block closed (391e7306) and the traffic's leftovers' block closed
  (03e3a9e7, pushed; doc/work/traffic.md); T036 and T037 had nothing left out.
- State (2026-11-14, after the traffic block closed): JUNCTIONS IS IN HAND, NO BLOCK OPEN. The one look with no
  block open is not spent. Read so far, both games: everything under "Read" below (this game's gate, control,
  timing, admission, crossings, signs, panels, flows' control; the reference's params, plan, fsm, admission,
  cycles, spillback, query, panel). Still to read: the list at the end of "Read". No design is written yet.
  Since the traffic block: the window of detailed traffic follows the view down to 350 m (window.cpp), a
  junction's flows are judged only inside it (flow_control.cpp look_at).
- The cases (doc/port.md; doc/specs/003-roadcraft-features/sheets/junctions.md has each with its evidence):
  1. gap acceptance by turn; 2. signal stages from conflicts, protected turns; 3. demand-driven signals;
  4. walkers' calls at signals; 5. bus priority at signals; 6. drivers give way to people waiting at a zebra;
  7. ring metering and the zipper merge; 8. mini-roundabout and "none" control priority; 9. spillback and deadlock;
  10. the junction panel; 11. turn classes, walkers-only stages, replanning after an edit.
- NEXT, in order: the tracer's report read; this game's files read whole (junction_admission.cpp, ped_crossing.cpp,
  light_rule.cpp, junction_sweeps.cpp, street_signs.cpp, green_wave.cpp, flow_control.cpp, lane_graph_types.h's
  Junction, road_network.h SignalSettings, inspector_rows.gd and inspector_panel.gd's junction part, traffic.json's
  junction keys); the reference read at the sheet's places (sim/junctions: params.ts, admission.ts, plan.ts, fsm.ts,
  query.ts, cycles.ts, spillback.ts; ui/junctionPanel.ts); facts written here with file:line; Context7 for each API
  and the internet for each value (critical gaps by turn: HCM / MBST; actuated control: MBST vol. V; pedestrian
  calls; bus priority; roundabout entry rules: CTB art. 29); each case's design written here; then
  `block open junctions`, every case coded with nothing run; the whole diff read; `block review`; `block verify`;
  the build; ONE run (the junctions' own tests named by `run tests --list`, a photo plan, zoom_ladder, e2e);
  lab_test; commit; push; `block close`.

## Designs (a draft written 2026-11-14 after the traffic block; each is finished, with its sources, before its code)
- 1 gap by turn (junction_gate.cpp may_enter :63-72, in place of the one look-ahead max(yield_near_m, v x
  yield_look_s)): a movement that gives way goes only when each outranking head (bound for the conflicting
  connector, not red, able to go) arrives at its line later than MY CLEARING TIME + MY CRITICAL GAP. Clearing
  time = (my distance to the line + the arc of my way past which I am clear of that connector: the other
  connector's conflict_clear entry for mine, junction_sweeps.cpp :245-254) over my pace through the turn.
  Critical gap = the turn's (data: through, right, left, U-turn; HCM's critical headways to cite) x the driver's
  own factor (temper, driver.h) less the impatience of the wait (a rate a second, capped), never under a floor;
  the wait's ceiling stays the claim (claim_after_s). A head standing at its own line with nowhere to go is
  skipped as now; two standing: first come as now. One read a head a tick, as now.
- 2 stages from conflicts (junction_control.cpp make_stages :135-150): the approaches are grouped as now (an arm
  and the arm most opposite it), then an approach left alone joins a stage none of whose movements any of its
  connectors conflicts with (Conn::conflicts across different approaches; the reference: plan.ts:225-278
  conflictFreeStages, greedy by the most opposite); each movement of a stage is PROTECTED when no green
  movement of another approach that outranks it (straight > right > left > U-turn) conflicts with it
  (plan.ts:87-97); an approach with a turn protected in no stage gets one more stage of its own (that approach
  alone green), run only on the demand of those turns, short (data: its target and its most; plan.ts:101-121),
  with no walk for its crossings.
- 3 demand-driven (signal_timing.cpp advance :69-88): a green ends from its least on when another stage has
  demand and its own has none (gap out), or at its target with another's demand, or at its most; with nobody
  else asking it rests in green; the next stage: one with demand not served for starve cycles first, then a
  crossing whose walkers waited past their limit, then the stage with demand of the greatest age weighted by
  what waits, a stage with no demand skipped (fsm.ts:214-241, :268-319). THE COST KEPT OUT: the demand of a
  stage is a counter kept as vehicles cross the detection section and leave the line (signal_call, the lane's
  head), never a walk of the lanes of every stage a tick.
- 4 walkers' calls: a (node, arm) keeps how many walkers wait there and since when (entered and left where a
  walker enters and leaves PedMode::WAIT at a crossing: ped_ways.cpp :76-88, despawn, fright); a stage's
  walkers' demand is its crossings' counts; a wait past ped_wait_limit_s is served next; a green is held while
  people are on its crossings (node_crossers), at most ped_hold_limit_s past its most; the lamp is walk / flash /
  don't walk (query.ts: walk while what is left protected covers the crossing at the design pace plus the start
  lag; flash when it does not; don't walk in any other stage): PedLamp in the pose frame for the render's
  walkers' heads if they exist (to read: signal_lamps), and walk_ok starts a walker only on walk.
- 5 bus priority: SignalSettings::bus_priority (saved under a new save version, older files false), the panel's
  switch (set_signal); where set: the green held up to bus_extension_s past its most for a bus within reach of
  its line on a green approach, cut after its least green for a bus waiting on another stage, that stage taken
  next (fsm.ts:201-211, :253-258); a bus near = a counter a signalled approach of the buses inside its detection
  reach (kept where signal_call is, vehicle_motion.cpp :205).
- 6 people waiting at a zebra: peds_in_way also gives way to the walkers WAITING at the kerb of a painted
  crossing the movement drives over, when the driver can still stop comfortably: at an uncontrolled crossing
  those waiting (the (node, arm) count of case 4), at a signal those whose lamp says walk; a driver who waited
  ped_courtesy_s is owed its turn (admission.ts:1069-1100).
- 7 ring metering and the zipper: a ring (the segments of a roundabout: Profile::ring arms; to read how the
  roundabout tool marks them) keeps the length of bodies on it against its storage; nobody enters from outside
  while the bodies on it plus the newcomer pass ring_meter_share of its length (cycles.ts:38-52), the count kept
  as vehicles enter and leave its lanes (no search a request). The zipper: where two lanes run into one (two
  connectors of different lanes into one lane at a node with no control), the heads take turns: a head leaves the
  next place to the other lane's head when it is that lane's turn and that head is ready (near its line or
  standing), for at most zip_patience_s (admission.ts:350-397).
- 8 mini-roundabout and "none": at CTL_MINI every leg gives way to who comes from the LEFT (who is on the
  circle), not from the right (junction_gate.cpp outranks :33-34 by the junction's side); at CTL_NONE nobody is
  ranked but a highway over the road it meets (CTB art. 29 III; Profile's class), else from the right.
- 9 spillback and deadlock: has_exit's room is where the tail's rear will come to REST when the exit lane runs
  into a signal or its queue is going to stand (each vehicle stops where what stands ahead holds it:
  spillback.ts:65-77), not the rolling tail's credit (junction_admission.cpp :49); a tail discharging elsewhere
  is room as now; jam_release stays the last resort.
- 10 the panel: the control's buttons (automatic, signal, signs, stop, mini, none) through a command of its own,
  the automatic choice's note and the lock / unlock, each leg's flow an hour and its main-road tag
  (FlowStats::rate; main_legs), the leg rules' reset, bus priority's switch; keys of this game's own.
- 11 turn classes, walkers-only stages, replanning: the turn's class by the angle with the reference's limits
  (lane_graph.cpp :32-35 to read against the reference's), a U-turn also between two segments nearly back on
  each other; a walkers-only stage for any crossing no vehicle stage can share (plan.ts:207-218), not only
  mid-block; a rebuilt plan keeps the stage that holds most of what was green, else goes through all-red
  (fsm.ts:363-402), not only when the stage count stays.

## The signal's controller as it will be coded (cases 2, 3, 4, 5, 11; decided 2026-11-14 with signal_timing.cpp,
## junction_control.cpp and the reference's plan.ts :76-323 and fsm.ts :150-330 read again)
- The reference's stages (plan.ts:225-278): groups that share a green have NO movement conflicting with another
  group's, so its stages are an approach group and whatever joins it conflict-free; a turn crossed by a green
  movement of higher rank is "permitted", and a group whose turns are protected in no stage gets a short stage of
  its own on their demand (:101-121). This game pairs opposite arms (make_stages :139-150): a left across the
  opposite straight is permitted the same way. KEPT: the pairing by the most opposite arm (the MBST's two-stage
  plan); BROUGHT: (a) an arm left alone joins a stage when none of its connectors conflicts with a connector of
  that stage's approaches (Conn::conflicts); (b) the protected mark and the turn stage; (c) a walkers-only stage
  for any crossing no stage can walk.
- Junction (lane_graph_types.h) gains, a stage: `turn_of` (the approach a turn stage serves, -1: an ordinary
  stage; a turn stage's demand is that approach's heads bound for a turn protected nowhere else), `walks` (the
  arms' segments whose crossing walks in it: no approach of the stage arrives by that arm and none of its
  straight movements leaves by it; a turning one gives way to the walkers: plan.ts:347-367), `served` (when its
  green last ended); and `bus_priority` (the node's setting).
- advance (signal_timing.cpp :69-88), an actuated junction: a green runs its least; then it ENDS when another
  stage has demand and (its own has none, or it reached its most), never while people are on its crossings
  (node_crossers of its `walks`) until ped_hold_limit_s past its most; with nobody else asking it RESTS in
  green. Bus priority, where set: held up to bus_extension_s past its most while a bus is inside the detection
  reach of one of its approaches; ended after its least for a bus standing on another stage. After the
  intergreen the next stage is: a stage with demand unserved for signal_starve_cycles x the cycle; else the
  stage whose walkers waited ped_wait_limit_s (the longest wait first); else the stage with demand whose age,
  weighted by its queue (1 + vehicles in its detection reach / signal_queue_weight), is the greatest; a bus's
  stage first where priority is set; a stage with no demand is skipped. A fixed plan runs as now (run_fixed).
- A stage's demand, with no walk of the map: for each of its approaches, a call still standing (green_wanted,
  signal_call) or a vehicle inside the detection reach of a lane (the lane's own list, from its head back: a
  handful); a turn stage: only heads bound for its turns; its walkers: the (node, arm) waiting counts of case 4.
  Asked only when a green may end or a stage is picked, at signalled junctions only (LaneGraph::signal_nodes).
- The lamp (case 4): walk while the stage walks that arm and what is left of its protection (to its most, the
  amber, the all-red) covers the crossing at the design pace plus the start lag; flash when it does not; don't
  walk otherwise (query.ts); walk_ok starts a walker only on walk.
- A rebuilt plan (replan_signal :241-254) keeps running the stage that holds most of the approaches that were
  green, at least its least green and its intergreen; with none, the junction goes through an all-red.

- Who waits at a kerb (cases 4, 6), decided with pedestrians.cpp :107-135, ped_ways.cpp :64-91 and
  ped_crossing.cpp read whole: a walker held at a crossing asks again 5 times a second (CROSS_RETRY_S, begin_line)
  and its own w.wait counts its seconds. So no enter / leave bookkeeping (a despawn, a fright would leak): a cell
  a (node, arm slot), beside cross_stamp's layout: `KerbWait {seen (the time of the last held ask), longest (the
  longest wait told since the cell was fresh)}`, written by begin_line where it holds a walker at a crossing
  (ped_ways.cpp :76), read as "somebody waits there, the longest for N seconds" while `seen` is within two
  retries. The signal reads it for a stage's walkers' demand and their wait; the gate (peds_in_way) for case 6.
- Case 6 in peds_in_way (ped_crossing.cpp :138-157): beside the walkers on the crossing, a vehicle gives way to
  somebody waiting at the kerb of a crossing its movement drives over (the arm it leaves by or enters by) when it
  can still stop comfortably before its line and has not waited ped_courtesy_s itself (courtesy_owed :28-31), at
  an uncontrolled crossing; at a signal only while that arm's lamp says walk (a turning movement and the walkers
  it must give way to). The walker's own check (approaching :34-47) lets it step off once the vehicle stands.
  A driver's temper decides who gives way (data ped_yield_temper: the pushiest do not): the reference's is a
  chance of 0.35 a request (params.ts:93-112), which flickers; a driver's own nature does not.
- The published walkers' lamp is two-state (pose_buffer.h:74 SignalRecord::walk, signal_lamps.cpp:48): it
  becomes walk / flash / don't walk (a byte), the render's lens flashing on `flash` (props.gdshader: to read).

- Cases 7 and 9, decided with junction_admission.cpp (read whole again) and admission.ts :340-408:
  THE ZIPPER: the lane that entered an exit lane last is remembered a lane (TrafficState::merge_last[exit lane
  key] = the lane of origin, written where a head's entry is admitted); in may_enter a head whose lane went last
  leaves the next place to the head of another lane bound for the same exit lane (rank not under its own) that
  is ready (within zip_ready_m of its line, or standing), while it has waited under zip_patience_s; the node's
  own connectors are walked (node_conn), nothing else. THE ROOM (has_exit :49): the rolling tail's credit only
  where the exit lane is discharging (its light green or no signal, its head moving); where it runs into a red
  or its head stands, the room is the lesser of the tail's rear and where the tail will rest: the stop line
  less the lane's vehicles x slot_m (a count, no walk). THE RING: each ring (the connected segments of
  Profile::ring) has a canonical segment kept a directed segment when its graph is built; its load a tick is
  read once (a stamp a ring): the bodies' lengths and gaps on its lanes against its lanes' length; a head
  entering from a road that is no ring is held while load + its own body pass ring_meter_share of it.
- CODING ORDER: the data and the params; the gate (1, 8, the zipper); the admission (9, the ring); the signal
  (2, 3, 4, 5, 11) with the kerb's cell and the lamp; case 6; the save and the commands (5, 10); the panel (10);
  the tests (a junctions test of its own: gaps, stages, calls, the lamp, the ring; the panel's rows) and the
  photo plan; doc/map.md.

## Sources (opened 2026-11-14; each value's words go to the data's `_doc`)
- Critical gaps by turn (case 1): HCM's base critical headways at a two-way stop, as the PTOE study guide's table
  gives them (open-exam-prep.com/study-guides/ptoe/managed-lanes-tsmo-its/unsignalized-intersections-roundabouts):
  major-street left turn "4.1 s (2-lane major) / 4.1 s (4-lane major)", minor-street right turn "6.2 s (2-lane
  major) / 6.9 s (4-lane major)", minor-street through "6.5 s (2-lane major) / 7.3 s (4-lane major)",
  minor-street left turn "7.1 s (2-lane major) / 8.1 s (4-lane major)"; follow-up 2.2, 3.3, 4.0, 3.5 s. The
  reference's are 4.5 through, 4.0 right, 5.5 left, 6.5 U-turn (its own, params.ts:61-66). DECIDED: the HCM's
  (a source): right 6.2, straight over 6.5, left across 7.1 for who comes off the lesser road; a left off the
  road that has the way 4.1; a U-turn as the left (no row of its own in the HCM: a choice).
- Impatience (case 1): the critical gap falls with the wait: A. Polus, S. Shmueli Lazar, M. Livneh, "Critical Gap
  as a Function of Waiting Time in Determining Roundabout Capacity", Journal of Transportation Engineering 129(5),
  2003, as the search gives its abstract: "The main hypothesis is that an increase in drivers' average waiting
  times reduces the average accepted gap and, consequently, the critical gap", "The model for a change in the
  critical gap was found to be logistic" (between a most and a least gap). No rate a second was found: the rate,
  its cap and the floor are choices (the reference's: 0.06 a second, 1.5 s, 3.0 s), said so in the `_doc`.
- Bus priority (case 5): RTIG, "How does traffic signal bus priority work" (RTIG-T065), as the search gives it:
  "Stage Extensions are where the current green stage is extended, usually to a preset maximum (typically 3-5
  secs)"; a study of limits "sometimes as little as 5 s" (Northeastern, 2025); the reference's 10 s is its own.
  The PDF itself did not open (over the fetch's size). DECIDED: bus_extension_s 5 (the source's most).
- Who has the way with no sign (case 8): CTB (Lei 9.503/1997) art. 29, III, as legjur.com and the commentaries
  give it: vehicles that, "transitando por fluxos que se cruzem, se aproximarem de local não sinalizado", the
  way is: a) "no caso de apenas um fluxo ser proveniente de rodovia, aquele que estiver circulando por ela";
  b) "no caso de rotatória, aquele que estiver circulando por ela"; c) "nos demais casos, o que vier pela
  direita do condutor". So: "none" = a highway's flow over the road that meets it, else from the right; a
  mini-roundabout = who is circulating (who comes from the left in right-hand traffic).
- The walkers' wait at a signal (case 4): no figure of MBST vol. V was reached (its appendix 5, "TESP: Tempo de
  Espera de Pedestres", is named in its index; the text did not open); a local rule as the press gives it:
  Belo Horizonte "o tempo máximo de espera em sinais vermelhos hoje é de 1 minuto e 30 segundos", São Paulo "a
  regra do um minuto e meio, limite de espera do pedestre nos semáforos da cidade". DECIDED: a walker's wait past
  ped_wait_limit_s is served next: 45 s (the reference's; half the cities' 90 s ceiling, so the ceiling is never
  reached at a junction with demand: a choice, said so).
- STILL TO SEARCH: the hold for people on the crossing (CTB art. 70, the walker who began has the way to the
  end), a ring's metering share, the zipper's rule (practice), a protected turn stage's length (MBST vol. V).

## Read (facts with file:line)
- native/src/sim/traffic/junction_gate.cpp (read whole, 123 lines): may_enter :39-75: a splice lets through; the
  light (light_stops :47); a Stop taken from a stand (must_stop and stood_at :48); room past the junction (has_exit
  :49); walkers in its way between the times its front reaches the line and clears the connector (peds_in_way :51);
  `claim` = wait > claim_after_s (:52), `can_stop` by claim_brake (:54); for each conflicting connector: busy
  (conflict_busy :57), a holder over its line (pass_holder :58), a claim made within claim_hold_s (:61); it yields
  to an outranking connector (outranks :28-35: the approach's rank, then the movement straight 2 > right 1 > left 0,
  then whoever comes from the right) whose lane's head is within max(yield_near_m, head.v x yield_look_s) of its
  line and takes that connector (:63-68), unless that head cannot go (:69) or both stand and this one waited
  FIRST_COME_S 0.3 s longer (:70-71). So case 1 stands as the sheet says: one look-ahead for every turn, no gap by
  turn, no impatience, no clearing time of the conflict zone. hold_reason :77-93; connector_to :102-114.
- native/src/sim/traffic/junction_control.cpp (read whole, 270 lines): arms_of :25-43, network_arms :49-66 (what
  the render reads); main_arms :72-95 (pairs of arms through_pair_dot apart, the highest lesser priority; the
  straightest pair for signs); decide :111-133 (a mid-block crossing or a U-turn node of two arms; under 3 arms
  NONE; CTL_SIGNAL; any other player control PRIORITY; a ring arm PRIORITY; auto_signal AUTO_SIGNAL / AUTO_ALL_STOP;
  before the flows speak: signal_arms arms or signal_lanes lanes give a signal, else by rank); make_stages :135-195:
  each arm with the arm most opposite it under stage_opposite_dot (:139-150: opposite approaches paired only: case
  2), one more stage for walkers at a mid-block node alone (:151: case 11), each stage's amber, all-red and least
  green (the detection's, the walkers' safety green :169-174), green_max = the cycle's equal share (:176-183), the
  player's fixed plan (:185-191), the start shifted by a hash of the node (:194); build_junction :199-227 (ranks
  RANK_MAIN 3 / RANK_YIELD 1 / the default 2; must_stop by the node's leg rule or the leg's plate); replan_signal
  :241-254 keeps the running stage only when the stage count stays (case 11).
- native/src/sim/traffic/signal_timing.cpp (read whole, 211 lines): amber_wanted :17-21 and intergreen :27-40 (MBST
  vol. V 5.7.1); detection :42-49 (the detection section and the least green of its queue); signal_call :51-57 (a
  vehicle crossing the section asks green until it reaches the line), signal_call_far :59-64; advance :69-88: every
  stage in order: its least green, then while calls want it up to green_max, then amber, all-red, the next stage:
  no stage skipped without demand, no rest in green, no call of the other stages read (case 3); run_fixed :92-109;
  compute_light :111-120; walk_signal :129-136: a yes or no (walkers may start while their clearance fits in the
  running stage's least rest): no call, no wait counted, no walk / flash / don't walk (case 4); file_signal
  :138-154 (the list of signalled nodes); update_lights :159-179 (only signalled junctions are timed and lit: no
  whole-map walk); next_green :193-208.
- native/src/sim/traffic/junction_admission.cpp (read whole, 195 lines; its header names the reference's functions:
  to be this game's own where the block changes it): is_splice :34-37; has_exit :39-67 (an exit claimed by another
  within CLAIM_FRESH_S 1 s; room = the tail's rear + min(tail.v, tail_credit_cap_ms) x tail_credit_s (:49: a tail
  rolling towards a red is credited as room: case 9), less the bodies inbound on connectors to that lane; a lane
  shut by a body off the lanes; a short exit behind a moving tail; a full lane by slot_m); pass_holder :69-75;
  conflict_busy :77-90 (a body on a conflicting connector, or just off it, not yet past the clearing place of the
  two classes: conflict_clear); box_clear :92-106; commit / release :108-119; claim_exit :121-130 (the longest
  waiter holds the exit's next spot); jam_release :132-148 (a junction nobody entered for jam_seconds lets one
  waiter of jam_seconds in, the box clear, release_every_s apart: case 9's "released one at a time"); admit
  :150-157; step_governor :174-192 (the fleet target eased while the share standing in full lanes passes gov_lo).
  No ring metering and no zipper anywhere in it (case 7).
- native/src/sim/traffic/ped_crossing.cpp (read whole, 168 lines): walk_ok :81-93 (at a signal: walk_signal's yes
  or no; elsewhere always yes), cars_on_crossing :95-136 (a head too near to stop, a body over the crosswalk, a
  head owed courtesy after ped_courtesy_s), peds_in_way :138-157: only walkers already crossing that node
  (t.node_crossers[c.node]: :143) whose line the lane meets, between the times the vehicle's front arrives and the
  walker clears (ped_span_reach_m, ped_critical_gap_s): nobody waiting at the kerb is given way to (case 6);
  front_time :159-165. One answer a (node, arm) a tick is kept (cross_stamp :60-79).
- The tracer's sheet (dist/port/junctions.json, its printed report read): the reference's junction code is
  src/sim/intersections (admission.ts 1,139 lines, claims.ts 208, crossingSpans.ts 240, cycles.ts 260, spillback.ts
  93), src/sim/signals (fsm.ts 430, plan.ts 433, permission.ts 43, query.ts 50), src/sim/crossings (permission.ts
  171, state.ts 35) and src/sim/params.ts (137): the sheet's paths (sim/junctions) are old. Feature 7 has no code
  here; the others exist in part.
- The reference, sim/params.ts (read whole): a vehicle asks to enter from REQUEST_TIME 3 s back, at least 8 m
  (:54-55); CRITICAL_GAP through 4.5, right 4.0, left 5.5, U-turn 6.5 s (:61-66); a waiting driver accepts a
  smaller gap: IMPATIENCE_RATE 0.06 a second up to 1.5 s off, never under CRITICAL_GAP_FLOOR 3.0 s (:69-71); after
  WAIT_CEILING 12 s it takes the next gap offered (:74); SIGNAL minGreen 6, baseGreen 20, maxGreen 34,
  exclusiveGreen 10, amber 3.2, minAllRed 1.2, starvationCycles 1.5 (:77-90); PED criticalGap 4.0, courtesyYield
  0.35 (the chance a driver yields to somebody waiting at an uncontrolled crossing), startLag 1.0 (:93-112).
- The reference, sim/signals/plan.ts (read whole): the stages of a signalled junction are sets of approach groups
  none of whose movements conflict with another group's (conflictFreeStages :225-278: greedy, by angle, the next
  group the one most nearly opposite that conflicts with none already in; two movements of one approach never
  count); where the conflicts are not known, opposite groups within 35 degrees (:281-315); each stage's protected
  movements: those no green movement of another approach that outranks them crosses (through 3 > right 2 > left 1 >
  U-turn 0: annotate :78-99, TURN_RANK :131); an approach with a turn never protected in any stage gets one more
  stage of its own, 10 s target and 16 s at most, that runs only on demand of those turns and shows no walk
  (:101-121); a crossing may walk in a stage when no green movement leaves from its leg and none goes through into
  it (a turning one yields to walkers: crossingsCompatibleWith :347-367); a crossing no stage can share gets a
  walkers-only stage (:207-218); the amber by Y = t + V / 2a (:57-75; this game times its intergreens by MBST vol.
  V already, richer).
- The reference, sim/signals/fsm.ts (read whole): a green may end from its least green on, never over people still
  on its crossings, held for them at most PED_HOLD_LIMIT 30 s past its maximum (:168, :186-188); it ends when
  another stage has demand and its own has none (gap out), or at its target green with another's demand, or at its
  maximum with another's demand (:214-241): with nobody else asking it rests in green; the next stage
  (pickNextStage :268-319): a stage with demand not served for 1.5 cycles first; else a crossing whose walkers
  have waited PED_WAIT_LIMIT 45 s; else the stage with demand whose age, weighted by what waits (1 + score / 6), is
  the greatest; else the next in order; a stage with no demand is skipped; bus priority, where the player set it:
  the green held up to BUS_EXTENSION 10 s past its maximum for a bus about to reach the line, cut after its least
  green for a bus waiting on another stage, and that stage taken next (:70-71, :201-211, :253-258); a rebuilt plan
  keeps the stage that holds most of what was green, else goes through all-red (:363-402). Its costs not brought:
  the demand of every stage asked every tick (:214-216, :285), maps keyed by strings.
- The reference, sim/intersections/admission.ts (1,139 lines; :330-460 and :840-1139 read): a movement that yields
  or stops must find a gap unless it has waited WAIT_CEILING (:331-337); hasAcceptableGap :906-946: critical =
  max(floor 3 s, the turn's gap x the driver's own factor - impatience (0.06 a second waited, at most 1.5 s)); for
  each movement it conflicts with from another approach that is not red: the head of that lane, bound for that
  movement, whose body can reach this one's (the swept zones), not standing at its own line unless protected:
  refused when its arrival (distance / max(v, 0.5)) is under clearTime + critical; clearTime :949-960 = (distance to
  the line + the place its body has left the last conflict zone) / (0.6 of its free speed, at least 0.8 m/s);
  yieldSide :1113-1120: at a mini-roundabout it gives way only to who comes from the left, where nothing is signed
  and no highway meets only to who comes from the right (comesFrom :1123-1130); row :846-874: stop, yield, the
  leg's sign, mini = yield, none: a highway goes before the road it meets (CTB art. 29 III), unsignalised: the
  road the junction is on first; the zipper :350-361, :394-397: where two lanes run into one, the lanes take
  turns: a head leaves the next place to the other lane's head when it is that lane's turn and that head is within
  ZIP_READY 8 units of its line or standing, for at most ZIP_PATIENCE 4 s; mergeFollows :364-381 (at a node of two
  legs a vehicle joins a merge behind a rolling holder with room for its body); people: crossingBusy :986-991
  (somebody on, or about to step onto, the stretch of zebra the movement drives over), crossingReachedFirst
  :1017-1044 (somebody on the zebra who reaches that stretch before the front is over it, CLEAR_MARGIN 1.2 and
  PED_REACH_TIME), pedestrianHasPriority :1069-1100: when the driver can still stop comfortably, people WAITING at
  the kerb of a painted zebra are given way to: at an uncontrolled one those who would be in its path within
  PED_REACH_TIME of stepping off, at a signal those whose lamp says walk; a driver who waited PED_COURTESY is owed
  its turn. Its costs not brought: string ids of crossings, sets and maps per request.
- The reference, sim/intersections/cycles.ts (read whole, 260 lines): a small closed loop of road (the shortest
  way from a link back to itself through connectors that are no U-turn, at most MAX_CYCLE_STORAGE 560 units of
  road: a ring, a roundabout drawn as a polygon); cycleFull :38-52: nobody from outside the loop is let onto it
  while the bodies on it (each its length and its standstill gap) plus the newcomer pass METER_SHARE 0.7 of the
  loop's length; the loops are kept by what each edit changed (CycleIndex). Its costs not brought: a Dijkstra a
  link, string keys of loops, sets a search.
- The reference, sim/intersections/spillback.ts (read whole, 93 lines): hasDownstreamStorage: a link shorter than
  the body and its gap holds nobody more while anybody is on or bound for it (:29-30); an empty lane accepts; the
  free room is the tail's rear; towards a signal, or where the lane's queue is going to stand, the room is where
  the tail's rear will come to REST (restingRear :65-77: each vehicle stops where what stands ahead of it holds
  it), less those already granted the lane (:47-53): a tail rolling up to a red is not counted as room; elsewhere
  a rolling tail is discharging.
- The reference, sim/signals/query.ts (read whole, 50 lines): a crossing's lamp: walk while its stage is green and
  the time it stays protected (what is left of the green's maximum, the amber and the all-red) covers its length
  at the design pace 1.0 m/s plus the 1 s start lag; flash when it does not; don't walk in any other stage.
- The reference's panel, ui/roads/junctionPanel.ts (read whole, 325 lines): the node's height by a stepper; for a
  junction of three legs or more: its control as one row of buttons (automatic, signal, signs, stop, mini, none:
  :24, :65-71); on automatic a note "automatic chose: <signal / priority / stop>" (:75-80), else "locked by you"
  with a button back to automatic (:81-89); each leg's flow in vehicles an hour, named by the way it points on the
  screen, tagged "main road" (:90-107), rewritten in place as the inspector refreshes (:207-217); under signs each
  leg's rule (priority, yield, stop) and a reset of the rules (:110-128); a signal's mode adaptive or fixed, each
  stage's green by 2 s, the offset, the green wave, a switch for bus priority (:130-172); the movements allowed, a
  row a leg, each turn a chip that bans or allows it (:174-203). sim/world.ts:1296-1311 busNear: a bus (by its
  shape) on a lane into the junction within BUS_REACH of its end whose next movement is of those groups;
  :1249- signalDemand (what a green for those groups would serve: the vehicles near the line and their score).
- This game's panel: project/ui/inspector_panel.gd (read whole, 290 lines) _node :146-155: the title, the live
  rows and lamps (InspectorRows.node_rows / lamps: inspector_rows.gd:61-82: control named by CONTROL_KEYS none /
  priority / signal, connections, movements, vehicles, queue, mean speed, capacity, volume, the cycle and stage),
  RoadPanel.point, the impossible-junction note, ApproachPanel.fill, ConnectorPanel.fill and, for a signal,
  JunctionPanel.fill (to read: road_panel.gd point, approach_panel.gd, connector_panel.gd, junction_panel.gd). The
  texts of the reference's panel exist unused in strings_b.csv:82-113 (junction.control.*, junction.locked,
  junction.unlock, junction.mainRoad, junction.perHour, junction.rulesReset, junction.busPriority,
  junction.mode.*, junction.legs, junction.movements): they are the reference's keys (saves-interface's last case
  renames keys area by area; this block uses keys of this game's own for what it adds).
- project/ui/junction_panel.gd (read whole, 53 lines): a signal's mode (adaptive / fixed, one button), each stage's
  green and the offset by hud.json signal_step_s, the green wave (dispatch green_wave); set_signal [id, fixed,
  greens, offset]: no bus priority. project/ui/approach_panel.gd (read whole, 54 lines): each leg named by the way
  it points on the screen (:12-19), its rule by HudWidgets.choices (set_approach_rule; SegRule 0 priority, 1 stop,
  2 yield), the movements as chips (set_movement_banned): no reset of the rules, no flow a leg, no main-road tag.
  project/ui/road_panel.gd point :42-57: a joint's height, crossing removal, join. So of case 10 here: the leg
  rules, the movement chips and the signal's timing exist; missing: the control's buttons, the automatic choice's
  note, the lock and unlock, each leg's flow and main-road tag, the rules' reset, bus priority's switch.
- native/src/sim/roads/road_network.h (read whole, 250 lines): NodeControl CTL_AUTO, SIGNAL, PRIORITY, STOP, YIELD,
  NONE, MINI (:64); SignalSettings {fixed, offset, greens} (:67-72: no bus priority; its operator== and the save
  read it); RoadNode ctl, auto_signal (-1 nothing yet, 0 none, AUTO_SIGNAL 1, AUTO_ALL_STOP 2: derived, never
  saved: :86-87), signal, crossing, uturn, links; SegRule NONE / STOP / YIELD in the segment's flags (:45).
  lane_graph_types.h:80-94 Junction (ctl, green, green_max, amber, all_red, least, approaches a stage, cycle, run,
  run_start, run_green, fixed, offset, settle, degree). The commands that exist: a control set by road_tabs.cpp:139
  (j_set_control: the Control tab cycles it), set_signal and green_wave (signal_commands.cpp:20, :35; api
  city_core_edit.cpp:43-52), set_approach_rule (street_commands.cpp:71), node_legs (city_core_lanes.cpp:65),
  node_traffic (city_core_inspect.cpp:214), the node's inspect dictionary (city_core_edit.cpp:143, :163: ctl).
- native/src/sim/traffic/flow_control.cpp (read whole, 165 lines; its header and control_advisor.h say "translated
  by the converter", with core/js_map.h: the sheet lists it to be redone; whether in this block: to decide in the
  design, the panel only reads it): count_entry :109-113 counts each entry by (node, arriving segment);
  FlowStats::rate(n, seg, tune) gives a leg's flow (:35: what the panel's "veh/h" a leg needs); look_at :91-105:
  an automatic junction of three arms or more inside the detailed window is judged by its main road's flow and its
  side roads' (advisor->evaluate), the choice AutoControl PRIORITY / SIGNAL / STOP; apply_flow_control :125-138
  writes node.auto_signal (derived, never saved). The panel's note "automatic chose" reads auto_signal.
- native/src/sim/traffic/connector_motion.cpp (read whole, 106 lines): a vehicle on a connector follows the one
  ahead, a body on another branch of its fork, a shut stretch of its exit, walkers on the exit crosswalk
  (peds_in_way :41) and the lights down its route; box_hold :86-103. may_enter and jam_release are called from the
  lane's step (vehicle_motion.cpp: to read).
- native/src/sim/traffic/vehicle_motion.cpp (read whole, 291 lines): step_lane :129-214: only a lane's HEAD
  arbitrates at its junction (:188-196): gate :70-114: at its line (at_line_m) a Stop is taken from a stand
  (stood_at :76); waited patience x detour_after_s with a full exit: a detour (:78-81); waited claim_after_s and no
  red: it claims the turn (conn_claim_t :82); waited starve_seconds: claim_exit (:83); waited jam_seconds:
  jam_release (:84); a commitment (v.pass) is taken back while the nose is behind the line when the light recalls
  it or its exit filled (:87-98), else it goes with box_clear; else may_enter (:104), and within its braking
  distance of the line (commit_at :105) it commits only into a clear box (:107-113); signal_call as it drives over
  the detection section (:205); admit counts the entry (:208). v.wait counts the seconds under WAIT_SPEED 0.5 m/s
  (:56). So case 1's gap rule belongs in may_enter (junction_gate.cpp:63-72), read once a head a tick.
- native/src/sim/traffic/ped_kerb.cpp (read whole, 115 lines): the waiting at a kerb: an approach point, then a
  queue leg along the way across, at whose end the walker "asks for the crossing (pedestrians.cpp)"; no count of
  who waits at which crossing nor since when is kept here (to read in pedestrians.cpp / ped_crossing callers: the
  mode a walker waits in, PedMode, and where walk_ok is asked): cases 4 and 6 need, a (node, arm): how many wait
  and the longest wait, kept as the walkers enter and leave the waiting mode (no walk over the walkers a tick).
- native/src/sim/traffic/street_signs.cpp (read whole, 104 lines): leg_rule :92-95: a Stop on every leg for
  CTL_STOP or the flows' all-stop; a yield on every leg for CTL_YIELD and CTL_MINI (:94: the mini-roundabout is
  only "all give way": no rule of the side: case 8); arm_plates :23-48 puts the rule's plate on each arm that
  gives way; plate_rule :97-101.
- native/src/sim/traffic/ped_ways.cpp (read whole, 126 lines) and ped_mode.cpp (44, the one setter enter_mode and
  PED_MODES): begin_line :64-91: a walker whose next line is a crossing (`to.cross` = the arm's segment, at
  w.way_node) asks walk_ok and cars_on_crossing (:74); held, it enters PedMode::WAIT (:76-78) and asks again each
  time begin_line is called for it (pedestrians.cpp:124-134: while it waits; w.wait counts its seconds:
  ped_acts.cpp:57 zeroes it out of WAIT); let over it enters WAY, w.cross_seg = the arm and it joins
  t.node_crossers[way_node] (:86-88); off_crosswalk :42-48 takes it off. So who WAITS at a crossing is: mode WAIT,
  way_node, ways[wi].cross: the place to count a (node, arm)'s waiters and the oldest wait is where a walker
  enters WAIT (:77) and where it leaves it (:84, a despawn, a fright): a counter and a since-time a (node, arm
  slot) in the crossing cache's own layout (cross_stamp's key: ped_crossing.cpp:62-66), no walk of the walkers.
- STILL TO READ before the design: road_tabs.cpp (the control command), the advisor's choice (control_advisor.h),
  city_core_lanes.cpp node_legs,
  city_core_inspect.cpp node_traffic and put_node, the walkers' kerb waiting (ped_kerb.cpp: who waits at which
  crossing, for case 4 and 6), junction_sweeps.cpp (conflict_clear: the clearing places, for case 1's clearing
  time), connector_motion.cpp (where may_enter and jam_release are called), traffic.json's junction keys, the tests
  of the area (`run tests --list`: junction, signal, flow_control, driving), the save of SignalSettings
  (save_read.cpp / save_write.cpp), the reference's crossings/permission.ts and claims.ts.
- native/src/sim/traffic/junction_sweeps.cpp (read whole, 260 lines): find_conflicts :196-257: each connector's
  way swept by the three body classes; a pair that meets keeps, per pair of classes, `conflict_clear`
  [theirs x BODY_CLASSES + mine] = the arc along THAT connector's way (0 at its curve's start, negative on the
  lane before) past which a body of `theirs` there is clear of mine (:245-254). So case 1's clearing time is
  already in the graph: my own way has left the conflict with connector k once my body's middle is past
  k's entry for me (the clear arc kept on k's side for my connector), no new sweep needed. max_class :212.
  lane_graph_types.h (read whole, 123 lines): DirSeg stage, rank, must_stop, stop_back, detector, extension
  (:32-39); Conn turn, conflicts, conflict_clear, forks (:57-78); Junction green, green_max, amber, all_red,
  least, approaches a stage, cycle, run, run_start, run_green, fixed, offset, settle, degree (:80-94);
  LaneGraph signal_nodes, signal_at (:104-107). The area's tests (run tests --list): flow_control_test,
  junction_sweep_test, street_crossing_test; e2e's amber and red checks.
- Who reads the signal's walk and calls (grep, 2026-11-14): walk_signal's one caller is walk_ok
  (ped_crossing.cpp:89); walk_ok is asked by ped_ways.cpp:74 and by the publication of the lamps
  (traffic_system.cpp:74: {ds, light, walk_ok}: a yes or no to the render: the walkers' lamp is two-state);
  node_crossers (traffic_state.h:124; filled ped_ways.cpp:37, :88, emptied :45; sized lane_graph_edit.cpp:132,
  pedestrians.cpp:208); green_wanted (traffic_state.h:118); a far bus waits by next_green and calls by
  signal_call_far (meso.cpp:250-253). Profile::ring marks a roundabout's segments, Profile::priority 0 local to
  5 highway (profile_types.h:92-94); the roundabout tool: commands/roundabout.cpp (215 lines, to read for case
  7). traffic.json's junction keys: signal_* :40-62, yield_look_s 3, yield_near_m 6, claim_after_s 8,
  jam_seconds 20, starve_seconds 30, tail_credit_s 1.5, tail_credit_cap_ms 6, ped_courtesy_s 8,
  ped_critical_gap_s 4, claim_hold_s 0.6, claim_brake 3, signal_extension_s (:450).
- native/src/sim/traffic/light_rule.cpp (read whole, 111 lines; this game's own): amber_stops :43-48,
  light_recalls :50-63, light_stops :65-67, lights_ahead :69-108 (the stop a vehicle sees down its route).
