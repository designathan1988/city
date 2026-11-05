# world:ecology

Grupo: world. Entrada: subsistema. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/ecology.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 1 linhas
- Alcance total: 40 unidades; usa 7 compartilhadas de outras; é dona principal de 32 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#newNature@194` (1), `src/main.ts#openCityNow@2175` (1)

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
- `tests/render/ecologyKept.spec.ts`, `tests/render/plateTexels.spec.ts`, `tests/world/ecology.spec.ts`, `tests/world/ecologyRegion.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 68

## Código próprio (linhas — declaração — comentário)

### src/world/ecology.ts
- 9-9 `DEFAULT_REGION` (variable)
