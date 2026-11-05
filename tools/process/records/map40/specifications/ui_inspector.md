# ui:inspector

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/inspector.ts`
- Unidades próprias (só esta funcionalidade alcança): 32 em 1 arquivo(s), 451 linhas
- Alcance total: 509 unidades; usa 477 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `control.disconnected`, `control.signal`, `control.…`, `direction.…`, `inspector.addHeightPoint`, `inspector.addSignalCrossing`, `inspector.addUturnToEnd`, `inspector.addUturnToStart`, `inspector.addZebra`, `inspector.capacity`, `inspector.classDefault`, `inspector.connections`, `inspector.control`, `inspector.curvature`, `inspector.curvePosition`, `inspector.cycle`, `inspector.demolish`, `inspector.direction`, `inspector.duplicate`, `inspector.duplicateHint`, `inspector.end.…`, `inspector.group`, `inspector.heightEnd`, `inspector.heightPoint`, `inspector.heightPointHelp`, `inspector.heightStart`, `inspector.impossible`, `inspector.impossibleBody`, `inspector.impossibleGap`, `inspector.joinAligned`, `inspector.junction`, `inspector.laneCount`, `inspector.lanes`, `inspector.lanesOneWay`, `inspector.lanesTwoWay`, `inspector.length`, `inspector.meanSpeed`, `inspector.mouths`, `inspector.movements`, `inspector.node`, `inspector.numbers`, `inspector.phase`, `inspector.queue`, `inspector.removeCrossing`, `inspector.reverse`, `inspector.road`, `inspector.roadClass`, `inspector.speed`, `inspector.splitMiddle`, `inspector.upgrade`, `inspector.vehicles`, `inspector.volume`, `junction.lower`, `junction.raise`, `parking.…`, `phase.…`, `signal.…`
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#showInspector@3573` (1), `src/main.ts#mutateBuilt@876` (1), `src/main.ts#frame@2644` (1), `src/main.ts#ExpressionStatement@3923` (1), `src/main.ts#ExpressionStatement@2406` (1), `src/main.ts#closeInspector@2402` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (78), `ui:i18n` (71), `world:ids` (38), `sim:host` (28), `world:road` (17), `worker:sim` (16), `ui:roads` (10), `world:network` (10), `world:roads` (9), `world:parking` (8), `editor:commit` (5), `editor:blueprintLibrary` (5)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.id`, `NodeCrossing.kind`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.end`, `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.laneLinks`, `RoadNode.smooth`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.direction`, `RoadSegment.lanes`, `RoadSegment.parking`, `RoadSegment.section`, `RoadSegment.type`, `SignalLayout.groups`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 50, outra sessão 28
- porte `roads`: não se aplica 12
- porte `traffic`: portado 2

## Código próprio (linhas — declaração — comentário)

### src/ui/inspector.ts
- 23-26 `InspectorSelection` (interface)
- 28-70 `InspectorActions` (interface)
- 72-78 `InspectorContext` (interface)
- 80-80 `current` (variable)
- 83-83 `builtFor` (variable): What the panel's controls were last built for.
- 86-96 `openInspector` (function): Inspector panel.
- 99-101 `refreshInspector` (function): Refreshes the open inspector without replacing its selection or actions.
- 103-145 `renderCurrent` (function)
- 148-148 `openListeners` (variable): Whether the panel is open, told to whoever shows it (`main.ts` writes it in the game's state).
- 149-149 `openNow` (variable)
- 150-154 `tellOpen` (function)
- 156-158 `onInspectorChange` (function): Called whenever the panel opens or closes, by a command or by itself (what it showed is gone).
- 160-171 `closeInspector` (function)
- 175-181 `endSelect` (function): What a road's end is (V8), for an end that is the end of this road alone: the map's edge or a turning circle.
- 183-187 `parkingSelect` (function)
- 190-208 `segmentStats` (function): The live numbers for a road, or null when the road no longer exists (the traffic's, as the simulation's worker last told them).
- 210-343 `renderSegment` (function)
- 346-382 `nodeStats` (function): The live numbers for a junction, or null when the node no longer exists.
- 384-444 `renderNode` (function)
- 447-458 `junctionHost` (function): What the junction panel changes, through the inspector's actions.
- 463-465 `actionsForNode` (function): Whether the junction's list of movements is unfolded (kept across the panel's rebuilds).
- 467-472 `directionOptions` (function)
- 474-485 `laneOptions` (function)
- 488-488 `signalLabel` (function)
- 490-490 `phaseLabel` (function)
- 493-493 `meters` (function)
- 496-497 `curveText` (function): A curve's bulge: how far the arc leaves the straight line, signed by side.
- 499-499 `percent` (function)
- 502-505 `setTitle` (function): Names what is selected in the panel's own heading.
- 507-508 `card` (function)
- 510-511 `grid` (function)
- 514-519 `heightStepper` (function): A height as a stepper of the interface, half a metre a step (V5).
