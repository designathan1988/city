# render:pole

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/poleBake.ts`, `src/render/poleCells.ts`, `src/render/poleLayer.ts`
- Unidades próprias (só esta funcionalidade alcança): 48 em 2 arquivo(s), 378 linhas
- Alcance total: 668 unidades; usa 519 compartilhadas de outras; é dona principal de 101 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onBeforeRender`, `onBeforeShadow`
- Timers: 0
- Usa: `world:utilities` (29), `render:utilities` (20), `world:ids` (11), `world:changes` (7), `world:doc` (5), `world:pole` (5), `world:view` (3), `world:network` (3), `editor:barriers` (2), `render:scenery` (2), `render:mesh` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/poleLayer.spec.ts`, `tests/render/poleLevels.spec.ts`, `tests/render/utilityWires.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: portado 230, já existia 64

## Código próprio (linhas — declaração — comentário)

### src/render/poleCells.ts
- 17-17 `POLE_CHANGES` (variable): What moves a pole or its parts: the poles and wires, the roads (its kerb), the roads' heights and the ground under it.
- 19-19 `SPAN_REACH` (variable): How far a change reaches a block's things: a pole within `POLE_REACH` of it reframed, and every span strung to it, up to the longest a span may run (`doc.ts` SPAN_REACH).
- 25-25 `BLOCK_OVERHEAD` (variable): A held block's own objects besides its arrays.
- 52-52 `PoleCells.specs` (field)
- 53-53 `PoleCells.live` (field)
- 54-54 `PoleCells.scale` (field)
- 55-55 `PoleCells.scratch` (field)
- 66-76 `PoleCells.constructor` (method)
- 78-82 `PoleCells.triangles` (method)
- 137-173 `PoleCells.derive` (method): A block made from the document, its buffers let go once on the GPU (and given back after a lost context: the block made again).
- 176-213 `PoleCells.build` (method): A block's stores filled from the document: the poles standing in `rect` (its own, filed as `blockKey` files them) and the spans strung from them.
- 215-219 `PoleCells.release` (method)
- 222-232 `PoleCells.view` (method): Each held block's nearest corner as this camera sees it, each time three draws the layer: what its parts fade and coarsen by.

### src/render/poleLayer.ts
- 69-69 `POLE_REACH` (variable): How far from a pole a change of the roads or the ground can move it: its kerb (`kerbward`), its lamp's pool.
- 108-150 `ScreenScale` (class): What a world unit measures on the screen where a bounding sphere comes nearest the eye, and a texel of the sun's shadow.
- 109-109 `ScreenScale.eye` (field)
- 110-110 `ScreenScale.ahead` (field)
- 111-111 `ScreenScale.pixels` (field)
- 112-112 `ScreenScale.perspective` (field)
- 114-114 `ScreenScale.texel` (field): A shadow texel, world units; 0 while unknown (every piece casts).
- 117-126 `ScreenScale.set` (method): The camera three is about to draw with.
- 129-131 `ScreenScale.at` (method): Pixels a unit spans at the sphere's nearest depth: Infinity when the eye is level with it or inside it.
- 134-142 `ScreenScale.atBox` (method): Pixels a unit spans at a box's nearest corner (three's axes: x0, y0, z0, x1, y1, z1): Infinity when the eye is in it or level with it.
- 144-149 `ScreenScale.atPoint` (method)
- 241-250 `PartStore.remove` (method): A slot let go: the last part moves into it. Returns the pole whose slot `count` became `slot`, if another moved.
- 279-281 `PartStore.box` (method): Its pieces' box (three's axes): where its block is nearest the eye with no pole of its own to say so (wires only).
- 284-298 `PartStore.view` (method): As the camera sees the nearest pole: a piece covering under a pixel (times its brightness) not drawn, nor cast where it covers under a shadow texel; the coarsest shape off by under a pixel.
- 301-305 `PartStore.seal` (method): Its instances made for good (a block of the view's, `PoleCells`): no pole's slot kept, the matrices to be let go once on the GPU. The buffer, none when empty.
- 308-310 `PartStore.drawn` (method): Its mesh, none while it holds nothing.
- 313-315 `PartStore.matrices` (method): The matrices' array as made (`seal`): what the GPU is given again after a lost context.
- 411-419 `WireStore.remove` (method): A block let go: the last span's wires move into it. Returns the span whose block `count` became `block`.
- 445-447 `WireStore.box` (method): Its wires' box (three's axes, before their sag).
- 450-461 `WireStore.view` (method): As the camera sees the nearest pole: the template its wires need (`wireLevel`), none once faded; cast while a texel or wider.
- 464-471 `WireStore.seal` (method): Its wires made for good (`PoleCells`): no span's block kept, the instances to be let go once on the GPU. The buffer, none when empty.
- 474-476 `WireStore.instances` (method): The wires' array as made (before `seal`).
- 479-481 `WireStore.drawn` (method): Its mesh, none while it holds nothing.
- 493-495 `ViewedRoot.constructor` (method)
- 497-499 `ViewedRoot.update` (method)
- 523-523 `PoleLayer.scale` (field)
- 525-529 `PoleLayer.constructor` (method)
- 541-545 `PoleLayer.triangles` (method)
- 548-553 `PoleLayer.polesIn` (method): The poles this layer holds inside a rectangle, where it drew them.
- 556-558 `PoleLayer.ids` (method): Every pole this layer holds.
- 568-592 `PoleLayer.redo` (method): These poles placed again as the source has them
- 595-608 `PoleLayer.follow` (method): The diary's changes since the layer was last brought up (none with a null rectangle: that is a whole build), from `source` (the document's, `docPoles`)
- 628-644 `PoleLayer.unplace` (method)
- 658-665 `PoleLayer.unstring` (method)
- 676-695 `PoleLayer.view` (method): Each block's nearest pole as this camera sees it, each time three draws the layer: what its parts fade and coarsen by.
