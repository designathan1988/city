# render:prop

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/propCells.ts`, `src/render/propGeometry.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 286 unidades; usa 224 compartilhadas de outras; é dona principal de 62 compartilhadas

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
- Bibliotecas externas: `three`, `three/examples/jsm/utils/BufferGeometryUtils.js`

## Testes do 4.0 que a exercitam
- `tests/render/propGeometry.spec.ts`, `tests/render/streetFurnitureLayer.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: já existia 29
- porte `roads`: não portado 1
- porte `street-furniture`: já existia 44, portado 27, não se aplica 8

## Código próprio (linhas — declaração — comentário)
