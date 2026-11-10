# render:sun

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/sunCascades.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 8 linhas
- Alcance total: 43 unidades; usa 4 compartilhadas de outras; é dona principal de 36 compartilhadas

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
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/personTextureUnits.spec.ts`, `tests/render/shellFinishes.spec.ts`, `tests/render/sunCascades.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: outra sessão 4
- porte `landscape`: portado em parte 2, não se aplica 148

## Código próprio (linhas — declaração — comentário)

### src/render/sunCascades.ts
- 69-69 `CascadeShadow.isDirectionalLightShadow` (field)
- 71-73 `CascadeShadow.constructor` (method)
- 74-77 `CascadeShadow.updateMatrices` (method)
