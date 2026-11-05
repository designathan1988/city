# render:signs

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/signs.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 6 linhas
- Alcance total: 165 unidades; usa 114 compartilhadas de outras; é dona principal de 49 compartilhadas

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
- Lê: `RoadNode.incident`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/plateFit.spec.ts`, `tests/render/signFaces.spec.ts`, `tests/render/signSheets.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `street-furniture`: portado 112, já existia 6

## Código próprio (linhas — declaração — comentário)

### src/render/signs.ts
- 312-314 `SignSheets.begin` (method): A build of the signs begins: the plates it reads are marked with it.
- 354-356 `SignSheets.sizes` (method): The size of each sheet now (a check that none passes what a card takes).
