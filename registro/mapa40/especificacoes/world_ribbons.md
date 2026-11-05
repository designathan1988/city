# world:ribbons

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/ribbons.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 1 arquivo(s), 9 linhas
- Alcance total: 184 unidades; usa 163 compartilhadas de outras; é dona principal de 16 compartilhadas

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
- Usa: `world:junction` (3), `worker:conflictTable` (1), `world:carriageway` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.incident`, `RoadSegment.a`, `RoadSegment.dashOrigin`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `road-elevation`: a portar 2
- porte `roads`: não portado 42

## Código próprio (linhas — declaração — comentário)

### src/world/ribbons.ts
- 65-65 `HELD_RIBBONS` (variable): How many ribbons' levels made again are held (`LeanRibbon`): the roads of the cells being drawn and walked.
- 70-70 `madeParts` (variable): The ribbons' levels made again (`LeanRibbon`), the latest asked held.
- 92-94 `LeanRibbon.centre` (method)
- 96-98 `LeanRibbon.rings` (method)
- 101-101 `partsOf` (function)
