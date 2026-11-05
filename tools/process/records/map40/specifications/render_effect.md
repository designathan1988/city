# render:effect

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/effectSwitches.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 17 unidades; usa 0 compartilhadas de outras; é dona principal de 16 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/effectsWiring.ts#createEffectsWiring@108` (2), `src/effectsWiring.ts#readKept@35` (1)

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
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 24, não se aplica 14

## Código próprio (linhas — declaração — comentário)

### src/render/effectSwitches.ts
- 39-41 `isEffectSwitch` (function)
