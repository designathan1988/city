# people: the hurt, the fallen and the dead

- State: read (both games' files read whole, 2026-11-13)
- Inventory rows: r2900 `sim/people/casualties`; r2457-r2460 `render/ragdoll`, `ragdollAxes`, `ragdollGround`, `ragdollJolt`; the
  clips hitChest, hitHead, getUpFront, getUpBack, staggerBack, crawl (people-motions) | this game: sim/commands/strike_people.cpp,
  shoot_people.cpp, shoot_commands.cpp, shoot_internal.h; sim/demolition/wounds.cpp, damage_event.h; sim/traffic/ped_fright.cpp,
  ped_wounds.cpp; render/agents/ragdoll.h/.cpp, body_renderer.h/.cpp; render/effects/effect_bodies.cpp, effect_things.cpp

## Reference (read for behaviour; nothing copied)
- src/sim/people/casualties.ts 1-75 of 75: a casualty record (where, heading, fate dead, torn or knocked, the blast point and its
  power 0..1, limbs lost before and severed now, how long a knocked one lies, crawl, faded (bled out where they lay), charred, the
  part a bullet struck, opened); wounds kept apart (256 at most) for the clothes; records kept a minute.
- src/render/ragdoll.ts 1-1980 of 1980: the body built from the pose drawn at the blow (20 particles on the skeleton's joints and a
  belly point); a blast throws it away and up with a pitch, the limbs scattered; a bullet only pushes the struck part and the body
  goes over the way the round went (a leg wound buckles the knees); the knocked put their arms out; a torn body thrown harder, blood
  and pieces; severed limbs a ragdoll of their own drawn with a twin of the body, now and then the bone out of the stump; charred
  bodies draw up by muscle pulls; the gravely hurt writhe (knee pulls) and after 4 s crawl off (the crawl clip); the knocked get up
  by how they lie (getUpFront or getUpBack, else a key-pose roll, hands and knees, crouch, then crouchUp), the simulation told where
  they stand; the dead kept for good (40 bodies, the oldest dead first); the dead can be shot (push, blood, a limb off at the second
  hit, the belly at the second trunk hit, a heap at twelve); pools under each piece when it rests, splashes on hard landings, a torn
  body sprays while it flies; occupants of a struck building thrown from their floor; physics in Jolt (12 parts: a box trunk and
  capsules, cone and hinge joints with limits, weak motors at knees and elbows, heightfield tiles shared by the bodies on them, prisms
  for the walls) or Verlet (sticks with ranges, hinges, ankle limits, bone capsules, walls and roofs). ragdollAxes.ts 1-60 of 60
  (joint frames always valid), ragdollGround.ts 1-72 of 72 (heights in tiles while a body moves on them), ragdollJolt.ts 1-398 of 398.
- Faults there: Math.random in the renderer (the same blow never falls the same way), allocations per body and per frame
  (`Math.min(...map)`, clones in `prepare`), 3.5 MB of physics loaded for a few bodies, the dead kept for good at 40 bodies, the
  casualty records shared state between the simulation and the renderer.

## This game
- Read whole: strike_people.cpp 1-136 of 136 (blast probits per walker: dead, torn, knocked, maimed; a leg lost crawls and bleeds),
  shoot_people.cpp 1-104 of 104, shoot_commands.cpp 1-147 of 147, shoot_internal.h 1-28 of 28, wounds.cpp 1-41 of 41, damage_event.h
  1-43 of 43, ped_fright.cpp 1-222 of 222, ped_wounds.cpp 1-16 of 16, ragdoll.h 1-63 of 63, ragdoll.cpp 1-158 of 158, body_renderer.h
  1-72 of 72, body_renderer.cpp 1-234 of 234, effect_bodies.cpp 1-163 of 163, effect_things.cpp 1-167 of 167.
- The simulation decides who dies, falls, is maimed or bleeds (deterministic draws of the blow's stream); the render is told once
  by a CASUALTY event (the ChangeLog's channel, no shared state); the body: the walker's own cooked mesh, its bones from a
  20-particle PBD ragdoll (knees and elbows hinged, walls as boxes), at most 64, bones in a float texture row written only while it
  moves; a knocked body gets up by blending to the crouch then crouchUp.

## Differences (each registered)
- Prender-261: crawl, opened belly and struck part sent and never drawn.
- Prender-262: bodies go after 90 s; the gravely hurt vanish while down, a bled-out one leaves nothing.
- Prender-263: the body falls from the idle pose; severed limbs thrown as tinted cylinders.
- Pwalkers-11: a shot at somebody on the ground meets a straight capsule; the dead cannot be shot.
- Prender-264: no ankle or neck limits, no bone collision, no roofs, no joint friction.
- Pwalkers-12: a trip is a crouch, not a fall.
- Prender-265: pools at the blow, not at the body's rest.
- Prender-266: charred bodies do not draw up, one get-up, no occupants thrown.
- Pwalkers-9 (already registered): bodies pass through one another.
- Here better: deterministic draws, events instead of shared records, bones written only while a body moves, no physics engine
  loaded.

## Research (2026-11-13)
- Context7 /godotengine/godot-docs 4.7 (ragdoll_system.rst, class_physicalbonesimulator3d.rst, using_jolt_physics.rst): Godot's
  ragdoll is PhysicalBone3D nodes under a Skeleton3D, each bone a body with a cost; this game's bodies have no Skeleton3D (instanced,
  bones in a texture), so the engine's ragdoll would mean a node tree per body: not taken.
- XPBD rigid bodies with joint limits (swing, twist, hinge): Müller, Macklin, Chentanez, Jeschke, Kim 2020,
  https://diglib.eg.org/handle/10.1111/cgf14105 (the paper's full text to be read for the limit formulas before the code).
- Getting up by orientation, blended from the lying pose to the clip's first frame: Unreal Pose Snapshot
  https://docs.unrealengine.com/4.27/AnimatingObjects/SkeletalMeshAnimation/PoseSnapshot; the face-up and face-down clips, Unity 5.x
  Animation Cookbook https://subscription.packtpub.com/book/game-development/9781785883910/9/ch09lvl1sec96/getting-up-from-a-ragdoll.

## Design here (T008)
- The fallen body as simulation state (owner sim/traffic, a record of each fallen: who, fate, where it rests, its parts' capsules
  once settled, lost parts, crawl, bleed): its life follows the simulation (down, bleeding, dead), the dead capped by count, the
  oldest first; rounds meet it as it lies (Pwalkers-11, Prender-262). The render's ragdoll stays the motion (pure C++, no engine
  ragdoll) and tells the settled place back through the render's own channel to the simulation's record at rest only.
- The ragdoll (render/agents/ragdoll.cpp): XPBD angle limits at ankles, neck and hips; bone capsules against the ground and walls;
  roofs as tops; joint damping while falling (Prender-264); bodies against one another within a cell grid (Pwalkers-9).
- The body begins from the walker's drawn pose (its clip and phase, sent with the event) (Prender-263); a severed part is a piece of
  the same cooked body on a ragdoll of its own, drawn with the lost-part mask inverted.
- Each flag drawn (Prender-261): writhe then crawl (the clip), the belly opened, the push at the struck part; trips as falls through
  the same event (Pwalkers-12); pools under each piece at rest and splashes (Prender-265); get-up by orientation, charred drawn up,
  occupants thrown (Prender-266). The clips needed (getUpFront, getUpBack, crawl, hit*) checked in people.bin once the cook tool is
  restored (Ptools-115).

## Proof
- A probe of each body (bones' error, depth under ground and in walls, speed, asleep); a test that shoots a body on the ground and
  one that keeps a bled-out body; photos (setViewNear walker) of a blast, a shot walker going over, a writhing and crawling one, a
  get-up face down and face up, a charred body, near, mid and far, day and night.
