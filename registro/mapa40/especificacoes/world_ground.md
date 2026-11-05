# world:ground

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/groundPatch.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 9 unidades; usa 0 compartilhadas de outras; é dona principal de 8 compartilhadas

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
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/growthWorker.spec.ts`, `tests/render/landStroke.spec.ts`, `tests/render/plateTexels.spec.ts`, `tests/render/sharedGround.spec.ts`, `tests/render/terrainLod.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `build`: a portar 10
- porte `landscape`: já existia 20

## Código próprio (linhas — declaração — comentário)

### src/world/groundPatch.ts
- 52-54 `PatchMiss.constructor` (method)
