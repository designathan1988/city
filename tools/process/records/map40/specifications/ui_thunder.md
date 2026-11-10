# ui:thunder

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/thunder.ts`
- Unidades próprias (só esta funcionalidade alcança): 6 em 1 arquivo(s), 66 linhas
- Alcance total: 6 unidades; usa 0 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#ExpressionStatement@1674` (1)

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
- porte `interface`: portado 12

## Código próprio (linhas — declaração — comentário)

### src/ui/thunder.ts
- 4-4 `LONGEST` (variable): The longest clap (far off), in seconds: `length` below at `near` 0.
- 6-6 `NOISE_SECONDS` (variable): Seconds of noise kept: the longest clap, and room to start it anywhere in the first part.
- 8-13 `Audio` (interface): DO NOT make it per strike: that was 125 to 270 thousand samples of `Math.random` on the page at each bolt
- 14-14 `audio` (variable)
- 15-28 `noiseOf` (function)
- 31-73 `playThunder` (function): Plays a thunderclap for a strike `metres` away, at `volume` (0..1).
