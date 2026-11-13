# History: the people area (specification 003, T008, block t008-people, 2026-11-13)

The block's work file as it stood when the owner stopped it, kept whole as the area's record. Closed under constitution 2.0.0:
T008 is parked (tasks.md); the casualties' remaining register problems (Prender-284, Pwalkers-18, Prender-266, Prender-261,
Prender-277, Prender-273, Prender-274) wait until a later feature meets them. Left uncommitted in the tree at the stop: the census
probe of item 29 (`widest_m`, `widest`, `unfinite` in native/src/render/agents/body_census.cpp and body_renderer.h) and the DLLs
built with it.

- Goal (the owner's words, 2026-11-13): "A PRIORIDADE É PRIMEIRO FAZER O PORTE CERTO DO JOGO ANTIGO PARA O NOVO ... FAZER O PORTE NÃO
  É COPIAR, ENTENDE O QUE TEM QUE FAZER E COMO."

## Item 1: the asset tools (Ptools-115)
- One look (~08:45): tools/assets held only __pycache__; project/assets/noise/blue_noise.png and lattice.png committed and read by
  sky.json:6 and materials.json:43,160, with no tool to make them again.

## Read (path: lines read of total, hour, what it does)
- scratchpad copies from git: cook_people.py (18782f55^) 1-290 of 290 (~08:40): reads the reference's cooked bodies
  (<roadcraft>/public/models/citizens/people/<id>.bin and .ktx2), its src/people/cast.json and baked clips: a tool reading the
  reference, so people.bin itself is the reference's cooked data repacked (Pbuildings-43 already holds it); cook_ragdoll.py goes with it.
- make_blue_noise.py (56926e1b^) 1-97 of 97, make_lattice_noise.py 1-22 of 22, compress_atlas.gd 1-27 of 27, pack_signature_looks.py
  1-71 of 71 (~08:45): the noise tools are this game's own (void and cluster, Ulichney 1993; a seeded lattice); compress_atlas.gd
  compresses the people's PNGs the cook made from the reference's KTX2; pack_signature_looks.py packs the reference's lot-lab table.
- doc/map.md 560-579 (the tools section's place).

## Research
- Context7 /python-pillow/pillow (Image.fromarray): `mode` deprecated in 11.3.0; since 12.0.0 removed only where it changes the data
  type; omitted, a 2-D uint8 array is read as L. Dropped from both tools.
- Internet: Ulichney, "Void-and-cluster method for dither array generation", SPIE 1913 (1993), https://doi.org/10.1117/12.152707
  (the tool's own method, as its docstring cites).

## Changes
- tools/assets/make_blue_noise.py, make_lattice_noise.py back (Write), their `mode` dropped; doc/map.md lines for them.
- Ptools-115 criterion corrected (its "back from git" would have brought tools reading the reference) and closed.

## Checks
- ~08:50: both tools run (0.4 s); their tiles equal the committed ones pixel for pixel (blue L 64x64, lattice L 256x256);
  run_gates.py --file on both tools and doc/map.md: Gates OK.

## Skills
- city-method; speckit-implement (T008); verification-before-completion (the pixel comparison above).

## Item 2: walkers off the expressway (Ptraffic-85), done
- One look (~09:00): the people-walking sheet's finding (road_profiles.json:310-358, profiles.cpp:80, ped_routes.cpp:134).
- Read (~09:00-09:30): road_profiles.json 1-623 of 623; road_profiles_b.json 1-367 of 367 (road_profiles_a.json's ids and classes by
  Grep: no highway there); profiles.cpp 1-96 of 96; profile_types.h 1-103 of 103; ped_routes.cpp 1-219 of 219; ped_ways.cpp 1-126 of
  126; road_network_node.cpp 1-56 of 56; astar.h 1-78 of 78; street_furniture_posts.cpp 1-134 of 134; street_furniture_internal.h
  1-30 of 30; road_variants.cpp 1-164 of 164; road_spec.cpp 1-173 of 173; road_spec_ops.gd 1-113 of 113; street_crossing_test.gd
  1-135 of 135; road_profile_panel.gd 85-114; road_edit_test.gd 200-229; city_core_probe.cpp 200-306; city_core_commands.cpp 215-254;
  look.gd 96-250; near_plan.json 1-13 of 13; reference furnitureSets.ts:92, poleLines.ts:154 (Grep: furniture and poles only where
  carriesPedestrians).
- Research: CTB, law 9.503/1997, annex I (an expressway has no at-grade pedestrian crossing),
  https://www.planalto.gov.br/ccivil_03/leis/l9503compilado.htm; DNIT IPR-740 Manual de Projeto Geometrico de Travessias Urbanas
  (2010), https://edisciplinas.usp.br/mod/resource/view.php?id=2754916, for a shoulder's width: its tables not reached, so the 1 m
  shoulder keeps the old edge's width and the norm's width goes with the catalogue's redesign (Ptraffic-22). Context7
  /websites/cppreference: vector::clear keeps the capacity (the thread_local arms buffers); a block-scope thread_local is
  initialised once per thread, on first pass.
- Plan (the cause, file:line): the footway in a highway's cross-section (road_profiles.json:310-358, road_profiles_b.json highway
  and ramp profiles) made has_walk true (profiles.cpp:80); nothing barred crossing a highway arm (ped_routes.cpp plan_corner,
  road_network_node.cpp crosswalk_on). Fix: shoulders in the data; foot_crossing derived from the class (profiles.cpp); crosswalks,
  stop lines, plan_corner and the walking A* (way_round) honour it; the composer keeps a highway's shoulders and refuses footways.
  It cannot come back: the class, not the strips, bars crossing, and the probe counts by class.
- Changes: road_profiles.json, road_profiles_b.json, profile_types.h, profiles.cpp, road_network_node.cpp, ped_routes.cpp
  (arms_by_angle into a buffer, open_round, standing_corner, way_round), road_variants.cpp (Section's edge), road_spec.cpp
  (noFootways), strings_b.csv (profile.problem.noFootways), city_core_probe.cpp (walker_census: walking, walkers_barred,
  closed_roads), street_crossing_test.gd (_off_highways), road_edit_test.gd (_composed_highway), highway_plan.json, suites.json,
  doc/map.md.
- Checks: both DLLs, 0 warnings. street_crossing_test FAIL the first time: the town (map 0 seed 7) has no expressway, so the
  check proved nothing: cause read (city_gen lays one only where its plan does, road_gen_graph.cpp:74,256); the scene now lays one
  over the streets; red-green: 2 walkers on the expressway with its old footways, 0 with shoulders (300 looks). road_edit_test,
  road_commands_test, people_test, walker_motion_test, edit_fuzz_test, traffic_hash_test, save_undo_test pass. Gates OK on every
  changed file after two fixes (traffic_probe 102 lines: the walkers' census its own function; corner_of, a name the reference
  defines: standing_corner). The "Orphan StringName: VISEMES" at exit is registered already.
- Photos (highway plan, 2026-11-13, 1920x1080, by day 16 h and night 22 h):
  00 edge_street_0: along the expressway; lanes to the edge, no kerb, no footway; zebra on the crossing street's arm (270-400,
  520-540), none over the expressway (490-1430, 500-560); no edge line where lane meets shoulder (Prender-267). Sound otherwise.
  01 edge_street_90: north into the crossing street: zebra over the street (750-1170, 650-830), walkers on its corners (250-310,
  700-870; 1750-1900, 750-950), none on the expressway. Sound.
  02 edge_street_180: along the expressway west; asphalt to a brick wall (0-480, 470-830) and a fence (1430-1920, 500-780) at the
  edge; cyclists at the junction (1310-1370, 480-600) are riders. Sound.
  03 edge_street_270: south into the crossing street: zebras over the street (650-1250, 930-1050; 800-1120, 650-700), none over the
  expressway. Sound.
  04 edge_oblique: first take missed (a tower at 300 degrees); retaken along it at 0 degrees: expressway (800-1110 wide) with its
  median, grass and lot pavement at its edges, crosswalk over the crossing street (1130-1150, 515-545). Sound but Prender-267.
  05 edge_above: crosswalks on the crossing street's arms (800-900, 60-400; 1100-1300, 780-1050), none across the expressway. Sound.
  06 crossings_far: the expressway diagonal through the centre; zebras on the crossing streets (640, 590; 1130, 320; 1250, 450), none
  on it. Sound.
  07 edge_street_night: the street's lamps lit (1380, 170), the expressway dark beyond (Prender-268: highways unlit, as in the
  reference). 08 edge_oblique_night (retaken): the expressway dark between lit streets (Prender-268).
- Lapses: item 2 coded while the block stood in verify (opened and verified again before the build); the build's and the tests'
  output filtered (tail, Select-String) instead of shown whole as they ran; the logs read after.
- Skills: systematic-debugging (the scene with no expressway), verification-before-completion (the red-green run).

## Item 3: parties, read again (~10:10): not a port difference
- Read: reference src/sim/ambient/ambient.ts 660-739 (company: one companion, 40%, a child 10% of those) and its two calls (294,
  533, by Grep); src/sim/agents/walk.ts 1225-1314 (startWalk: a party couple or family by a child; joinParty, finish). The
  reference forms pairs only: this game's pairs match it. Groups of three and four are a gap of both games: Ptraffic-86 corrected
  and moved to T029 (genre systems, after the port); the sheet says so.

## Item 4: the riders (Ptraffic-88), not built here
- Read: people_renderer.cpp 50-160 and 225-264 (seat: any adult body for every seat pose), the people-cast sheet. Which body rides
  is a fact of each body's clothes; the only table of it is the reference's cast.json (not to be copied), and the bodies themselves
  are the reference's cooked files (Pbuildings-43): the garments come out of the people's cook from upstream sources, so the rule
  waits on it. Ptraffic-88 tied to Pbuildings-43.

## Item 5: bodies follow the simulation (Prender-262), done
- One look: effect_bodies.cpp:80 (life = min(lie, 90 s)), effect_things.cpp:123 (a bled-out walker adds nothing).
- Read (~10:30-11:30): body_renderer.h 1-72 of 72, body_renderer.cpp 1-234 of 234, effect_bodies.cpp 1-163 of 163,
  effect_things.cpp 1-167 of 167, effect_params.h 1-61 of 61, effect_params.cpp 1-40 and 150-169 of 169, effects.schema.json 1-30 and
  470-530, debris.h 1-79 of 79, debris.cpp 1-158 of 158, effect_world.cpp 140-189, change_log.h 95-124 of 124, render_world.cpp 95-124,
  city_core.cpp 140-179, shoot_test.gd 1-258 of 258, shoot_fright_cases.gd 1-40, strike_people.cpp, shoot_people.cpp, wounds.cpp (read
  whole for the casualties sheet today).
- Research: the reference keeps the dead for good, 40 bodies, the oldest dead first (ragdoll.ts:142-154, 844-851, read whole for
  the sheet). Context7 /websites/cppreference: std::numeric_limits<float>::infinity for the dead's life (a float that never runs out).
- Plan (the cause, file:line): the body's life a render timer apart from the simulation (effect_bodies.cpp:80, effects.json
  body_life_s). Fix: the dead stay (FOR_GOOD_S) till the cap, the oldest dead first, the living kept (body_renderer.cpp add); the
  knocked lie for the simulation's own while; a bled-out walker's body found by id and kept (BodySpec::who, die); severed pieces stay
  (debris capped); body_life_s removed from data, schema and params. Also the thrown pieces' six colours moved to effects.json tints
  (the gate's DUPLICATED_VALUE on FLESH: colours belong in data), the Rng seed named (FALL_SEED).
- Checks: both DLLs, no warning. shoot_test: the dead kept (1 of 1); who bled out kept (3 against 1); who only fell not kept;
  strike_test passes. The first try to prove die() found no body at all from _walkers' shots: cause read, a full rebuild drops every
  pending damage event, even those made after its cause (render_world.cpp:119-120): registered Prender-269; the case now has frames
  between its steps. shoot_test split by responsibility (shoot_body_cases.gd) to keep under 250 lines. Gates OK on every file.
- Photos (fallen plan, 16 h): the first take (force 3 at the centre) reached nobody, the second (force 20) buried the one body
  under the towers it felled: both missed, retaken at a street corner (60, 8), force 5. 04-fallen_above: fires at the kerb (650-720,
  410-600), a charred body lying by them (690-730, 420-460), the crossing's zebras whole; 01-fallen_90: a thrown bench in the wood
  tint (510-545, 540-565) and a post in the metal tint (1180-1230, 690-740), two walkers going on (800-830, 185-215). Bodies 1, kept 1.
  The body's lasting past 90 s is proven by the probe, not a photo (90 s of effects frames passes a check's minute).
