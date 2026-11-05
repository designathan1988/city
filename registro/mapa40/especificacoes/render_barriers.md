# render:barriers

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/barriers.ts`
- Unidades próprias (só esta funcionalidade alcança): 20 em 1 arquivo(s), 152 linhas
- Alcance total: 202 unidades; usa 175 compartilhadas de outras; é dona principal de 7 compartilhadas

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
- Timers: 0
- Usa: `world:barriers` (96), `editor:barriers` (36), `world:changes` (6), `render:instance` (6), `world:cells` (5), `world:doc` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: parcial 64

## Código próprio (linhas — declaração — comentário)

### src/render/barriers.ts
- 23-23 `PIECE` (variable): Longest straight piece a run is cut into, world units.
- 25-25 `POST_STEP` (variable): Distance between fence posts, world units.
- 27-27 `SINK` (variable): How far a piece is sunk into the ground, so a slope never shows under it.
- 29-29 `Box` (interface)
- 32-32 `KINDS` (variable): The parts, in the order of the layer's stores.
- 33-33 `PartList` (type)
- 40-40 `READ_REACH` (variable): How far round a barrier's points the ground it reads lies (a piece's corners, a margin).
- 43-43 `BarrierProduct.name` (field)
- 44-44 `BarrierProduct.kinds` (field)
- 45-45 `BarrierProduct.byId` (field)
- 46-46 `BarrierProduct.filing` (field)
- 48-48 `BarrierProduct.constructor` (method)
- 51-56 `BarrierProduct.allKeys` (method): @rebuild Every barrier of the map: read only when everything changed (a load, the diary forgotten).
- 58-66 `BarrierProduct.dirty` (method)
- 68-77 `BarrierProduct.compute` (method)
- 79-81 `BarrierProduct.equals` (method)
- 83-86 `BarrierProduct.changed` (method)
- 90-102 `barrierInto` (function): A barrier's pieces put in a run of the layer's stores (`KINDS` order); how many boxes.
- 161-212 `placeBarrier` (function)
- 215-257 `placeKerbBarrier` (function): A road guardrail (a steel W-beam about 0.3 m deep, its top at 0.75 m, on posts every 2 m, as highway standards set it)
