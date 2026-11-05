# world:walking

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/walkingLines.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 20 linhas
- Alcance total: 115 unidades; usa 71 compartilhadas de outras; é dona principal de 43 compartilhadas

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
- Usa: `worker:conflictTable` (1), `editor:growthFeed` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `clipper2-ts`

## Testes do 4.0 que a exercitam
- `tests/sim/topologyOffPage.spec.ts`, `tests/world/walkwayCorners.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/world/walkingLines.ts
- 160-179 `NearestEdge.constructor` (method)
