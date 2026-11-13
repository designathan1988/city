# Research: Bring the reference game's features, correctly and fast

Each entry: the question, what was read or asked (with its source), the decision, why, and what else was weighed. The reference is
`C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` (read for behaviour only, CLAUDE.md).

## R1. How the reference's camera keeps out of the buildings

**Read whole**: `src/render/isoViewport.ts` 1-460 of 460 (the rig: `apply`, `settle`, `setSolids`); `src/world/buildings/cameraSolids.ts`
lines 8-16 and 440-518 (the solids' floor and roof: `floorAt`, `roofAt`, `lowestEye`); `src/render/renderer.ts` 3655-3685 (where the
solids are set). Context7 `/websites/unity3d_packages_com_unity_cinemachine_3_1`, CinemachineDeoccluder: "Minimum Occlusion Time: do
not take action unless the occlusion has lasted at least this long"; "Damping When Occluded: how quickly to move the camera to avoid
an obstacle"; "Smoothing Time: minimum number of seconds to hold the camera at the nearest point to the target"; "Damping: how quickly
to return the camera to its normal position after an occlusion has gone away".

**What the reference does** (behaviour, its own words in quotes):
- Two fields of the buildings (`cameraSolids.ts`): `roofAt(x, y)`, "the roof over (x, y) when the point is inside a solid volume's
  plan (its highest, no margin round it)", and `floorAt(x, y)`, "the lowest the eye may stand": a volume's top plus a clearance of 1 m
  over its plan, "easing down to the building's foot" over a reach of 2 m round it ("the camera's radius"), by a smoothstep of the
  distance to the plan. Volumes are filed by their plan's box in 50 m cells (a building is found from any cell its plan touches).
