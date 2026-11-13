# people: how people are drawn (the body, its levels, the far card)

- State: read (both games' files read whole; body_renderer.cpp, the fallen's drawing, is people-casualties')
- Inventory rows: r2809 `cookedLoad`, r2810 `cookedPerson`, r2811-r2816 `VERTEX_LAYOUT/*`, r2826 `lodPool`, r2827 `lodWorker`, r2828
  `paletteTargets`, r2829 `personImpostor`, r2830 `personMaterial` | this game: people_asset.h/.cpp, people_renderer.cpp,
  people_levels.cpp, body_renderer.cpp, people.gdshader, people_far.gdshader, people_blob.gdshader

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/render/people/cookedPerson.ts 1-292 of 292 (a body a file: one skinned mesh, its clothes, hair and
  eyes fused, 26 bytes a vertex quantised (KHR_mesh_quantization), two levels of 5,000 and 2,504 triangles in one index buffer, a level
  chosen by its error as a share of the body's height, sparse face moves, an impostor strip of yaw views); cookedLoad.ts 1-39 of 39
  (read in a worker, the atlas a KTX2); personImpostor.ts 1-132 of 132 (past the coarse mesh a card turned to the eye showing the
  nearest yaw view, lit as a cylinder, casting nothing, under 8 device pixels); lodPool.ts 1-60 of 60 and lodWorker.ts 1-23 of 23 (the
  simplifier's lanes for vehicle parts: "nothing of a person is simplified in play"); personMaterial.ts 1-160 of 160 (one material
  for every body: skin wrapped past the terminator with a scattering tint, hair lit along its strands in two lobes, skin and hair dull
  to the sky, the mouth's inside dark, roughness from the vertex); paletteTargets.ts 1-103 of 103 (the palette passes' float targets
  shared by width: WebGL's own mechanism; this game samples the cooked palette in the vertex shader: justified, not brought).
- Copies found here (registered): people_look.gdshaderinc keeps personMaterial.ts's values and names it as its source.

## This game
- Read whole: native/src/render/agents/people_asset.h 1-78 of 78, people_renderer.cpp 1-336 of 336, people_levels.cpp 1-42 of 42,
  project/render/shaders/people_far.gdshader 1-48 of 48: the bodies' levels by distance (lod_m), the coarsest casting nothing past the
  shadows' reach, a card under the card's pixels on the frame's lens, a blob under the far ones, a hold at a level's edge so it does
  not flicker; the card a translation of the reference's impostor shader (registered).

- Also read whole: native/src/render/agents/people_asset.cpp 1-315 of 315 (people.bin read and checked end to end: clips, card,
  variations, their levels' meshes with float attributes, the palette as 16-bit fixed point in RGBA8 pairs, the face table, the
  ragdoll's joints; the atlases DXT5 with mips); project/render/shaders/people_look.gdshaderinc 1-118 of 118 (the light: the copy
  registered), people_skin.gdshaderinc 1-29 of 29 (the four-bone blend from the palette rows); body_renderer.cpp 1-234 of 234.

## Differences
- The reference quantises a body's vertex to 26 bytes (KHR_mesh_quantization, cookedPerson.ts:20-33); this game's meshes are made
  of float attributes (people_asset.cpp:54-114): to measure against the budgets and compress (the engine's compressed vertex
  attributes, read in Context7 first), part of the drawing item of T008.
- Paired by behaviour: the levels by distance, the far card by yaw view, nothing cast far off, the cooked bodies (people.bin, all
  bodies in one file, a palette texture of every clip).
- lodPool and lodWorker belong to the vehicles' drawing (their own comment): justified here, the vehicles area's.
- A copy where a port was due: the far card's shader.

## Design here (T008, after the copy scan reads shaders, T024)
- The far card rebuilt from the impostor literature (yaw-strip or octahedral impostors, the source researched and cited), in this
  game's design. Owner: project/render/shaders/people_far.gdshader.
- The people's light rebuilt from published models with their sources (wrap or pre-integrated skin scattering; Kajiya-Kay or
  Marschner hair) and values measured for this game. Owner: project/render/shaders/people_look.gdshaderinc.
- The bodies' vertex attributes compressed by the engine's own compression, measured on km24's crowd. Owner:
  native/src/render/agents/people_asset.cpp.

## Proof
- Photos of a crowd from near to far across the level changes and the card (setViewNear walker at 40, 12, 3 px a metre), no pop or
  flicker; the copy scan finds nothing in the shaders.
