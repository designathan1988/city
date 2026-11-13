# People: the area's index (specification 003, T007)

The inventory's reference-only rows of the area (`python tools/atlas/cli.py inventory --area people --unpaired --game reference`,
2026-11-13: 159 rows), each given to the feature sheet it belongs to, or justified. A row's place in a feature is decided by its own
source file; each sheet then reads every file its trace reaches, in both games (contracts/comparison-sheet.md).

| Feature | Sheet | Rows (reference source) | State |
|---|---|---|---|
| The person maker: make anybody, turn and look closer, save them in the cast | people-maker.md | r124 `hint.person` (src/ui/i18n/en.ts:130); r2060 `TOOLS/person`, r2076 `Tool.person` (src/editor/toolMachine.ts:20-23); r2227 `people/spec` (src/people/spec.ts); r2125 `lab/catalog/people` (src/lab/catalog/people.ts) | drafted |
| The cast: who the town's people are, by sex, age and dress | people-cast.md | r2211-r2226 `cast/*` (src/people/cast.json:3-123: female and male; adult, young, senior, child; casual, sport casual, smart casual, business) | read |
| What people do: the motions and activities | people-motions.md | r2705-r2756 `rocketboxFemale/clips/*`, r2757-r2808 `rocketboxMale/clips/*` (src/render/motion/*.json: walks of five paces, run, turns, idle, look, phone, talk, listen, sit, read, carry a bag or trolley, umbrella, cheer, dance, wave, drink, photo, crouch, laugh, angry, argue, knock, headphones, eat, work at a table, drunk, injured, nervous); r2817 `render/people/crowdAnimation`; r2474 `RIDER_CLIPS/bikePedal` (src/render/riderPoses.ts:307) | read |
| What people hold | people-held.md | r2819 `render/people/heldProps`; r2820-r2825 `HELD/read, drink, phone, photo, bag, umbrella` (src/render/people/heldProps.ts:15-20) | read |
| Faces and their expressions | people-faces.md | r2818 `render/people/faceExpression` | read |
| Hurt, fallen and the dead: casualties and the body that falls | people-casualties.md | r2900 `sim/people/casualties`; r2457-r2460 `render/ragdoll`, `ragdollAxes`, `ragdollGround`, `ragdollJolt`; the clips hitChest, hitHead, getUpFront, getUpBack, staggerBack, crawl (also in people-motions) | drafted |
| How people are drawn: the body, its detail levels and far forms | people-drawing.md | r2809 `render/people/cookedLoad`, r2810 `cookedPerson`, r2811-r2816 `VERTEX_LAYOUT/*` (src/render/people/cookedPerson.ts:23-32), r2826 `lodPool`, r2827 `lodWorker`, r2828 `paletteTargets`, r2829 `personImpostor`, r2830 `personMaterial` | read |
| Where people walk: footways, crossings, signals, ways into buildings | people-walking.md | r2901 `sim/people/engine`, r2902 `sim/people/view`; r2898 `sim/peds/sidewalk`, r2899 `sim/peds/signalPostIndex`; r3155 `world/pedestrianAccess` | read |

**Justified here, belongs elsewhere**: r2517 `PLATE/pedestrian` (src/render/signs.ts:26): the pedestrian sign's plate, a street
sign: the street furniture area's (T011).

**Order of the sheets** (the player's view first): where people walk, what they do, the cast, what they hold, faces, how they are
drawn, casualties, the person maker.