- Every frame, before drawing (`apply`), with no easing, in this order:
  1. "DO NOT lift the point whenever it falls in a building": only when the eye and the point are both inside walls, both rise over
     the roof plus the eye's lift.
  2. The eye inside walls and the point not: the eye comes in along the line to where it leaves them (halving 20 times).
  3. Both still inside (a point on a roof the eye's tower overhangs): the eye over the floor.
  4. The eye out of the walls but inside the margin: it steps back along the line, up to 4 m ("BACK"), out of the margin.
  5. The near plane "short of the nearest building": `min(distance * 0.02, (gap + 1 m) * 0.5)`, never under 0.08 m, `gap` the eye's
     height over the floor.
  6. Never under the ground: 1.6 m over it, still looking at the same point.
- The occlusion pull (only near, by a weight of the zoom, `pullWeight`): 48 samples of the line, each in a building's soft margin
  (1.5 m) weighing smoothly; eased by Cinemachine's four times: wait 0.35 s, in 0.12 s, hold 0.4 s, out 0.6 s.
- The solids are built only when the camera tilts low enough to enter a building (`elevation < 1.2` rad) and only round the target
  and the eye (prefetched cells): never the whole town.

**Decision**: this game's rig does that behaviour in its own design. What this game already has of it: `CityCore::eye_free` cases 1
to 3 and `sight_pull` (the pull) mirrored the reference, but (a) its building query was blind (only lots whose centre is within 1 m:
Psimulation-83, fixed by `BuildingRecords::meeting`), (b) it lacks the floor field with its margin and case 4, (c) the near plane is
not shortened by the gap, (d) the roof read the ground at the point, not the lot's floor (fixed: `b40::lot_floor`), (e) pitched roofs
were read as flat (fixed: `roof_height_at`). The session's spring arm (never lifting the point) departs from the reference's rule 1
and is replaced by the reference's behaviour. The values (1 m clearance, 2 m reach, 4 m back, 1.5 m soft margin, the four times, the
near plane's shares) are what the game must do: they go to `project/data/game/camera.json`, read by DataTables, never as literals.

**Why**: the reference is the game with everything working (CLAUDE.md); its soft margin is what makes an eye meet a wall smoothly
(the floor rises over 2 m before the wall), which this game's version missed, so its corrections struck the walls as jumps.

**Alternatives**: the standard spring arm (Unreal, Cinemachine) alone: smooth only with a short arm, and it never lifts the point,
which the reference does when both stand inside; clipping through buildings (Cities: Skylines 1): against the order that the eye
never enters a building.

**What the tests must state**: the reference's corrections are at once by design, so a wall reached is a step of the eye; the
margin and the look ahead make it rare and small. The camera tests count the frames with the eye inside walls (at most 1 in 1,000:
`INSIDE_SHARE`), the jolts of the rig's own smoothing (none), and report the at-once corrections with their size (SC-002).

### R1b. Superseded (2026-11-13): what shipped engines do, from their documentation

The decision above failed its checks three times in this session (at-once corrections 5.6% of frames; then eased corrections with a
margin grown with the view: jolts up to 14 views, camera_feel_test 158 s; dist/tasks/20261113-030533-34572-camera_feel_test). The
causes read: a correction computed from the eye's spot alone jumps whenever its answer changes topology (the line's first exit passes
a corner, the margin's way flips, rule 1 lifts both by a whole building at once); easing a jumping answer trails into the walls.
Researched on the internet (sources below), the engines split the problem in three and none of them computes it as this game did:

1. **Collision of the camera's own body** (Cinemachine Decollider, Unreal SpringArm): the camera is a sphere of `Camera Radius`;
   obstacles overlapping it push it out "along the direction toward the target"; "if the sphere doesn't actually intersect an object,
   the camera isn't moved" (Decollider manual). Unreal's SpringArm sweeps a sphere of `ProbeSize` from the target to the desired
   position and places the camera at the hit (`bDoCollisionTest`, `ProbeChannel`); its lag smooths the target's motion, the collision
   displacement is reported apart (`IsCollisionFixApplied`, `GetUnfixedCameraPosition`) and the default blend returns the hit location
   itself. Undersander (Game Developer, "Accurate collision zoom for cameras") sweeps the near plane's rectangle, not a ray: what must stay
   out of the walls is the near plane, so the radius is the near plane's half diagonal plus a pad.
2. **Occlusion of the line of sight** (Cinemachine Deoccluder): strategy "Pull Camera Forward" moves the camera along its axis in
   front of the nearest obstacle; "Minimum Occlusion Time: do not take action unless the occlusion has lasted at least this long";
   "Damping When Occluded: how quickly to move the camera to avoid an obstacle"; "Smoothing Time: minimum seconds to hold the camera at
   the nearest point to the target"; "Damping: how quickly the camera returns after an occlusion clears". This game's CameraPull is
   this already (pull_wait_s, pull_in_s, pull_hold_s, pull_out_s).
3. **Height over terrain and roofs**: the Decollider casts "downward from above the camera" and moves it up to the hit (terrain
   layers); the RTS camera of Halo Wars reads a proxy surface instead of the raw ground: a sparse heightfield with "two-pass asymmetric
   smoothing", because a symmetric blur "brings everything toward an average, which isn't what you want for a camera" (Sullivan, via the
   LinkedIn post below; SpringRTS's thread: the camera follows a smoothed height field, not the terrain). A two-pass forward/backward
   sweep that keeps each cell at least its neighbour's value less a slope times the distance is the chamfer distance transform's
   propagation (Felzenszwalb and Huttenlocher, Theory of Computing 8, 2012, for the linear-time envelope of sampled functions; the
   chamfer two-pass scan, arXiv 2106.03503): the upper envelope of the roofs whose slope never passes a set value.

**Decision** (this game's design, the reference's behaviour kept where it is right: the eye never in a building, the point raised
over a building it stands in, the pull with the four times):
- **The camera's floor** (`CameraFloor`, the core's derived data, built per 64 m tile from CityState's buildings under the shared lock
  only for the tiles the camera asks, invalidated through ChangeLog, an LRU of tiles so memory never grows with the area explored):
  2 m cells, each the highest roof top plus `eye_roof_clearance_m` over it (the ground where no building stands), then the two-pass
  slope limit `floor_slope` (the cell kept at least its neighbour less the slope times the step). Read bilinearly: continuous, its slope
  bounded by construction, one lookup a question.
