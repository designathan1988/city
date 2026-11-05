# editor:roadParking

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/roadParking.ts`
- Unidades próprias (só esta funcionalidade alcança): 8 em 1 arquivo(s), 32 linhas
- Alcance total: 11 unidades; usa 3 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#mountInterface@1859` (5), `src/main.ts#roadTool@458` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#drawOverlayScreen@2942` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:parking` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `roads`: portado 16
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/editor/roadParking.ts
- 4-4 `ROAD_PARKING_PRESETS` (variable): The parking a new road is drawn with, chosen in the road tool's options as Cities: Skylines II offers road variants
- 5-5 `RoadParkingPreset` (type)
- 7-7 `KEY` (variable)
- 8-8 `preset` (variable)
- 9-14 `TryStatement` (statement)
- 16-18 `roadParkingPreset` (function)
- 20-27 `setRoadParkingPreset` (function)
- 30-40 `roadParking` (function): The parking of a road drawn with the current preset, or undefined for none.
