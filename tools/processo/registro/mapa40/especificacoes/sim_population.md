# sim:population

Grupo: sim. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/sim/populationGovernor.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 21 unidades; usa 2 compartilhadas de outras; é dona principal de 18 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#startPopulationShare@310` (1)

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
- `tests/sim/populationGovernor.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/sim/populationGovernor.ts
- 18-20 `cpuPopulationShare` (function): The most of the town a machine of `threads` threads is given to start with (the governor fills it from there when its steps are cheap): DO NOT start a four-thread machine at the full town, cut a 0.85 step every few seconds while its first s
