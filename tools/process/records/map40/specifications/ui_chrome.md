# ui:chrome

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/chrome.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 3 unidades; usa 0 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:keydown`, `ouve:pointerdown`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#gameKeys@2479` (1), `src/main.ts#ExpressionStatement@2556` (1), `src/main.ts#ExpressionStatement@1984` (1)

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
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: já existia 6

## Código próprio (linhas — declaração — comentário)

### src/ui/chrome.ts
- 3-3 `tabbing` (variable): Where the keyboard focus came from, for the game's own keys: kept out of `main.ts` because it needs nothing but the DOM.
- 6-8 `focusCameFromKeyboard` (function): Whether focus was last moved with the keyboard (Tab), not the pointer.
- 11-15 `trackFocusModality` (function): Starts telling a Tab from a click. Call once.
