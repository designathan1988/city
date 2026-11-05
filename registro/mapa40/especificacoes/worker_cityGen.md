# worker:cityGen

Grupo: worker. Entrada: worker. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/cityGen.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 22 linhas
- Alcance total: 2117 unidades; usa 2114 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#cityData@2056` (14), `src/main.ts#openedOf@2079` (9), `src/main.ts#cityJob@2109` (8), `src/main.ts#nextCitySeed@2136` (2), `src/main.ts#openCityNow@2175` (2), `src/main.ts#cityAhead@2146` (1), `src/main.ts#openCityOffPage@2169` (1), `src/main.ts#citiesAhead@2135` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:{ id: ask.id, data, buildings } satisfie`, `envia:{ id: ask.id, error: String((error as Er`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `worker:bays` (4), `world:doc` (1), `editor:smallCityMap` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/cityGen.worker.ts
- 7-14 `CityGenAsk` (interface): THE CITY MADE OFF THE PAGE (`main.ts` `cityData`): the game's own generator (`buildCityTable`) building a whole finite city in a job worker, so the page is not frozen for its seconds.
- 17-17 `CityGenReply` (type): `buildings`: the city's buildings as one table (`buildCityTable`, as `prepareCity` makes it), its `data` holding none: the page reads them from it (`preparedCity`).
- 19-31 `serve` (function)
