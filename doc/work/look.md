# look: the game's look by distance, photographed for the owner

## Now
- The owner's words (2026-11-13), after a proof picture of the far cars (a crop magnified 4 times of a view from 2100 m)
  and then the photos of the plan `roofs`: "O JOGO NÃO ESTAVA ASSIM ... OS PRÉDIOS ESTAVAM SUPER BEM FEITOS"; "VOCÊ ME
  MOSTROU FOTOS QUE NÃO TINHAM DETALHES ALGUM E ERAM DO JOGO ATUAL, AGORA MOSTRA IMAGENS TOTALMENTE DIFERENTES DO JOGO
  ATUAL"; "AFINAL, QUERO SABER COMO ESTÁ O JOGO DE VERDADE"; "TIRA UMA FOTO DIFERENTE DO JOGO ATUAL, QUE NÃO TENHA EM
  NENHUMA OUTRA FOTO ANTERIOR, DEIXA EU VER". Standing: answered in Portuguese; pictures, not words; whole photos, never
  a magnified crop alone; no stopping to ask (the /goal).
- The block `look` is open for one new photo plan (no game code): project/tests/visual/zoom_ladder_plan.json, the item
  `zoom_ladder` of `looks` (tools/tasks/suites.json), its line in doc/map.md. One place never photographed (250 m west
  and 180 m north of the town's middle, seen from azimuth 70, 32 degrees up, at 10 h), at 6, 3, 1.5, 1, 0.8 and 0.5 px a
  metre: the same game at each distance.
- Facts read: look runs use the tests' graphics tier, settings.json:4 `tests_preset` "medium"; that tier's
  `detail_cutoff_zoom` is 0.85 px a metre (settings.json:126; low 1.25, high 0.65, ultra 0.5): under it the camera stops
  drawing the furniture, the people, the vehicles and the roads' paint (project/game/settings/render_switches.gd:61-64,
  :100, follow_zoom :113-121; quality_test.gd:93-95 checks it), so the pictures at 0.8 and 0.5 are past the cut-off and
  those at 1 and over are not. None of that was changed by the traffic's work but the far cars' own layer and global.
- Next, in order: (1) the plan, its item, its map line written; (2) `block review`, `block verify`, ONE run:
  `python tools/tasks/cli.py run zoom_ladder`; (3) the six photos opened and sent whole to the owner, with what each
  distance shows said plainly; (4) commit, push, `block close`; (5) back to the board's next line, roads
  (doc/work/t015-roads.md and doc/specs/003-roadcraft-features/sheets/roads.md were read again; its note
  doc/work/roads.md is to be written before its code).
