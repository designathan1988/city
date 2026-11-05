# editor:roads-powerLine

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/roads/powerLine.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 578 unidades; usa 567 compartilhadas de outras; é dona principal de 11 compartilhadas

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
- Lê: `RoadNode.heightOffset`, `RoadNode.incident`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/powerLineBuried.spec.ts`, `tests/editor/powerLinePlanner.spec.ts`, `tests/editor/streetKit.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: não se aplica 1
- porte `roads`: não se aplica 12
- porte `street-furniture`: parcial 1

## Código próprio (linhas — declaração — comentário)
