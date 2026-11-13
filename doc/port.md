# The port's board: every feature, in order

The one list of what the port has to do and where it stands. **The next feature is the first `- [ ]` line.** A line is checked
(`- [x]`) in the commit that passes the feature's checks; `- [-]` is parked, with why. Nothing else says what is next: no other
list, no other order. `python .claude/hooks/port_state.py` prints the next line, the open block and its work file.

A line names its area's sheet and its row there (`doc/specs/003-roadcraft-features/sheets/<area>.md`, row N): what the player
meets, what differs from the reference, the evidence on both sides with `file:line`; the port tool reads that row: `python tools/tracer/port.py sheet <area> --feature N`. How a feature is done: `CLAUDE.md`,
"One feature, start to end". An order of the owner in the conversation goes before this list.

## 0. The backbone the port lands in (doc/specs/003-roadcraft-features/architecture.md)

- [x] T036 errors seen: an engine ERROR or WARNING fails a test; every refusal told in one place; the city kept on a failed save (02beaa45)
- [x] T037 records kept true: record invariants after every command and at every test's end; a removal reaches every table (90b67e27)

## 1. Roads (sheets/roads.md; work so far: doc/work/t015-roads.md)

- [ ] roads 1: the heights of the roads, solved in this game's design (already read and researched, nothing coded: what was found is in doc/work/t015-roads.md)
- [ ] roads 2: laying a road, what it will be and cost (bridge, fill, cut, tunnel stretches and their price in the draft)
- [x] roads 3: the network's links and bans kept through edits (T037, 90b67e27)
- [ ] roads 4: markings (turn arrows, bus legends, words, plate marks; far dashes and colours)
- [ ] roads 5: the profile editor
- [ ] roads 6: streets and their life (traffic from the map's edge, street names and house numbers, the basic furniture set)
- [ ] roads 7: lanes that obey the road (bus lanes, solid lines, per-lane turns read by the traffic)

## 2. Junctions and signals (sheets/junctions.md)

- [ ] junctions 1: gap acceptance by turn
- [ ] junctions 2: signal stages from conflicts, protected turns
- [ ] junctions 3: demand-driven signals
- [ ] junctions 4: walkers' calls at signals
- [ ] junctions 5: bus priority at signals
- [ ] junctions 6: drivers give way to people waiting at a zebra
- [ ] junctions 7: ring metering and the zipper merge
- [ ] junctions 8: mini-roundabout and "none" control priority
- [ ] junctions 9: spillback and deadlock
- [ ] junctions 10: the junction panel
- [ ] junctions 11: turn classes, walkers-only stages, replanning after an edit

## 3. Traffic (sheets/traffic.md)

- [ ] traffic 0: the tracer's empty `--cost` on `city.traffic.step` (tools/tracer/tracer/cost.py), needed by this area's sheets. Done when: `python tools/tracer/trace.py city.traffic.step --cost --root native/src/sim/traffic` prints the loops; the tracer's tests pass
- [ ] traffic 1: slowing for bends
- [ ] traffic 2: lane changes (politeness, indicators, positioning)
- [ ] traffic 3: cars leave only out of sight
- [ ] traffic 4: each driver different
- [ ] traffic 5: following that anticipates
- [ ] traffic 6: the mix of vehicles
- [ ] traffic 7: lanes in the route
- [ ] traffic 8: far traffic as flows; an adaptive detailed window
- [ ] traffic 9: re-routing by patience

## 4. Zones, lots and growth (sheets/zones-lots-growth.md)

- [ ] zones-lots-growth 1: growth's order and pace
- [ ] zones-lots-growth 2: the 24 km city opens fast (research.md R4: a candidate's look from its massing; `fill_all` in a 2 x 2 checkerboard of 256 m cells over the job pool, the same city for any pool size; `settle_sites` in parallel); `window_traffic_test` under 60 s with the same check
- [ ] zones-lots-growth 3: rezoning keeps the building
- [ ] zones-lots-growth 4: lots cut to the land and kept
- [ ] zones-lots-growth 5: buildings that age into decay
- [ ] zones-lots-growth 6: a polygon lot's building fits its outline; set-backs and lower densities tried; eras by quarter
- [ ] zones-lots-growth 7: industrial densities; the generator's parks

## 5. Transit (sheets/transit.md)

- [ ] transit 1: random city buses stop to board
- [ ] transit 2: the transit tools' help
- [ ] transit 3: picking and previewing without whole-plan walks
- [ ] transit 4: trains redesigned off the reference's costs
- [ ] transit 5: buses leave only out of sight
- [ ] transit 6: a save's lines clamped on load; trains kept through an edit only when their track is the same
- [ ] transit 7: transit vehicles culled by the view
- [ ] transit 8: bus lanes (with roads 7)

## 6. Buildings (sheets/buildings.md), with the people's and the landmarks' own cook (Pbuildings-43)

- [ ] buildings 1: drawing a building from a shape, as a new building, a wing, a block on top or a cut
- [ ] buildings 2: the numeric inspector
- [ ] buildings 3: buildings that age and are renovated
- [ ] buildings 4: roof shape (pitch, ridge, the side a shed falls to)
- [ ] buildings 5: copy, paste, duplicate, mirror, group, delete the selected building
- [ ] buildings 6: grown buildings stepping down a slope, the garage under a house
- [ ] buildings 7: paint presets, swatches, custom colour, trim
- [ ] buildings 8: facade zone scope; patterns following the scope
- [ ] buildings 9: snap modes, hide others
- [ ] buildings 10: landmarks sized to the building; party walls between neighbours

## 7. People (sheets/people.md and sheets/people-*.md; doc/history/people.md)

- [x] people: where people walk (sheets/people-walking.md)
- [x] people: casualties and the body that falls (sheets/people-casualties.md)
- [ ] people: what people do, the motions and activities (sheets/people-motions.md; needs the cook of buildings, Pbuildings-43)
- [ ] people: the cast (sheets/people-cast.md)
- [ ] people: what people hold (sheets/people-held.md)
- [ ] people: faces and their expressions (sheets/people-faces.md)
- [ ] people: how people are drawn, the body and its detail levels (sheets/people-drawing.md)
- [-] people: the person maker (justified in sheets/people-maker.md: a half-done remnant of the reference, not brought)

## 8. Terrain (sheets/terrain.md)

- [ ] terrain 1: the relief brush's feel
- [ ] terrain 2: rivers that follow the land
- [ ] terrain 3: the ground's colour from the ecosystem
- [ ] terrain 4: natural relief at map start
- [ ] terrain 5: the ecosystem reads all the land
- [ ] terrain 6: water flows
- [ ] terrain 7: landforms with their outline
- [ ] terrain 8: ground paint blends
- [ ] terrain 9: the ground beside a road at the footway's top

## 9. Street furniture (sheets/street-furniture.md)

- [ ] street-furniture 1: the basic set
- [ ] street-furniture 2: lamps' beams at night
- [ ] street-furniture 3: street trees that grow, and down planted medians
- [ ] street-furniture 4: detail levels by size on screen
- [ ] street-furniture 5: rows placed by hand along the curve
- [ ] street-furniture 6: speed plates by the marking style, none on highways
- [ ] street-furniture 7: signal heads' sway and halos

## 10. Effects and sky (sheets/effects-sky.md)

- [ ] effects-sky 1: vehicle exhaust and road dust
- [ ] effects-sky 2: lamp beams in rain at night
- [ ] effects-sky 3: canyon light through the day
- [ ] effects-sky 4: cloud shadows by the light
- [ ] effects-sky 5: poles and wires in a blast; a signal post knocked out
- [ ] effects-sky 6: the bulldozer's reach
- [ ] effects-sky 7: smoke with the weather's wind
- [ ] effects-sky 8: small differences

## 11. Saves and interface (sheets/saves-interface.md)

- [ ] saves-interface 1: the player told when a city cannot be read or a save fails (the failed save is T036's, done; the unreadable city is open)
- [ ] saves-interface 2: autosave silent
- [ ] saves-interface 3: the inspector's parking and street numbers
- [ ] saves-interface 4: a new map undoable, or the old city kept
- [ ] saves-interface 5: the session with the city
- [ ] saves-interface 6: the help and small defects
- [ ] saves-interface 7: this game's text keys stop being the reference's own: LAB_COPY and lab_scan read the text tables (tools/gates/gate_lab.py, lab_scan.py, rules.json), then the keys the reference defines renamed area by area in project/data/i18n/*.csv with every `tr("KEY")` and data `*_key`. Done when: the gate's fixtures prove the new finding; `python tools/gates/lab_scan.py` finds no reference key

## 12. The camera and the rest (sheets/camera-and-rest.md), last

- [ ] camera 0: the eye never in a building, no jolt: research.md R1b built by contracts/camera-solids.md (the camera floor as the core's derived tiles, `eye_clear`, the point's rest over the floor by the zoom weight, the pull unchanged, values in camera.json), its sheet first with the proof before code R1b names. Done when: `python tools/tasks/cli.py run camera_feel_test camera_input_test look_view_test` passes, each under 60 s (eye inside walls at most 1 frame in 1,000; no frame's jolt past 1% of the view outside the counted push-outs; push-outs at most 0.1% of frames); the frozen street view (`run photos`, 02-street) shows a street with the eye outside every building
- [ ] camera-and-rest 1: the camera kept over the map
- [ ] camera-and-rest 2: saves in the folder Steam syncs
- [ ] camera-and-rest 3: reset view frames the map; perspective/isometric remembered; the building tool's frame selection and top view
- [ ] camera-and-rest 4: the camera's timings
- [ ] camera-and-rest 5: problems logged on the player's disk
- [ ] camera-and-rest 6: Steam achievements
- [ ] camera-and-rest 7: a name per person
- [-] camera-and-rest 8: the lab's developer tabs (justified in the sheet: developer-only, not brought)

## 13. After the port: what the genre has and neither game does (doc/specs/003-roadcraft-features/genre-gaps.md)

- [ ] genre: water and sewage networks
- [ ] genre: happiness and well-being
- [ ] genre: land value
- [ ] genre: pollution
- [ ] genre: disasters
- [ ] genre: modding

## Only on the owner's order

Measurements at scale: `python tools/tasks/km24.py` (the 24 km city, two builds compared) and the scale probe that was T038
(one scene on the 24 km map: memory by system, CPU by section of the frame and by tick, GPU time, draws and triangles against
`doc/specification.md`'s budgets; not built). `tools/build/quick.sh --ready`, the whole gates against the register,
`python tools/hunt/hunt.py`.