- **The point raised** (reference rule 1, made smooth): the point's rest height is the higher of the ground and the floor at the
  point, by the pull's zoom weight (full at `pull_full_half`, none at `pull_none_half`: far views never ride the roofs), through the
  ground spring with two stages (`CameraDamp.ease_to`), so the point rises over a building before the eye could reach its wall and the
  rise starts from rest.
- **The eye's body** (Decollider): a sphere of radius `max(near half diagonal + pad, frame move share)` at the orbit's eye; the core
  answers the sphere's push-out toward the point (`eye_clear`), zero when it overlaps nothing; continuous, since it depends on the
  eye's own spot and the walls' distance only.
- **The line** (Deoccluder, unchanged CameraPull): the share taken off with its wait and times; while the eye is held in front of a wall,
  its release goes no farther than the line is free (Smoothing Time's hold).
- **Rejected**: the eased correction of a jumping answer (it trails into walls); the eye's own floor lifted over roofs (it lifts the
  street view beside every tower); the at-once rules (5.6% of frames struck walls).
- **Proof before code** (no run to find out): the worst vertical acceleration of the point is bounded by `floor_slope` times the
  fastest pan (1.94 views a second) through the two-stage spring of `ground_smooth_s`; the sweep's push-out is a continuous function of
  the eye's spot; both computed in the camera's sheet (T026) against the jolt limit (1% of the view a frame) before T027 is coded.
  The camera is the port's last area (the owner, 2026-11-13: the port first, the camera last).