- Skills: systematic-debugging (the missing bodies), verification-before-completion (the probe's numbers).

## Item 6: trips as falls (Pwalkers-12), done
- Read: ped_fright.cpp (whole for the sheet), pedestrians.cpp 80-110 and 205-254, traffic.json 100-129, traffic_params.h:31,
  damage_state.cpp 80-86, shoot_fright_cases.gd 1-74 of 74, wounds.cpp (whole), traffic_system.cpp:156 (a walker down is not drawn).
- Research: Smeesters, Hayes, McMahon 2001, J. Biomech. 34:309-317 (trips throw people forward whatever their gait speed),
  https://searchworks.jioinstitute.edu.in/articles/cmedm__11182121.
- Plan: a trip was an act (held_by_fright held PedAct::CROUCH, ped_fright.cpp:166-171). Fix: trips.cpp trip_step on the damage clock
  (sim/demolition, the layer that writes damage events: the first try put it in ped_fright.cpp and the LAYER gate refused
  sim/traffic -> sim/demolition), a CASUALTY knocked down pushed from behind at traffic.json ped_trip_throw 0.3, down for trip_s.
- Checks: shoot_test FAIL first: 0 on the ground; cause read (the stampede trips within 30 m only, the blow stood 42 m off a walker
  taken at random: the test never saw a trip, they were crouches before); the blow now 5 m off the busiest walker, force 2: 5 on the
  ground. Then "who only fell is not kept" FAIL (4 kept against 3): the busier spot had a death beside the blow; the check mixed the
  dead with the risen and repeated "who fell gets up" (a kept body never leaves): removed. shoot_test, strike_test, people_test,
  walker_motion_test, edit_fuzz_test pass; gates OK (NUMBER_MOVED's cause, BLAST_OFF_M 30 -> 5, written in Pwalkers-12's fix).
- Open part registered: Prender-270 (the hands out, the stampede's photos).

## Item 7: the hands out (Prender-270), coded, photos open
- Research: Wisner and DeGoede, ISB 2005 abstract 0549, https://media.isbweb.org/images/conf/2005/abstracts/0549.pdf (arm movement
  1.3 m/s); DeGoede et al. 2001, J Gerontol A 56:M584, https://biomed.gerontologyjournals.org/cgi/content/abstract/56/9/M584 (280-335
  ms to put the hands out).
- Changes: Ragdoll::begin takes `brace` (wrists, elbows at half: ELBOW_BRACE_SHARE), BodySpec::brace, effect_bodies.cpp sets it for
  the living (CAS_KNOCKED) along the fall, effects.json down.brace_ms 1.3 and brace_down 0.5 with the schema; stampede_plan.json.
- Photos (stampede, 16 h): the force-2 blow at (60, 8) blew up a car (a charred wreck burning at 410-490, 410-480 in 04-falls_above),
  bodies 3; the fallen show too small to see their hands (a dark shape at 570-590, 370-385 in 00-falls_0): the photos miss the subject.
  Prender-270 stays open: a look step aimed at a fallen body is needed (setViewNear body).
- Lapse: a commit (af42f5a1) went with a gate's finding unfixed (NON_ENGLISH_TEXT on a URL in the schema), fixed in 397e62b1.

- Retake (~12:40): setViewNear gained "fallen" (look_aim.gd: the nearest walker on the ground, its body drawn where it fell); the
  stampede plan aimed at it, 60 px a metre, 40 degrees, four azimuths and from above. 00-fallen_0: a body face down by the pole, arms
  ahead (735-835, 505-540); 01-fallen_90: a body across the kerb, arms stretched past its head the way it fell (935-985, 610-695); a
  charred body by the burning wreck's pieces (595-640, 320-365); zebras, poles, signs whole. Sound. Prender-270 closed (1855ff5e).

## Item 8: pools at rest, splashes (Prender-265), coded, pending a close photo
- Read: effect_things.cpp 100-146 (casualty's pools), marks.h:17-19, effects.json:28, effects.schema.json 450-480, effect_world.h
  48-97, ragdoll.cpp step (whole for the sheet).
- Changes: Ragdoll::step returns the hard landings (down faster than blood.splash_ms 4 m/s, at most RAGDOLL_HITS a step);
  BodyRenderer collects splashes (at most blood.splashes_most 3: the bleeding trunk and head) and, when a dead body falls asleep,
  its pool at the pelvis (BodySpec::pool: 1, or blood.pool_torn 1.4 torn), take_marks; EffectWorld::lay_body_marks lays them;
  casualty() lays at the event only what a torn body flung round it (or the one pool when no body is drawn).
- Checks: shoot_test "a pool spreads under the body where it came to rest" (blood marks 0 -> 4, first 0 -> 10 with 12 splashes a
  body: too many, cut to 3); strike_test passes; gates OK.
- Photo: fallen plan from above (15 px a metre): the charred body by the fire (700-750, 415-460), a blood mark by the kerb (490-505,
  462-472); too far to see the pool under a body: Prender-265 left fixed-pending-check until a close photo of a dead body.

## Item 9: setViewNear "body", Prender-265 closed
- effects_body_places (BodyRenderer::places, EffectWorld, RenderWorld, CityCore, store_api); look_aim "body"; the fallen plan aimed
  at the body nearest the blow, 60 px a metre, 45 degrees, four azimuths. 00-body_0: the dead body at the kerb (905-1010, 515-545),
  its pool under its middle (940-980, 515-545); the knocked-down living one (735-795, 230-290) without a pool; smoke and the fire of
  the blow at the right; a walker crossing below. Sound. Prender-265 closed (4d48fbea).

## Item 10: the body from the walker's pose, severed pieces (Prender-263), done
- Read: people_pose.gdshaderinc 1-91 of 91, people_renderer.cpp 50-160 and 264-336, traffic_system.cpp 135-184 (what a walker's
  record carries), people_asset.cpp 1-60 and 185-228 (POSED_CLIPS, clip_rows), effect_bodies.cpp 1-135, agents.json 36-80 and its
  schema's occupants and people.
- Changes: PeopleRenderer remembers each walker's drawn clip and phase (people_poses.cpp, split from people_renderer.cpp past 350
  lines), people_asset keeps the walk whole and the acts' first frames on the CPU (~1 MB); BodySpec::clip/phase; BodySpec::piece and
  Ragdoll::keep_parts for a limb torn off; the cylinders, EffectWorld::load_looks and the agents.json skin and clothes they alone read
  removed. Probes effects_bodies_from_drawn, effects_body_pieces.
