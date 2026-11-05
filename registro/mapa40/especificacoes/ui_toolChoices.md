# ui:toolChoices

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/toolChoices.ts`
- Unidades próprias (só esta funcionalidade alcança): 19 em 1 arquivo(s), 65 linhas
- Alcance total: 98 unidades; usa 18 compartilhadas de outras; é dona principal de 61 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#SLIDERS@1715` (38), `src/main.ts#dabSettings@591` (6), `src/main.ts#MAP_ACTIONS@1798` (4), `src/main.ts#cloudTool@1137` (1), `src/main.ts#ExpressionStatement@1985` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:elements` (8), `world:trees` (6)
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
- porte `interface`: outra sessão 136, portado 24
- porte `interior`: outro porte 30
- porte `landscape`: não portado 28
- porte `street-furniture`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/ui/toolChoices.ts
- 79-79 `TREE_BRUSH_KEY` (variable)
- 80-82 `treeBrushNow` (variable)
- 87-87 `treeBrush` (function)
- 88-91 `setTreeBrush` (function)
- 106-106 `ELEMENT_BRUSH_KEY` (variable)
- 107-109 `elementBrushes` (variable)
- 113-115 `elementBrush` (function)
- 116-119 `setElementBrush` (function)
- 135-139 `CloudBrush` (interface): The next cloud's size (m), base height (m) and density (%). Kept between sessions.
- 140-140 `CLOUD_BRUSH_KEY` (variable)
- 141-149 `cloudBrushNow` (variable)
- 150-152 `cloudBrush` (function)
- 153-156 `setCloudBrush` (function)
- 159-163 `FogBrush` (interface): The fog brush's settings: strength (%), height (m), speed (m/s). Kept between sessions.
- 164-164 `FOG_BRUSH_KEY` (variable)
- 165-173 `fogBrushNow` (variable)
- 174-176 `fogBrush` (function)
- 177-180 `setFogBrush` (function)
- 233-233 `onRoadGridChange` (function): Called whenever the grid is shown or hidden (the map redrawn).
