# render:flat

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/flatAtlas.ts`, `src/render/flatGroundPool.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 953 unidades; usa 897 compartilhadas de outras; é dona principal de 55 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:terrain` (2), `world:bounds` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.nature`, `RoadDoc.nodes`, `RoadDoc.terrainPaint`, `RoadDoc.terrainRelief`, `RoadDoc.terrainStamps`, `RoadDoc.trees`, `RoadNode.x`, `RoadNode.y`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/farGround.spec.ts`, `tests/render/farSlab.spec.ts`, `tests/render/flatAtlas.spec.ts`, `tests/render/platePrograms.spec.ts`, `tests/render/plateProgramsFail.spec.ts`, `tests/world/unboundedPlates.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: não se aplica 84
- porte `street-furniture`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/render/flatAtlas.ts
- 141-143 `writeFarPatch` (function): Writes plate (cx, cy)'s patch into `slot` from the land `ground` (the base added here), and marks only that slot to be sent.
