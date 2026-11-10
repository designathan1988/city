# render:citizen

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/citizenAssets.ts`, `src/render/citizenBake.ts`, `src/render/citizenCasting.ts`, `src/render/citizenGait.ts`, `src/render/citizenPalette.ts`, `src/render/citizenPlays.ts`, `src/render/citizenStride.ts`, `src/render/citizenWalk.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 5 linhas
- Alcance total: 346 unidades; usa 131 compartilhadas de outras; é dona principal de 214 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 3
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `public/models/citizens/LICENSE-ANIMATIONS.txt`, `public/models/citizens/LICENSE-MICROSOFT.txt`, `src/render/motion/rocketboxFemale.json`, `src/render/motion/rocketboxMale.json`, `src/render/motion/walkFemale.json`, `src/render/motion/walkMale.json`, `url`
- Bibliotecas externas: `three`, `three/addons/utils/SkeletonUtils.js`

## Testes do 4.0 que a exercitam
- `tests/arch/citizenCasting.spec.ts`, `tests/people/dressed.spec.ts`, `tests/people/rig.spec.ts`, `tests/render/citizenAtlasAppend.spec.ts`, `tests/render/citizenGait.spec.ts`, `tests/render/citizenLocomotion.spec.ts`, `tests/render/citizenPalette.spec.ts`, `tests/render/citizenPlays.spec.ts`, `tests/render/classRigMade.spec.ts`, `tests/render/cookedPerson.spec.ts`, `tests/render/crowdAtlas.spec.ts`, `tests/render/occupantFit.spec.ts`, `tests/render/palettePass.spec.ts`, `tests/render/riderFit.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: não se aplica 287
- porte `traffic`: de outra funcionalidade 6

## Código próprio (linhas — declaração — comentário)

### src/render/citizenCasting.ts
- 62-66 `compatible` (function): Which wardrobes may stand together.
