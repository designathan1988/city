# ui:roadSwatch

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/roadSwatch.ts`
- Unidades próprias (só esta funcionalidade alcança): 11 em 1 arquivo(s), 85 linhas
- Alcance total: 33 unidades; usa 22 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#roadClasses@1558` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:road` (29)
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
- porte `roads`: não portado 24

## Código próprio (linhas — declaração — comentário)

### src/ui/roadSwatch.ts
- 5-5 `CASING` (variable): A picture of a road class, drawn from the same numbers the meshes are built from
- 6-6 `MARGIN` (variable)
- 9-11 `MAX_SPAN` (variable): World units a tile's full height covers: the widest class, plus margin.
- 13-13 `GRASS_TOP` (variable)
- 14-14 `GRASS_BOTTOM` (variable)
- 15-15 `CASING_COLOR` (variable)
- 16-16 `FOOTWAY_COLOR` (variable)
- 17-17 `MEDIAN_GREEN` (variable)
- 20-23 `dashed` (function): Paint dashed along a line, so it reads as a marking and not as an edge.
- 26-26 `CACHE` (variable): Drawn once per class, profile and size.
- 28-97 `roadSwatch` (function)
