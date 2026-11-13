# Junctions and signals: the area's index (specification 003, T017)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present here: crossroads and T with fillets, bulbs and tapers; the roundabout tool; the automatic control choice and the flow
  advisor; main road first; stop from a standstill; swept conflict zones; exit claim by the longest waiter; turning on green while
  giving way; amber and all-red (by the Brazilian manual, richer than the reference's ITE clamp); stop-or-go at amber; fixed
  time, offset and green wave; mid-block crossings and median U-turns; leg rules; movement bans; signal settings.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | Gap acceptance by turn | PARTIAL, high | drivers look for a head within max(6 m, speed x 3 s) and claim after 8 s; the reference judges a critical gap by turn (4.5/4.0/5.5/6.5 s) scaled by the driver, shortened by impatience to a 3 s floor, any gap after 12 s, the conflict zones' clearing time counted | junction_gate.cpp:68, traffic.json:65; params.ts:61-74, admission.ts:906-960 |
| 2 | Signal stages from conflicts, protected turns | PARTIAL, high | opposite approaches paired only; the reference groups conflict-free approaches and adds a short protected-turn stage for turns never protected | junction_control.cpp:135-150; plan.ts:78-126,225-278 |
| 3 | Demand-driven signals | PARTIAL, high | every stage in order with min, extension, max; the reference ends a green early only when another stage calls, rests in green, skips stages without demand, picks by demand-weighted age with a 1.5-cycle starvation limit | signal_timing.cpp:69-88; fsm.ts:213-241,268-319 |
| 4 | Walkers' calls at signals | ABSENT | walker calls counted, a 45 s kerb wait served next, a green held up to 30 s while people cross; walk / flash / don't walk lamp (here a yes/no) | signal_timing.cpp:129-136, ped_crossing.cpp:81-93; fsm.ts:167,186-188,306-322, query.ts:83-93 |
| 5 | Bus priority at signals | ABSENT | no field in SignalSettings; its string unused | road_network.h:67-72, strings_b.csv:80; fsm.ts:70-71,201-211,253-258 |
| 6 | Drivers give way to people waiting at a zebra | PARTIAL | only to walkers already on it; the reference to those who would reach its path within 4 s | ped_crossing.cpp:138-157; admission.ts:1069-1100 |
| 7 | Ring metering and the zipper merge | ABSENT | rings can gridlock (the reference stops entries past 70% full); no zipper | cycles.ts:19-38; admission.ts:350-397 |
| 8 | Mini-roundabout and "none" control priority | PARTIAL | every leg yields and ties broken from the right (the reference: yield to traffic from the left); "none" becomes priority for the straightest pair, no highway-first rule | street_signs.cpp:94, junction_gate.cpp:28-35, junction_control.cpp:116,218-225; admission.ts:851-860,1113-1119 |
| 9 | Spillback and deadlock | PARTIAL | a tail rolling toward a signal credited as room; a jam released after 20 s one vehicle at a time (the reference avoids it) | junction_admission.cpp:49,132-148; spillback.ts:47-53, admission.ts:411-443 |
| 10 | The junction panel | PARTIAL | no control buttons, no automatic-choice note, no lock, no flow per leg, no main-road tag, no reset of leg rules; strings exist unused | inspector_rows.gd:45-48, strings_b.csv:93,95,102; junctionPanel.ts:65-125,161,167 |
| 11 | Turn classes, walkers-only stages, replanning after an edit | PARTIAL, low | about 45 degrees for straight, U-turn only on the same segment; walkers-only stage at mid-block only; state kept only when the stage count stays | lane_graph.cpp:32-35, junction_control.cpp:151,241-254 |

## Translated from the reference (redone in this game's design when built)

junction_admission.cpp:1-6 (exitRoom, boxClear, passHolder, claimExit, jamRelease, citing an old path); ped_crossing.cpp:1-5
(carsOnCrossing, pedsInWay, walkOk, PED_COURTESY); control_advisor.h:1-2 ("translated by the converter") and flow_control.cpp:1;
green_wave.cpp keeps the reference's structure. signal_timing.cpp, light_rule.cpp and junction_gate.cpp are this game's own.

## Reference faults never brought

String-keyed token maps rebuilt each pass (admission.ts:552-558); a Banker's test building sets per grant (admission.ts:674-783);
a held head's reservation chain re-derived each tick (admission.ts:276); string ids for crossings and connectors (admission.ts:993,
plan.ts:370-374, junctionBuild.ts:23); an array built and sorted per request (spillback.ts:13-24); the whole fleet walked and the
map searched on each edit (cycles.ts:37,54,217); demand asked for every stage each tick (fsm.ts:214-216,285); every connector of
the map searched (greenWave.ts:16).
