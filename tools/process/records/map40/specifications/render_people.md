# render:people

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/people/bodyPixels.ts`, `src/render/people/cartoon.ts`, `src/render/people/classRigMade.ts`, `src/render/people/cookPack.ts`, `src/render/people/cookedLoad.ts`, `src/render/people/cookedPerson.ts`, `src/render/people/crowdAnimation.ts`, `src/render/people/crowdBodies.ts`, `src/render/people/crowdLod.ts`, `src/render/people/faceExpression.ts`, `src/render/people/facialMorphs.ts`, `src/render/people/garmentPadding.ts`, `src/render/people/garmentPaddingPool.ts`, `src/render/people/garmentSlots.ts`, `src/render/people/gpuRows.ts`, `src/render/people/hairTexture.ts`, `src/render/people/heldProps.ts`, `src/render/people/lodPool.ts`, `src/render/people/lodPort.ts`, `src/render/people/lodWorker.ts`, `src/render/people/paletteTargets.ts`, `src/render/people/personBuild.ts`, `src/render/people/personMesh.ts`, `src/render/people/personRig.ts`, `src/render/people/personVariants.ts`, `src/render/people/proceduralBake.ts`, `src/render/people/proceduralCook.ts`, `src/render/people/proceduralCrowd.ts`, `src/render/people/skinAppearance.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 4 arquivo(s), 57 linhas
- Alcance total: 653 unidades; usa 281 compartilhadas de outras; é dona principal de 365 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:dispose`, `ouve:webglcontextrestored`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onAfterShadow`, `onBeforeCompile`, `onBeforeShadow`
- Timers: 4
- Usa: `worker:bays` (7), `render:part` (5), `render:buildings` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `/__cook/procedural/…`, `path`, `public/models/people/skins/index.json`, `url`, `…`
- Bibliotecas externas: `meshoptimizer`, `three`, `three/addons/utils/BufferGeometryUtils.js`, `three/examples/jsm/utils/BufferGeometryUtils.js`, `virtual:model-urls/people/skins?ext=webp`

## Testes do 4.0 que a exercitam
- `tests/people/dressed.spec.ts`, `tests/people/rig.spec.ts`, `tests/render/agentPrograms.spec.ts`, `tests/render/classRigMade.spec.ts`, `tests/render/cookPack.spec.ts`, `tests/render/cookedPerson.spec.ts`, `tests/render/crowdAtlas.spec.ts`, `tests/render/crowdBodies.spec.ts`, `tests/render/faceCards.spec.ts`, `tests/render/gpuRows.spec.ts`, `tests/render/palettePass.spec.ts`, `tests/render/paletteTargets.spec.ts`, `tests/render/peopleMemory.spec.ts`, `tests/render/personPlace.spec.ts`, `tests/render/personTextureUnits.spec.ts`, `tests/render/personVariants.spec.ts`, `tests/render/rigDeltas.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `people`: já existia 567, portado 169, não se aplica 18
- porte `traffic`: de outra funcionalidade 17

## Código próprio (linhas — declaração — comentário)

### src/render/people/lodWorker.ts
- 38-38 `PartRequest` (interface): A vehicle part's levels made whole here (`partLevelsWork.ts`): welded, simplified, creased.
- 82-111 `serve` (function): The people's levels' service of the job system (`job.worker.ts`): each request answered.

### src/render/people/proceduralBake.ts
- 295-306 `IfStatement` (statement): The cook (`scripts/cook-people.mjs`, run by hand)

### src/render/people/proceduralCook.ts
- 46-46 `proceduralCookHash` (function)

### src/render/people/proceduralCrowd.ts
- 47-47 `ExpressionStatement` (statement): The class bodies built on this page simplify their levels in the page's pool (`lodPort.ts`).
- 654-664 `personShaderText` (function): The shader text a piece of variant `v` is compiled with (`depth`: the shadow's, cast by the next level), as three hands it to `onBeforeCompile`: one key, one text.
- 860-860 `IfStatement` (statement)
