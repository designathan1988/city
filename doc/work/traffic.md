# traffic: the complete traffic, ported in one block (doc/port.md, section 3)

## The owner's orders (2026-11-13, their words)
- "pega uma funcionalidade inteira e faça o porte completo ... funcionalidade não é um botão, é a funcionalidade inteira, por
  exemplo: trânsito completo". Trace with the tool, read the code whole myself (no agent), code everything with nothing run, at
  the end build, test, photograph, analyse, and a quick CPU, GPU and memory measure. "Trabalho caro": causes, not effects.
- Nothing may be lost to a compaction: this note is written before the code and as it goes.

## Done before the code
- One look: `python tools/tasks/cli.py run vehicle_motion_test` PASS 7.1 s (242 vehicles, 0 jumps, 0 vanish in view).
- Trace: `python tools/tracer/port.py sheet traffic` (28 s, dist/port/traffic.json). Cases: sheets/traffic.md rows 1-9.
- Read whole, reference (src/sim): vehicles/curvature.ts, driver.ts, laneChange.ts, archetypes.ts, spawn.ts:296-327,
  drive/operational.ts, physicalMotion.ts, tactical.ts, vehicleSignals.ts, params.ts, routing/router.ts:300-335.
- Read whole, here (native/src/sim/traffic): vehicle_speed.cpp, vehicle_motion.cpp, connector_motion.cpp, lane_change.cpp,
  vehicle_pose.cpp, lane_geometry.cpp, routing.cpp, astar.h, motion_law.h, traffic_system.h, traffic_state.h,
  catalog/traffic_params.h; parts: demand.cpp:140-200 (new_vehicle), lane_graph.cpp:44-70 (make_curve),
  lane_list.cpp:143 (move_vehicle_lane), traffic_system.cpp:18 (despawn), :150 (look bits), render/agents/agent_renderer.cpp:226-262,
  project/render/shaders/agents.gdshader.

## Facts the design rests on
- A vehicle's motion: step_lane (vehicle_motion.cpp:157) per lane, back to front: v0 = desired_speed; Lead l (gap, v) from
  lane_leads, lights, gate; acc = finish_accel(idm_accel(class, v, v0, gap, vl)); integrate. Connectors: step_connector
  (connector_motion.cpp:51), top = top_speed(v, conn_speed(c)).
- Turns: conn_speed (vehicle_speed.cpp:22) = exit limit when Turn::STRAIGHT else the fixed turn_speed_kmh (22); turn_limit brakes
  to it. No curvature anywhere; a road segment is one curve (RoadSegment::q, table) and lanes are offsets u of it.
- Every vehicle of a class has the class's a, b, headway, min_gap (traffic.json classes rows: a, b, b_emergency, headway,
  min_gap, speed_share, length, half_width); only vf (0.92-1.08) and a drift phase differ (demand.cpp:159).
- Lane change: change_lanes every lc_every_ticks; choose() -> evaluate(): own gain + bias > lc_threshold, new follower's braking
  under lc_safe_brake; no politeness; the move is at once (move_vehicle_lane slides lc_off in lc_duration_s); no signal.
- Leaves in view: step_lane:174 (no trip), leave_lane:252, :293 retry_route fails, :299 stuck > stuck_despawn_s.
- Router: astar over directed segments (routing.cpp:86), one connector per target segment (next_of); the lane is fixed later
  by mandatory changes; a route whose changes do not fit is still chosen.
- Render: AgentRecord::look bits (pose_buffer.h:39): type, paint<<8, brake<<16, empty<<17, aboard<<18 (2), door<<20 (5).
  INSTANCE_CUSTOM = (paint, roll + 8 braking + 16*door, passengers, 0). Indicator lenses exist as fixed-colour lens pairs
  (car_parts.cpp:145-146, trim.indicator), never lit.
- Reference faults not brought: a per-vehicle walk of sampled profiles each tick with WeakMaps (curvature.ts:22,84);
  a second A* for a plan starting with a change (tactical.ts:123); every vehicle walked in id order (laneChange.ts:52).

## Design, case by case (this game's names; values in project/data/catalog/traffic.json params, none in code)
A. Each driver different (row 4) and patience (row 9). `Vehicle::temper` (-1 timid .. +1 pushy), drawn once in new_vehicle as
   rng.f() + rng.f() - 1 (triangular). New `sim/traffic/driver.h`: `driver_of(p, v)` = the class row with a x (1 + temper x
   temper_accel), b x (1 + temper x temper_brake), headway x (1 - temper x temper_headway), min_gap x (1 - temper x temper_gap),
   the emergency brake the vehicle's own; `patience(p, v, base)` = base x (1 - temper x temper_patience), used at
   vehicle_motion.cpp gate (detour_after_s) and step_lane (reconsider_after_s); `politeness`, `change_threshold`.
   Used wherever a vehicle's own IDM is computed: step_lane, finish_accel, gate's commit_at, turn_limit, step_connector,
   box_hold, lane_change View::accel.
