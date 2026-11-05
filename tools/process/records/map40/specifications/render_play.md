# render:play

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/playEffects.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 1543 unidades; usa 1536 compartilhadas de outras; é dona principal de 7 compartilhadas

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
- Lê: `RoadDoc.buildings`, `RoadDoc.changes`, `Wound.fromX`, `Wound.fromY`, `Wound.id`, `Wound.part`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/tracers.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: não se aplica 1
- porte `people`: já existia 1
- porte `street-furniture`: não se aplica 1
- porte `traffic`: de outra funcionalidade 1

## Código próprio (linhas — declaração — comentário)