**Sources**: [Cinemachine Deoccluder](https://docs.unity3d.com/Packages/com.unity.cinemachine@3.0/manual/CinemachineDeoccluder.html);
[Cinemachine Decollider](https://docs.unity3d.com/Packages/com.unity.cinemachine@3.1/manual/CinemachineDecollider.html);
[Unreal USpringArmComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/USpringArmComponent);
[Unreal spring arm forum: BlendLocations](https://forums.unrealengine.com/t/enable-camera-lag-on-spring-arm-only-for-collisions/379251);
[Undersander, Accurate collision zoom for cameras](https://gamedeveloper.com/programming/accurate-collision-zoom-for-cameras);
[Halo Wars style proxy camera (LinkedIn post)](https://pt.linkedin.com/in/factobot);
[SpringRTS camera height thread](https://springrts.com/phpbb/viewtopic.php?p=29215);
[Felzenszwalb and Huttenlocher, Distance Transforms of Sampled Functions](https://theoryofcomputing.org/articles/v008a019/abstract.txt);
[The Distance Transform and its Computation, arXiv 2106.03503](https://arxiv.org/pdf/2106.03503);
[Unreal critically damped smoothing](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/FMath/CriticallyDampedSmoothing);
[Cities: Skylines street level was never intended (community)](https://steamcommunity.com/sharedfiles/filedetails/comments/1756089251);
[Manor Lords: seamless zoom to ground level (preview)](https://www.techradar.com/gaming/manor-lords-preview-camera-feature-brings-you-down-to-earth).

## R2. The inventory of both games and their pairing

**Done and checked**: `python tools/atlas/cli.py inventory` (tools/atlas/inventory*.py, rules only, no hand list). Run on 2026-11-13:
1.27 s; reference 866 files (100 actions, 861 data entries, 859 systems, 1827 texts), this game 1172 files (767 actions, 188 queries,
385 data entries, 812 systems, 118 tests, 2632 texts); 2998 pairs by evidence (same English text 2569, same Portuguese text 2524,
same key 1536, same id 228). Six sampled rows matched their source lines (strings.csv:253, strings_ui.csv:95, en.ts:1112, en.ts:222,
puffs.h:1, sourceMap.ts:1).

**Decision**: the inventory is the list of what each game has; it pairs by evidence only and leaves the behaviour to the comparison
sheet (R3). Its gaps that need the sheet: actions the reference builds at run time from keys, features with no text, systems paired
only by area.

**Found**: 1536 of this game's text keys are the reference's own keys (`same key`): a name taken from the reference, against CLAUDE.md
("no name or data key"); the copy scan (LAB_COPY, lab_scan.py) does not read the text tables (FR-015).

## R3. One feature's comparison sheet

**Decision**: per feature, `python tools/tracer/trace.py --feature <output> --root <its files>` on each game, then each file the trace
reached read whole (city-method section 3, item 4), written as a sheet in `doc/specs/003-roadcraft-features/sheets/<area>-<name>.md`
(contract: contracts/comparison-sheet.md): the reference's outputs, conditions, cases, values with file:line, its costs (`--cost`) and
faults (the list of city-method section 3, item 8), this game's same, the differences, the design here.

**Why**: the tracer reads, never runs; the sheet is the requirement; the reading prevents the trial and error this session did on the
camera (six runs, each finding a cause the reading of `isoViewport.ts` would have given at once).

## R4. Why the 24 km city opens slowly (window_traffic_test 90 s)

**Read** (`native/src/sim/simulation/growth.cpp` 110-198): `fill_all` walks every lot of the map one after another (183-196); for a
lot with neighbours, `best_candidate` makes four complete bodies (`record_body`, `LOOK_CANDIDATES` = 4) in a 4-way `parallel_for`
only to compare their looks (`body_look` reads volumes, main wall paint, main roof, use) and keeps one (131-147). Measured: the debug
DLL's opening of km24 69.8 s (dist/km24/this.log:4), the release 23.6 s (doc/specification.md:312); 473,839 buildings. The 1200 ticks
after it take at most about 15 s.

**Decision**: (1) a candidate's look from its massing only, before the lot's layout and facades, and the chosen look kept, so a lot
costs one body; (2) `fill_all` in a 2 x 2 checkerboard of 256 m cells over the job pool (each cell's lots in order; every read of a
neighbour reaches under 256 m: the look reach 100 m, the organic reach 30 m), the same city for the same seed whatever the pool; (3)
`settle_sites` reusing the edges `record_for` read and its sites made in parallel, inserted in order; then measure; the lasting fix,
derived by region where a reader looks (Psimulation-14), stays registered for its own work. The city's digest moves with (2): the
reference written again with its cause in the history.

**Why**: the cost is per lot and serial; the window traffic test proves the window's share of demand, which needs the big map, so the
map is not changed (that would hide the defect).

**Sources** (2026-11-13): the usual way to generate chunks in parallel yet deterministically is a generator per chunk seeded from
the master seed and the chunk's coordinates, since "the order in which threads complete is non-deterministic"
([seeds and determinism](https://abratabia.com/procedural-generation/seeds-and-determinism.php)); neighbours are read only from
passes already finished ([Voxel Tools, procedural generation](https://voxel-tools.readthedocs.io/en/latest/procedural_generation/);
[LayerProcGen](https://runevision.com/tech/layerprocgen/): layers that depend on neighbouring chunks generated first). The 2 x 2
checkerboard is that rule with four passes: no two cells running at once are neighbours, every neighbour read lies in a finished
pass or in the cell itself, each cell's lots in their fixed order with the cell's own seed.

## R5. The fast loop and the slow checks

**Measured** (dist/tasks runs of 2026-11-13): test scenes 1.7-37 s except window_traffic_test 88-90 s and camera_feel_test 57-66 s
(after the sight was fixed: its eye_free calls per frame); photo plans `roofs` and `photos` 16 s each; an item's check runs its
scenes and its plan in one `cli.py run`, the build, import and script check from the cache when unchanged.

**Decision**: the two scenes over a minute are defects of their own (FR-011): window_traffic_test by R4; camera_feel_test by asking
the core once per frame for the eye (the test's own extra `eye_free` call reuses the rig's answer) and by fewer rides where its
cases repeat (each zoom x tilt x bearing x start x six motions: the cases the reference's behaviour distinguishes are kept, the
repeats measured and dropped only where two rides prove the same thing).

## R6. Photos aimed at the element changed

**Done**: `CityCore::highest_roof` and the look step `setViewRoof` (the rig's point stays on the ground, so the view is held on the
roof with `CityCamera.pin`). **Decision**: the same for each kind of element a change touches: a step that aims at the nearest
element of a kind to a point (a building's roof, a pole, a vehicle, a walker: the core's probes already answer positions:
`shot_targets`, `street_watch`), so a plan names the element, never guesses coordinates.

## R7. Documents and tools that contradict or are dead

Two reading agents surveyed; each claim is checked in the files when its task acts on it (CLAUDE.md agents rule).

**Tools** (survey of 2026-11-13; no Python import under tools/ resolves to nothing; all 59 schemas referenced):
- Unused, to remove: the task-as-a-Python-call mechanism (`tools/tasks/call.py`, `Task.call`, its branch in `suites.py`, the `call`
  field of suites.schema.json: no suites.json task uses it); the tracer's bench (`tools/tracer/bench/**`: `projects.json` is `{}`, so it
  measures nothing); `project/tests/visual/capture_falls.gd` and `capture_blast.gd` (no caller); `tools/tracer/tests/run.py` (a second
  test runner beside `unittest discover`, 600 s a file, its own process handling).
- Broken by design: the pair captures (`capture_parity.gd`, `capture_street_pairs.gd`, `pair_camera.gd`, the `_pair` shots of
  `capture_roads.gd`) match photos of the 4.0, which is no reference; `capture_transit.gd:2` cites the removed `ports/`.
- Run by nothing: `capture_clock`, `capture_features` (+ `capture_flag`), `capture_roads` (+ `capture_road_tools`), `capture_street`
  (+ gestures, views), and `capture.gd`'s `--only`/`--soak` branches (Ptools-18): each becomes a `looks`/`capture_scenes` item or goes.
- Duplicates: `inventory_ts.py` tokenises TypeScript beside `tools/gates/lex_c.py`, which lexes `.ts`; `inventory_json.py` positions
  beside gate_data's JSON reading (to check).
- Slow: window_traffic_test 89-102 s, camera_feel_test 57-66 s (both tasks of this specification); deadlines far past a minute on
  the city capture (1800 s), gates_all (1200 s), hunt (600 s), km24 (900 + 1500 s), pgo (900 s a scene).

**Documents**: the survey's findings are appended when it returns.

## R9. Beyond the reference: the genre's systems and the reference's own faults

**Decision**: (1) each comparison sheet judges the reference's rule against its norm (traffic: CTB/MBST; poles: CELESC E-313.0002;
signals: published warrants; people: published walking speeds) and records where this game goes further; (2) a genre gap list,
`doc/specs/003-roadcraft-features/genre-gaps.md`, from the standard city builders' documented feature sets (to be researched with
cited sources: the publishers' feature pages and manuals of Cities: Skylines, Cities: Skylines II, SimCity 2013), each system marked
present here (its scene), in the reference only, or in neither; the "neither" systems become items of the loop by a cited method;
(3) every brought system measured against doc/specification.md's budgets on the median and floor profiles (km24 on the owner's
order at a block's end).

**Why**: the owner: the reference is not all right nor complete; the product is sold on Steam against the genre, on median PCs.

**Done** (2026-11-13): the gap list is [genre-gaps.md](genre-gaps.md), from the publishers' feature pages and dev diaries (Cities:
Skylines II's economy, lifepath and traffic pages opened and their sentences checked here), 18 systems; six named by neither game
(water and sewage, land value here, happiness, pollution, disasters, modding).

## R8. Making the method hold (skills, Context7, no trial and error)

**Decision**: the block's close (`tools/tasks/block.py close`) refuses while the work file `doc/work/<block>.md` lacks, for each item:
the reference read (its sheet or `Read` lines), the Context7 answers (`Research`: source, question, answer) for the APIs it calls, and
the skills used (`Skills`: city-method, systematic-debugging when a check failed, verification-before-completion). A project tool, not
a hook (the owner refused hooks).
