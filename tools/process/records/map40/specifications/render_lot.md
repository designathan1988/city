# render:lot

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/lotLattice.ts`, `src/render/lotOverlay.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 2 linhas
- Alcance total: 128 unidades; usa 42 compartilhadas de outras; é dona principal de 84 compartilhadas

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
- `tests/render/lotLattice.spec.ts`, `tests/render/lotOverlay.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/render/lotOverlay.ts
- 58-58 `ExpressionStatement` (statement)
- 59-59 `ExpressionStatement` (statement)