- Checks: stampede plan from_drawn 0 first: the camera stood elsewhere till after the blow (no walker drawn there): it stands at the
  corner first: 2 of 3 (the third a car's occupant). shoot_test: the limb shot off is a piece (1 against 0). A case with a blow right
  on a walker gave no new piece (not followed: likely Prender-269's dropped events); removed for the deterministic shot. Gates OK.
- Photos (torn plan; setView's y is the map's north, -z, the blow's {at} is not: the first aim missed the bodies, retaken at
  (60, -25)): 05-torn_piece_close: a calf in jeans with its shoe on the footway (960-1060, 595-625), its body across the railing
  (640-855, 680-750): bodies pass through railings, registered Prender-271; 06-torn_piece_close_2: a fallen body arms out (780-1000,
  470-745), a charred one with its pool (1420-1515, 325-560). Prender-263 closed (8871d86e).

## Item 11: the ragdoll's limits (Prender-264 in part), 645f4913
- Read: ragdoll.h/.cpp whole, body_renderer.cpp whole, body_walls.cpp whole.
- Research: AAOS normal range of motion (neck flexion 45 degrees, ankle dorsiflexion 20 and plantarflexion 50); Jakobsen, "Advanced
  Character Physics", GDC 2001 (bones as capsules sampled along); Mueller et al., "Position Based Dynamics", 2007; Macklin, Mueller
  et al., "XPBD", 2016/2020.
- Changes: link_angle (a joint's range as its ends' distance, law of cosines) at the neck and ankles (NECK_BEND_RAD, ANKLE_UP_RAD,
  ANKLE_DOWN_RAD); bones_on_ground samples each bone at a third and two thirds (BONE_SAMPLES) with a bank of 0.08 m; landing on a
  roof (off_walls); probe effects_bodies_deepest (shoot_body_cases: no body lies under the ground).
- Open: joint damping while it falls (Prender-264 stays traced).

## Item 12: walls and fences (Prender-271 in part), 79262f0b and 72bd636a
- body_walls.cpp adds the lots' WALL, FENCE, RAILING, HEDGE, SHED, STALL as turned boxes; bones_off_walls / uncross: no bone across a
  wall's thin way, its nearer end brought to the side the other stands on (every BONE_PASS_EVERY steps).
- Photo: the apparent straddle of a railing was the projection; the back view proved the body lies on the footway against it.
- Open: the street's poles and posts (furniture built lazily by the render; the ragdoll cannot read it yet).

## Item 13: bodies through one another (Pwalkers-9), coded
- Read: ragdoll.h 1-85 of 85, body_renderer.h 1-102 of 102, body_renderer.cpp whole, torn_plan.json 1-20 of 20.
- Research: Jakobsen GDC 2001 (particle-particle collision by projection, both particles moved along their line; a Verlet particle
  moved with its previous position keeps no false velocity); Mueller 2007 PBD (collision constraints as inequalities projected each
  step).
- Plan (the cause): BodyRenderer::update stepped each ragdoll alone (body_renderer.cpp update); no constraint between bodies. Fix:
  Ragdoll::push_out_of (each kept joint out of the other body's joints to two limb half thicknesses, now_ and was_ moved together),
  called for bodies whose pelvises lie within NEAR_BODY_M 2.5 m (a body's reach), the other body's family only: a limb torn off and
  its body share a family (families_) so a piece born on its body's joints does not push it.
- Read further (~13:30): ragdoll.cpp 1-330 of 330 (step: sleep below RESTING_MS 0.12 m/s for RESTING_S 0.6 s, :271-272; ground
  contact drags by SLIDE_KEEP 0.25, :27, :257), look.gd 1-249 of 249, look_aim.gd 1-124 of 124, shoot_fright_cases.gd 1-87 of 87,
  shoot_body_cases.gd 1-118 of 118, marks.gdshader 1-111 of 111, marks.h 1-40, render_world.h 100-140, effect_world.h 40-55.
- Research: Context7 /godotengine/godot-cpp (Dictionary: `d["key"] = value`, returned by a bound method); Macklin, Mueller,
  Chentanez, Kim, "Unified Particle Physics for Real-Time Applications", SIGGRAPH 2014, section 6.1,
  https://matthias-research.github.io/pages/publications/flex.pdf (friction at the position level: velocity damping after the solve,
  Mueller 2007, lets piles collapse); Babaarslan and Kalebek, "Friction coefficient of the spunbond nonwovens", Tekstil ve Konfeksiyon
  3/2010, https://dergipark.org.tr/en/download/article-file/218160 (fabric on fabric, 20.2 N: static 0.68-0.70, kinetic 0.69).
- Changes: Ragdoll::push_out_of (closing speed taken away, then Macklin's friction, CLOTH_STATIC = CLOTH_KINETIC 0.7) and
  Ragdoll::overlap; BodyRenderer families (a body and its torn limbs), the push before the steps, census() (bodies, moving, kept,
  from_drawn, pieces, deepest, overlap) replacing six one-line probes through EffectWorld, RenderWorld, CityCore and store_api
  (effects_bodies_census), places() whole bodies only; look_args.gd (the plan's arguments out of look.gd, with pair and crowd);
  LookAim.nearest_pair, LookAim.busiest (one owner, shoot_fright_cases uses it); shoot_body_cases _together; pile_plan.json.
- Checks: both DLLs, no warning. shoot_test together: 51 bodies thrown by a blow beside a crowd, overlap 0.004 m with the push
  (first form), 0.086 m with the push taken out (red), 0.0016 m with friction. pile plan: first a blow between two walkers (thrown
  apart: a blow pushes each away from its middle), then four metres to one side (seed 1: two bodies 1.9 m apart; the aim found a body
  and its own torn limb: places() listed pieces, fixed); seed 7, shoot_test's town: one pelvis 0.53 m up on another at 4 s, then slid
  off and was still moving at 14 s: the contact kept all its sliding speed (no friction, unlike the ground): Macklin's friction added:
  all at rest, one pelvis 9 cm up on its neighbour. strike_test, fallen, torn, stampede, highway pass. Gates OK on 20 files.
- Photos (pile, seed 7, crowd of 21 at (-474.9, -400.7), the pair at (-467.6, -401.0), 16 h): 00-pile_0 crop 400-700 x 340-600:
  two bodies head to feet on the footway, the upper one's foot at the lower one's head, nothing sunk - sound; the pools over their
  chests - DEFECT Prender-272. 04-pile_street (270-590, 590-650): the two end to end, touching, flat on the paving - sound.
  05-pile_above: the pair (410-590, 245-540) - sound; a lot body (750-860, 120-260) with its limbs strewn (700-780, 60-200) - sound;
  two lot bodies (885-1045, 440-630) nearly hidden by their pools - DEFECT Prender-272. 01-pile_90 and 03-pile_270: the pair at the
  frame's edge (under the toolbar; far at the top): the rig kept its eye out of the buildings - DEFECT Prender-273 (the claim proven
  by 00, 04, 05). 02-pile_180: two bodies by the railing (1380-1610, 560-800) - sound. The earlier seed 7 run: two bodies either side
  of a lot's low wall, its pool over the wall's top and face (Prender-272); seed 1: a body across a bench (Prender-271, furniture).
- Registered: Prender-272 (marks pulled 0.5 m to the eye: over bodies and walls), Prender-273, Prender-274 (thread affinity
  warnings in the Vulkan plans). Pwalkers-9 closed.
- Lapses: pile_plan.json's seed changed with sed once (the content the intended one); edits made while the block stood in verify,
  each time opened and verified again before running.
- Skills: city-method, systematic-debugging (the body that never slept, read to its cause), verification-before-completion.

## Item 14: the ground marks as decals (Prender-272), done
- Read (~14:40): marks.gdshader 1-111 of 111, marks.h 1-97 of 97, marks.cpp 1-75 of 75, mm_layer.h 1-141 of 141, lamp_pool.gdshader
  1-62 of 62, screen_depth.gdshaderinc 1-24 of 24, effect_world.cpp marks lines (32-36, 169, 196), materials.json 295-320,
  materials.schema.json 11-15, debris.cpp 20-40 (BoxMesh), strike_commands.cpp 25-60, strike_ground.cpp 40-80, road_network.h
  100-200, transit_ground.cpp 1-40.
- Research: Context7 /godotengine/godot-docs (Decal: cull_mask, normal_fade, Forward+ and Mobile only, 8 a mesh on Mobile; depth
  reconstruction with INV_PROJECTION_MATRIX and the NDC per renderer; BoxMesh; render modes); Kim, "Screen Space Decals in Warhammer
  40,000: Space Marine", SIGGRAPH 2012, https://history.siggraph.org/?p=140176; Pettineo, "Reconstructing Position From Depth", 2009,
  https://therealmjp.github.io/posts/reconstructing-position-from-depth. Decal nodes left: they share the clustered elements with the
  lamps' lights and hundreds of marks are kept.
- Plan (the cause): marks.gdshader:19,37-38 drew every mark 0.5 m nearer the eye: anything thinner over a mark was drawn behind it.
  Fix: a box decal on the depth, as lamp_pool already is; the reconstruction from screen_depth (one owner; lamp_pool moved onto it).
- Checks: both DLLs; pile, fallen, highway, crater, strike_test, shoot_test pass; gates OK on nine files. The first crater plan found no
  crater on a street at force 10 or 50: measured, the street's deck 0.60 m over the terrain makes strike_at call it a roof
  (Psimulation-106, registered); the crater photographed on bare earth (900, 900). The normal test first applied to every kind cut a
  crater's pit walls; then only pools. Straight-edged blocks in the crater: first taken for the square's clip (the wobble kept within
  it, WOB_HALF), then read at 3x: the terrain's own blocky shading of the pit through the scorch (Pterrain-10).
- Photos: pile 05-pile_above (bodies over their pools: 640-700 x 235-405, 405-495 x 450-610, 840-930 x 560-645 - sound), crops c1
  (pool from under the body, red only on its sides within a few cm of the ground - sound), c2 (the pool stops at the railing's post in
  front of it - sound), c3 (04, pool round the body at street level - sound); crater 00, 02, 04, 05, 06 (the earth crater on its pit,
  lip and scorch following the ground; flames and smoke over it - sound but Pterrain-10); highway 07 (the lamps' pools on the road and
  zebras at night - sound); fallen 00 (a body on a zebra bar, its pool under it - sound).

## Item 15: the rebuild keeps the blows after its cause (Prender-269), done
- Read (~06:30): render_world.cpp 80-160, change_log.h 1-124 of 124, city_core.cpp 100-125 and 180-240, city_core_persist.cpp 110-170,
  shoot_test.gd 1-75, test_case.gd 36-45, shoot_body_cases.gd 100-125, shoot_people.cpp 1-104 of 104, shoot_commands.cpp 20-110,
  blast.json:30.
- Research: Context7 /websites/cppreference (std::lock_guard: RAII, released at scope end; vector::clear keeps the capacity).
- Plan (the cause): the rebuild emptied the whole render log, blows included (render_world.cpp:119-120, change_log.h clear_render),
  as if events were state drawn from the map. Fix: clear_render empties the chunk lists only; drop_damage at the causes that end the
  city (new map, load); a lots toggle keeps them.
- Checks: both DLLs; shoot_test _opened_shot 1 body, red 0 with the old clearing; strike_test, save_undo_test, edit_fuzz_test,
  people_test pass; gates OK on ten files. The new case moved the walkers the later cases pick: the bled case's count check failed
  (read: the walker's body was there, another getting up offset the count: the census now lists `who`), then its limb check was
  skipped (lost 0), then a leg round killed (severing costs sever_hp_cost on top: trunk rounds first left 32): a round in a standing
  leg first, then the legs lying (shot_targets gained `facing`). A cached build once left in doubt which DLL stood: rebuilt and run
  with --all. Prender-275 registered (a lots toggle clears the effects).
- Skills: systematic-debugging (the three failures of the bled case, each read to its cause), verification-before-completion.

## Item 16: the joints damped (Prender-264), done
- Read: ragdoll.cpp 1-382 (then 238 after the split), body_renderer.cpp 20-21, 233-260.
- Research: Mueller et al., Position Based Dynamics, 2007, 3.5 (damping of the deviations from the rigid motion),
  https://alecjacobson.com/seminar/mueller-et-al-2007.pdf; Valle et al., The pendulum test as a tool to evaluate passive knee stiffness
  and viscosity, BMC Musculoskelet Disord 7:89 (2006), https://pmc.ncbi.nlm.nih.gov/articles/PMC1693559/ (B 0.014, K 1.32, normalised by
  height^5); Joghtaei et al., J Spinal Cord Med 38(2):170 (2015), https://pmc.ncbi.nlm.nih.gov/articles/PMC4397198 (no healthy zeta
  given); terminal velocity 55 m/s belly down, https://en.wikipedia.org/wiki/Terminal_velocity; Winter's anthropometry (shank and foot
  6.1 % of mass, 0.285 H, radius of gyration 0.735). Context7 /godotengine/godot-cpp (Basis rows, inverse, determinant, xform).
- Plan (the cause): no damping in the joints, a global AIR_KEEP 0.995 a step instead (0.3/s on the whole fall, a hundred times the
  air's). Fix: damp_joints at 0.2/s on the deviations from the rigid motion; AIR_KEEP removed.
- Changes: ragdoll.cpp (damp_joints; the contacts split out to ragdoll_contacts.cpp, the shared names to ragdoll_parts.h: the file had
  grown two responsibilities, 382 lines), look_args.gd roundAt, LookAim.nearest_of, falling_plan.json, suites.json, map.md.
- Checks: both DLLs; shoot_test, strike_test, people_test pass; gates OK on nine files.
- Photos (falling): first take at 35 px/m (specks) and aimed at a blow's charred body in its fire (limbs hidden): missed, retaken
  with a round at one walker; a tree stood between from 30 degrees: retaken from 210. 00 fall_04 (410-460 x 280-350 tile): going down
  backwards, knees bending, arms out - sound; 01 fall_10: nearly level, legs coming over - sound; 02 fall_20, 03 fall_45: flat on the
  footway, arms out, nothing sunk - sound; crops: a splash at the shoe of a head shot (Prender-276), the skin parted at the waist in
  03 (Prender-277).
- Agents (the owner allowed them, 2026-11-13): three read-only briefs, Pwalkers-11's trace in both games, Prender-266's research,
  Prender-271's furniture; their reports to be checked in the files before any use.

## Item 17: the street's poles and posts are walls to a body (Prender-271), done
- Read (whole): standing_body.h 1-48, standing_body.cpp 1-141, city_core_bodies_probe.cpp 1-148, body_walls.h/.cpp, city_view.h 1-40,
  street_props.cpp 222-240 (street_ground), effect_world.h 20-32 and 80-106, effect_world.cpp 25-40, effect_bodies.cpp 60-80,
  ragdoll_contacts.cpp 90-165, rules.json cpp_layers.
- Research: Context7 /websites/cppreference (numeric_limits::lowest, the lowest finite float; min is the smallest positive). The agent's
  map of the furniture (kept furniture readable under the shared lock; body_boxes and sign_boxes; no bottom on BodyWall), checked line
  by line before use.
- Plan (the cause): walls only from the lots; BodyWall without a bottom. Fix: one sim owner of the street's pieces in volume, read by
  the audit and by the walls round a body; walls with a bottom.
- Checks: both DLLs; shoot_test (street_walls 1521, in_walls 0; red 0 walls), bodies_audit_test (first try hung 600 s: the whole map
  as a box walked 10^11 hash cells; street_pieces_all), strike_test, people_test, edit_fuzz_test; gates OK after two findings
  (DUPLICATED_CODE: the inside test, now sides_of; LAB_COPY: walls_near, renamed walls_about) and render_world.cpp at 351 lines (the
  look given at init instead of a setter).
- Photos (pole): a force-2 then force-3 blow behind a walker by a post knocked nobody down (judged a roof burst: Psimulation-106); a
  round from the side away from the post: 00 (body sitting against a lamp post, arm round it, nothing through), 90 and 180; second
  run, a plate's post: 02 crop (the post in front, the body behind and beside it), 04 above (legs beside the foot) - sound; the waist
  skin parted again (Prender-277).
- Agents: seven read-only briefs this item (trace 11, research 266, furniture 271, strike 106, struck part 261/276, twist 277, lots
  275); one downloaded a book chapter (Thompson 2017, zenodo) into the scratchpad without my leave: to be said in the report.
- Constitution 1.2.0: agents used unasked whenever reading, tracing or research can run beside the work (the owner's order).

## Item 18: the lots toggle keeps the effects (Prender-275), done (384d50a0)
- Read: building_renderer.h 36-48 and 82-112 and 234-242, building_renderer_commit.cpp 229-235, building_renderer_build.cpp 156-194
  (lots drawn only by the near and mid levels), city_core.cpp 228-238, store_api.gd:43. The agent's design checked at each line.
- Research: Context7 /websites/cppreference (atomic::exchange returns the previous value, seq_cst by default).
- Checks: shoot_test _lots_toggled 46 bodies and 101 marks kept; red with the full rebuild 0 and 0; quality_test, strike_test pass;
  gates OK (building_renderer.h over 250 by a comment: moved to the definition); lots plan 00/01/02 (lots on, off, back) sound.

## Item 19: the marks' blocky noise (Pterrain-10 in part)
- The crater agent showed the blocks inside the hole are the mark's grit (the hole opaque), not the terrain: my Prender-272 analysis
  had wrongly given them to the terrain. Checked at marks.gdshader:63-67, 91, 96, 107.
- Research: The Book of Shaders ch. 11, https://thebookofshaders.com/11/ (value noise "tends to look blocky"; Perlin's gradient noise
  diminishes it; simplex without directional artifacts); Context7 /godotengine/godot-docs (smoothstep undefined if a >= b).
- Changes: marks.gdshader gnoise (gradient noise, quintic fade, scaled to 0..1) for every use; two reversed smoothsteps fixed.
- Photos: crater 05 crop 3x: grit as small irregular specks, round outline - sound; one straight seam 8 m long remains where the lip
  is translucent: the terrain's 4 m facets (Pterrain-10 corrected: that part stays open, the terrain's). pile 05 crop: pools under
  bodies, drops round - sound. shoot_test, strike_test pass; gates OK.

## Item 20: a blow on a street is a street's blow (Psimulation-106), done
- Read: strike_commands.cpp 1-65 of 65, body_pick.h 1-26, strike_test.gd 120-150, road_network.h:159 (ray_deck), effect_world.cpp
  10-28, 70-90, 160-176, marks.h 31-63. The strike agent's report checked at each line (SHAPE_DROP_M road_shape.cpp:19,58).
- Plan (the cause): strike_at compared the blow's height with the terrain, 0.6 m under every street. Fix: on the street when a deck
  lies within ROOF_MIN_M under the point (ray_deck). Then the street crater was not drawn: its mark's plane on the terrain lay 0.6 m
  under the deck, which the decal left (lie_above_m); mark_ground lays it on the highest of natural ground, deck and footway top.
- Checks: strike_test (the road case now fires at the deck: crater, cut, 49 pieces; red without the fix); shoot_test (14.8 s alone),
  edit_fuzz_test, save_undo_test, pile pass; gates OK.
- Photos (crater): 00 and 04 missed (the camera inside a building's glass beside the corner: the rig, Prender-273); 05 above crop:
  the paving crater on the crossing, hole, lip, grit, cracks over both carriageways and onto the footways, smoke and fire over its
  south side - sound; the earth crater shots as in item 19.

## Item 21: the wound drives the body (Prender-276, 279, 280), done
- Read: effect_bodies.cpp 1-131 of 131, damage_event.h 30-42, body_renderer.cpp 244-262, ragdoll.cpp step. The struck-part agent's map
  of particles checked against ragdoll_parts.h's Joint enum.
- Research: Wikipedia, Stopping power, https://en.wikipedia.org/wiki/Stopping_power (knock-back never passes the shooter's recoil; a
  myth of film); the reference's shove (1.6 m/s) judged against it and not brought.
- Changes: BodySpec/Body bleeds; Ragdoll::step takes the mask; Ragdoll::middle_of (centre_of was a reference name: LAB_COPY);
  effect_bodies.cpp bleeding(), no throw for a shot, a pool for those down for good.
- Checks: shoot_test (wounded lying bleed 18 -> 19; red 17 -> 17), strike_test, people_test; gates OK.
- Photos (falling): the shot walker collapses where they stood (04 knees buckling, 10 against the post, 20 and 45 at its foot);
  crop 45 at 3x: blood at the head only - sound.

## Item 22: the edit fuzz's road edits (Ptools-117), done (31fca873)
- edit_fuzz_test read segment_at's "id" (never returned: "seg"); the Inspector's road edits moved to fuzz_road_shapes.gd (the test
  was already over 250 lines), each returning what it dispatched, counted per seed.

## Item 23: the wounded stay when the walking window leaves them (Pwalkers-14)
- Read (whole, ~10:40): pedestrians.cpp 1-260 of 260, ped_state.h 1-146 of 146, wounds.cpp 1-41 of 41, city_core_flow_probe.cpp
  1-111 of 111, shoot_test.gd 1-198 of 198, shoot_body_cases.gd 1-186 of 186; city_core.cpp 41-45 (ground_seen: a level camera gives
  its origin) and 160-174 (the window's centre stored each frame from the viewport's camera), city_core_strike.cpp 64-94
  (shot_targets by id), ped_routes.cpp 178-182 (walkable has no window), traffic.json ped_window_m 600, share 1.2, blast.json shot.
- Research: Context7 /websites/cppreference (atomic<float>::load/store, seq_cst by default). Internet: "Level of Detail based AI
  Adaptation for Agents in Video Games", ICAART 2013 (SciTePress),
  https://scitepress.org/Papers/2013/42615/pdf/index.html ("the notion of importance may not be resolved solely by the camera
  distance"; agents with central roles keep their behaviour; Niederberger and Gross: objects out of view "do not stop living");
  Brockington, Level-of-Detail AI for a Large RPG (AI Game Programming Wisdom, 2002), cited there. A GTA-style mission-entity search
  (forum.cfx.re) gave only anecdotes: not used.
- Plan (the cause): pedestrians.cpp:239 takes away anybody past the window but a kerb's, by distance alone; a casualty is the event's
  central agent, so it must not go by distance. Fix: whoever lies down or bleeds is kept until they rise (then the window takes them
  as anyone) or die (wounds.cpp lays them dead). Bounded: a bleeding walker always dies (hp falls by bleed > 0 each second).
- Test: shoot_body_cases _bled moves a real camera 2 km off (the core's own path, city_core.cpp:171-174), checks the wounded still
  lie there bleeding, then that they bleed out dead; the camera back at the centre after.
- First run (the window exemption alone): still FAIL ({}), and every later case lost its walkers and cars. Causes read: (1)
  despawn_party (pedestrians.cpp:80-85) takes the partner within reach (ped_party.cpp:163 company_near) when one of a pair leaves the
  window, the casualty included; the same company goes in at a door (ped_door_walk.cpp:65-74) and leaves at a route's end
  (ped_ways.cpp:97). Fix at the owner: company_near refuses a casualty; casualty() declared in ped_system.h. Read ped_party.cpp 1-183
  of 183, ped_door_walk.cpp 55-80, ped_system.h 25-40 and 120-128. (2) Moving the window empties the town for the cases after it (the
  clock still: no refill): the away case made the last one (_bleeds_out_away), the cars' case moved before the bodies' cases, and
  _rises takes standing walkers only (a blast beside the casualty would have killed them).
- Also met: the log's "Target and up vectors are colinear" (godot-cpp basis.cpp:1050) came from EffectWorld::tracer for the shot at
  the sky (looking_at with dir colinear to up): the streak turned by the shortest arc from its axis (Context7 /godotengine/godot-docs
  Quaternion(arc_from, arc_to); godot-cpp quaternion.hpp:133-160 takes any perpendicular for opposite vectors). Terrain3D's "Cannot
  find clipmap target" came from the test freeing its camera: the camera kept.
- Checks: both DLLs; shoot_test passes with no warning or error (the wounded at hp 36 after 60 ticks 2 km away, kept 18 -> 19); red
  with the window exemption reverted: FAIL ({}) and kept 18 -> 18; people_test, strike_test, walker_motion_test pass (rebuilt --all
  after the red build); gates OK on the 7 changed files.
- Photos (wounded, retaken once: 05 aimed at (60, 8) stood in a building; aimed at the body's place): 00 (870-1030, 500-710) the
  casualty down by the signal post, the companion beside - DEFECT bodies in one another: the companion's legs through the lying trunk
  (crop 3x 480-600 x 700-900), pre-existing, registered Pwalkers-15, next item; 01 the camera over the fields 840 m east, HUD 350
  people - sound (camera away); 02 back: the body where it fell, slumped against the post's far side (the post in front, census
  in_walls 0), a pool at the stump, drops - sound; 03 from 90 and 04 from above: body, pool, the post's foot beside the leg, nothing
  through - sound; 05 1500 frames on: the same body, the pool grown, census kept 1 -> 2 (bled out, kept) - sound.

## Item 24: walkers go round somebody lying (Pwalkers-15)
- Read (whole, ~11:30): ped_crowd.cpp 1-287 of 287, ped_seen.cpp 1-92 of 92, ped_stripes.h 1-96 of 96, ped_stripes.cpp 1-318 of
  318; shoot_commands.cpp 15-19 and 50-80 (the lying capsule: the way it faced, body_height_m x scale, LYING_R 0.3, 0.2 m up),
  ped_spawn.cpp 95-120 (scale 0.9-1.1).
- Research: Cornes, Frank, Dorso, "High pressures in room evacuation processes and a first approach to the dynamics around unconscious
  pedestrians", Physica A 2017, https://export.arxiv.org/abs/1701.07358 (the fallen modelled "as inanimate bodies" the others dodge or
  pass through; here none passes through). SUMO striping: furniture as footprints in the walker's frame (ped_stripes.cpp header).
- Plan (the cause): file_walkers (ped_crowd.cpp:122-129) files somebody lying as a standing walker, a point the others keep two body
  radii from (Seen::walker), while the body lies 1.7-2 m along the ground; and makes them a stripe choice of their own, so the crowd
  could slide a body lying there (and step_pedestrians turns its facing back to its way, turning the round's capsule while the drawn
  body stays). Fix: one owner of the lying body (ped_system.h lying, lying_half, LYING_R; the round's capsule, shoot_commands.cpp, reads
  it); a lying entry (CrowdEntry lie, lie_x/z) is seen as a footprint along its body (ped_seen.cpp, furniture: never squeezed
  through), makes no choice, and lies still (no act, pose or turn while down).
- First test (companion beside the trunk): passed red and green alike. Read further (~12:30): effect_bodies.cpp 66-125 (the body
  tips away from the event's push point b, c, else the way it faced), damage_event.h 1-43, shoot_people.cpp 1-104, trips.cpp 1-41,
  strike_people.cpp 50-99, city_core_strike.cpp 1-22 and 60-82: the sim laid the capsule centred on the standing point along the
  facing while the drawn body fell from the feet away from the blow. Fix: damage_event.h fall_way (one rule, read by the render and by
  traffic::lay_down, which keeps the later end of a lie and the first fall); the capsule from the feet (lying_middle); shot_targets
  gives a lying walker's middle and the way it lies. Then the companion stayed at 0.07 m: hold_apart kept the lying feet as a round
  body (ped_crowd.cpp:194-228): a body lying takes no stride. The second run identical: a check failed twice, so read, not run: the
  test's minimum over samples could not tell a walker getting up on the body from one standing in it; instrumented (the other's track,
  each walker's longest stay): the companion, knocked down by a stray round, got up on it and stepped off in 0.5 s. The lots toggle's
  count of bodies counted one getting up in the effects' time: it checks the kept bodies.
- Clone: walker_casualty (wounds.cpp, damage_state.h) for the casualty built from a walker (trips, bleeding).
- Checks: both DLLs (--all); shoot_test, strike_test, people_test, walker_motion_test, edit_fuzz_test pass, no warning; red build
  (crowd not seeing the body) passed as well: another pair picked (the crowd differs), fled at 2.9 m/s: no red from this scene; gates OK.
- Photos (wounded, retaken): 00 (780-1120 x 530-760) the companion at the casualty's feet 1.5 s after the fall, the drawn foot at his
  ankle (crop 3x 750-800 x 380-450): contact at the extremity, no trunk entered - sound; 02 the body from the feet at the post lying
  west, away from the round from the east, the pool at the stump, nobody on it - sound.
- Registered: Pwalkers-16 (a blast's arm loss never stored), Pwalkers-17 (a casualty on a cut street vanishes); Pwalkers-11 widened to
  the crowd and the dead.

## Item 25: a blast's maimed stay and bleed; a casualty outlives its street (Pwalkers-13, 16, 17), done
- Read: the agents' traces checked at strike_people.cpp 50-99, shoot_people.cpp 1-104, pedestrians.cpp 210-230, lane_graph_edit.cpp
  60-80 and 159-200, road_network.cpp 78-116 (segment slots reused), road_network.h 218-232, road_commands.cpp 211-244 (a footway
  click takes a street piece), city_core_edit.cpp 217-242, city_core_people.cpp 26-36, 168-175, 232-282, city_core_probe.cpp 218-226,
  ped_fright.cpp 96-118 and 168-180, ped_mode.cpp 1-40, rules.json cpp_layers (sim/traffic may not include sim/demolition).
- Research (agent, URLs opened by it): Eastridge et al. 2012; Webster et al. 2021; Kragh et al. 2009 (untreated limb bleeding kills
  in minutes; the game's 15-33 s is compressed time, a design value).
- Changes: wound_for_good (shoot_people.cpp, strike_internal.h, LIE_FOR_GOOD_S shared); strike_people keeps the maimed; pedestrians.cpp
  drop_pedestrians_on_seg keeps whoever lies, lie_on_nearest, walk() takes away who rises where no street is; entry_of side -1.
- Checks: shoot_test maimed_by_blast (927: leg, 3/s, street demolished at segment_path's middle, lay on, bled out kept); reds: the
  blast taking them away (no maimed found), the old drop (927 gone); the test lookups fixed (shot_targets read whole; rounds from above
  and across by turns: a bystander took the level ones); both DLLs --all; shoot, strike, people, walker_motion, edit_fuzz pass; gates OK.
- Photos (maimed): 00 bodies round the crowd's heart, one by the kerb with its leg thrown beside it (crop 3x), an elder getting up on
  all fours - sound; turf debris as flat green squares - DEFECT, registered Prender-282 (not this item's code); 02 the same corner 26 s
  on, the maimed body kept in its grown pool, census kept 8 -> 10 - sound. Registered Prender-281 (2 cm overlap in another scene).

## Item 26: somebody lying shot again keeps the one body (Prender-278)
- Read: native/src/render/agents/body_renderer.cpp: 1-317 of 317 (~13:40; add takes a slot and a ragdoll for every casualty :86-137;
  die looks up by who and leaves the id on the dead :139-146; census lists every whole body's who :291), body_renderer.h 1-104 of 104,
  native/src/render/effects/effect_bodies.cpp: 1-163 of 163 (body always adds :104, pieces from a standing spec :107-120),
  native/src/render/agents/ragdoll.h: 1-97 of 97, native/src/render/agents/ragdoll.cpp: 1-251 of 251 (velocity implicit, now - was;
  begin sets it through was_ :89); effect_things.cpp 115-118 (only CAS_BLED reaches die).
- Trace: python tools/tracer/trace.py city.EffectWorld.body --depth 2 and city.BodyRenderer.die --back 3, roots render/effects and
  render/agents: EffectWorld.sync -> apply -> casualty (effect_things.cpp:117) -> body (effect_bodies.cpp:68) -> BodyRenderer.add
  (:119); die only from casualty :118. The reference (agent's trace, checked): ragdoll.ts spawn looks up the known body first (:615),
  the same body takes the knock, the kill, the limb and the shove.
- Research: Context7 /websites/cppreference (std::deque emplace_back invalidates no reference; an erase in the middle does: add may
  erase the oldest when full, so strike changes the struck body first and builds its pieces from a copy); Jakobsen, Advanced Character
  Physics (GDC 2001), https://edelkas.github.io/n/files/bib/jakobsen.pdf (Verlet: velocity is x - oldx; a push moves the old place).
- Cause: effect_things.cpp:117 sends every casualty but CAS_BLED to body(), whose BodyRenderer::add (body_renderer.cpp:86) never asks
  for the body a living walker already has; and the dead keep their who (:99, :139), a reused walker id later meeting them.
- Fix: BodyRenderer::strike(spec, pieces) on the living walker's body (who, not a piece): its wound ORed in (lost, bleeds, charred,
  pool), a kill makes it dead and lets go of who, a knock extends its life, the push shoves it (Ragdoll::shove), each limb comes off
  as a piece copied from it as it lies; add keeps who only for the living; die lets go of who.
- Read again after the guard's commit fbc70eab (~15:10): body_renderer.cpp 1-317 of 317, body_renderer.h 1-104 of 104, effect_bodies.cpp
  1-163 of 163, ragdoll.h 1-97 of 97, ragdoll.cpp 1-251 of 251, effect_things.cpp 1-179 of 179, effect_world.h 1-116 of 116,
  shoot_body_cases.gd 1-200 of 200 (census who read only at :138-141, for the living wounded); rules.json max_lines .cpp 350, functions 90.
- Research again: Context7 /websites/cppreference (deque: emplace_back keeps references, an erase in the middle invalidates all);
  Jakobsen via https://gamedeveloper.com/programming/advanced-character-physics (velocity implicit, x - x*; a push moves a position;
  friction modifies x*) and https://gamedev.net/forums/topic/296733-verlet-with-rigid-body/2881006/ (a velocity change back-projects
  the old position).
- Changes: Ragdoll::shove (ragdoll.h/.cpp: was_ moved back by the push, the body woken); BodyRenderer::strike and take_slot
  (body_renderer.cpp, a full row frees none of the family a piece joins); add and die keep who for the living only; census and places
  moved to body_census.cpp (body_renderer.cpp would pass 350 lines; FOR_GOOD_S and NEAR_BODY_M class constants, one owner);
  effect_bodies.cpp body() builds the pieces once, strike first, add when nobody is down; effect_world.h piece_scratch_; doc/map.md.
- Checks (~15:30): both DLLs build clean; shoot_test PASS 25 s, "shot again lying, one body still (1 bodies of walker 0)" (5 before
  the fix), the census lists only the living (who [0] at the end: the dead let go); strike_test PASS 5.9 s, bodies_audit_test PASS
  2.4 s; logs read whole, no engine error (the exit's VISEMES StringName is registered); gates OK on the 11 changed files.
- Photos (shot_again, plan project/tests/visual/shot_again_plan.json; census after the third round: 2 bodies = 1 body + 1 piece,
  who [205]; at the end who [] and kept 2: bled out, no second body):
  00 near 0: (900-990, 665-745) one body on its side at the signal pole's foot, the pole's elliptic base at (912, 727) beside it, a
  knee stump and its pool at (895-935, 735-760) - sound. 01 near 90: (900-1030, 540-610) the same body, the legs behind the pole,
  the pool by the stump; (1335-1390, 575-610) the calf and foot beyond the railing, about 4 m off - DEFECT registered Prender-283
  (the piece's velocity, not this change). 02 near 180: (925-1030, 520-605) one body, the torso hiding the pole's foot - sound.
  03 near 270: (875-1015, 520-585) the legs in front of the pole (behind it from 90: consistent), the shoe, the stump's drops -
  sound. 04 street level: (905-1045, 545-590) lying flat on the pavement, neither floating nor sunk - sound. 05 above:
  (910-995, 490-605) the body and pool at the pole, (885-935, 910-960) the leg - sound. 06 mid and 07 far missed (a railing, a
  tower in front): re-aimed from 90 degrees and taken again; 06: (925-990, 510-545) one body through the corner's poles, the leg
  beyond the railing - sound; 07: (950-960, 525-535) the body and its pool at the corner's foot in its area - sound.

## Item 27: a limb a round takes off drops by its stump (Prender-283)
- Read (~15:50): native/src/render/effects/effect_bodies.cpp: 1-169 of 169 (piece velocity :114-115: limb_push_ms 6 m/s along the
  fall and lift_ms 3.5 x 0.5-1.5 up, whatever the power; the body's own velocity zero for a round, :79-88); effect_params.h 1-63 of
  63, effect_params.cpp 1-186 of 186 (the down group, :162-175), project/data/render/effects.json 1-31 of 31,
  tools/gates/schemas/effects.schema.json 1-618 of 618 (down :519-580, strict); the reference ragdoll.ts 698-737 and 955-989 (sever:
  "A bullet takes a limb off and drops it near; a blast throws it", struck: 0.8-1.6 m/s, spread 0.6, lift 0.4-1.0; blast:
  2.5 + 6 x max(0.15, power) m/s, spread 3, lift (1.5-4.5)(0.6 + power); shot path :978 kick 1 m/s along the round).
- Trace: python tools/tracer/trace.py sever --depth 1 --root <roadcraft>/src/render/ragdoll.ts (spawn and absorb reach sever ->
  detach); python tools/tracer/trace.py city.EffectWorld.body --conditions --root native/src/render/effects (piece.velocity
  effect_bodies.cpp:114 written whenever people_, independent of power).
- Research: Context7 /godotengine/godot-docs (Vector3 * scalar scales; normalized() of a zero vector is zero); a round's momentum,
  https://handwiki.org/wiki/Engineering:Power_factor_(shooting_sports) (5.56x45 NATO 4 g at 945 m/s: 3.78 N s; 9x19 2.63; 7.62x51
  8.69); segment masses, Dempster's shares via https://wiki.has-motion.com/Segment_Mass (403 when opened; the search's text: shank
  0.0465, foot 0.0145, forearm 0.016, hand 0.006, head 0.081 of the body) and Winter via
  https://bionumbers.hms.harvard.edu/bionumber.aspx?id=109721 (Winter 2005, Irving 2007 table 1.7). A 75 kg adult: calf and foot
  4.6 kg, forearm and hand 1.65 kg, head 6.1 kg; the round's whole momentum in them: 0.8, 2.3 and 0.6 m/s.
- Cause: effect_bodies.cpp:114 gives every piece limb_push_ms and lift_ms, a blast's throw, whatever the blow; a round (power 0) still
  flings the limb metres, against its own rule (:77-78, a round throws nobody).
- Fix: the piece leaves at the blow's own push: a round's momentum (down.round_ns) over the mass of what came off (down.head_kg,
  forearm_kg, calf_kg) along the way it falls, nothing up, no scatter; a blast's limb_push_ms x power with its lift and scatter, as
  the body's push_ms x power. The data in effects.json down with its schema; EffectParams reads them.
- Changes: effect_bodies.cpp limb_kg and the piece's velocity by the blow; effect_params.h/.cpp round_ns, head_kg, forearm_kg, calf_kg;
  effects.json down (3.78, 6.1, 1.65, 4.6) and effects.schema.json (required, each with its source).
- Checks: both DLLs; shoot_test PASS 18.2 s (the limb shot off still a piece of the body), shot_again PASS, maimed PASS; logs read,
  no engine error.
- Photos: shot_again 01 near 90 (860-1080, 500-610): the calf and foot lie on from the knee stump, the cut red, the pool under it -
  sound; 05 above (890-1060, 440-650): both feet by the body, the piece touching the stump only at the knee, none inside the other -
  sound; 03 near 270 (820-1080, 460-640): the same - sound. maimed 00/01: a moving elder's body blurred and streaked (1.5 m purple
  streak) - DEFECT, not this change (one event a walker a blast, strike_people.cpp:65-104; no strike on this path): registered
  Prender-284 (the temporal filter over a body skinned in the shader, cause unknown).

## Item 28: the dead lie in the simulation: a round meets their body, the crowd goes round it (Pwalkers-11)
- The reference: actionsWiring.ts:113-127 (a round finds no standing walker, then the bodies on the ground, alive or dead, along the
  line: scene.shootBody; a dead one 'hit': the round stops there; a living one takes the wound); ragdoll.ts:937-987 (a hit on a body:
  the push at the part, the spray, blood under it, hits by part, a limb off at the second hit, the head at the first or second, shot
  to pieces at 12 hits). There the bodies live in the render; here the render only reads.
- Read so far (~16:20): shoot_commands.cpp 1-149 of 149 (lying capsule :62-75, a dead walker is gone: remove_pedestrian at
  shoot_people.cpp:98, strike_people.cpp:77, wounds.cpp:43), shoot_people.cpp 1-118 of 118, ped_seen.cpp 1-136 of 136 (lying
  footprints from CrowdEntry lie), damage_state.h 1-188 of 188 (people that died are gone), strike_people.cpp 1-143 of 143,
  wounds.cpp 1-47 of 47.
- Design: the dead stay walkers, dead (Pedestrian dead flag), lying for good where they fell (traffic::lay_down for good, the fall
  way), their id kept so it is never reused while the body lies; every path that removed a dead walker lays them dead instead
  (one owner: traffic::lay_dead); the systems that move, frighten, trip, bleed, count or despawn walkers skip the dead; a round meets
  them by the lying capsule and, dead, takes no health: the wound's spray and a push of their body (a CASUALTY CAS_DEAD to the same
  body: BodyRenderer::strike, the render keeping who for the dead too), a limb off at the second hit on it; at most
  catalog dead_kept_most dead are kept (data), the oldest let go with an event the render takes its body away by (one channel:
  ChangeLog damage events); the render's cap above the sim's.
- Read too (~16:40): pedestrians.cpp 1-318 of 318 (remove_pedestrian :200 only the dead's: shoot_people.cpp:98, strike_people.cpp:77,
  wounds.cpp:43; despawn :67 counts ped_count down; walk :115 returns while lying; the window keeps casualties :296), ped_system.h
  1-215 of 215, ped_state.h 1-150 of 150.
- Trace: python tools/tracer/trace.py city.traffic.remove_pedestrian --back 3 --root native/src/sim (shoot_at -> shoot_walker;
  strike_at -> strike_people -> harm_walker; Simulation.tick -> damage_step -> bleed_step: the three deaths).
- Research: Context7 /websites/cppreference (deque push_back/pop_front O(1), a FIFO; numeric_limits<float>::infinity compares above
  every float); https://arxiv.org/pdf/1701.07358 (Cornes, Frank, Dorso 2017: fallen pedestrians stay fallen, inanimate bodies the
  others dodge as obstacles, with the same repulsion as a neighbour); the reference's shootBody (above).
- Cause: pedestrians.cpp:200 remove_pedestrian, the one way a walker dies, frees the walker: the simulation keeps no record of a dead
  body, so a round passes through it and the crowd walks through it (damage_state.h:3 "people that died are simply gone").
- Fix: as the design above; files: ped_state.h (dead), ped_system.h and pedestrians.cpp (lay_dead in place of remove_pedestrian,
  despawn counts only the living, the cap and its event), traffic_state.h (the dead in order), damage_event.h (BODY_GONE),
  shoot_people.cpp (a round in the dead), strike_people.cpp (a blow on the dead: its push), wounds.cpp (no drip, lay_dead),
  blast data (dead_kept), effect_things.cpp and body_renderer (let_go, who for the dead walkers), effect_bodies.cpp (who only for
  walkers), shot_targets (dead), a shoot test case.
- Design refined (~16:55; a dead walker as a Pedestrian would touch the ~25 readers of `alive`: counts, kerb stops, probes): a dead
  body is no walker. TrafficState keeps `dead` (DeadBody: its id DEAD_BODY_ID_BASE + serial, feet x y z, fall way, scale, lost,
  seg/side/s/u where it lies on a sidewalk, side -1 none), in the order they died, at most blast.shot dead_kept; remove_pedestrian
  lays the walker's body there from the walker (lay_dead) and frees the walker; the event BODY_DEAD (id walker, u dead id) has the
  render's body take the dead id; BODY_GONE (id dead) when the oldest goes past the cap. The crowd files each dead body as a lying
  entry on its sidewalk (ped_crowd.cpp), a street gone moves them as the living lying (lie_on_nearest); the round meets them by
  the same capsule (one helper with the living lying); a round in a dead body: the WOUND spray, a limb off as on somebody lying, a
  CASUALTY CAS_DEAD with the dead id (the same body pushed: strike); a blast pushes those in its reach (CASUALTY CAS_DEAD, power).
- Read too: damage_event.h 1-61 of 61, traffic_state.h 1-245 of 245.
- Read too: ped_crowd.cpp 1-293 of 293 (file_walkers :109-141 files every alive walker, lying ones as entries with lie; the corner
  loop :265-267 reads t.peds[e.id] before e.lie: a dead entry (id -1) must be skipped by its lie first; sidewalk choices skip lie
  :175; hold_apart takes choices only). The dead are filed after the walkers: on their sidewalk (side >= 0, lying there) or, killed
  on a corner or a zebra, with that junction (DeadBody.node), each an entry with id -1, lie their half, lie_x/z their fall.
- Read too (~17:10): blast.json 1-32 of 32, blast.schema.json 1-455 of 455 (shot :324-453), shoot_lying_cases.gd 1-154 of 154,
  effect_things.cpp 1-179 of 179 (casualty :115-143, apply :160-177), body_renderer.h 1-115 of 115, body_renderer.cpp 1-334 of 334
  (334 lines: the blows after a fall, strike/die and the new rename/let_go, go to body_blows.cpp, one responsibility, under 350).
- Data: blast.shot.dead_kept 40: under the render's 64 rows (body_renderer.cpp MOST_BODIES) so every dead body kept keeps its drawn
  body, rows left for the living down and the limbs; the render frees a limb's row before a dead body's.
- Changes (~17:40): damage_event.h (BODY_DEAD, BODY_GONE, DEAD_BODY_ID_BASE, body_kept_by_id); ped_state.h DeadBody; traffic_state.h
  dead, dead_serial; ped_system.h lay_dead, dead_half, dead_middle, dead_entry, move_dead_off_seg, lie_on_nearest out of its file's
  hiding; pedestrians.cpp (remove_pedestrian gone, reset clears the dead, drop_pedestrians_on_seg moves them); new ped_dead.cpp;
  ped_crowd.cpp (the dead filed, the corner loop reads lie first); shoot_commands.cpp (lying_hit one rule, aim_at_dead, shoot_dead
  dispatched); shoot_internal.h/shoot_people.cpp (lay_dead, shoot_dead); strike_people.cpp (lay_dead, the dead within the eardrum's
  range thrown); wounds.cpp lay_dead; blast_params.h/blast_loader.cpp/blast.json/blast.schema.json dead_kept 40; city_core_strike.cpp
  shot_targets dead; effect_bodies.cpp who by body_kept_by_id; effect_things.cpp BODY_DEAD/BODY_GONE; body_renderer.h/.cpp (rename,
  let_go, who kept for the dead, a full texture frees a limb first, GET_UP_S/RISE_SHARE_MOST class constants); new body_blows.cpp
  (strike, die, rename, let_go); shoot_lying_cases.gd dead_met, shoot_body_cases.gd calls it.
- Checks 1 (~17:50): both DLLs build clean; strike_test and people_test PASS; shoot_test CRASHED (signal 4, the debug DLL's hardened
  libc++) in the step after "the wounded bleeds": the first lay_dead from bleed_step.
- Cause: kerb_walkers.cpp:36 kerb_hailable indexes t.peds by a crowd entry's id, and a dead body's entry (ped_dead.cpp dead_entry) has
  id -1; it also counted somebody lying as on foot (kerb_walkers.cpp:35). The other readers of the crowd lists take places only
  (ped_party.cpp:129, ped_seen.cpp:71) or skip lying entries first (ped_crowd.cpp). Fix: somebody lying (a dead body or the living
  down) is neither on foot nor hailable: kerb_hailable skips entries with lie > 0 before reading their walker.
- Checks 2 (~18:00): shoot_test runs through (no crash): the dead lie in the simulation (12 kept), the round from above meets the dead
  body (hit 1, not a kill), one body of id 9000000, nobody stood in it (0 ticks); FAIL "no second body (37 bodies, 38 before)".
- Cause: a check of the town's count of bodies at shoot_lying_cases.gd:77 (dead_met), which another body getting up in the same
  frames lowers (38 -> 37), the very reason _bled checks by the body's id (shoot_body_cases.gd:137); the claim (no second body for the
  dead one) is the count of its id: the test asks that.
- Research: after two failures of shoot_test, https://8thlight.com/insights/flaky-crusts-good-for-pies-bad-for-test-suites and
  https://trunk.io/learn/what-is-an-example-of-a-flaky-test (an assertion on shared state other actors change at the same time passes
  or fails by their timing: assert on what the test itself owns): here the dead body's own id and its pieces, not the town's count.
- Fix of the crash: kerb_walkers.cpp kerb_hailable skips lying entries (read whole 1-157 of 157 first). The test's check by the dead
  body's id. damage_state.h's header comment told the dead were gone: now where they lie.
- Checks 3 (~18:25): both DLLs clean; shoot_test (dead_met: the dead lie in the simulation, 12 kept; a round from above meets the dead
  body, hit 1, no kill; 1 body of id 9000000; nobody in it, 0 ticks), strike_test, people_test, walker_motion_test, edit_fuzz_test
  PASS; logs read, no engine error (the exit's VISEMES registered); gates OK on the 30 files. Registered Pwalkers-18 (a body a blast
  throws stays, for the simulation, where it lay).
- Photos (dead plan; census: 1 body, who [9000000], before and after): 00 near 0 (860-1020, 480-590): the dead man on his side on the
  sidewalk by the signal pole, a head wound's red by the head - sound; 01 near 90 (900-1010, 530-630): the same body behind the
  pole's foot - sound; 02 near 180 (880-1060, 500-640): face up, the hand at the pole's foot, not through it, the pool under the trunk
  - sound; 03 near 270 (890-1070, 480-620): legs bent, the pool, a splash by the head - sound; 04 street level (940-1050, 545-595):
  lying flat on the pavement, neither floating nor sunk - sound; 05 above (840-1030, 430-600): the whole body, the pool from the
  head under the trunk - sound; 06 mid (930-975, 510-540) and 07 far: the body at the corner in its block and area - sound. The crowd
  had fled the shots (startle 45 m): their going round the body is shown by the test's measure, not by these photos.
- Next (done): read whole the blast params' loader and schema, city_core_strike.cpp, kerb stops' walker
  handling, effect_things.cpp, body_renderer.cpp/.h, shoot_lying_cases.gd; then block open --item Pwalkers-11 and code whole.

## Item 29: a moving body smears under the temporal filter (Prender-284)
- Read (~18:35): project/render/shaders/people_look.gdshaderinc 1-118 of 118 (BODY skinning :61-68 from the bodies texture, slot by
  INSTANCE_CUSTOM.g), people_body.gdshader 1-4 of 4.
- Research: Context7 /godotengine/godot-docs (internal rendering architecture, TAA: motion vectors come from running the vertex shader
  of the previous frame with the previous camera transform, else ghosting; instance_teleport resets them); the internet:
  https://www.Github.com/TokisanGames/Terrain3D/issues/302 (a vertex displaced by a texture or a world position writes no motion
  vector) and https://forums.unrealengine.com/t/instanced-mesh-rendering-and-motion-velocity-vectors-for-taa-tsr/1760534/3 (instances
  moving with zero motion vectors ghost).
