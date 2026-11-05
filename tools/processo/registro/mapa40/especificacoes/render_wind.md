# render:wind

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/wind.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 25 unidades; usa 4 compartilhadas de outras; é dona principal de 21 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/depthNoColour.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/render/sharedPlantPrograms.spec.ts`, `tests/render/sunCascades.spec.ts`, `tests/render/swayProgramKey.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado 22
- porte `street-furniture`: parcial 5

## Código próprio (linhas — declaração — comentário)
