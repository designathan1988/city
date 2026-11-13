# people: what people do (the gait and the activities)

- State: read (the reference's crowdAnimation.ts and citizenGait.ts read whole; its clips' data are frames, read as their names;
  citizenPlays.ts and riderPoses.ts to read with the item's design; this game's ped_acts.cpp, people_pose.gdshaderinc and
  people_renderer.cpp read whole)
- Inventory rows: r2705-r2756, r2757-r2808 (the Rocketbox clips by name), r2817 `render/people/crowdAnimation`, r2474
  `RIDER_CLIPS/bikePedal` | this game: native/src/sim/traffic/ped_acts.cpp, native/src/render/agents/people_renderer.cpp,
  project/render/shaders/people_pose.gdshaderinc, project/assets/people/people.bin

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/render/people/crowdAnimation.ts 1-365 of 365 (the GPU's crowd animation: every clip's frames in one
  texture, up to 4 plays blended a person, a CPU palette for a ragdoll or a wounded body, a refit to each body's joints);
  src/render/citizenGait.ts 1-608 of 608 (what the legs play, from what the simulation says).
- Rules and values (citizenGait.ts): walks blended by pace among the shuffle, the slow walk, the person's own walk style for life
  (six Rocketbox walks by id) and the brisk walk (201-262); stride ∝ pace^0.6 below the slowest capture (293-294); a run blended in from
  a per-person speed of 1.75-2.10 m/s (197-198, 554-555); walk start and stop clips joined by their travel curves (284-292, 507-547);
  turns on the spot stepped by the feet (266-283, 469-491); back, left and right walks weighted by the direction of travel (549-569);
  elder, drunk, hand-in-hand and carrying walks (94-97, 200-209); standing: a party's turns of talk with laughter and argument by mood
  (350-361), crouch down, stay, up (365-371), phone, a look at what it waits for, headphones and bag at a kerb (374-383); the gestures a
  person stops on the walk to do: read, drink, photo, wave, dance, cheer, umbrella, trolley, knock, eat, work (387-392); benches: sit
  down, seated, stand up in each sex's seconds (436-443, view.ts:45-46); crossfades 0.18-0.3 s.
- Faults never to bring: a gait object a walker on the CPU every frame (AGT-32 its own note), Maps per style, plays rebuilt per frame.

## This game
- Read whole: native/src/sim/traffic/ped_acts.cpp 1-72 of 72; project/render/shaders/people_pose.gdshaderinc 1-91 of 91;
  native/src/render/agents/people_renderer.cpp 1-336 of 336.
- One walk cycle a body variation, its phase the metres walked over the variation's stride (people_renderer.cpp:284), blended with
  standing by the eased rest share (people_pose.gdshaderinc:62-90); 8 acts, only while waiting at a kerb (ped_acts.cpp:36-52,
  pose_buffer.h:45); a crouch played from its start (people_renderer.cpp:300-304, 325-334); riders seated by pose: car, cab, chair,
  moto, bicycle pedalling and their stops (people_renderer.cpp:37-38, 247-264): the reference's rider clips paired by behaviour.
- The clips come from people.bin, MakeHuman (CC0) bodies on the Rocketbox (MIT) skeleton and clips (people_asset.h:1): the same
  open library the reference animates with; its cook tool is gone (registered: the asset cook tools were removed).

## Differences
- Gait: one walk at one stride against the reference's walks blended by pace with a walk style for life, the run, start and stop,
  turns on the spot, back and side steps, elder, drunk, hand-in-hand and carrying walks.
- Activities: 8 acts at the kerb against ~24 gestures, pauses anywhere on the walk and benches.

## Design here (T008, one item each, after the cook tools are back)
1. The cook tools back (tools/assets from git), then the Rocketbox clips the gait needs cooked into people.bin with each clip's
   stride and travel (the library's MIT clips: walk styles, run, start, stop, turns, back and side walks, sit down, seated, stand up,
   and the gestures' clips); memory and the shader's clip table measured against the budgets (people_renderer.cpp CLIP_TABLE 24 grows
   with the clips: its limit is the GPU's uniform room, read before).
2. The gait on the GPU, as the reference keeps it there and this game's people are already drawn: per walker, two walk clips and
   their weight by pace (the pace already in the pose record), the run share and the direction weights, written by the simulation's
   pose record (AgentRecord), blended in people_pose.gdshaderinc; no gait object per walker on the CPU (the reference's own fault).
3. The activities in the simulation (PedAct and its data in traffic.json): pauses on the walk by place and hour, benches (the street
   furniture's benches as seats), the gestures by archetype of party (people-walking's parties), each with its clip.

## Proof
- A scene counting each act and gait clip over an hour (the share of walkers by pace band, running, pausing, seated) against its data;
  photos of a walker at each pace, one running, one seated on a bench, a party talking (setViewNear walker), near and mid, day.