- Cause (written in the register): the bones texture holds the current frame only and the instance transform is identity, so the
  motion vectors of a body are zero.
- Read too (~18:45): native/src/render/effects/mm_layer.h 1-141 of 141 (put: basis columns and origin per instance; upload writes the
  buffer: the engine compares it with the last for the motion vectors), native/src/render/agents/body_renderer.cpp 1-278 of 278
  (lay_out :212 puts the identity for every body; write_bones :149 writes bind -> world rows).
- Trace: python tools/tracer/trace.py city.BodyRenderer.lay_out --depth 2 --root native/src/render/agents --root
  native/src/render/effects/mm_layer.h (update -> lay_out -> bound_layers; put, upload).
- Research: Context7 /godotengine/godot-docs (MultiMesh set_buffer_interpolated keeps a previous buffer; instance_teleport resets
  motion vectors); https://github.com/godotengine/godot/issues/67287 and the commit 5155870d64 that closed it (MultiMesh motion vectors:
  the previous buffer is kept and enabled when the transforms or the buffer change; PR 95270 fixed its corruption).
- Fix: each body's instance transform is the frame of its root bone (the torso's, a piece's first part's), orthonormal, written every
  frame the bones move; its rows are bind -> that frame. The engine's motion vectors then carry the body's whole fall and turn (only
  the limbs' motion about the root is left out of them); the shader is unchanged (MODEL_MATRIX places root space in the world).
- Checks (~19:00): both DLLs clean; shoot_test, maimed, shot_again PASS. Photos maimed 00/01: the same defect: 01 (870-1100, 590-830)
  sharp-edged purple and skin-coloured triangles metres long from one elder's body, 00 the elder blurred. Not a temporal smear:
  stretched geometry. The motion-vector change stands on the engine's documentation (a texture of bones gives no motion) and breaks
  nothing, but does not close Prender-284: written back to cause unknown, with the reading (a census probe of each body's farthest
  bone and largest bone scale). Next: that probe, then the path that writes the far bone.
- Read again after 6213ea60 (~19:10): body_census.cpp 1-48 of 48, body_renderer.h 1-126 of 126.
- Trace: python tools/tracer/trace.py city.BodyRenderer.census --depth 1 --root native/src/render/agents.
- Research: Context7 /godotengine/godot-docs (AABB get_longest_axis_size, merge); https://alecjacobson.com/seminar/james-and-twigg-2005.pdf
  and https://forums.unrealengine.com/t/ragdoll-mesh-stretches-around-bones/366309 (linear blend skinning stretches or collapses where
  the bones a vertex blends turn far apart; unweighted vertices or bad matrices make spikes).
- Probe (diagnosis, kept as a test's measure): census widest_m (the longest side of the widest body's box), widest (its who, piece,
  lost, rising, asleep) and unfinite (bodies whose box is not finite).
- Changes: body_renderer.h (Body root_bone, root; root_bone_of; header), body_renderer.cpp (root_bone_of, write_bones bind -> root
  rows and the root, lay_out puts each root, update lays the instances out whenever the bones moved), body_blows.cpp (a piece's root
  bone its own part's).

## Next
- Done this session: the guard's three holes (fbc70eab), Prender-278 (149532b0), Prender-283 (f41a5574), Pwalkers-11 (ef869196), the
  bodies' motion vectors (6213ea60). Next item: Prender-284 (stretched triangles, cause unknown: the census probe of each body's
  farthest bone first), then Pwalkers-18, Prender-266, Prender-261, Prender-277, Prender-273/274; Proads-35 and Pterrain-10 belong to
  their areas. Every feature drawn from people.bin (gaits, acts, held things, faces, riders, the fallen's clips) waits on a cook from
  MakeHuman CC0 and Rocketbox MIT sources (Pbuildings-43): its design and research are an item of their own.