B. Following that anticipates (row 5). vehicle_speed.cpp `follow_accel(c, coolness, v, v0, gap, vl, al)`: the ACC blend of the
   improved IDM with the constant-acceleration heuristic (Kesting, Treiber, Helbing 2010, coolness 0.99: `follow_coolness`).
   `Lead` carries the leader's acceleration (`take(g, lv, la = 0)`): given at lane_leads, lead_beyond, connector_leads, sliders,
   fork_leads. finish_accel: the pedal moves at `jerk_up` easing on and `jerk_down` braking, a brake past the comfortable one
   or under `follow_crawl_ms` not held back (ISO 15622: 2.5 m/s3).
C. Slowing for bends (row 1). `Conn::bend` (1/m), the greatest curvature of its curve, set in make_curve. DirSeg `bend` (signed,
   centre line, in the ds's direction), `bend_from`, `bend_to` (the stretch at or over `bend_keep_share` of the peak), set where
   the ds is built. `bend_speed(p, v, k)`: lateral acceleration accepted falls with speed, a(v) = max(bend_lat_min,
   bend_lat_max - bend_lat_fall x v) (AASHTO Green Book 2011, side friction of low-speed urban streets: 0.32 at 15 mph to 0.15
   at 45 mph: 4.0 - 0.125 v, floor 1.4), x bend_heavy_share for the heavy body class, x (1 + bend_temper x temper).
   conn_speed = min(exit limit, bend speed); turn_limit brakes to the vehicle's own bend speed; step_lane caps v0 by the bend
   of its lane ahead and of the next segment (sqrt(vb^2 + 2 b d)); no per-tick walk of samples. turn_speed_kmh leaves the data
   if nothing else reads it.
D. Lane changes (row 2). evaluate(): MOBIL (Kesting, Treiber, Helbing 2007): own gain + politeness x (new follower's change +
   old follower's change) + bias over the driver's threshold. A change is signalled first: `Vehicle::lc_want`, `lc_lead`
   (seconds left of `lc_signal_s`), moved when the lead ran and the gap still holds; a forced one at once. Positioning: with a
   mandatory change pending, v0 capped at max(lc_crawl_ms, room left / (lanes to cross x lc_duration_s)).
   Indicators: `signal_side(s, v)` (sliding: the side it slides to; wanting: the side of lc_want; on a turning connector or
   within max(signal_min_m, signal_s x v) of its line before one: the turn's side) -> look bits LOOK_SIGNAL_SHIFT 25 (1 left,
   2 right) -> INSTANCE_CUSTOM.a -> a new AgentTag for the indicator lenses, lit amber on that side at 1.5 Hz (UNECE R48: 90
   per minute), agents.gdshader.
E. Cars leave only out of sight (row 3). `in_sight(s, v)`: within publish_radius_m of the window's camera point. In sight a
   car with no trip drives on by any connector its lane has (`wander`), a failed retry does the same, a stuck one stays; they
   leave once out of sight.
F. The mix (row 6). classes rows of bus and truck: a and b from published transit and heavy vehicle figures (TCQSM: bus
   1.2, comfortable brake 1.5; truck 0.9, 1.8). bus_share stays: the buses with no line are the transit area's row 1.
G. Lanes in the route (row 7). find_route over (segment, lane of entry) states: each connector out of the segment costs its
   time plus lc_duration_s per lane crossed to reach its lane, and is no way when the segment has no room for the changes
   (lanes x limit x lc_duration_s over its length less mouth_clear_m); the start state's lane is free (its callers check their
   own lane). One search, no second A*. astar gets a goal predicate (any lane of the goal segment).
H. Far traffic as flows, an adaptive window (row 8): NOT in this block. It replaces meso.cpp and window.cpp (the far model)
   and adds a render of far cars; it stays open under the traffic line.

## Coded so far (update as each is done)
- nothing yet.

## Next
- Code A, B, C, D, E, F, G in that order (files: driver.h new, traffic_state.h, traffic_params.h, traffic.json,
  vehicle_speed.cpp, motion_law.h, vehicle_motion.cpp, connector_motion.cpp, lane_change.cpp, lane_graph.cpp and where ds is
  built, lane_graph_types.h, routing.cpp, astar.h, demand.cpp, traffic_system.cpp, pose_buffer.h, agent_renderer.cpp,
  car_parts.cpp, agent_meshes.h, agents.gdshader, doc/map.md for driver.h).
- Then `block verify`, both DLLs, vehicle_motion_test, traffic_hash_test (its digest moves: new draws; bless with the cause),
  junction_sweep_test, records_traffic_test, window_traffic_test, street_crossing_test, transit_test, yard_test, lab_test
  (CPU, GPU, memory), photos near vehicles day and night, gates of the changed files, lab_scan.
