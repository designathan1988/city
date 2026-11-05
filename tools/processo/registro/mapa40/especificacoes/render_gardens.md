# render:gardens

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/gardens.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 30 linhas
- Alcance total: 348 unidades; usa 331 compartilhadas de outras; é dona principal de 15 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:flowers`, `recebe:hedge`, `recebe:shrub`, `recebe:tree`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:cell` (7), `render:scenery` (3), `render:plant` (2), `render:instance` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/census/worldCensus.spec.ts`, `tests/render/gardenCull.spec.ts`, `tests/render/gardenFrame.spec.ts`, `tests/render/gardenLists.spec.ts`, `tests/render/gardenPacks.spec.ts`, `tests/render/plantLevels.spec.ts`, `tests/render/shadowCasters.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: já existia 14
- porte `street-furniture`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/render/gardens.ts
- 162-165 `Gardens` (interface)
- 192-217 `createGardens` (function): Every building's garden in one layer, given its lists (`gardenLists.ts`): what a test of the whole map places to compare.
