# t015-roads: specification 003, T015 and T016, the roads area

- Goal (the owner, 2026-11-13): "FAÇA O PRÓXIMO, FAÇA O QUE É NECESSÁRIO PARA ESSE JOGO SER FEITO DE UMA VEZ POR TODAS, SEM MAIS
  ENROLAÇÃO OU DESVIOS." The next of the port: roads, the main systems first.
- State: comparing both games by feature group (seven reading agents, the tracer on both sides); then the sheets, then the
  features built one block each.

## Sheet
- Copy scan (tools/gates/lab_scan.py, 9 s): 1806 findings, 79 strong (passages or numbers shared) in 54 files; in roads the grade
  solver (road_grade_profile.cpp 56%, the register's Proads-10 and Proads-15: elevation.ts translated, its names one to one),
  road_economy.cpp 45%, road_structures.cpp 56%, terrain/road_shape.cpp 56%; control advisor and flow stats translated (Proads-11).
- The grade (read whole, ~12:00): road_grade.h 1-97, road_grade_internal.h 1-46, road_grade.cpp 1-203, road_grade_profile.cpp
  1-321, road_grade_solve.cpp 1-351, city_core_grade_probe.cpp 1-103. Faults: four float lines a segment kept for good (Proads-8,
  ~390 MiB at 24 km), fixed pass counts deciding results (SLOPE_PASSES 6, ENVELOPE_PASSES 4, PIN_ROUNDS 3: Proads-33), node rounds
  up to 64, the reference's tuning shares (0.55, 1.5, 0.8...) as magic numbers. Consumers read h (at()), ylo/yhi, structure,
  plates, len/step/half (road_network.cpp:245, road_network_deck.cpp:30, road_network_cut.cpp:14, road_grade_sync.cpp:29, the
  state hash city_core_state_probe.cpp:101, the probe).
- Agent B (structures, checked where used): the drawn road's build modes (bridge, fill, cut, tunnel bands and their lengths) ABSENT
  (road_draft.gd:38-40 shows length and cost; preview heights from terrain and the height in hand, city_core_commands.cpp:72); no
  automatic tunnel for a deep cut (reference: under 18 m of cover after the solve); a short raised piece gets no bent
  (road_structures.cpp:178-188); prices and clearances already the same.
- Agent A (drawing): the grade-limited end clamped (road_plan.cpp:197-199) but never told (road_tool.gd:224-236, no string);
  hint.road.invalid missing from the CSVs; a start on a raised road does not take its height (road_snap.cpp:127); a short drag
  dropped silently (road_tool.gd:127); the alignment key loses the chain (road_tool.gd:146).

## Plan

## Next
- Paused (2026-11-13, the owner's question on the architecture): feature 1 (the grade solver in this game's design) was read,
  traced and researched (one look: road_grade_test 46 checks pass, 2.0 s; Whittaker-Eilers smoothing, Eilers 2003; exact
  two-pass envelopes, Felzenszwalb and Huttenlocher; vertical alignment as constrained least squares, UBC thesis 1.0400121);
  nothing coded, no block opened. Resume from its design once the owner settles the architecture's order.
