# render:agents

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/agents.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 13 linhas
- Alcance total: 1624 unidades; usa 1539 compartilhadas de outras; é dona principal de 81 compartilhadas

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
- Usa: `render:page` (2), `worker:job` (2), `render:people` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.changes`, `RoadSegment.structure`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/agentPrograms.spec.ts`, `tests/render/agentsParked.spec.ts`, `tests/render/agentsResidency.spec.ts`, `tests/render/pageGarbage.spec.ts`, `tests/render/vehicleCalls.spec.ts`, `tests/render/vehiclesInView.spec.ts`, `tests/render/windows.spec.ts`, `tests/sim/agents.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `demolition`: portado 2
- porte `landscape`: já existia 6
- porte `people`: não se aplica 2
- porte `street-furniture`: não se aplica 2
- porte `traffic`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/render/agents.ts
- 74-74 `ExpressionStatement` (statement): The page that draws people and vehicles hands their bakes, garments and levels' simplifier the job system's lanes (`pagePorts.ts`, `lodPort.ts`): the cooked bodies' code imports neither.
- 75-75 `ExpressionStatement` (statement)
- 76-76 `ExpressionStatement` (statement)
- 294-303 `windowPane` (function): How a door's window is drawn with a share `drop` of it lowered (0 shut, 1 fully down), written into `out`: what the glazing's program does to a pane (`vehicleWindows.ts`).
