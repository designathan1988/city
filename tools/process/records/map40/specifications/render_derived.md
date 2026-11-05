# render:derived

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/derivedWriterLane.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 2 arquivo(s), 6 linhas
- Alcance total: 16 unidades; usa 0 compartilhadas de outras; é dona principal de 14 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
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
- `tests/core/writeBudget.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `build`: não se aplica 2
- porte `interior`: não se aplica 1

## Código próprio (linhas — declaração — comentário)

### src/core/derivedCache.ts
- 206-208 `WriteBudget.bytes` (method): Bytes in flight now.

### src/render/derivedWriterLane.ts
- 28-30 `DerivedWriterLane.bytes` (method): Bytes posted and not committed yet.
