# people: faces and their expressions

- State: read
- Inventory rows: r2818 `render/people/faceExpression` | this game: project/render/shaders/people_face.gdshaderinc,
  native/src/render/agents/people_renderer.cpp (FACE_ACTS, set_faces)

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/render/people/faceExpression.ts 1-123 of 123: channels as ARKit shapes (blink, joy, sadness, anger,
  surprise, brow raise, five mouth shapes for visemes, gaze), blinks every 3-5 s, a wandering gaze, the mood's resting joy or sadness,
  the activities' faces (talk, listen, laugh, cheer, wave, argue, angry, phone, dance), speech by syllables with stressed brows, and
  FACS faces for terror (AU1+2+4+5+20+26), sobbing (AU1+4+15), pain (Prkachin's pain face, AU4+6+7+9+10+43) and the dead (slack).

## This game
- Read whole: project/render/shaders/people_face.gdshaderinc 1-94 of 94 (on the GPU, near mesh's face vertices only); people_renderer.cpp
  set_faces and FACE_ACTS (read for people-motions).
- The shader is a line-for-line translation of the reference (the salt, the hash constants of view.ts personHash, the viseme order,
  the syllable rate, the blink constants, the terror and sob formulas): a copy, registered; the copy scan misses shaders, registered.
- Faces here: blink, gaze, mood, talk, listen, phone, terror, sobbing; missing: pain, the dead, laugh, cheer, wave, argue, angry, dance.

## Differences
- A copy where a port was due; and the pain, dead and activities' faces missing.

## Design here
- The face rebuilt from published sources in this game's design (T008): the FACS action units of each emotion (Ekman and Friesen's
  FACS manual; Prkachin's pain face), a measured blink rate and duration (spontaneous blinking, researched with its source before the
  code), a measured syllable rate of speech (researched with its source), this game's own hash (core/random.h) and channel order; the
  faces of pain (wounded), the dead (a body struck down) and the activities of people-motions item 3. Owner: project/render/shaders/
  people_face.gdshaderinc. The copy scan extended to .gdshader and .gdshaderinc first (T024), so the rebuild is checked.

## Proof
- The copy scan over the shaders finds nothing; near photos of a talker, a frightened walker, a wounded one and a body struck down
  (setViewNear walker), each face analysed.
