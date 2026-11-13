# Effects, sky, light and weather: the area's index (specification 003, T019)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present: the sun and moon over the day; the gradient sky; rain streaks; lightning and thunder; placed clouds with drift and
  bodies; mist and far haze; painted fog; Godot's own shadow cascades, night windows, bloom; the explosion; craters (beyond the
  reference: they cut roads, blast_model.cpp is this game's own Kingery-Bulmash); building damage, collapse, ruins, fire;
  hydrant geysers.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | Vehicle exhaust and road dust | ABSENT, medium | no emitter: puffs only from blasts; the reference: idle 0.15 puffs/s, moving 0.5 + min(1, 0.02 speed), dust 6/s on unpaved roads | native/src/render/agents; exhaust.ts:205-216, renderer.ts:3830-3832 |
| 2 | Lamp beams in rain at night | ABSENT, medium | lamp_glow's `haze` never set, fixed 0 in materials.json; the reference: night x max(rain, storm) | lamp_glow.gdshader:13,31, materials.json:316; renderer.ts:3909, scenery.ts:234-236 |
| 3 | Canyon light through the day | PARTIAL, medium | its strength set only when canyon_revision moves (buildings, the view), not with the hour or weather; the reference every frame, 0.85 - 0.35 x direct share | sky_view.gd:136-140,249, canyon_field.cpp:81,152; canyonLight.ts:369-372 |
| 4 | Cloud shadows by the light | PARTIAL, medium-low | a constant 0.62 x (1 - dark): 62 % under rain; the reference: direct share x max(0, 1 - 1.5 night) | atmosphere.gdshader:71; postprocess.ts:451,504-507 |
| 5 | Poles and wires in a blast; a signal post knocked out | PARTIAL, medium | whole or splinters and one spark burst; no snap with a stump, no torn wires pulling the next poles, no arc sparks; a knocked signal post leaves its junction's control on | effect_things.cpp:49-60, strike_ground.cpp:35-63; playEffects.ts:418-460, actionsWiring.ts:299-339, blast.ts:572-584 |
| 6 | The bulldozer's reach | PARTIAL, medium | a click takes building, rubble, item, road (no pole); a box only buildings, rubble, roads; the reference's box also lots, poles, items, barriers | road_commands.cpp:211-249, demolish_box.cpp:20-49; bulldozer.ts:94-148 |
| 7 | Smoke with the weather's wind | PARTIAL, low-medium | a fixed wind from the effects' parameters | effect_world.cpp:49; wind.ts:22-24, exhaust.ts:58 |
| 8 | Small differences | PARTIAL, low | overcast sky colour and horizon share; lightning flash's light; strikes land twice as far and bunch at the centre, bolt top fixed; no cloud fade far out; one flow for fog wisps; no sparking wiring or leaking pipes in a hit building | daylight.json:57-58, weather_view.gd:87,136, atmosphere.gdshader:150-210, effect_blast.cpp:107-111 |

## Translated from the reference (redone in this game's design when built)

daylight.gd (cites a render/scene.ts that does not exist; the source is environment.ts:249-306), sky.gdshader, weather_view.gd and
rain.gdshader (line by line), sky_view.gd (the same hash, puff table and slot layout), atmosphere.gdshader and
cloud_field.gdshaderinc, canyon_light.gdshaderinc (identical constants), puffs.gdshader and puffs.cpp (exhaust.ts verbatim),
effect_blast.cpp, demolish_tool.gd, demolish_box.cpp, strike_commands.cpp, sky.h's scatter_clouds.

## Reference faults never brought

Its own 1189-line cascade system (sunCascades.ts); quarter-resolution clouds with a history buffer (postprocess.ts:301-357); the
clouds re-laid and resent each frame (postprocess.ts:470); every footprint edge of each nearby building walked per blast and
buildings broken four a frame (actionsWiring.ts:250-262,400-405); building pieces cut on the main thread in 4 ms slices
(destruction.ts:19); per-puff GPU update ranges (exhaust.ts:198-201); canvas textures for craters (blast.ts:133).
