# Cobertura do rastreamento: build

Uma linha por arquivo e por declaração própria de `feature.md` (fonte: `converter/out/features/build/boundary.json`; gerada por `evidence/cobertura/gerar.py`). Regra desta tabela: `portado` e `já existia` só existem com arquivo e símbolo do jogo, em `provas.py`, e o gerador confere cada um no clone: símbolo que não está no arquivo aborta a geração. Sem prova, a declaração é `a portar`, sem exceção por nome ou pasta. `não se aplica` traz o motivo; `outro porte` diz a funcionalidade da fila que é dona.

Arquivos: 224. Declarações próprias: 4178.

| Estado | Declarações |
|---|---|
| a portar | 3825 |
| portado | 171 |
| não se aplica | 128 |
| outro porte | 54 |

### `src/actionsWiring.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência.

**Motivo:** Funcionalidade demolition da fila.


### `src/buildingsWiring.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The building tool's share of the composition root.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingWiring` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingWiringDeps` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building tool's share of the composition root. |
| `PRIMITIVE_OF_TOOL` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The basic shape each tool of the Draw tab throws. |
| `READY` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Every tool the catalogue lists, all of them wired to the engine.  Every tool the catalogue lists is wired to the engine. |
| `createBuildingWiring` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.activate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.afterGesture` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | After a gesture: if a creating tool just made what it makes, it is put down. |
| `createBuildingWiring.afterRoadEdit` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | After a road edit, inside the same undo step: the road-wins rule over what changed since `before`, the network's state read just before the edit. |
| `createBuildingWiring.backToSelect` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Puts down whatever is in hand: the pointer selects again. |
| `createBuildingWiring.beforeDraw` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Before each draw: hands the preview to the renderer. |
| `createBuildingWiring.buildReference` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Builds the reference where it stands, in place of the building it was placed on. |
| `createBuildingWiring.bulldozeAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The bulldozer: demolishes the building under a screen point, if any. |
| `createBuildingWiring.cancelOperation` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Right button: cancels the operation in progress, if any. |
| `createBuildingWiring.clearArming` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Choosing a tool puts the previous one away: only one thing may be taking the pointer at a time |
| `createBuildingWiring.copyStyle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.deactivate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.defaultSplitFloor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.drawGizmos` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The Builder's own layer: the construction grid, on the selected building's floor while the grid is on. |
| `createBuildingWiring.drawOverlay` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.floorInHand` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The floor the floor chip shows: the picked face's storey, or the ground. |
| `createBuildingWiring.hintKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The sentence the hint bar shows: the plan being drawn wins, otherwise the tool the tray has chosen. |
| `createBuildingWiring.hintKey@1206` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The sentence the hint bar shows: the plan being drawn wins, otherwise the tool the tray has chosen. |
| `createBuildingWiring.insideClick` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A click with nothing in hand: opens (or switches) the building seen inside; true if taken. |
| `createBuildingWiring.inspectorFields` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The precise numbers of the current selection, for the inspector. |
| `createBuildingWiring.key` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Returns true when the tool used the key. |
| `createBuildingWiring.languageChanged` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.loadReference` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.materialLine` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.notify` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A transient answer to an action, shown by the interface as every answer of the game. |
| `createBuildingWiring.openInside` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | A building opened from inside at its ground floor (the player went in), or closed. |
| `createBuildingWiring.planActionOfTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Wing, stack and cut are drags too: a rectangle against the selection. |
| `createBuildingWiring.pointerDown` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.pointerMove` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.pointerUp` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.referenceAction` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The reference's own actions; true when `id` was one. |
| `createBuildingWiring.referenceTarget` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where the reference stands: on the selected building, centred on its plan. |
| `createBuildingWiring.refresh` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The workspace follows the tool at once, not a frame later: the panels are cheap to re-render |
| `createBuildingWiring.requestThumbnails` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Photographs the parts a gallery is about to show, a few per frame. |
| `createBuildingWiring.restored` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | After an undo, a redo or a load. |
| `createBuildingWiring.runTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Runs an action tool, arms a mode tool, or opens a gallery (the workspace's job). |
| `createBuildingWiring.selectionName` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What the inspector is looking at, in one line. |
| `createBuildingWiring.setField` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | One inspector number, applied to the model. |
| `createBuildingWiring.setFree` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Alt held frees a drag from the grid |
| `createBuildingWiring.shapeOfTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The shape a draw tool draws, when it is a drag shape. |
| `createBuildingWiring.state` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `createBuildingWiring.tellInside` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | DO NOT add a second bar for it: a floating one |

### `src/core/derivedCache.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `keepDerivedNow` | não se aplica | core, JobPool, RenderWorld | src/core/derivedCache.ts:162-173 function keepDerivedNow: the feature's own |
| `readDerivedAll` | não se aplica | core, JobPool, RenderWorld | src/core/derivedCache.ts:107-126 function readDerivedAll: the feature's own |

### `src/core/forkMap.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/core/hypot.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `hypot3` | não se aplica | core, JobPool, RenderWorld | src/core/hypot.ts:27-48 function hypot3: the feature's own |

### `src/core/idSlots.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `IdSlots.capacity` | não se aplica | core, JobPool, RenderWorld |  |
| `IdSlots.forEach` | não se aplica | core, JobPool, RenderWorld | src/core/idSlots.ts:91-94 method IdSlots.forEach: the feature's own |
| `IdSlots.size` | não se aplica | core, JobPool, RenderWorld |  |

### `src/core/minMax.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL. _(no 4.0: `Math.min` AND `Math.max` OF TWO NUMBERS, BIT FOR BIT, AS COMPARISONS)_

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `least` | não se aplica | core, JobPool, RenderWorld | `Math.min(a, b)` to the bit: NaN if either is, -0 under 0. |
| `most` | não se aplica | core, JobPool, RenderWorld | `Math.max(a, b)` to the bit: NaN if either is, 0 over -0. |

### `src/core/polyline.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `segmentDistSq` | não se aplica | core, JobPool, RenderWorld | src/core/polyline.ts:366-383 function segmentDistSq: the feature's own |

### `src/core/rng.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Rng.fromState` | não se aplica | core, JobPool, RenderWorld |  |
| `Rng.state` | não se aplica | core, JobPool, RenderWorld | src/core/rng.ts:30-32 method Rng.state: the feature's own |
| `RngState` | não se aplica | core, JobPool, RenderWorld | src/core/rng.ts:3-3 type RngState: the feature's own |

### `src/core/scalar.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `steadyRatio` | não se aplica | core, JobPool, RenderWorld | src/core/scalar.ts:56-56 function steadyRatio: the feature's own |

### `src/desktopJobs.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/editor/barriers.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/blocks.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays

**Para o jogador:** Editar lotes: dividir, juntar, acrescentar, apagar, definir frente; camada dos lotes. _(no 4.0: Several blocks at once: a grid of streets laid in one gesture)_

**Motivo:** Sem prova por símbolo até agora. Aba Zonas › Lotes está 'em construção'.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BlockGrid` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Several blocks at once: a grid of streets laid in one gesture |
| `blockGridLines` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The street lines of a grid, as pairs of world points. |
| `blockGridLines.at` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `commitBlockGrid` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | DO NOT make a network per line: each was built from nothing over the whole map, as many times as the grid has lines. |

### `src/editor/blueprintLibrary.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: DO NOT drop the oldest past a count: the 49th save erased the first without a word)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BlueprintLibrary` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BlueprintLibrary.constructor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BlueprintLibrary.kept` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BlueprintLibrary.list` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BlueprintLibrary.remove` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Removes one blueprint, the rest as they were; false when nothing could be written. |
| `BlueprintLibrary.save` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Saves a body under a name (the one of that name replaced); returns the stored blueprint, or null when it was not saved. |
| `KEY` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT drop the oldest past a count: the 49th save erased the first without a word |
| `StoredBlueprint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `isStored` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `readBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A stored body, repaired through `migrateBuilding` (placement fields are dummies). |
| `readEntry` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | One stored entry as a blueprint, or null when this build cannot read it (it stays stored). |

### `src/editor/buildingFloors.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: DO NOT key the floors on a global revision (`${doc.revision}:${doc.terrainRevision}:${buildings.revision}`))_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingFloors` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingFloors.cursor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingFloors.floorOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingFloors.floors` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingFloors.follow` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `KINDS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT key the floors on a global revision (`${doc.revision}:${doc.terrainRevision}:${buildings.revision}`) |

### `src/editor/buildingPlans.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The basic shapes thrown at a building: a plan and a roof.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `PRIMITIVES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `PlanShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `Primitive` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The basic shapes thrown at a building: a plan and a roof. |
| `UpperMassPlacement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Creates an editable shaped volume on a roof. Values are world units. |
| `addPlanMass` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Adds a mass to the same building at ground or on an existing roof. |
| `addShapedUpperMass` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `addShapedUpperMass.atScale` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `bevelPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Bevels corners of a plan: each corner becomes two, `distance` back along its two sides (never past half a side). |
| `changePlanVertex` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Inserting/deleting a vertex invalidates edge-specific decorations. |
| `changePlanVertex.remap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `changePlanVertex.remapSides` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `movePlanEdge` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `movePlanVertex` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `offsetPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Offsets the selected volume's whole plan; preserves edge identities. |
| `setVolumePlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Store a ring in a volume's bounding frame. Existing edge overrides stay indexed. |
| `shapeBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `shapePoints` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Editable polygon starters. Curved forms are deliberately explicit vertices. |
| `shapePoints.points` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/editor/buildingProfile.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Splits one continuous mass at a floor boundary.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `reshapeTier` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Turns a tier into another editable plan while mapping facade work by face direction. |
| `reshapeTier.nearest` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `reshapeTier.replicate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `splitVolumeAtFloor` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_split_at_floor(` | Splits one continuous mass at a floor boundary. |

### `src/editor/buildingRoofs.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: A roof part belongs to one mass and follows every building edit.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `SIZE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `addRoofDetail` | portado | `native/src/sim/commands/body_finish.cpp` → `std::string op_add_gear(` | A roof part belongs to one mass and follows every building edit. |
| `removeRoofDetail` | portado | `native/src/sim/commands/body_finish.cpp` → `std::string op_remove_gear(` |  |
| `updateRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/editor/buildingSnap.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Where a footprint goes when the pointer is at `cursor`: to a road first)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BUILDING_REACH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far a building still lends its rotation to one being placed. |
| `EDGE_SNAP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How close an edge must come to another to be made flush. |
| `FootprintSize` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The size of a footprint, measured from its anchor. |
| `PlacementSnap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where a footprint goes when the pointer is at `cursor`: to a road first |
| `ROAD_REACH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far from a road a footprint still snaps to it, beyond its own depth. |
| `cornerFlush` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | At a corner, the side of the footprint next to the crossing road slid onto the back of that road's footway |
| `flush` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Makes the footprint's edges flush with a neighbour of the same bearing: along the local x axis always |
| `footprintSize` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `frontAnchor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The front-centre anchor that puts the footprint's CENTRE at `centre`. |
| `nearestBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `snapPlacement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | @perInput Runs once a pointer move (the snap under the cursor): nothing it reaches may walk the map. |
| `snapToRoad` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT walk `net.ribbons` here: this runs on every pointer move. |

### `src/editor/buildingTool.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The building tool, as a state machine in world coordinates.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BaySelection` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingModelTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingPreview` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingSelection` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.activeModelTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.addSetback` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.addStoreys` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.addUpperShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.addWing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.applyFacadeGrammar` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.applyStyle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Dresses the whole selected building in a factory style. |
| `BuildingTool.applyToSelectedBay` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Applies the armed component to the selected bay, with the current scope. |
| `BuildingTool.armComponent` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armCore` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Arms (or with the same kind again, disarms) a free element to place on the selected building. |
| `BuildingTool.armFurniture` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armModelTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armPrimitive` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.armed` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The kind of free element the palette has armed: the pointer places one. |
| `BuildingTool.backPoint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.beginHandleDrag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.beginShapeDrag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.bevelBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Bevels every corner of the selected block, or only the picked corner in point mode. |
| `BuildingTool.blockLift` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far the selected block's floor stands from the building's ground floor (`Volume.lift`), world units. |
| `BuildingTool.blockOffset` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The selected block's centre, as an offset from its reference block's centre (world units). |
| `BuildingTool.blueprintKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.body` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a click in place mode builds. |
| `BuildingTool.builderHintKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A key for the workspace's hint sentence: `hint.builder.<key>` in the dictionaries. |
| `BuildingTool.buildingAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building under a screen point, or null. |
| `BuildingTool.cancelOperation` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Right button, or Escape: ends whatever the tool is doing, staying in the Builder. |
| `BuildingTool.cancelPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.carried` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.changeVertex` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.chooseBlueprint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.chooseShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.clearArmingFor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.click` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.clipboard` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.component` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The component the picker has armed: a click on a bay puts it there. |
| `BuildingTool.constructor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.copyBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A copy of the selected block, set beside it on its free side (or on top when it stands alone) |
| `BuildingTool.copySelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.coreKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A core in hand (lift or stair), or 'remove': the next click on the cut floor places or removes one. |
| `BuildingTool.currentMaterial` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The material the current target is built in. |
| `BuildingTool.cutAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.cutDir` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.cutLevel` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | The interior view: the selected building drawn cut open above this level, or null. |
| `BuildingTool.cutPreview` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.cutRecord` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.cutView` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The way the camera looks (world, horizontal), set by the wiring each frame. |
| `BuildingTool.cyclePalette` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.deactivate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The tool is put away: no ghost, no gesture, no hover. The selection stays. |
| `BuildingTool.deleteRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.deleteSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.detachBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Takes the selected block - and what stands on it - out of the building into a building of its own, in the same place |
| `BuildingTool.drag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.dragging` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.dropPrimitive` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.duplicateSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.editFurniture` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.editFurniture.snap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.editFurniture.under` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The piece under the pointer: the nearest whose footprint holds it. |
| `BuildingTool.endPaintStroke` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The stroke is over: the next press starts a new one. |
| `BuildingTool.endShapeDrag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Releases the drag: the shape is built where the ghost stands. |
| `BuildingTool.extrudeFace` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Extrudes the picked face along its normal: positive out, negative in (push/pull). |
| `BuildingTool.extrudeFaceBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Extrudes the picked face as a NEW block: a brick the face's width and the block's height, set against it |
| `BuildingTool.faceInHand` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.faceRegion` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The picked rectangle of bays and storeys of one face, or null. |
| `BuildingTool.faceRegion.clampTo` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.finishElementRun` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.finishPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Validates the actual building on the terrain, then commits one undo step. |
| `BuildingTool.floorOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Ground-floor height: cached for stored buildings, measured for a preview. |
| `BuildingTool.floors` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.focusSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.free` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Alt held: drags follow the pointer freely, without snapping to the grid. |
| `BuildingTool.freeHand` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Nothing is in hand: the pointer selects, and drags what it selected. |
| `BuildingTool.furnitureAngle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.furnitureKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Furnishing, as in The Sims: a piece (or a light) in hand is put down where the cut floor is clicked |
| `BuildingTool.generate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The generator: a neutral block of the current parameters. |
| `BuildingTool.ghostAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.groupWith` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Agrupar: every volume of another building moves into this record, brought into its frame; the emptied record goes. |
| `BuildingTool.handleAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.handles` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The handles of the selection, in world 3D. Only in edit mode. |
| `BuildingTool.handles.nearest` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Move and rotate go on the two footprint corners nearest the viewer. |
| `BuildingTool.hover` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.hoverElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.hoverHandle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.hoverPlace` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Recomputes the placement ghost for the pointer at `world`. |
| `BuildingTool.hoverPrimitive` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.hoversOnly` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | @perInput Runs once a pointer move: nothing it reaches may walk the map.  Whether a pointer move now only hovers (`pointerMove`: no bay zone dragged,  |
| `BuildingTool.insertStorey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | One more storey in the selected mass, a copy of storey `at`, put in at `at` (below it) or `at + 1` (above it) |
| `BuildingTool.insetFace` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Insets the picked face: its bays, all but a frame of one bay at each end and the top and bottom storeys |
| `BuildingTool.interiorPreview` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | The selected building cut open at the interior level, drawn solid in place of the stored one. |
| `BuildingTool.judge` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.judged` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.judgedCursor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.key` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Returns true when the tool used the key. |
| `BuildingTool.lastPaintKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.lastScreen` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.lastWorld` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.massMoveArmed` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The Move block tool: the press takes hold of the selected mass itself. |
| `BuildingTool.materialScope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.materialTarget` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The surface a material pick paints now, or null (a face scope needs a clicked facade). |
| `BuildingTool.measure` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What the gesture in progress measures, for the overlay to show beside it: a length or a depth |
| `BuildingTool.mirrorSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Mirrors the selected building left to right, in place. |
| `BuildingTool.mode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.moveRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.offsetBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Grows (or with a negative distance shrinks) the selected block's whole plan. |
| `BuildingTool.onCutFloor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.onSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.paint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Paints the current target: a new finish keeps its colour, a new colour keeps its finish. |
| `BuildingTool.paintBrush` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.paintStroke` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.params` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.paste` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.pathKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A run of fence, wall or paving, traced as a path |
| `BuildingTool.pick` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.pickAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building face under a screen point, if any (the bulldozer asks). |
| `BuildingTool.placeCore` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.placeDrawn` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.placeElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.placeHere` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planAction` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planArea` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planCursor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planHeight` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.planPoints` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.pointMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Point mode: every corner of the selected block is a handle, boxes included. |
| `BuildingTool.pointOnPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.pointerDown` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Returns true when the tool used the press. |
| `BuildingTool.pointerMove` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.pointerUp` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.preview` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.primitive` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A basic shape in hand: the next click drops it on a roof, against a wall, or on the ground. |
| `BuildingTool.primitiveDraft` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.primitiveDraft.ring` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Read before the drag is put away: the ring is drawn from its start, and read afterwards it was empty |
| `BuildingTool.primitiveDraft.shape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.problem` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The last problem a gesture reported, for the overlay's label. |
| `BuildingTool.referenceOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The block a block is measured from: the one it stands on |
| `BuildingTool.reliefDepth` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far the picked region is pushed now (0 when flush or nothing is picked). |
| `BuildingTool.removeElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.removeVolume` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.repeatElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Repeats the selected element in a row along the building. |
| `BuildingTool.report` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.reshapeTier` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.resize` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.roofDetailKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.rotateSelected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.rotation` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Rotation the player has asked for; snapping may override it. |
| `BuildingTool.scope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.selectBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Selects a building just made elsewhere (built from a 3D reference) |
| `BuildingTool.selectFloor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Points the face tools at one floor of the selected volume. |
| `BuildingTool.selectRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.selectVolume` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.selected` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.selectedElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The selected free element, if one is. |
| `BuildingTool.selectedRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.selection` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A stair laid along the path climbs the floor being edited |
| `BuildingTool.serial` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setBlockBase` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The level the selected block starts on. |
| `BuildingTool.setBlockLift` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Raises (or, negative, lowers) the selected block's floor to `lift` above the building's ground floor, as typed |
| `BuildingTool.setBlockMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How the selected block combines with the others: solid (null), void, intersect or exclusive. |
| `BuildingTool.setBlockOffset` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Puts the selected block's centre at an offset from its reference block's centre. |
| `BuildingTool.setCutLevel` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  |  |
| `BuildingTool.setFacadeGeometry` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setFunction` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What the selected building is for (null: nothing in particular). |
| `BuildingTool.setGroundHeight` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setLevelHeight` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setMaterialScope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setPaintBrush` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Sets the swatch carried by the canvas paint tool; the edit happens on a face click. |
| `BuildingTool.setParameter` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setPointMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setPreview` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setRelief` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Pushes the picked region out (positive) or in (negative); 0 flattens it. |
| `BuildingTool.setRoof` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setRoofDetailFlag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setRoofDetailFlagDesign` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The selected spire's or lantern's flag design: its pattern and colours. |
| `BuildingTool.setRoofDetailHeight` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setRoofShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Changes the selected volume's roof pitch, ridge or fall. |
| `BuildingTool.setScope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setStage` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setStoreys` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.setVolumeSize` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The selected block's width and depth (world units, null keeps one), about its centre |
| `BuildingTool.shapeDragAction` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.shapeDragAngle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.shapeDragRing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.shapeDragShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.shapeDragSpan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.shapeDragStart` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The Draw tools: a preset footprint sized by dragging on the ground. |
| `BuildingTool.shapeDragZ` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.showDraft` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.splitAtFloor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.stage` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.startElementRun` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.startPlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.sync` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Drops a selection whose building or volume no longer exists (after an undo). |
| `BuildingTool.turnBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Turns the selected block about its own centre by `degrees` (the block, not the building). |
| `BuildingTool.turnFurniture` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A quarter turn for the piece in hand, or for the one being moved. |
| `BuildingTool.turnRoofDetail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.updateElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Resizes or turns the selected element. |
| `BuildingTool.updatePlanPreview` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.updateShapeDrag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.useBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Places from an arbitrary body (a user blueprint, the clipboard). |
| `BuildingTool.verdict` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingTool.zone` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A facade zone being dragged: one side of one block, from a bay to a bay. |
| `BuildingToolMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `CreatorTool` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `DRAG_START_PIXELS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `Drag` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `LONG_PRESS_MS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A press held this long without moving counts as a long press. |
| `MaterialScope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a material pick paints: the whole building, the selected volume, one face of it, or its roof. |
| `PICK_BOTTOM` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `PICK_PAD` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far round the ray's track a step looks: a building is picked on its own volumes, inside its stored box. |
| `PICK_REACH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far along the ground the ray is walked: the deepest ground a view takes in (a low view zoomed out, `isoViewport.ts`), the world having no edge to  |
| `PICK_STEP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far along the map a step of the pick's walk reaches (the store's grid cell, `world/buildings/grid.ts`). |
| `PICK_TOP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The highest a roof can stand (the tallest building on the highest land) and the lowest ground: a ray is walked between them. |
| `PREVIEW_ID` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `PlaceParameters` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Parameters of the generated block (the sliders); lengths in world units. |
| `PlanAction` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ROTATE_STEP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ToolHost` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ToolView` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building tool, as a state machine in world coordinates. |
| `now` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `pickAlongRay` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT copy `buildings.all()` here: every move of the pointer over the map copied and tested the whole city (audit A9). |
| `pickAlongRay.fresh` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `placeAt` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Near a street it faces the street, as a new building does. |
| `stripId` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/editor/buildings.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The building commands.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingContext` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuildingParameters` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `EditResult` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ElementPatch` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `FAIL_MISSING` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `FUSE_GAP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far apart two parts may be and still be welded into one run. |
| `FacadeScope` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where a component is applied when a bay is clicked.  Where a component goes: the picked bay; its row (that floor of that face); its column |
| `FaceRegion` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A rectangle of whole bays and storeys on one face of a volume, inclusive. |
| `MIRROR_SIDE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `RELIEF_STEP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Depths a relief snaps to: five centimetres. |
| `RoofShape` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `UNCHANGED` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `addBuildingRecord` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Stores an already-built record (a preview the tool validated) as a new building. |
| `blockSnap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where a dragged block clicks into place: the shift (dx, dy) that lays one of its sides on a side of another block |
| `boxesMeet` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Whether two boxes meet, `pad` round the first. |
| `buildingsIn` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The buildings whose box meets one of `rects`, each once, in the store's order per rectangle. |
| `clearBuildingsAfterEdit` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT look since the last edit: the roads a tile brought in between two edits would demolish or rewrite the buildings they meet. |
| `clearBuildingsOnRoads` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT walk `buildings.all()` for an edit |
| `clipRing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The part of `ring` inside `within` (local units), as one simple ring - the largest piece when the two meet in several. |
| `copyStoreys` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Copies a volume's storeys for a new volume, keeping the facade fill. |
| `cutRelief` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What is left of a relief once `region` is cut out of it |
| `deleteBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `duplicateBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A copy of a building beside it: tried to the right, left, behind and in front, flush against the original |
| `editBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Runs an operation on a copy of a stored building, validates the copy and stores it. |
| `everyBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | @rebuild What changed is not known (a map loaded): every building, once, one record read at a time: DO NOT copy every record into an array (the whole  |
| `groupInto` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Agrupar: every volume of `source` moves into `target`, brought into the target's own frame. |
| `lookOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Fuses a volume with a neighbour it is flush against, when the two make one rectangle on the same levels.  What a mass looks like, resolved: its walls, |
| `normaliseAngle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `opAddElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Adds a free element (validated with the building by the caller); returns its id. |
| `opAddSetback` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_add_setback(` | Stacks a setback on a volume: inset by `inset` world units |
| `opAddWing` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_add_wing(` | Adds a wing against `side` of a volume: same base and height, `depth` world units out, `length` along the side |
| `opFuseElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Fuses a free part placed beside an identical one |
| `opMirror` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Mirrors a building left to right in its own frame |
| `opMirror.flipSide` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `opMoveBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Moves one block of a building by (dx, dy), with every block that stands on it (and on those) |
| `opRemoveElement` | portado | `native/src/sim/commands/body_parts.cpp` → `std::string op_remove_part(` |  |
| `opRemoveVolume` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_remove_volume(` | Removes a volume and everything that loses its support with it. |
| `opRepeatElement` | portado | `native/src/sim/commands/body_parts.cpp` → `std::string op_repeat_part(` | Repeats an element in a row across its facing - pillars along a facade, a run of canopies |
| `opResize` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_resize(` | Moves one side of a volume by `delta` world units (out is positive), snapped to the grid. |
| `opRotate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Turns the building by `angle` about a world pivot (default: its footprint centre). |
| `opScalePlan` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Scales the whole plan by `factor` about its centre: every mass, the free parts and the roof equipment move with it |
| `opSetComponent` | portado | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_bay(` | Puts a component into the facade. |
| `opSetComponentZone` | portado | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_zone(` | A facade zone: every bay from `index0` to `index1` on every storey from `storey0` to `storey1` of one side takes |
| `opSetLevelHeight` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Overrides one level's height for the whole building; null restores the default. |
| `opSetParameters` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `opSetParameters.set` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `opSetRelief` | portado | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_relief(` | Pushes a region of a face out (positive `depth`) or in (negative), in world units snapped to `RELIEF_STEP` |
| `opSetRoof` | portado | `native/src/sim/commands/body_finish.cpp` → `std::string op_set_roof(` |  |
| `opSetRoofShape` | portado | `native/src/sim/commands/body_finish.cpp` → `std::string op_roof_shape(` | Changes the pitch, the ridge direction or the fall of a volume's roof. |
| `opSetStoreys` | portado | `native/src/sim/commands/body_mass.cpp` → `std::string op_set_storeys(` | Sets a volume's storey count; volumes stacked on it ride up or down with it. |
| `opUnionVolumes` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `opUpdateElement` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Changes an element's size, height or facing. |
| `opUpdateElement.size` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Snapped to the grid unless the player holds Alt; free, to the centimetre. |
| `placeBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Places a new building. The record is validated before it is stored. |
| `polygonBounds` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `removeVolume` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Removes a volume; the last volume on the ground removes the building. |
| `replaceBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Stores a complete replacement record for a building (the tool's drag result). |
| `rideWith` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Moves every volume standing on `v`'s old roof by `delta` levels, recursively. |
| `shiftBays` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Re-indexes single-bay overrides on `sides` by `shift` (a side grew at its start). |
| `simplePieces` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The pieces of a plan polygon (one outer ring, maybe holes) as simple rings without holes |
| `snapLength` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A length snapped to the editor's grid. |
| `storeyTemplate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Storeys copied up when a volume is pulled taller: the top one, in the building's own style. |
| `weldInto` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Welds every building the draft touches or overlaps into the draft |

### `src/editor/bulldozer.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/chunkedSave.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/cityGenerator.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/fromReference.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Builds a building from a reference model)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ReferenceBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ReferenceOptions` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `TOUCH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `Top` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `blocksFromTriangles` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The model's blocks, from its upward faces. |
| `blocksFromTriangles.absorb` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `blocksFromTriangles.overlaps` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `buildFromReference` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building the reference shows; null when the model has no block big enough to build. |
| `buildFromReference.height` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `buildFromReference.level` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `buildFromReference.stoneOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `buildFromReference.wallColour` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | 2. Colour: what the walls show, a storey at a time, on the blocks standing on the ground. |
| `buildFromReference.wallColour.med` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The median of each channel: the wall, not its windows or shadows. |
| `convexHull` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `convexHull.chain` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `convexHull.cross` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `distance` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `luminance` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `normalisedOutline` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A block's hull in its own box, 0..1, without points closer than a few centimetres. |
| `normalisedOutline.clamp` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `toHex` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/editor/growthFeed.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A rectangle grown by `pad` on every side.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Asked` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthHost` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where the growth reads the document: the page's document, network and ground. |
| `GrowthLane` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where the growth's worker answers (`growthPool.ts`; a spec's stand-in). |
| `GrowthOutcome` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What an answer came to: the buildings written (their ids, in order), the lots set aside, and whether the rest was let go to be asked for again (the do |
| `GrowthSession` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.ask` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Asks the worker for the next building (`pickLot`); false when no lot is open (`refused` set aside) - or true, already asking, while one is made. |
| `GrowthSession.asked` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.buildingsRead` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.busy` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.forgetRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.lotOpen` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.most` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.moves` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.nextAsk` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.readChanges` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.receive` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The worker's answer, its buildings written in order (`onGrown` told of each as it is, before the next) while each still stands where it was made - che |
| `GrowthSession.refusals` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.resetBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.resetRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.restart` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The worker started again with nothing (it failed, `growthPool.ts`): all is sent again as it is asked for, and the building being made asked for again. |
| `GrowthSession.roadsOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.roadsState` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.seed` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.send` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.send.union` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.sentBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.sentKerbs` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.sentRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.staleBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.staleKerbs` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.staleRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.syncBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSession.syncRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `MISS_MARGIN` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | More read round a lot each time the worker asked outside what it was sent (`GrowthMiss`), world units. |
| `MOST_MISSES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Misses before a lot is set aside: a lot that reads farther than this is not one the growth can build. |
| `MOST_REFUSALS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Times a lot's building may be refused on the page, the document having moved, before the lot is set aside. |
| `MOVES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The changes a building made off the page may have missed: its neighbours, its roads, its ground, its lot. |
| `READ_PAD` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far past the region its lots read the document: their frame, its margin (`growthReachOf`), a lot's size. |
| `REGION_HALF` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Half the side of the region round the lot asked for whose open lots the growth goes on to in the worker (`GrowAsk.region`). |
| `grow` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A rectangle grown by `pad` on every side. |
| `junctionPiece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A junction's kerb ring as the worker keeps it (`JunctionPiece`), or null: none filed. |
| `roadPiece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A road as the worker keeps it (`RoadPiece`), or null: no ribbon filed for it. |

### `src/editor/growthReplica.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: THE GROWTH'S WORKER: a building grown on a lot - its plan, its body made to measure, its yards furnished, each try checked against its neighbours, its roads and its slope (`zoning.ts` `designOnLot`) -)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BATCH_MOST` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Most lots grown for one ask: the page writes them in one go, and each is drawn after it (a few frames' worth, never the town). |
| `GrowAnswer` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowAsk` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Buildings asked for: the first lot (picked by the page, `pickLot`) and the stream it is made with, then the lots the growth goes on to (`nearestLot`)  |
| `Grown` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot grown: the building made for it, null when nothing fits it. |
| `GrowthMiss` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A question asked outside where the request said its lot reads: the page sends it again with more round it. |
| `GrowthReplica` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What the growth's worker keeps, and the buildings made for each ask (`handle`). |
| `GrowthReplica.handle` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One ask: what changed taken in; the first lot's building made, then the next lot's - the open lot nearest the building grown last, as `pickLot` finds  |
| `GrowthReplica.neighbours` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthReplica.roads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GrowthSync` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What changed of what the worker keeps since the last request (`growthFeed.ts`). |
| `JunctionPiece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A junction's kerb ring as the page's network has it, filed over `rect` (`Network.junctionCells`). |
| `NeighbourReplica` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The buildings round the lots grown, as their neighbours read them. |
| `NeighbourReplica.apply` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `NeighbourReplica.covered` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where the request being answered reads them. |
| `NeighbourReplica.grew` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A building grown here, read by the next lots of the same ask. |
| `NeighbourReplica.grid` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `NeighbourReplica.items` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `NeighbourReplica.near` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `NeighbourReplica.own` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `NeighbourReplica.put` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadPiece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A road as the page's network has it (`validate.ts` `RoadReachRibbon`), filed over `rect` (`Network.segmentCells`). |
| `RoadReplica` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The roads and junctions round the lots grown, as the checks read them (`RoadReach`). |
| `RoadReplica.apply` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.check` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.covered` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where the request being answered reads them. |
| `RoadReplica.doc` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.dropKerb` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.dropRoad` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.junctionCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.junctions` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.kerbCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.kerbs` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.records` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.ribbons` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.ribbonsMeeting` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.roadCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.roads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RoadReplica.segmentCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `Structured` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A road's structure as `RoadReach.doc` answers it. |
| `pavedLookup` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The paving's height as the page reads it: at the points it read for the lot (`paved`), else off the roads handed over (`pavingReader`, the page's numb |
| `within` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether rectangle `a` lies within `b`. |

### `src/editor/history.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/lotTool.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays

**Para o jogador:** Editar lotes: dividir, juntar, acrescentar, apagar, definir frente; camada dos lotes. _(no 4.0: How far apart a zone stroke is sampled between two pointer reports: under any lot's front.)_

**Motivo:** Sem prova por símbolo até agora. Aba Zonas › Lotes está 'em construção'.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DRAG_REACH` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | How near a dragged corner the corners it moves are (`drawLot`). |
| `GESTURE_RANK` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotDecor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What a lot's group is drawn with besides the lot itself (`LotTool.drawLot`): -1 for no lot. |
| `LotOverlay` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What the scene draws of the lots, a group at a time (the shape of `render/lotOverlay.ts` `LotOverlayShapes`), and the labels. |
| `LotOverlayBook` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | DO NOT hand the scene every lot again for a pointer move: the overlay by group, each put again only when it changed |
| `LotOverlayBook.changed` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotOverlayBook.changedFor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The groups put or taken away since `reader` last asked; null: all of them (another reader than the last). |
| `LotOverlayBook.groups` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotOverlayBook.labels` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The labels for the 2D layer: a lot's size, a side's length. |
| `LotOverlayBook.put` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A group put (null: taken away). |
| `LotOverlayBook.reader` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotOverlayGroup` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | One group of the overlay: its shapes and its rank in the drawing order (`render/lotOverlay.ts` `LotOverlayGroup`). |
| `LotSplitKind` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | How the split tool cuts (`LotCut`): across the front, parallel to it, or along a drawn line. |
| `LotTool` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.adoptProposal` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.advanceProposal` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The proposal brought up to the land as it is: when what it stands on changed, the plan asked of the host (planned in the simulation's worker, incremen |
| `LotTool.anyZoned` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Whether any lot is zoned: the faint zones are shown with the other tools. DO NOT walk every lot when one changes. |
| `LotTool.asking` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.book` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.brushProposed` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.builtCursor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.cancel` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Every gesture dropped without committing it. |
| `LotTool.commitStroke` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.constructor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.corner` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.curve` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.cutLine` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.doc` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.down` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | @perInput Runs once an input on one thing (a press, a release, a click): nothing it reaches may walk the map. |
| `LotTool.drawAllLots` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawLot` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawLot.dragged` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawn` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawnCorner` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawnFrom` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.drawnWith` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.erase` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The whole way from the last sample, as the brush's stroke. |
| `LotTool.eraseUnder` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.frontSideOf` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.gesture` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What the player is in the middle of doing with it, named (`main.ts` `currentGesture`). |
| `LotTool.gestureKey` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.held` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.holdOverlay` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Holds the quiet overlay as it is (a city growing, a lot a slice); released, the next frame lays it again. |
| `LotTool.hover` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The pointer over the map while the tool is in hand. |
| `LotTool.joinFirst` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.landLot` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.lotAt` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.lotsCursor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.lotsNear` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.lotsSeen` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.modeChanged` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Another mode picked: a polygon half drawn or a join half chosen is dropped. |
| `LotTool.move` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | @perInput The pointer moved; true when one of this tool's gestures took it. |
| `LotTool.net` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.nextProposalId` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.overlay` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The lots as the scene draws them (`render/lotOverlay.ts`), brought up to date: in the Zoning tool (`editing`), or the faint zones |
| `LotTool.painted` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.paintedRemove` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.polygon` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposal` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposalCursor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposalFiling` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposalKey` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposalNet` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposalReady` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Whether the proposal for the land as it is now is ready (probes). |
| `LotTool.proposalStamp` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.proposedAt` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.readPending` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshGesture` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshGesture.at` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | DO NOT decide by the samples alone (EDT-12: a lot narrower than a twentieth of the cut fell between two and was not cut): where the cut meets the lot' |
| `LotTool.refreshGesture.hoverSnapped` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshLots` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshProposal` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshProposal.draw` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refreshProposal.stateOf` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refused` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refusedNet` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.refusedNow` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | DO NOT let every refused lot of the town go for an edit |
| `LotTool.shownProposal` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.sideAt` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.sideNear` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.snap` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.snapExcept` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.snapReach` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.streetAngleNear` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.stroke` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.takeWaiting` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.up` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | @perInput The pointer let go: the gesture it held is committed (`commit`), or dropped. |
| `LotTool.waiting` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.zonedCursor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotTool.zonedIds` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotToolHost` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What the tool reads of the game and does to it. |
| `NO_PROPOSAL` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | No proposal drawn (out of the tool, or not worked out for the land as it is). |
| `PROPOSAL_CELL` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Side of the grid the proposed lots are filed in for the pointer (`proposedAt`): a few lots wide, as the planner files them. |
| `PROPOSAL_RANK` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Drawing ranks of the overlay's groups (`render/lotOverlay.ts`, under 2^29): the lots by their order, then the proposal, then the gesture. |
| `REFUSED_REACH` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | How far apart a zone stroke is sampled between two pointer reports: under any lot's front.  How far from a road edit a refused lot is tried again: its |
| `STROKE_STEP` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `ZONE_COLOURS` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `ZoneMode` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Brush paints the cells under the pointer; the other modes edit the lots. |
| `segmentCrossesLot` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Whether the segment a-b passes through the inside of a lot. |

### `src/editor/persistence.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/preparedCity.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/regionFrames.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/smallCityMap.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/editor/transitTools.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/growth.worker.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL. _(no 4.0: This service of the job system (`job.worker.ts`): the growth's buildings made off the page (`growthPool.ts`), one replica per port.)_

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `serve` | não se aplica | core, JobPool, RenderWorld | This service of the job system (`job.worker.ts`): the growth's buildings made off the page (`growthPool.ts`), one replica per port. |

### `src/growthPool.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL. _(no 4.0: THE GROWTH'S LANE OF THE JOB POOL, FROM THE PAGE (`growth.worker.ts`): one building at a time; a failed lane falls back to a replica on the page. DO NOT take a silent worker for a dead one: it shares )_

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GrowthPool` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.constructor` | não se aplica | core, JobPool, RenderWorld | `take`: an answer came, false to offer it again next frame; `restarted`: the replica is new, all to be sent again. |
| `GrowthPool.dispose` | não se aplica | core, JobPool, RenderWorld | Ends the worker (the page going away). |
| `GrowthPool.fail` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.held` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.local` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.offer` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.pending` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.post` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.postedAt` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.pump` | não se aplica | core, JobPool, RenderWorld | Once a frame: an answer held offered again, the page's replica's lot answered, a silent worker reported (and still waited for). |
| `GrowthPool.start` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.told` | não se aplica | core, JobPool, RenderWorld |  |
| `GrowthPool.worker` | não se aplica | core, JobPool, RenderWorld |  |
| `SILENCE_MS` | não se aplica | core, JobPool, RenderWorld | Silence past which a worker with a lot waiting is reported, once a stretch, and still waited for (one lot is milliseconds; the module's first load, so |

### `src/jobs/jobPool.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `warmJobWorkers` | não se aplica | core, JobPool, RenderWorld | src/jobs/jobPool.ts:537-539 function warmJobWorkers: the feature's own |

### `src/lab/catalog/capture.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/lab/catalog/city.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador. _(no 4.0: Everybody drawn last frame and the body the casting gave them (`renderer.ts` census).)_

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/lab/catalog/engineBuildings.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DENSITIES` | não se aplica | project/ui, GameFlow | src/lab/catalog/engineBuildings.ts:30-30 variable DENSITIES: the feature's own |
| `LOTS` | não se aplica | project/ui, GameFlow | src/lab/catalog/engineBuildings.ts:28-28 variable LOTS: the feature's own |
| `USES` | não se aplica | project/ui, GameFlow | src/lab/catalog/engineBuildings.ts:29-29 variable USES: the feature's own |
| `engineBuildingFarItems.ground` | não se aplica | project/ui, GameFlow | src/lab/catalog/engineBuildings.ts:41-41 function engineBuildingFarItems.ground: the feature's own |

### `src/lab/recorder/report.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/main.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/render/bake/finishRecipes.ts`

**Estado:** a portar · **Dono no jogo:** building_common.gdshaderinc, assets/surfaces/building-finishes.*

**Para o jogador:** Materiais das paredes (reboco, tijolo, pedra, metal) e o intemperismo. _(no 4.0: The building finishes' ("Materials") texture recipes, baked at build time (`scripts/vite/surface-bake-plugin.ts`))_

**Motivo:** Sem prova por símbolo até agora. Camadas cozidas e finish_layer/weather já no jogo.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FINISH_LAYER_SIZE` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | The side of the finishes' texture arrays (`catalogue.ts`): the largest finish's; a smaller one is tiled in its layer. |
| `FinishLook` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |
| `LOOKS` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |
| `Shade` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |
| `bond` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | A running-bond masonry pattern: `rows` courses and `across` units per course in one tile |
| `cellHash` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | A small integer hash, stable and tiling: per brick, per board, per tile. |
| `finishTiles` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | How many times a finish's own texture repeats across its layer of the arrays, each way. |
| `grey` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |

### `src/render/buildingSwitches.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE BUILDINGS' SWITCHES (the effects panel, `effectSwitches.ts`): each part of the layer and each kind of piece hidden for the frame, their materials drawn plain, their shadows left out of the sun's p)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingSwitcher` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `KIND_SWITCH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A piece by the name its mesh is given (`buildingMesh.ts`, `partCells.ts`: `building-<kind>`), and the switch that hides it. |
| `KIND_SWITCHES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ROOT_SWITCH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingSwitcher` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingSwitcher.afterRender` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | After it: the materials given back, the shadows' hiding undone. |
| `createBuildingSwitcher.beforeRender` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Before the frame is drawn: the parts switched off hidden (by `hider`), the materials swapped. |
| `createBuildingSwitcher.beforeRender.walk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Only what is drawn: a hidden group's meshes are not walked. |
| `createBuildingSwitcher.casters` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each sun cascade's pass (`texel`), and their end (null): the buildings left out of it while their shadows are switched off. |
| `createBuildingSwitcher.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingSwitcher.giveBackShadow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `kindOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |

### `src/render/buildings/buildingMesh.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_*, roof_*, lot_*, shell

**Para o jogador:** Toda a geometria do prédio: paredes, vãos, telhados, lote, escadas. _(no 4.0: Where the flags of the building being emitted go (`emitChunk` sets it).)_

**Motivo:** Sem prova por símbolo até agora. Emissores já no jogo; o que falta (interior no corte, escadas abertas, relógio, bandeira) é tratado por item.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ALUMINIUM_COPING` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A parapet's metal coping: aluminium flashing. |
| `APRON` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ARCADE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ARCADE_FLOOR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ATTIC_H` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A deep plain parapet: the attic band that crowns a block of the 'attic' dress. |
| `AssemblySelection` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What `assembleBuildingMeshes` puts together, and where the shells go. |
| `BAND_H` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `BAND_OUT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `BIN_BODY` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `BIN_LID` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `BOLLARD_BAND` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `BayFace` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A face of one bay: its frame in the building's local plan, and its storey. |
| `BuildingMeshes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CAB` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CAR_GATE_OPEN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The narrowest gate drawn standing open: a car's (`sim/agents/parking.ts` `CAR_GATE`); a person's is 1.2-2 m. |
| `CHEEK_BURY` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far into the building's wall that wall's back is let. |
| `CHEEK_FOOT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far under the storey's floor that wall starts: under the slab, at no height another part ends at. |
| `CHEEK_IN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far inside its bay that wall stands: off the bay's edge, where the slab and the next bay end. |
| `CHIMNEY` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CLOCK_FACE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CLOCK_INK` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CONDENSER` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `COPING` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `CORNICE_OUT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `DECK` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `DOOR_PAINT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `DOWN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `DRAIN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `DRAIN_GRATE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `EAVES` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `EITHER_SIDE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ELEMENT_CONCRETE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.L` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A local plan point at absolute height z, in world axes. |
| `Emitter.N` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A local direction as a world normal. |
| `Emitter.P` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A point on a bay face: `a` along, `h` up, `depth` into the wall. |
| `Emitter.apart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The elements' boxes as instances of the shell's parts (`shellParts.ts`, `emitChunk` `apart`); false lays them here face by face. |
| `Emitter.box` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.boxOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.building` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.c` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.constructor` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `fallback`: the colour of an instance placed with none (`PART_DEFAULT_COLOUR`). |
| `Emitter.corner` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.cylinder` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | An axis-aligned box in the local plan: sides and top (the bottom is never seen).  A round tank or drum: an upright prism of `sides` faces, capped on t |
| `Emitter.cylinder.at` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A model point (three's axes) through the matrix, then into the map's (x, y north, z up). |
| `Emitter.flat` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The bay being emitted is flattened (the far level, `emitChunk` `far`): its wall and opening laid by `emitFacadeRuns`. |
| `Emitter.glazed` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pane and its frame over it, the frame `thickness` deep and 3 cm proud of the glass as the kit's frame was put: the frame drawn by the pane's  own ma |
| `Emitter.glazedRecessed` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pane laid on the wall's face over the whole opening, its glass `depth` behind it: the pane's material draws the reveals, the sill, the  head and the |
| `Emitter.inElement` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | An element being emitted (`emitElement`): its boxes are the shell's parts where the shell lets them be (`Shell.apart`). |
| `Emitter.jamb` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A vertical strip across the wall's thickness at `a`, from h0 to h1. |
| `Emitter.parts` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.propColour` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.put` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.putAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | An instance at a point of the building's plan (`L`), square to the building, its centre at height `z`. |
| `Emitter.quad` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.rect` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A rectangle on (or parallel to) a bay face. |
| `Emitter.room` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The array, with room for `n` more numbers after `length` (a new one, twice as large, when full). |
| `Emitter.s` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Only along the outside: against another block of the ground the apron lay inside the building. |
| `Emitter.shellPart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A shell's part (`shellParts.ts`) centred at plan point (lx, ly), from z0 to z1, `along` its own x turned `turn` from the building's and `across` it. |
| `Emitter.sides` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A box whose top another part lies on (a wall's body under its coping, a post under its head, a gate's bar between its rails): its four sides. |
| `Emitter.slab` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A box whose sides stand `inset` in from the edges of its top. |
| `Emitter.slot` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | An instance placed on a bay face, turned so its local +Z is the face's outward normal.  The space of the bay being drawn, for its panes (-1: none). |
| `Emitter.spot` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Emitter.strip` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A horizontal strip at height h, from depth d0 to d1 into the wall. |
| `FACE_NORMAL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The face being laid's normal, octahedral (`octNormal`), written over by each face. |
| `FLAT_DEPTH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The deepest opening the far level lays flat (its error, `massCells.ts` `FACADE_ERROR`): a loggia's deeper recess keeps its geometry. |
| `FLOOR_FINISH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `FLOWER_KERB` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `FacadeStamp` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What the windows of a run of bays are, for the far level's shader to draw them where the detail has them (`facadeLevel.ts`) |
| `FarBay` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A bay of the far level: its wall's paint, its opening (none: a plain wall) and the foot its grime is measured from. |
| `FarRun` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A run of bays the far level lays as one quad: its first bay, how many along, how many storeys up. |
| `FurnitureSink` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The furniture's rows by kind, made as a kind is first placed. |
| `GATE_LANTERN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The lantern on a person's gate post: width and height, standing on the post's cap. |
| `GATE_PAINT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `GATE_POST` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A gate pillar's width along the gate. |
| `GOURMET_CHEEK` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How thick the side wall at the end of a run of varandas gourmet is. |
| `GOURMET_OUT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far a varanda gourmet's slab stands out of the wall. |
| `Grow` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT lay a shell in number[]: 8 bytes a number and its regrowth, tens of MB of garbage a cell; a typed array that doubles as it fills, viewed at its |
| `Grow.array` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Grow.bits` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The array's bytes as 32-bit words (a new view only once it grew): what the vertex weld hashes and compares. |
| `Grow.constructor` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `fallback`: the colour of an instance placed with none (`PART_DEFAULT_COLOUR`). |
| `Grow.copy` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What was written, in an array of its own size (the arrays written again for the next building, `ShellWriter`). |
| `Grow.length` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT ask the built-in: its result comes back boxed, one heap number a face (`core/hypot.ts`). |
| `Grow.room` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The array, with room for `n` more numbers after `length` (a new one, twice as large, when full). |
| `Grow.view` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What was written, without a copy. |
| `Grow.words` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `HANDRAIL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `KERB_INSET` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Layers at a lot's edge, so no two faces there share a plane (a paving's `COVER_INSET` in `shellParts.ts`, its program stands its sides in by it) |
| `KIT_BOXES` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Each part's and piece of furniture's geometry box as the kit draws them (`cellArrays.ts` works the batches' bounds from them). |
| `LAMP_POST` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `LAWN_LATTICE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The open blocks of a building, laid on the ground  The grid the terrain is drawn on, whose triangles a lawn's parts are seated over (`lawnGround.ts`). |
| `LOGGIA_DEPTH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How deep a loggia is let into the wall. |
| `LOT_KERB` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `LOT_LIFT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `LOT_PAINT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Each lot surface in its finish, in the colour the far view lays it in too (`massing.ts`). |
| `MACHINE_ROOM` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A machine room's walls where the building's skin is glass: rendered blockwork. |
| `Opening` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARAPET_H` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARAPET_T` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARASOL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARKING_ASPHALT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARKING_LINE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PARTITION` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PART_DEFAULT_COLOUR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PART_INSET` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far a part fitted inside another (a shutter in its frame) keeps off its edges, so no two faces share a plane. |
| `PATH_REACH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PATH_WIDTH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PAVING` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PLANTER_SOIL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PLINTH_GROW` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PLINTH_RISE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far a plinth's ledge and a door's threshold stand over the floor |
| `POOL_COPING` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The coping round a pool's rim: white stone, a little proud of the deck. |
| `POOL_FLOOR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pool's floor, under the water: pale blue plaster. |
| `POOL_TILE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `POOL_WATER_DROP` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Depth of the water under the coping. |
| `PORCELAIN_FLOOR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PORCH_WASH` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How far its light reaches over the wall round it, either way and up or down. |
| `POT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `POT_SOIL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Paint` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A surface's colour (linear) and the finish it is drawn with. |
| `PartRows` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A kind's instances as the arrays of its batch (`PartBatch`), written as each is placed: DO NOT make an object of each placement (its numbers boxed, ev |
| `PartRows.add` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One instance at (x, y, z) in world axes, turned `yaw`, sized (sx, sy, sz): its pose (`partPose.ts`), the kind's colour, no look, slot or room; its ind |
| `PartRows.batch` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The batch of what was placed, in arrays of its own (the rows are written again for the next building). |
| `PartRows.clear` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Emptied for the next building, its arrays kept at their size. |
| `PartRows.colour` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Instance `i`'s own colour (a kind drawn with none keeps none). |
| `PartRows.colours` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartRows.constructor` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `fallback`: the colour of an instance placed with none (`PART_DEFAULT_COLOUR`). |
| `PartRows.count` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartRows.look` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A shell's part's look (`shellParts.ts`): its finish id, its wear as the shell's vertices carry it and its ground (`NO_GROUND`: none). |
| `PartRows.looks` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartRows.pane` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pane's light slot and the room behind it (`roomBehind.ts`, ROOM_FLOATS numbers as half floats), when known. |
| `PartRows.paneFrame` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pane's frame, drawn by the pane's own material (`glassRoom.ts` PANE_FRAME_GLSL): its trim colour and its thickness from front to back. |
| `PartRows.paneRecess` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A pane laid on the wall's face over its opening: how deep its glass stands behind it (`glassRoom.ts` PANE_RECESS_GLSL draws the reveals). |
| `PartRows.poses` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartRows.rooms` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartRows.slots` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `PartSink` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Every part kind's rows, written again for each building (`ShellWriter`). |
| `PlanRect` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A rectangle of the plan, local: x0, y0, x1, y1. |
| `RAIL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `RAIL_OFF` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A fence rail's height off the whole centimetre: 5 mm. |
| `REVEAL` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | ------------------------------------------------------------------ dimensions |
| `ROOF_PLANT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ROOF_THICK` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Depth of a pitched roof's edge: rafter, battens and tiles, read at the fascia (18 cm). |
| `Rgb` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `SAW_GLASS` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `SHADER_DRAWN` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What a flattened bay's shader draws in its opening (`facadeLevel.ts`): the parts set into it are left out. |
| `SHAFT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `SHELL_PART_MATRIX` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One instance's matrix, written over by each instance `shellPartFaces` lays. |
| `SHUTTER_PAINTS` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The shutters' paint: a colour of their own against the wall, as Brazilian houses paint them |
| `SKYLIGHT` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `SOLAR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `STAIR_SOLID` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A flight taller than this is built open: treads on a raking slab, with handrails. |
| `STEP` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `STREAKED` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Walls that take a rain streak: render, stucco, concrete and stone (not glass, tile, brick or metal). |
| `Shell` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Triangles in THREE's axes, one buffer set per finish; winding fixed per face. |
| `Shell.apart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The elements' boxes as instances of the shell's parts (`shellParts.ts`, `emitChunk` `apart`); false lays them here face by face. |
| `Shell.clear` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Emptied for the next building, its arrays kept at their size. |
| `Shell.decay` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | How run-down the building being emitted is (`Building.decay`): stains, peeling paint, grime. |
| `Shell.face` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT lay only the first two triangles: a lantern's octagonal and 24-sided caps |
| `Shell.far` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The far level being emitted (`emitChunk` `far`): its bays laid as runs, the runs with openings in `runs`. |
| `Shell.ground` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The ground floor's height for the building being emitted: walls darken towards it |
| `Shell.noise` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | No two stretches of wall quite the same tone, and the base weathered. A vertex's own shade (a streak under a sill darker at its foot, `emitStreak`). |
| `Shell.order` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.parts` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.reused` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Written again for each building of a job (`ShellWriter`): its chunks copy what was written out. |
| `Shell.runBytes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.runFlip` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.runOrder` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.runs` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Up the facade: a run over another alike, the same bays, one storey higher, joins it, but never over the base storey the grime darkens. |
| `Shell.stamp` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The windows of the face being laid (the far level's runs of bays). |
| `Shell.stampRun` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.tap` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Shell.triangles` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One finish's share of a shell, written as laid in the GPU's layout (`ShellChunk`): flat-shaded, painted triangles, the uv worked out on the GPU. |
| `ShellPart.clear` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Emptied for the next building, its arrays kept at their size. |
| `ShellPart.clearSlots` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.constructor` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `fallback`: the colour of an instance placed with none (`PART_DEFAULT_COLOUR`). |
| `ShellPart.facade` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Half floats (`cellArrays.ts` `FACADE_FLOATS`): a bay's width and a storey's height to a millimetre or two. |
| `ShellPart.grid` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.hash` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.hashes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.index` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.normal` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Two signed shorts a vertex, the normal octahedral: unfolded by the shell's program (`finishes.ts` `SHELL_NORMAL`). |
| `ShellPart.opening` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The far level's window grid and openings, for its shader (`facadeLevel.ts`). |
| `ShellPart.paint` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A building paint in its own colour: held to 70 % chroma the town read grey (the player, 2026-10-09). |
| `ShellPart.position` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Each -0 written +0 (x + 0, 0 - y), so the two weld: the GPU draws them alike. |
| `ShellPart.rehash` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.room` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The array, with room for `n` more numbers after `length` (a new one, twice as large, when full). |
| `ShellPart.same` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The same chunks make the same pieces in the same order. |
| `ShellPart.slots` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.streams` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.vertices` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.views` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellPart.weld` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The vertex just written at `vertices`: kept as a new one, or the one laid before with every number alike; its index. |
| `ShellPart.widths` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellTap` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What the engine's catalogue is told of each face laid (`src/lab/catalog/shellPieces.ts`): its corners (world axes, z up), its normal and its paint. Nu |
| `ShellWriter` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT grow a new shell's arrays for each building of a job from nothing (every one doubling its way up, the half let go, its chunk holding views of a |
| `ShellWriter.constructor` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `fallback`: the colour of an instance placed with none (`PART_DEFAULT_COLOUR`). |
| `ShellWriter.furniture` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellWriter.next` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The shell and the rows emptied for the next building. |
| `ShellWriter.parts` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `ShellWriter.shell` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The base weathered at the block's own foot (a split level stands at its own floor). |
| `SlotAllocator` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The light slot of a bay's window: the space on its floor behind it.  Who gives a space its seed for its windows' light (`lightSlots.ts` `spaceSeed`; a |
| `Stream` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One of a part's arrays as a vertex fills it: its numbers and its 32-bit words a vertex (every vertex whole words, so the weld reads them as words). |
| `TANK_PAINTS` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Water tanks: the blue fibreglass of most roofs, the grey and the white of others, a sand-coloured one. |
| `TERRACE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `TREAD` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `TRUNK` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `Typed` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `UP` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Straight up and down, and the two sides of a centre (-1, 1): DO NOT write them as literals in what lays every bay (a new array each time). |
| `V3` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `WAIST` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `WHITE_RGB` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `WHOLE` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What a batch takes by default: every part, the shells and the furniture. |
| `WOOD_FLOOR` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `alikeBays` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Whether two far bays have the same key in `emitFacadeRuns` (`keyOf`), field by field: DO NOT write each bay's key out as text, ten numbers to strings  |
| `assembleBuildingMeshes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Meshes for a set of building chunks: one merged shell, one instanced batch per part. |
| `band` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A moulded surround in the trim, then the louvred leaves folded back on the wall either side, in the shutter paint |
| `band.tier` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A moulding is a PROFILE, not a slab |
| `bayHash` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A stable number per bay, for choosing among window variants. |
| `bayKey` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A bay's place as one number (its block's id under 2^20, its level from -1,024 to 1,023, its side, its index from -1 under 2^20): DO NOT key the bays b |
| `bothCovered` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The stretches covered in both lists (sorted, each its own spans). |
| `boxOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `buildBuildingMeshes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Meshes for buildings, emitted afresh (the preview; tests). |
| `canopyAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The height of a door canopy's slab (its centre, `half` its thickness) |
| `chunkSummary` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What the page keeps of a building (`cellWork.ts` `ChunkSummary`), as `emitChunk` makes it, without laying it: its lots' ground recipe and its signatur |
| `clearOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A level piece of paving with the ground of the flights taken out of it: what is left, in pieces of the same kind. |
| `copingOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A parapet's coping: the trim where it is a masonry moulding of a quiet colour (a stone or rendered cap) |
| `elementPaint` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What an element is made of until the player says otherwise. |
| `emitBay` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitBay.glaze` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The opening's pane: laid on the wall's face where the opening is clear (`flush`), else at its glass. |
| `emitBuilding` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitBuilding.entranceOpening` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The opening of an entrance, in its bay's face frame. |
| `emitBuilding.entranceStart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Where an entrance's bay starts along its side. |
| `emitBuilding.wallOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitChunk` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `far`: the building's far level (L1) - walls as runs of bays with their windows drawn by the shader, no parts (`facadeLevel.ts`).  `apart`: the elemen |
| `emitChunk.inLot` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitClock` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A facade clock: a pale dial on a dark rim, twelve marks and two hands standing just proud of it |
| `emitClock.layer` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitCornice` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Its top stays just under the roof: a face shared with the roof cap z-fights into stripes. |
| `emitElement` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One free element: its boxes, its pavings and its fences' rails the shell's parts, instanced (`shellParts.ts`); the rest laid in the shell. |
| `emitFacadeRuns` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT lay the far level bay by bay: a storey of like bays is one quad, like storeys over the base one quad, the windows drawn by the shader (`facadeL |
| `emitFacadeRuns.keyOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Bays alike: same block, side, plane, bay and storey size, paint, opening and grime foot. |
| `emitFence` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A run of fence: posts and two rails along the box's length. |
| `emitFence.at` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A model point (three's axes) through the matrix, then into the map's (x, y north, z up). |
| `emitFlag` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A flag flown from a mast whose top is at (x, y, flagTop) |
| `emitFlowers` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A flower bed: soil in a stone kerb, with a handful of coloured blooms. |
| `emitInterior` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Inside a building cut open at level `cut` (the Construction tool's interior view) |
| `emitInterior.zAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The cut floor's height where a point of the plan is: each block's own, on a split level (`Volume.lift`). |
| `emitLantern` | portado | `native/src/render/buildings/roof_gear.cpp` → `void lantern(` | The lighthouse on a tower top, measured on the Altino Arantes model |
| `emitLantern.at` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A model point (three's axes) through the matrix, then into the map's (x, y north, z up). |
| `emitLantern.disc` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitLantern.ring` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The kerb round it, down to the ground, so no edge stands in the air. |
| `emitLot` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | ---- the lot: a paved apron round the base, and a path to the street |
| `emitLot.paved` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitLot.strip` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A horizontal strip at height h, from depth d0 to d1 into the wall. |
| `emitLotPart` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The furniture of a lot. |
| `emitLotPart.part` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Every vertex of every shell passes here (a town's opening: 2.5-3 s of its 6 s of buildings): straight into the GPU's layout, its uv worked out there ( |
| `emitLots` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `entrances`: the building's ways in as its body's emission worked them out (`emitBuilding`): DO NOT work its foundation out again for the flights' fee |
| `emitLots.at` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A model point (three's axes) through the matrix, then into the map's (x, y north, z up). |
| `emitLots.hostAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT take the first lot that holds it. |
| `emitLots.onLawn` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitOpenStair` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A tall flight as it is really built: every step a tread and a riser, the lot carried by a raking concrete slab |
| `emitOpenStair.P` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A point on a bay face: `a` along, `h` up, `depth` into the wall. |
| `emitOpenStair.soffit` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitParking` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A parking apron: asphalt and the stall lines painted on it. |
| `emitParking.lift` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Laid on the lot, following its fall |
| `emitParking.quad` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitPlinth` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A ground volume's plinth, from `bottom` to `floor`, PLINTH_GROW proud of the walls |
| `emitPlinth.coveredOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Only along the outside: where another block of the ground stands against a side, the plinth |
| `emitPolygonRoof` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Cut a pitched roof into planar regions, then clip each region to any outline. |
| `emitPolygonRoof.heightAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitPolygonRoof.rect` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A rectangle on (or parallel to) a bay face. |
| `emitPorchLight` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The lantern beside a front door, a little under the door's head, on the side where the bay's wall has room for it |
| `emitRailLine` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Both handrails of a flight in an arbitrary frame. |
| `emitRecessedFlight` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A flight set into the building: from the plinth's face up to the door at `recess` behind the facade |
| `emitRelief` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The frame of a relief: its two cheeks and its head and sill, between the side's plane and the region's |
| `emitRelief.P` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A point on a bay face: `a` along, `h` up, `depth` into the wall. |
| `emitRocks` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Boulders: two or three low prisms, each turned a little. |
| `emitRoof` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitRoofDetails` | portado | `native/src/render/buildings/roof_gear.cpp` → `void emit_roof_gear(` | User-placed details follow the mass and rest on its actual roof surface. |
| `emitRoofPlant` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | What stands on a flat roof: drains in two opposite corners |
| `emitStairRails` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The handrail of a flight: a slim rail over each edge, at a constant height above the nosings |
| `emitStairRails.P` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A point on a bay face: `a` along, `h` up, `depth` into the wall. |
| `emitStreak` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The rain streak under a sill: water off the sill runs down the wall and leaves a darker wash fading downwards |
| `emitTerrace` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A roof terrace that is used (the player's order of 2026-10-05) |
| `emitTerrace.rnd` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `emitTurnedStair` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A stair laid along a traced path |
| `emitTurnedStair.P` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A point on a bay face: `a` along, `h` up, `depth` into the wall. |
| `facadeKind` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The far level's kind of a bay's opening: which panes, doors or shutters `emitBay` puts in it. |
| `faceIds` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Each corner of the face being laid: the vertex it was welded to (grown for a lantern's 24-sided cap). |
| `flagOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A roof part's flag, or null: its own design, else the fixed one it names. |
| `flagSink` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Where the flags of the building being emitted go (`emitChunk` sets it). |
| `fliesFlag` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Whether a building flies a flag: a spire or a lantern on its roof with a flag (`flagOf`). |
| `flightFeet` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The ground the doors' flights of steps stand on, in the plan (local rectangles) |
| `flightSpan` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Where a door's flight of steps stands along its side, in the side's own axis (`sideStart`) |
| `flushBand` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A band flush with the wall, standing out only `out`: a slab edge, not a moulding. |
| `furnitureRows` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `gridBounds` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | World bounds in a grid's frame (`ShellGrid`): over its whole span from its origin. |
| `gridBounds.at` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A model point (three's axes) through the matrix, then into the map's (x, y north, z up). |
| `hashWord` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A 32-bit word into a running hash (MurmurHash3's block step, A. Appleby). |
| `insideRing` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `interiorFurniture` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The furniture of every floor of a building, as it stands inside: what a blow throws out of it when it breaks |
| `kitBoxes` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | @rebuild Worked out once per kit; every later ask returns the boxes kept. |
| `layElement` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `letGoEmitted` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT hold a job's every building's furniture and bays until the job is done (a dense cell's 15 MB of furniture and 9 MB of bays as JS objects in the |
| `linear` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `looseParts` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The parts of a grown lot nothing holds up (`unsupportedElements`): DO NOT work them out again for the building's lot after its body (`emitBuilding`, ` |
| `lotGroundAgain` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A building's lot ground made again from its recipe, as `emitLots` made it |
| `lotGroundOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The ground of a building's lots as drawn: each lot's surface, and the level a parking bay outside every lot is laid at. |
| `lotGroundOf.heightAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `meshesOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A batch's meshes from its arrays (`cellArrays.ts`), made here or in a worker |
| `meshesOf.dispose` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT name `meshes` here: a shell taken into the multi-draw batch (`shellBatch.ts` add) leaves the group, and this closure, kept as long as the cell, |
| `meshesOf.refilled` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `meshesOf.setVisible` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Shown or hidden as a whole. |
| `openingOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The hole a component cuts in its bay, or null for a solid bay. |
| `paint` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A building paint in its own colour: held to 70 % chroma the town read grey (the player, 2026-10-09). |
| `partSink` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `placeFurniture` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | One piece of furniture, as an instance of its model |
| `postAt` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A slim square post of the handrail, from a tread up to the rail. |
| `previewWriter` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | DO NOT emit each preview into a new shell: the ghost is emitted again at every change of the draft, and a new shell's arrays start at 64 and double up |
| `runChunks` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The far level's runs of bays with openings by finish, each vertex with its window grid; undefined when none was laid. |
| `sameFixed` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Two numbers written alike to four places (`toFixed(4)`), the text made only when they differ. |
| `sameLength` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | Two lengths the same to the emitter's sums. |
| `setShellTap` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The catalogue's tap on every face of a shell, or none. |
| `shadeOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The canvas colour of an awning, from the building's own palette. |
| `shaded` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `shellChunkOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A part's arrays viewed at their length, no copy. |
| `shellChunks` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A shell's fragments by finish: the arrays it was laid in (every face but the far level's runs with openings). |
| `shellPartFaces` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The shell's parts of a chunk laid as the shell's faces again, as `Emitter.box`, `slab` and `emitFence` laid them, from the instances alone (`shellPart |
| `shutterColour` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `sideFrame` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The frame of a face of `side`, starting `along` units from the side's start, its plane `push` units out from the side |
| `sideFrameInto` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | `sideFrame` written into `out` (`sideStart`'s numbers without its object): DO NOT make a frame and a face for each bay of every building. |
| `signatureFloorOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A signature building's ground floor (`BuildingChunk.signatureFloor`) without emitting it |
| `slabColour` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `slabColours` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A balcony slab's colour: the wall's own, a shade darker underneath the light (a slab of the building, not a white tray). |
| `slotOfBay` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `sphereOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `streakOf` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The streak `emitBay` puts under a bay's sill (`emitStreak`), as the far level's shader draws it: the components with a sill, on a wall that takes one, |
| `sub2` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A rectangle inside an element's plan, as fractions of the axis its `d` runs on: 0 at the edge it faces, 1 at its back. |
| `toByte` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A share 0..1 as a byte. |
| `trimColour` | a portar | render/buildings/facade_*, roof_*, lot_*, shell |  |
| `trimColours` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | The trim's colour as a three Color, one per paint. |
| `wearShort` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A wear as the shell's vertices carry it to the GPU: a byte's step (`cellArrays.ts` `finishWear`), so a shell's part laid again from it is the shell. |
| `withShellParts` | a portar | render/buildings/facade_*, roof_*, lot_*, shell | A chunk with its shell's parts laid in its shell again (`shellPartFaces`): what a ruin breaks (`destruction.ts`), and what the specs compare with the  |

### `src/render/buildings/cell.worker.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE BUILDINGS' CELLS IN THE JOB SYSTEM'S QUEUE (`cellPool.ts`): each job carries all it reads, the ground read off shared memory; one answer a job)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `serve` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE BUILDINGS' CELLS IN THE JOB SYSTEM'S QUEUE (`cellPool.ts`): each job carries all it reads, the ground read off shared memory; one answer a job |

### `src/render/buildings/cellArrays.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A BATCH OF BUILDINGS AS ARRAYS: what `buildingMesh.ts` `meshesOf` wraps in meshes)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BOUNDS_BOX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The box `boxOfInstances` widens: min x, y, z, max x, y, z. |
| `BOUNDS_MATRIX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One instance's matrix, written over by each instance `instanceBounds` reads. |
| `Bounds` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A box and a sphere: minX, minY, minZ, maxX, maxY, maxZ, then centre x, y, z and radius (three's `Box3` and `Sphere`). |
| `COLOURED_PARTS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Parts drawn with a colour per instance: awnings, the window frames and balcony railings in their building's trim (the materials are white), the shell' |
| `COLOUR_RANGE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The brightest linear colour a shell's paint holds: a light paint shaded up (1.1), the building's own shade (1.05) and the tone (1.03) stay under it. |
| `CellArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's buildings from afar as one mesh's arrays (`massCells.ts`) |
| `CellSelection` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a batch takes: the part kinds, whether the shells and the furniture; the preview's look (`ghost`) or the faded one (`dim`). |
| `FACADE_BYTES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Bytes per vertex of the far level's opening (a0, a1, h0, h1, shares of the bay) its frames' colour (r, g, b) and whether its rooms are offices (255) o |
| `FACADE_FLOATS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Half floats per vertex of the far level's window grid (`buildingMesh.ts` `FacadeStamp`): one bay's width and one storey's height, to a millimetre or t |
| `FACADE_GRID` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Shorts per vertex of the far level's window grid, each a whole number: bay and level (u, v, whole at a run's corners), kind + `FACADE_FLIP` where the  |
| `FINISH_ATTRIBUTE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The finish id of a shell's part per instance, and of a ruin's piece per vertex (`finishes.ts`): its place in `FINISHES`, its layer of the finishes' ma |
| `FragmentsOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A chunk's shell fragments as a piece reads them: the plain ones, or the far level's runs (with their window grid). |
| `HALF_ONE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | 1 in half float: an instance with no tint of its own. |
| `InstanceArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `KitBoxes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each part's and each piece of furniture's geometry box (minX, minY, minZ, maxX, maxY, maxZ), as the kit draws them. |
| `MASS_FACADE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mass vertex's numbers: its facade's full floats, its grid's half floats (`CellMass`): 44 bytes a vertex with its place, normal, colour and seed (52  |
| `MASS_GRID` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassSquare` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A square of cells a level's masses stand in (`levelKeys.ts`): its first column and row and its side in cells. |
| `NORMAL_SHORTS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Numbers a shell vertex carries in its normal (`normal`): two signed shorts, octahedral (`octNormal`). |
| `NORMAL_UNIT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The largest signed short: a normal's and a wear's channel over it (WebGL 2: a normalised short reads back as c / 32767). |
| `PAINT_ATTRIBUTE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The shell's vertex attribute of its paint: rgb over `COLOUR_RANGE` and the finish id in w, four unsigned shorts read normalised (`finishes.ts`). |
| `PAINT_UNIT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The largest unsigned short: a paint's channel and its finish id over it. |
| `PartBanding` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What filing a kind reads of its screen (`kit.ts` `PartScreen`), sent with each job (`kitBoxes`): its first coarser level's error, its box (the kit's ` |
| `PartBandings` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `SHELL_BOX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What `shellBounds`'s loops write: the box (min x, y, z, max x, y, z) and the farthest squared distance from its centre. |
| `SHELL_GRID_MARGIN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far past its cell a cell's buildings may reach on its grid, m. |
| `SHELL_GRID_STEP` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The grid's step: the cell and its margins in an unsigned short, 5.9 mm (a point within 2.9 mm, a hundredth of the far level's 0.3 m). |
| `SHELL_MESH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The name of a batch's one shell mesh (`buildingMesh.ts` `meshesOf`), as `fade.ts` and the memory counters find it. |
| `SHELL_NORMAL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The shell's vertex attribute of its normal, octahedral (`octNormal`): two shorts read normalised (`finishes.ts`). |
| `SHELL_PIECE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The most vertices a shell piece holds: every index of it fits 16 bits (meshoptimizer's advice: half the index memory and fetch), 65535 left out (WebGL |
| `SHELL_SHORTS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Numbers a shell vertex carries in its paint (`paint`): four shorts, rgb and its finish with its wear (`finishWear`). |
| `ShellArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A piece of a batch's shells of every finish, one geometry drawn in one call: 24 bytes a vertex, 44 on the far level's runs of bays with openings |
| `ShellGrid` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's grid its far level's shells are laid on (`cellShellGrid`): one origin and one step for every piece of the cell, so a corner two pieces share  |
| `WEAR_STEPS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A wear's steps in the paint's fourth short (`finishWear`): its high byte. |
| `boxOfInstances` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The box of every instance's corners, written into `out` (min x, y, z, max x, y, z) after each instance: nothing after the loop for its code to have ne |
| `boxOfPoints` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The box of the points, written into `out` after each one. |
| `cellArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The batch of `chunks` that `selection` takes, as arrays.  `groups`: each chunk's square (`cellWork.ts` gives the shells quadrants of two detail cells) |
| `cellBuffers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every array buffer of a batch: handed over, not copied (MDN, `postMessage` transfer). |
| `cellBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a batch sends to the GPU, in bytes: every array but the bounds (kept on the page), the kit's shared geometry not counted. |
| `cellMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The masses of a cell's buildings, or a tile's or a plate's (`massLevels.ts`), merged as one set of arrays, on `square`'s grid when given (`massGrid`); |
| `cellMassBuffers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every buffer of a cell's masses: handed over, not copied. |
| `cellShellGrid` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's grid, its height from the lowest of its shells. |
| `chunkVertices` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The vertices of a chunk's fragments, every finish. |
| `farthestFrom` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The farthest squared distance of the points from (cx, cy, cz), written into `out[6]` after each one. |
| `finishId` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A finish's id, as the shell's program reads it. |
| `finishWear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The paint's fourth short: the finish id in its low byte, the wear's byte over it (`finishes.ts` reads both back). |
| `furniturePoses` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The furniture of one kind of every chunk, its poses in chunk order. |
| `indexArray` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The index array a cell's mesh needs: 16 bits where its vertices fit (a cell of 240 m mostly does), else 32. |
| `instanceBounds` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | An instanced batch's box and sphere as three's `InstancedMesh.computeBoundingBox` works the box out  DO NOT end the loop over the instances with the b |
| `massGrid` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A square's grid for its masses (as `cellShellGrid` for a cell's shells): the square and its margins in an unsigned short, every axis alike, its height |
| `massOnGrid` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mass's places in floats put on its square's grid; the mass as it was where a point falls off it. |
| `massPlaces` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mass's places in floats, as the GPU reads them back (from its grid, if on one). |
| `mergedMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The masses merged, their places in floats. |
| `octDecode` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A normal back from its two shorts, as the shell's program reads it (`finishes.ts` `shellOctNormal`). |
| `octNormal` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A unit normal as two signed shorts, octahedral (Cigolle et al. 2014, "A Survey of Efficient Representations for Independent Unit Vectors"): 4 bytes, t |
| `offsetIndices` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A fragment's indices written into `out` at `at`, each `add` more (its vertices' place in the merged arrays): a loop of its own, DO NOT inline it in `s |
| `onGrid` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Pieces with their places on a grid: four unsigned shorts a vertex (the fourth left over: no 3-short format in Direct3D 11); null where a point falls o |
| `paintShort` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A linear colour channel as the shell's paint holds it: 16 bits over `COLOUR_RANGE`, a step of 3e-5 (a byte would band the dark paints). |
| `partArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A batch's own instance arrays from its chunks, in order: the matrices, the colours of a coloured kind |
| `shellArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The fragments of every finish of `chunks` in one set of arrays, finish by finish in `FINISHES` order (the chunks' arrays, already in the GPU's layout, |
| `shellBounds` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A shell's box and sphere as three's `BufferGeometry.computeBoundingBox` and `computeBoundingSphere` work them out  DO NOT work the box's centre and ra |
| `shellPieces` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every chunk's fragments in pieces, the chunks in order, a piece closed before a chunk would take it past `SHELL_PIECE` vertices (a chunk is never cut: |
| `snorm` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A number in [-1, 1] as a signed byte the GPU reads back normalised (x / 127, WebGL 2 `vertexAttribPointer`). |
| `snorm16` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A number in [-1, 1] as a signed short the GPU reads back normalised: the shell's normal and wear. |
| `wearByte` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A wear (0..1) as the shell carries it: a byte's step, `WEAR_STEPS`. |

### `src/render/buildings/cellPool.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: The cells' jobs kept from one session to the next, by this build's fingerprint of their code (`cook-plugin.ts` DERIVED).)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Asked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A job asked: what it is written from when a worker takes it (`write`), and where it stands. |
| `CELL_HASH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells' jobs kept from one session to the next, by this build's fingerprint of their code (`cook-plugin.ts` DERIVED). |
| `CELL_PREFIX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellEnv` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a job reads on the page: the ground as drawn and as it was, and the paving (with `pack`: read by the worker itself). |
| `CellPool` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.answer` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.asked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.base` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.byKey` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.cancel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A job no longer wanted: never sent if waiting, its answer dropped if begun (a worker runs a job through, one message). |
| `CellPool.coarse` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.coarseFirst` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.coarseFirst@171` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `workers`: false does every job on the page (tests). `tone`: each finish's colour from afar, for the masses (`kit.ts`).  `text`: where an entry's reco |
| `CellPool.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.failed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.floorKnown` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's floor measured on the same record, ground and paving; undefined when none was, or on other ground (its digest changed: a stamp under it,  |
| `CellPool.floors` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.forget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.forgetFloors` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT keep the floor of a building gone from the document (a tile let go as the eye walked on): the floors kept follow what is loaded, not the way wa |
| `CellPool.isRunning` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a job is still waiting or being worked on (one done on the page never is). |
| `CellPool.kitDigest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.kitSeen` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.learn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.nextId` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.offPage` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.onPage` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.run` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's job: its batches ('cell'), its far level ('facade'), its masses ('mass') or its buildings' summaries; `key`, what a newer job replaces  `hole |
| `CellPool.running` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.storedKey` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.submit` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.supersede` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A job a newer one for its cell replaces (`cancel`). |
| `CellPool.warm` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The job system's workers made now, ahead of the first job. |
| `CellPool.warned` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellPool.write` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `DETAIL_LATER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The bands the detail is put back by while the town is unseen (`coarseFirst`): past the masses (2-2.75). |
| `EDIT_TIER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | WHAT THE PLAYER JUST EDITED (a building cut open, a tile's near roads baked from the page's records): before anything - it is what the player looks at |
| `FAR_TIER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE FAR LEVELS' BAND (`farLevels.ts`, `farRoads.ts`): before every other kind - a far node the view wants and has not is a hole on the screen (nothing |
| `FLOOR_PAVED,FLOOR_NONE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Floor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's ground floor as a job measured it, and what it was measured on: its record, its ground (`CellEntry`) and whether the paving was read. |
| `FloorColumns` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE FLOORS MEASURED, IN COLUMNS BY BUILDING ID (`core/idSlots.ts`): a plate's job measures every building of 2 km round, and a `Map` entry and an obje |
| `FloorColumns.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `workers`: false does every job on the page (tests). `tone`: each finish's colour from afar, for the masses (`kit.ts`).  `text`: where an entry's reco |
| `FloorColumns.cut` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.digest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.flags` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.floor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.get` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT give the detail the queue first while the town is behind its curtain: the opening shows the masses and the far levels, the detail lands after ( |
| `FloorColumns.idOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.retain` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every floor whose key `keep` refuses let go. |
| `FloorColumns.set` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT leave the detail asked behind the curtain in its band once the town shows: the queue ranks a job by the band it was asked in, and the cell at t |
| `FloorColumns.sig` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FloorColumns.slots` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `KEPT_KINDS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The kinds kept: the masses and the far level, what the opening waits for (`layer.ts` coarsePending, viewPending). DO NOT make them again at every laun |
| `MOST_WIDER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Times a job is asked again over wider roads (`wider`) before the page puts it together. |
| `NEAR_TIER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The near products' band anchor (`nearTiles.ts` PRODUCT_TIER): after the buildings' masses (2-2.75). The roads' near level is no longer here: it goes i |
| `PAVING_REACH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far past its cell a job's buildings read the paving off the roads handed to its worker (`PavingData`) |
| `TIER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Which jobs go first, a band each, nearest the eye first within a band: the detail, its far level, the masses, the summaries; the base (the top level,  |
| `TIER_SPAN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A band: farther than any cell of any map is from the eye (the far levels' jobs take a band of their own, `farLevels.ts`). |
| `__CELL_WORK_HASH__` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `cellPriority` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell job's place in the queue (lower first): a building cut open before anything, else its kind's band (`TIER`, the detail put back past the coarse  |
| `forgotOthers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `keptCellKeys` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whole keys: `readDerivedKeys` gives them without their prefix. |
| `keptKeys` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The keys kept, read once at the start (a keys-only read) and added to as results are kept: a job not in it is asked at once. DO NOT read the store bef |
| `pavingRect` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The rectangle a job's worker reads the paving in: its square and `PAVING_REACH` round it. |
| `squareOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A job's square in `BATCH_CELL`s from its key: a cell ("x,y") or a tile or plate of masses ("t:i,j", "p:i,j", `massLevels.ts`); null when it is none. |

### `src/render/buildings/cellRequests.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE BUILDINGS' CELLS ASKED OF THE WORKERS AND LANDED (`layer.ts`))_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BatchKind` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's detail (L0) or its facade level (L1, `emitChunk` `far`). |
| `CellRequests` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.asked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.asking` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.clear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every request forgotten and every batch not taken disposed (a context restored, the layer gone). |
| `CellRequests.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `admit`: whether a level landed is still wanted within the GPU's room, its bytes measured (refused: disposed, never uploaded); `rank`: its upload's pi |
| `CellRequests.drop` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell no longer wanted (drawn far before it landed): its request cancelled, its batches if landed disposed. |
| `CellRequests.dropMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's masses no longer wanted (it has no building left). |
| `CellRequests.holeRoom` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether another hole may be asked past the limits (a full upload queue, `layer.ts` STREAM_AHEAD): fewer than `MOST_HOLES` asked or on their way to the |
| `CellRequests.holes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.holesInFlight` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.isAsked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell has been asked for and not landed yet. |
| `CellRequests.land` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.landed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.mass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's masses from afar made of `entries` (`cellWork.ts` 'mass'), handed to `done` when they come |
| `CellRequests.masses` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.massing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.summaries` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.summarize` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the page keeps of these far buildings (their flags), with no batch made. |
| `CellRequests.take` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's batches taken to be shown: the layer's to dispose from now on. |
| `CellRequests.waiting` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellRequests.want` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `cell`'s batches made of `entries`, once landed and (with `upload`) on the GPU; null while they are made |
| `LandedCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's batches as landed: its shells, panes and furniture, and its parts by sub-cell (`partCells.ts`). |
| `Landing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MOST_HOLES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The most holes (`want` with `hole`) asked past a full upload queue at once, until their meshes are on the GPU: what that queue grows by at a reopening |
| `disposeLanded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT dispose first: a mesh still queued was drawn after and its geometry went back to the GPU with no owner. |
| `meshesIn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every mesh of a landed cell, but its shells: those go into the multi-draw batch when the cell is shown, and their arrays are still  read there (a warm |

### `src/render/buildings/cellWork.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: DO NOT send the page a chunk per building)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `COARSE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellJob` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellReads` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a job reads: the ground (shared, or the page's own), and the paving, undefined where it cannot be read here. |
| `CellResult` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ChunkSummary` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the page keeps of a building emitted: its flags, how its lots' ground is made again, its signature floor. |
| `DETAIL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `JobKind` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a job makes: a cell's detail (its coarse batch and its detail cells'), the same at the far level (`facade`: L1, `emitChunk` `far`), a cell's buil |
| `MASS_DETAIL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What each kind of masses leaves out (`massing.ts` `MassDetail`). |
| `WorkReply` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `replyBuffers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every buffer of a reply: handed over, not copied (MDN, `postMessage` transfer). |
| `runCellJob` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT ask the page for the ground in rounds: a cell's job reads every height itself (`sharedGround.ts`), in one pass, nothing kept once it is done |
| `runCellJob.missed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether anything was read past what the job was handed: the paving's misses, and the ground's put with them. |
| `runCellJob.summaryOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the page keeps of a building, from its chunk or its summary alone. |
| `wider` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The roads and the ground to hand the job again: those it was handed and every point it read past them, a metre round. |
| `workerReads` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A job's reads in a worker: the shared ground, the paving off the roads it was handed (reading the shared natural ground where they need it). |
| `workerReads.groundMissed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |

### `src/render/buildings/chunk.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A part instanced per kind for the whole near ring (`kit.ts` PART_KINDS): declared with the cell's data, which the kit's materials do not need.)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingChunk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellEntry` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building of a cell as the cell workers are sent it (`cellWork.ts`): its chunk's key (an id, or a building cut open), its record as text and the grou |
| `CellJobEntry` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | An entry as a job is sent it: where its worker reads the record (its tile's shared table and row, `recordTable.ts`), never the record. |
| `LotGround` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LotGroundRecipe` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a lot ground is made from besides the record: the level its lots meet and whether they were laid (`emitLots`). |
| `PackedEntries` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's buildings as its masses are asked (`layer.ts` `askMass`, `massLevels.ts`), in typed arrays: each one's id and its record's digest without its |
| `PartBatch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One part batch of one building: its instances' poses (`partPose.ts`, `POSE_FLOATS` each) and colours. |
| `PartKind` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A part instanced per kind for the whole near ring (`kit.ts` PART_KINDS): declared with the cell's data, which the kit's materials do not need. |
| `RunChunk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The far level's runs of bays with openings (`emitChunk` `far`): a shell fragment whose every vertex carries its window grid (`facadeLevel.ts`). |
| `ShellChunk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Everything one building contributes, in typed arrays: a shell fragment with local indices and its instance batches.  A shell's vertices are written in |
| `sameEntries` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether two lists are the same buildings with the same records on the same ground (the strings compared by reference). |
| `samePacked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `unpack` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The entries of packed ones, appended to `out` (made when a job is asked, let go with it). |

### `src/render/buildings/facadeKit.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_props, roof_plant, roof_terrace

**Para o jogador:** Ar-condicionado, antenas, vasos, varais, equipamentos de cobertura. _(no 4.0: The facade kit: architecture built as geometry)_

**Motivo:** Sem prova por símbolo até agora. 

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BalconyKind` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `CurtainSpec` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `Face` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A wall of a rectangular mass `[x0, x1] x [y0, y1]`. |
| `Head` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `SIDES` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `Side` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `Sink` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | What the kit draws into (`signature.ts` `Parts`). |
| `WALL` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Wall depth: how far the glass and the inner mass stand behind the face. |
| `WindowSpec` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `archFill` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The wall above an arched head: the rectangle over the opening with the round head cut out of it. |
| `archRing` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A stone ring round an arched head (an archivolt), a little proud of the wall. |
| `archedDormer` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A dormer through a roof slope on face `f` |
| `awning` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A shop awning: a sloped fabric hood out from the wall over an opening. |
| `balcony` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A balcony on a face: a slab out from the wall, its railing (glass panels, iron bars, a solid parapet). |
| `balustrade` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A balustrade along a face: a base, a rail, pedestals every `every` m, balusters between. |
| `band` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A plain band (a string course, a slab edge) across a face. |
| `bellMansard` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A curved (bell) mansard over a rectangle |
| `cartouche` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A carved panel (a cartouche) on a face, in relief |
| `cartouche.boss` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `consoles` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Consoles (modillions) under a cornice, every `every` m. |
| `cornice` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A cornice: a stepped profile of `steps` courses growing out to `depth` |
| `curtain` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A curtain wall over a face: glass, mullions (fins), transoms at the slabs, spandrel panels. |
| `door` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A door bay: a glazed double door |
| `extrudeOnFace` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A flat piece on a face made from a 2D outline in the face's own plane |
| `faceOf` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facesOf` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The four faces of a mass. |
| `fbox` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A box on a face: `u0..u1` along it, `o0..o1` out of it, `z0..z1` up. |
| `finial` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A finial: a pedestal, a ball and a point (on a mansard's corners, a parapet's posts). |
| `fpoint` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A point on a face, in the building's frame. |
| `hedge` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `lampPost` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A black street lamp with a lantern. |
| `pediment` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A triangular pediment on a face: a gable `h` high over `u0..u1`, `depth` out. |
| `pier` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A pier or pilaster standing proud of a face. |
| `plantedTree` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A square planter on the ground with a tree, or a hedge in a long trough. |
| `punched` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A punched window in one bay of one storey: the wall round the opening |
| `railFace` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A railing along a face, `o` out of it. |
| `repeat` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A floating split with repeats (CGA `split(x){ ~w : Bay }*`) |
| `span` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The length of a face along `u`. |
| `spire` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A spire: a tapering mast of stacked prisms ending in a needle (an Art Deco crown's top). |
| `turn` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The angle (radians about three's up axis) that turns a part made facing -y (the front) to face `f`. |

### `src/render/buildings/facadeLevel.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: THE BUILDINGS' FAR LEVEL (L1): the same building emitted again with its walls as runs of bays (`emitChunk` `far`))_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FACADE_FLIP` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | Added to a vertex's kind where its run's bays go against its face's own horizontal (`shellUv`'s t): the shader turns the derived direction round. |
| `FACADE_KIND` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | How the far level's shader fills a bay's opening (`buildingMesh.ts` `facadeKind` writes the same numbers), after `emitBay`'s panes |
| `FACADE_STREAK` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | Its run's rain streak times this added to a vertex's kind (`buildingMesh.ts` `streakOf`): the shader darkens the wall under each sill as `emitStreak`  |
| `FRAME` | portado | `project/render/shaders/building_pane.gdshaderinc` → `FAR_FRAME_SHARE` | How wide a window's frame reads round its pane, m: the frame part's border (`kit.ts`), with the reveal beside it. |
| `FacadePanes` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | The detail's materials whose look the far level's openings take (`kit.ts`): the panes, the doors, the shutters, the frames. |
| `STREAK_SILL` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | How far under the opening a sill's streak starts, by the run's streak: the windows' sill, the shuttered window's deeper one (`emitBay`). |
| `createFacadeMaterial` | portado | `project/render/shaders/building_pane.gdshaderinc` → `void far_window(` | Made once with the kit: the far level's material, the shell's own (`finishes.ts`: every finish's maps, weathering, lots), its openings drawn from the  |
| `createFacadeMaterial.glass` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms |  |
| `createFacadeMaterial.solid` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms |  |
| `facadeColour` | portado | `project/render/shaders/building_pane.gdshaderinc` → `void far_window(` | GLSL after `color_fragment`: the bay's opening, its frame and its pane or door, by the share of the pixel each covers |
| `facadeFragmentPars` | portado | `project/render/shaders/building_pane.gdshaderinc` → `void far_window(` | GLSL: the far level's window grid and its pick of each bay's pane, as `emitBay` picks it (`bayHash`) |
| `num` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms |  |
| `vec3Of` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms |  |

### `src/render/buildings/facadePropModels.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_props, roof_plant, roof_terrace

**Para o jogador:** Ar-condicionado, antenas, vasos, varais, equipamentos de cobertura. _(no 4.0: The models of the props on the facades and balconies (`facadeProps.ts` places them))_

**Motivo:** Sem prova por símbolo até agora. 

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `AC_FRONT_GLSL` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The condenser's front as its material draws it, on the case's face (+z at d / 2, model space): the fan's dark hole, the grille's three  rings and two  |
| `AC_FRONT_Z` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The case's front face: its model-space front (z) within this of `AC.d / 2`, facing +z. |
| `AC_LID` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The lid's height over the case (it was a slab of its own). |
| `acFarGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A condenser from afar (`kit.ts` `far`): its case and arms; the fan and louvres drawn on the front by the material, as near. |
| `acUnitGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A split condenser, centred on its case: the case (its top 2 cm the lid, drawn by the material), two arms, two braces down to two plates on  the wall,  |
| `airerGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A floor-standing clothes airer (the varal de chao), 1.2 m along X |
| `airerGeometry.cloth` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The washing, over the lines: each a thin cloth hanging both sides. |
| `bikeGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A bicycle on its stand, along X, from the floor up: two wheels |
| `bikeGeometry.bar` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The frame: from the rear hub to the crank, up to the saddle, across to the head and down to the front hub. |
| `block` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `bushGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A round potted shrub (a boxwood, a fern, a geranium) |
| `chairGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A balcony chair from the floor up, facing +Z: seat, a slanted back and four legs. |
| `createFacadePropGeometries` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Every prop's model, made once (`kit.ts`). |
| `dishGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A satellite dish on its wall arm, the arm's foot at the origin on the wall: the dish 0.6 m across, facing up and out. |
| `foliageGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A potted plant's leaves from the soil (y 0) up: a rosette of leaves arching out and down |
| `grey` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `leafStream` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A small seeded stream for the plant's leaves: the same model every load. |
| `loungerGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A sun lounger (espreguicadeira) along X, from the floor up: its bed, the raised back and four short legs; 1.9 m long. |
| `merged` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Parts merged into one model, the faces another part covers taken out first (`mesh/hiddenFaces.ts`); `floor`: it stands on y = 0, what lies on it facin |
| `potGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A plant pot from the floor (y 0) up: a tapered body, its rim and the soil in it. 0.34 m across, 0.32 m high (`POT_HIGH`). |
| `tableGeometry` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A small round table from the floor up: top, a post and its base. |
| `tone` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A part of a model: a geometry and its tone (multiplied by the instance's colour).  DO NOT `toNonIndexed` a part: every instance of the prop drew each  |

### `src/render/buildings/facadeProps.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_props, roof_plant, roof_terrace

**Para o jogador:** Ar-condicionado, antenas, vasos, varais, equipamentos de cobertura. _(no 4.0: WHAT HANGS ON A FACADE AND STANDS ON ITS BALCONIES: the life of a Brazilian block of flats)_

**Motivo:** Sem prova por símbolo até agora. 

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `AC` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The condenser's case: width, height, depth (m), and its clearance off the wall. |
| `AC_CASES` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A condenser's cases: white, off-white, beige, grey, and the yellowed white of an old one. |
| `BIKES` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `FACADE_PROP_KINDS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `FINE` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `FURNISHED` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The components a prop is hung on or stood on, above the ground floor. |
| `FacadeProp` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A prop placed on a bay: along it (`a`), up from its storey's floor (`h`, the model's origin), out of the wall (`out`), turned (`turn`, radians about u |
| `FacadePropKind` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `GREENS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `LOUNGERS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `NO_PROPS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `PLASTIC` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A plain block's plastic chairs (white, green, blue, red) and a better one's (wood, black, natural fibre). |
| `POTS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Pots: terracotta, cement, white, black, a glazed blue, an ochre. |
| `POT_HIGH` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A plant pot's height, m (`facadePropModels.ts`). |
| `PropOpening` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The bay's opening, in its own frame: `a0`..`a1` along, `h0`..`h1` up, `depth` into the wall. |
| `SHADOWED_PROPS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The props whose shadow is worth its pass: the condensers, the pots and the plants. |
| `Standard` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Which flats a balcony belongs to: a block's varandas gourmet are its better flats; a plain block's balconies |
| `THIN_PROPS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The props of thin bars, pre-filtered as the frames are (`thinParts.ts`): bar width, m, and the tone behind. |
| `facadeProps` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The props of one bay; DO NOT make its pickers for a bay that takes none (every wall, door and ground-floor bay of the town, in every emission). |
| `facadeProps.acCase` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.furnish` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A balcony's floor from `a0` to `a1` along, out to `deep`, `gourmet` a varanda gourmet. |
| `facadeProps.furnish.airer` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.furnish.chair` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.furnish.corner` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.furnish.table` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.one` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A bicycle along the wall. |
| `facadeProps.pick` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `facadeProps.plant` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A pot of 30 to 55 cm and its plant: a rosette (a lily, a spider plant) or a round shrub. |
| `facadeProps.rnd` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `standardOf` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |

### `src/render/buildings/fade.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/part_models, facade_parts, building_parts.gdshader

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas. _(no 4.0: DO NOT emit the town again to fade it)_

**Motivo:** Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FURNITURE` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | DO NOT emit the town again to fade it |
| `fadeMeshes` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `partLook` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | How a part kind is drawn faded ("Ocultar outros") or not: its material, its order and whether it is drawn at all (a lantern's light is not). |

### `src/render/buildings/farParts.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE FACADE LEVEL'S DETAIL PARTS, ONE DRAW A KIND FOR THE WHOLE TOWN)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FarPartBatch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.add` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.boxes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.castShadow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The far cascades take the buildings by their masses alone: the batch casts nothing (`layer.ts` `castInto`). |
| `FarPartBatch.casting` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.cells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.clear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every instance let go (a lost context's cells asked again): the buffers kept, nothing drawn until cells land. |
| `FarPartBatch.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.drawn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.faded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.group` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.grow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.kinds` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.remove` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.setFaded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Its instances drawn at the coarsest level the far view was made for. |
| `FarPartBatch.sync` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells of `cells` the view draws at their facade level (`shown`) are in the batch: those that came are copied in, those gone taken out. |
| `FarPartBatch.tellBoxes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarPartBatch.trianglesOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The triangles the batch draws of a cell, for the view's budget. |
| `FarPartBatch.view` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each kind drawn only while its pieces span a pixel where the nearest cell stands: a balcony a kilometre off is under one, and only its cost is left (t |
| `KindBatch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One kind over every facade cell: its master buffers, its mesh, and each cell's run. |
| `Run` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's instances of one kind in the batch: a contiguous run of the kind's buffers. |
| `dirty` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The run's poses and colours sent to the GPU, its range only. DO NOT clear the ranges a change already added: every cell added or taken out this frame  |
| `reachPoint` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |

### `src/render/buildings/finishes.ts`

**Estado:** a portar · **Dono no jogo:** building_common.gdshaderinc, assets/surfaces/building-finishes.*

**Para o jogador:** Materiais das paredes (reboco, tijolo, pedra, metal) e o intemperismo. _(no 4.0: The building finishes ("Materials"): their recipes in `bake/finishRecipes.ts`, baked at build time as one texture array per map, a layer per finish)_

**Motivo:** Sem prova por símbolo até agora. Camadas cozidas e finish_layer/weather já no jogo.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DIELECTRIC_F0` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | The base reflectance three gives a dielectric (r186 `lights_physical_fragment`: specularColor 0.04 at ior 1.5). |
| `FAR_FINISH` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL after the map, the far program's (`createShellMaterial` far): its finish's colour from afar for its texture (`finishAlbedo`), the wear's mean for |
| `FINISH_PARS` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL: the pixel's finish rows, and the sky's share it reflects (read by three's IBL in place of the material's one `envMapIntensity`). |
| `FarLook` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | Each finish from afar, its texture's texels summed as the light treats them: - in linear, as the shader reads the sRGB map; DO NOT average the encoded |
| `LOOK_ROWS` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | Four-number rows a finish takes in the look table: (uv scale, normal scale, metalness, envMapIntensity), then (metal read from the map, roof, glassy,  |
| `MAP_PARS` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL: the finishes' maps as texture arrays, and the tangent frame three builds for a normal map without tangents (r186 `normalmap_pars_fragment`; C. S |
| `SHELL_NORMAL_GLSL` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL: a shell vertex's normal, octahedral in two normalised shorts (\`octNormal\`, Cigolle et al. 2014), unfolded. |
| `WEATHERED_MEAN` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | How dark the light weathering of every wall leaves it on average (`WEATHERING`, `d` of 0.3: 0.3 x (0.18 + 0.12 x 0.75 + a few streaks)). |
| `WEATHERING` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL after the map: buildings that are not maintained run down (the player's order of 2026-10-05), each finish as its row says (roof, glassy) |
| `WEATHER_PARS` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | GLSL: the weathering's noise. |
| `createFarShellMaterial` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | THE SHELL'S PARTS FROM AFAR (a lot's boxes, rails and slabs, `partCells.ts` LIGHT_PIXELS): each instance its paint times its finish's colour from afar |
| `createShellMaterial` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | THE ONE SHELL MATERIAL: every finish of a cell's shells in one draw, textured where the build's baked surfaces are loaded (`surfaceBakeClient.ts`; non |
| `farLooks` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |
| `farTones` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | Each finish's colour from afar, in finish order (the far programs' table). |
| `finishAlbedo` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | Each finish's colour as it reads from afar, three linear numbers to multiply a wall's linear tint by (`massing.ts` `rgbOf`): its texture's mean light  |
| `finishFar` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* |  |
| `finishLookTable` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | Each finish's scalars, indexed by its id (its place in `FINISHES`, its layer in the arrays); `baked`: its maps are loaded. |
| `linearOf` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | An sRGB-encoded value as linear (IEC 61966-2-1): the finish map is a colour texture, decoded so before it is lit. |
| `shellShader` | a portar | building_common.gdshaderinc, assets/surfaces/building-finishes.* | The shell's program: each pixel's finish picks its layer of the maps and its rows of the look table; `weathered` adds the wear. A shell's vertex carri |

### `src/render/buildings/flagLayer.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/flags + props.gdshader

**Para o jogador:** Bandeiras em mastros de prédios públicos, ao vento. _(no 4.0: Every flag in the city, one instanced draw: a cloth that waves in the wind.)_

**Motivo:** Sem prova por símbolo até agora. Item próprio do render.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FlagInstance` | a portar | render/buildings/flags + props.gdshader | One flag, in three's frame (y up): the top of the hoist, and its size. |
| `FlagLayer` | a portar | render/buildings/flags + props.gdshader |  |
| `GLSL_PATTERN` | a portar | render/buildings/flags + props.gdshader |  |
| `createFlagLayer` | a portar | render/buildings/flags + props.gdshader |  |
| `createFlagLayer.dispose` | a portar | render/buildings/flags + props.gdshader |  |
| `createFlagLayer.set` | a portar | render/buildings/flags + props.gdshader |  |
| `material` | a portar | render/buildings/flags + props.gdshader |  |

### `src/render/buildings/fracture.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência. _(no 4.0: Pre-fracturing a building's own meshes)_

**Motivo:** Funcionalidade demolition da fila.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BROKEN` | outro porte | ports/demolition | The grey of broken masonry, on the cut faces. |
| `FRAGMENT` | outro porte | ports/demolition | Edge of a Voronoi cell: the size of a fragment. |
| `FractureInput` | outro porte | ports/demolition | The fragments of one building, from its drawn chunk.  What a fracture needs, as plain arrays (so it can run in a worker, `fracture.worker.ts`). |
| `Fragment` | outro porte | ports/demolition | What a ruin knows of each fragment besides its vertices (which are in the ruin's buffer, `StillData`). |
| `FragmentData` | outro porte | ports/demolition | A fragment as arrays, vertices round its centre: gathered into its ruin's buffer (`mergeFragments`). |
| `MAX_EDGE` | outro porte | ports/demolition | Longest triangle edge after subdivision. |
| `MODEL_ARRAYS` | outro porte | ports/demolition | A kit model's vertices as plain arrays, made once per model. |
| `STRIDE` | outro porte | ports/demolition |  |
| `StillData` | outro porte | ports/demolition | Every fragment of a building in one buffer, in world space (three's, y up), one draw group per material |
| `Tri` | outro porte | ports/demolition |  |
| `WALL` | outro porte | ports/demolition | Thickness given to a wall fragment. |
| `fractureData` | outro porte | ports/demolition | The fracture itself, pure arrays in and out: run in a worker. |
| `fractureData.cellOf` | outro porte | ports/demolition |  |
| `fractureData.centroid` | outro porte | ports/demolition |  |
| `fractureData.push` | outro porte | ports/demolition |  |
| `fractureData.rand` | outro porte | ports/demolition |  |
| `fractureData.seedOf` | outro porte | ports/demolition |  |
| `fractureData.vkey` | outro porte | ports/demolition |  |
| `fragmentGeometry` | outro porte | ports/demolition | A fragment's own geometry, round its centre, from its ranges in the ruin's buffer as they stand |
| `lerp` | outro porte | ports/demolition |  |
| `mergeFragments` | outro porte | ports/demolition |  |
| `modelArrays` | outro porte | ports/demolition |  |
| `prepareFracture` | outro porte | ports/demolition | The arrays of a building's chunk and kit a fracture reads, and the materials its fragments are drawn with. |
| `prepareFracture.materialIndex` | outro porte | ports/demolition |  |
| `prepareFracture.model` | outro porte | ports/demolition |  |
| `subdivide` | outro porte | ports/demolition | Splits a triangle along its longest edge until every edge is under `max`. |

### `src/render/buildings/fracture.worker.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência. _(no 4.0: This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.)_

**Motivo:** Funcionalidade demolition da fila.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `serve` | outro porte | ports/demolition | This service of the job system (`job.worker.ts`): its messages answered on the scope it is given. |

### `src/render/buildings/frameBudget.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE ERROR SCALE THAT FITS THE FRAME: the levels are chosen by their error on the screen (`massCells.ts` `measureSeen`, 2 px at scale 1); when what they add up to passes the view's budget of triangles,)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ERROR_STEP` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each step of the scale: a quarter coarser or finer. |
| `FrameScale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE SCALE OF A FRAME (`layer.ts` `levelOfDetail`): the most triangles the levels may add up to (`budget`, the view's, `core/perfBudget.ts`) and the sc |
| `FrameScale.budget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FrameScale.fit` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The levels chosen at the scale that fits the budget: `count` chooses at a scale and says their triangles. |
| `FrameScale.scale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | WHAT A LEVEL DRAWS OR HOLDS, reckoned before it lands (`layer.ts` `chosenTriangles`, `capNow`): as it measured when it last landed, else the mean of i |
| `LandedMeasure.changed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `LandedMeasure.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.forget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell gone from the town (its tile left the active region): its measures let go, out of the means too (DO NOT keep every cell the camera ever passed: |
| `LandedMeasure.landed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.of` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.othersOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.ratios` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.set` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.size` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.sums` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.unweigh` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LandedMeasure.weightOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a cell weighs whatever its level (`layer.ts`: its masses' triangles, always at hand), 0 when unknown: a level never landed, or let go, is reckone |
| `LandedMeasure.weights` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MOST_ERROR_SCALE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The coarsest scale: 64 px of error, every cell at its masses or its plate (DO NOT climb for ever: a count that never fits, a view of nothing, would sp |
| `SLACK` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Under this share of the budget the scale may come down a step (a gap under the budget, so a step down does not step up again next frame). |
| `TYPICAL_LEVEL_TRIANGLES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a cell's level is reckoned to draw before any of its kind landed: a dense cell's detail and facade level with their parts, as measured by the pre |
| `fitErrorScale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The scale that fits `budget`: `count(scale)` chooses the levels at that scale and says their triangles; `floor`, the least scale the frame's time allo |
| `letBackFitting` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE CELLS HELD BACK THAT FIT AGAIN (`layer.ts` capTriangles): each a level back, the ones spanning most of the screen first (`rank`, least first), whi |
| `letGoPassed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE CELLS HELD BACK THAT THE EYE CAME NEARER TO THAN A CELL KEPT (`layer.ts` capTriangles): let go. The budget coarsens the farthest first; a cell hel |

### `src/render/buildings/furnitureKit.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/furniture

**Para o jogador:** Modelos dos móveis no corte, miniaturas das peças. _(no 4.0: The furniture people use inside buildings, as models)_

**Motivo:** Sem prova por símbolo até agora. Depende do editor.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BLACK` | a portar | render/buildings/furniture |  |
| `BOARD` | a portar | render/buildings/furniture |  |
| `BOOKS` | a portar | render/buildings/furniture |  |
| `BULB` | a portar | render/buildings/furniture |  |
| `CERAMIC` | a portar | render/buildings/furniture |  |
| `CHROME` | a portar | render/buildings/furniture |  |
| `CUSHION` | a portar | render/buildings/furniture |  |
| `DUVET` | a portar | render/buildings/furniture |  |
| `FABRIC` | a portar | render/buildings/furniture |  |
| `GOODS` | a portar | render/buildings/furniture |  |
| `LEAF` | a portar | render/buildings/furniture |  |
| `LEAF_DARK` | a portar | render/buildings/furniture |  |
| `LINEN` | a portar | render/buildings/furniture |  |
| `OAK` | a portar | render/buildings/furniture |  |
| `POT` | a portar | render/buildings/furniture |  |
| `RED` | a portar | render/buildings/furniture |  |
| `RGB` | a portar | render/buildings/furniture |  |
| `ROUND_FRAGMENT` | a portar | render/buildings/furniture |  |
| `ROUND_FRAGMENT_MAIN` | a portar | render/buildings/furniture |  |
| `ROUND_P,ROUND_B,ROUND_X,ROUND_Y` | a portar | render/buildings/furniture |  |
| `ROUND_VERTEX` | a portar | render/buildings/furniture | The rounding of a box's edges in its shading: the normal of the rounded box at the point of the plain one, the gradient of its distance  outside its i |
| `ROUND_VERTEX_MAIN` | a portar | render/buildings/furniture |  |
| `SCREEN` | a portar | render/buildings/furniture |  |
| `SHADE` | a portar | render/buildings/furniture |  |
| `STEEL` | a portar | render/buildings/furniture |  |
| `WALNUT` | a portar | render/buildings/furniture |  |
| `WHITE` | a portar | render/buildings/furniture |  |
| `ball` | a portar | render/buildings/furniture |  |
| `box` | a portar | render/buildings/furniture | A box with sharp edges, its bottom at `y`: wood, steel, books, treads, boards (no rounding to shade: their edges are sharp). |
| `build` | a portar | render/buildings/furniture |  |
| `createFurnitureGeometries` | a portar | render/buildings/furniture | One geometry per furniture kind, merged, vertex-coloured. |
| `createFurnitureMaterial` | a portar | render/buildings/furniture |  |
| `cyl` | a portar | render/buildings/furniture | An upright cylinder, its bottom at `y`. |
| `furnitureParts` | a portar | render/buildings/furniture | What one furniture kind is built from, part by part, before its hidden faces are dropped (`scripts/model-sheet.mjs`): the primitive, its parameters, i |
| `furnitureParts.channel` | a portar | render/buildings/furniture |  |
| `furnitureTones` | a portar | render/buildings/furniture | Each kind's colour as the eye takes it in from across a room |
| `hex` | a portar | render/buildings/furniture |  |
| `legs` | a portar | render/buildings/furniture | Four legs under a top `w` x `d`, inset `inset`, `h` high. |
| `paint` | a portar | render/buildings/furniture | Paints a geometry one colour (as a vertex attribute) and moves it into place: kept indexed (DO NOT `toNonIndexed` it: every instance behind every wind |
| `rbox` | a portar | render/buildings/furniture | A box with rounded edges, its bottom at `y`: a plain box (12 triangles), its edges rounded in the shading (`createFurnitureMaterial`).  DO NOT model t |
| `turnY` | a portar | render/buildings/furniture | A part turned about the up axis, its rounding's axes turned with it (three turns only its positions and normals). |

### `src/render/buildings/glassRoom.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: The kit's window frame (\`kit.ts\` frameGeometry, its material): its bars in the unit square (border \`t\`, a mullion and a transom 0.8 t wide,)_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FRAME_OUT` | portado | `project/render/shaders/building_pane.gdshaderinc` → `FRAME_OUT_M` | How far in front of its pane a frame's middle stands (\`buildingMesh.ts\` \`glazed\`: the frame put 3 cm out of the glass). |
| `FRAME_T` | portado | `project/render/shaders/building_pane.gdshaderinc` → `FRAME_SHARE` | The kit's window frame (\`kit.ts\` frameGeometry, its material): its bars in the unit square (border \`t\`, a mullion and a transom 0.8 t wide,  the t |
| `PANE_FRAME_GLSL` | portado | `project/render/shaders/building_pane.gdshaderinc` → `float pane_frame(` | GLSL: the share of the pixel a pane's frame covers and the normal of the face of it seen (view space), drawn on the pane itself.  Near, each bar is an |
| `PANE_FRAME_LOOK` | não se aplica | `project/render/shaders/building_pane.gdshaderinc` → `mix(0.0, 0.05, frame)` — valores escritos direto no shader; lin() era conversão de hex do JS |  |
| `PANE_RECESS_GLSL` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | GLSL, first in the pane's colour: a pane laid on the wall's face (`buildingMesh.ts` glazedRecessed, its glass `vRoomE.x` behind it). The  eye's ray in |
| `PANE_RECESS_LIGHT_GLSL` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | GLSL, after the frame: the light at the point met in a recess (`PANE_RECESS_GLSL`), on a side, the glass or the frame's face.  The sun reaches it wher |
| `REVEAL_LOOK` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | How an opening's reveals are drawn (`PANE_RECESS_GLSL`), as the shell's trim finishes are (`bake/finishRecipes.ts`: render, stone and  concrete): thei |
| `roomBehindGlass` | portado | `project/render/shaders/building_pane.gdshaderinc` → `void pane(vec2 uv` | The window panes' material: the room behind each pane drawn by interior mapping |
| `roomBehindGlass.lin` | não se aplica | `project/render/shaders/building_pane.gdshaderinc` → `room_floor(` — valores escritos direto no shader; lin() era conversão de hex do JS |  |

### `src/render/buildings/gpuBatchedMesh.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL. _(no 4.0: A BATCHED MESH WHOSE BUFFERS LIVE ON THE GPU ONLY)_

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GpuBatchedMesh` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh._initializeGeometry` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.buffer` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.colourHidden` | não se aplica | core, JobPool, RenderWorld | Instances left out of the draws seen from one camera alone (`render/occlusion.ts`: behind the walls standing before the eye). The  shadow passes draw  |
| `GpuBatchedMesh.compact` | não se aplica | core, JobPool, RenderWorld | The live geometries packed to the front of storage of `maxVertexCount` vertices and `maxIndexCount` indices, the holes deleted geometries left gone (t |
| `GpuBatchedMesh.compact.flush` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.constructor` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.copy` | não se aplica | core, JobPool, RenderWorld | What is stored copied on the GPU: up to the next free start (every geometry, live or deleted, lies before it). |
| `GpuBatchedMesh.deleteGeometry` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.dispose` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.flipped` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.gl` | não se aplica | core, JobPool, RenderWorld | STATIC_DRAW, as three's own attributes: written when a cell comes, drawn every frame. DO NOT hint DYNAMIC_DRAW: ANGLE takes it to its streaming buffer |
| `GpuBatchedMesh.inner` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.layouts` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.letGo` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.localIndex` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.made` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.onBeforeRender` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.onBeforeRender.list` | não se aplica | core, JobPool, RenderWorld | three passes no scene when it lists the draws for a shadow (r186 `BatchedMesh.onBeforeShadow`). |
| `GpuBatchedMesh.raycast` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.setGeometryAt` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.setGeometrySize` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.setGeometrySize.copy` | não se aplica | core, JobPool, RenderWorld | What is stored copied on the GPU: up to the next free start (every geometry, live or deleted, lies before it). |
| `GpuBatchedMesh.sharing` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.storage` | não se aplica | core, JobPool, RenderWorld |  |
| `GpuBatchedMesh.usedCounts` | não se aplica | core, JobPool, RenderWorld | The vertices and indices the live geometries reserve: what the batch holds once its holes are packed (`compact`). three counts its  room from the end  |
| `INDEX_BYTES` | não se aplica | core, JobPool, RenderWorld | Every index 32-bit: a batch grown past 65,535 vertices keeps its indices as they are (three's 16-bit start would need them converted, read back from t |
| `Inner` | não se aplica | core, JobPool, RenderWorld | BatchedMesh's own state this class reads and keeps (three r186 `BatchedMesh`): not in its typings. |
| `Layout` | não se aplica | core, JobPool, RenderWorld | One attribute's storage: as the first geometry gave it. |
| `TypedArray` | não se aplica | core, JobPool, RenderWorld |  |
| `TypedArrayOf` | não se aplica | core, JobPool, RenderWorld |  |
| `context` | não se aplica | core, JobPool, RenderWorld | The context the batches write with, given by the renderer (`renderer.ts`); none in tests, where a batch keeps three's arrays. |
| `glTypeOf` | não se aplica | core, JobPool, RenderWorld |  |
| `gpuBatchContext` | não se aplica | core, JobPool, RenderWorld |  |
| `normalizer` | não se aplica | core, JobPool, RenderWorld | What a normalized value of 1 is stored as in `of` (three's MathUtils.normalize); a float array stores it as is. |

### `src/render/buildings/kit.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/part_models, facade_parts, building_parts.gdshader

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas. _(no 4.0: Share of people awake, by hour from midnight: 100 % less the share asleep in the American Time Use Survey 2007-11)_

**Motivo:** Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ALL_NIGHT` | portado | `project/data/game/daylight.json` → `outside_all_night` | Outside lights after dark. 43 % of homes leave one on all night |
| `AWAKE_BY_HOUR` | portado | `project/data/game/daylight.json` → `awake_by_hour` | Share of people awake, by hour from midnight: 100 % less the share asleep in the American Time Use Survey 2007-11 |
| `Axes` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Three lengths along a part model's own axes (x along the wall, y up, z out of it): metres once scaled by an instance's matrix. |
| `BOX_FACES` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A box's faces in three's order (`BoxGeometry`: +x, -x, +y, -y, +z, -z), two triangles each. |
| `BRISE_BLADE` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A blade's thickness across the unit width. |
| `BRISE_FINS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A bay's rack of sun fins: six vertical blades across the unit width, the storey's height, standing out of the wall |
| `BuildingKit` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `FEWER` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `FRAME_BAR` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A window frame's bar across the unit square. |
| `LAMP_LOT_VERTEX` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The lot a lamp draws, from its instance's position (`outsideOn`), for both the lantern and its wash. |
| `PART_KINDS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `PORCH_LAMP` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The lantern beside a front door: width, height and depth out of the wall; its gap from the door's edge. |
| `PORCH_LIGHT` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The lantern beside a front door: a warm lamp behind frosted glass. |
| `PartLevel` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A part's geometry at one level of detail. |
| `PartScreen` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | How a part kind is drawn by the error on the screen (`partCells.ts`). |
| `ROOM_DAYLIGHT` | portado | `project/data/game/daylight.json` → `room_daylight` | How bright a room seen through its window is by day, as light it sends out (`setNight`) |
| `ROOM_LIT_MEAN` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A lit room's mean brightness over the glass |
| `SIMPLIFIED` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The props given simplified levels by meshoptimizer (`simplerLevels`): modelled in metres, their curves and leaves reducible.  Not the louvres and bris |
| `SIMPLIFIED_ERRORS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The errors those levels are asked at, metres (meshoptimizer's absolute error); one is kept only if it draws a quarter fewer triangles.  And 15 cm: at  |
| `WASH_OUT` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | How far the wash of its light (the `wash` part, placed at the lantern's centre) stands out of the wall. |
| `acUnitMaterial` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The condenser's material: its case's front drawn with the fan, grille and louvres (`AC_FRONT_GLSL`) where the model has a plain face,  and the lid as  |
| `awakeShare` | portado | `project/render/daylight.gd` → `func _awake_share` | The awake share at a clock time (hours), between the survey's hourly figures. |
| `awningGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `box` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A box of the given size whose centre is at (x, y, z). |
| `boxWithout` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A box without the faces another piece covers in the same plane: never drawn, and never two surfaces in one plane (`coplanar.spec.ts`). |
| `briseFarGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A brise's rack from afar: each blade a plane through its middle, seen from both sides; its thickness (`BRISE_BLADE`) left out. |
| `briseGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `card` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A card: `faces` with every vertex's pattern (`CARD`) set by `patternOf` (its normal), the material drawing its bars (`thinParts.ts`). |
| `createBuildingKit` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | `anisotropy`: the tier's taps for the facades' maps (`quality.ts`), as the ground's: at 4 a wall seen along the street blurred into its mean. |
| `createBuildingKit.coarser` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Their street faces only, as cards: a frame's depth, a railing's bars' and rails' thickness. |
| `createBuildingKit.dispose` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `createBuildingKit.extentOf` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `createBuildingKit.fade` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The faded copies: same colour and maps, a fraction of the presence, and no depth write |
| `createBuildingKit.furniture` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The furniture models, built the first time an interior is drawn. |
| `createBuildingKit.setGhostValid` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `createBuildingKit.setNight` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Lights the windows from inside as night falls: 0 by day, 1 at night; `hour` |
| `doorFarGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A door from afar: its leaf alone, the panels' relief and the handle left out. |
| `doorGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A door leaf in the component frame |
| `frameCardGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | THE THIN PARTS FROM AFAR AS CARDS: a window frame is one face over its window, a railing a face a side, each bar drawn by the fragment (`thinParts.ts` |
| `frameGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Window frame: outer border, a mullion and a transom, in a unit square  The sides', the mullion's and the transom's ends lie in the border's outer face |
| `glassRailGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A glass parapet: a clear pane the full width standing on the slab, a slim metal cap along its top. |
| `louvreGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A louvred shutter leaf: a stile-and-rail frame round slats tilted to shed rain. The slats are one box, drawn by the material  (`LOUVRE_SLATS_GLSL`): e |
| `mergedParts` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Parts merged into one model, the faces another part covers taken out first (`mesh/hiddenFaces.ts`); the parts let go. |
| `outsideLit` | portado | `project/render/daylight.gd` → `outside_on` | The share of outside lights lit at a clock time (hours). |
| `outsideOn` | portado | `project/render/daylight.gd` → `outside_on` |  |
| `quad` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A flat quad facing `axis` ('z' out of the wall, 'x' along it), centred at (x, y, z). |
| `railingCardGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `railingGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A balcony railing: top rail and balusters on three sides, z from 0 (wall) to 1. |
| `roofRailingCardGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `roofRailingGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A straight railing along a roof edge, centred at z = 0. |
| `roomDaylight` | portado | `project/render/daylight.gd` → `room_daylight` |  |
| `roomsAwake` | portado | `project/render/daylight.gd` → `rooms_awake` | The share the glass reads (`setNight`). |
| `scheduleOutsideLight` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Switches an outside light by its lot. |
| `shellPartGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A shell part's model (`shellParts.ts`) in three's axes (plan x, up, -plan y): each face's corners in its order, fanned and wound as `Shell.face` winds |
| `simplerLevels` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The coarser levels of a prop: welded, then simplified at each error with its normals and tones weighed in (meshoptimizer README, `simplifyWithAttribut |
| `topFaceGeometry` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A box's top face alone, its sides left out: a unit square at y = 0.5 facing up, for a thin plate seen from afar. |
| `trianglesOf` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `tvClock` | portado | `project/render/shaders/building_pane.gdshaderinc` → `tv_light(TIME` | Seconds since the kit was made, for a television's flicker (`roomLamps.ts`). |

### `src/render/buildings/landmarks.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/civic*, render/buildings/signature

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas). _(no 4.0: The five buildings of the reference sheet the player gave on 2026-10-09)_

**Motivo:** Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Design` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `LANDMARKS` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The landmark designs, by tower kind. |
| `LandmarkSink` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `decoTower` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `glassTower` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `glassTower.mass` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The front wing, standing forward, to the 21st storey; the side wing to the 13th. |
| `hotelBeauxArts` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `level` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The bottom of storey `k` (0 = the ground storey). |
| `modernOffice` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `plaza` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The raised stone plaza the building stands on, with its kerb. |
| `posts` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A face's interior posts: `n` bays between `a` and `b`. |
| `residentialTerrace` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |

### `src/render/buildings/lawnGround.ts`

**Estado:** a portar · **Dono no jogo:** render/trees/tree_place (b40::lot_trees)

**Para o jogador:** Árvores e canteiros nos lotes. _(no 4.0: The highest the drawn ground rises under a part laid on a lawn.)_

**Motivo:** Sem prova por símbolo até agora. Pendência do CLAUDE.md: árvores do lote.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GroundLattice` | a portar | render/trees/tree_place (b40::lot_trees) | The terrain grid the ground is drawn on: cell size and the half width (world units). |
| `highestGroundUnder` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `highestGroundUnder.gx` | a portar | render/trees/tree_place (b40::lot_trees) | Grid coordinates as `sampleGrid` reads them: x from the west edge, y from the north. |
| `highestGroundUnder.gy` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `highestGroundUnder.inside` | a portar | render/trees/tree_place (b40::lot_trees) | The grid's corners inside the footprint (a convex quad, wound either way). |
| `highestGroundUnder.read` | a portar | render/trees/tree_place (b40::lot_trees) |  |

### `src/render/buildings/layer.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: What the page keeps of a building emitted in a worker beyond the digests it was made for (`TownColumns` `keptSig`, `keptDigest`): its summary when it says anything (flags, lots, a signature floor), an)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CAST_RADIUS_SHARE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | SHADOW CASTERS BY THEIR SIZE IN THE CASCADE (Unreal's r.Shadow.RadiusThreshold, 0.03 at its epic scalability to 0.06 at low: a caster  whose bounding  |
| `DETAIL_CLASSES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The ledger's classes of the cells' detail (L0) and facade level (L1): what the room left under the cap is shared by. |
| `GAME_DETAIL_MEMORY_MB` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The most the cells' detail may hold on the GPU as the game sets it (MB): a large city keeps its far levels and masses for the  whole map, but the deta |
| `Kept` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the page keeps of a building emitted in a worker beyond the digests it was made for (`TownColumns` `keptSig`, `keptDigest`): its summary when it  |
| `PlanTest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A world box (three's axes) as the plan's (x, y, height) to the occlusion's test. |
| `createBuildingLayer` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.admit` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's level landed: uploaded only if the view still wants it at that level within the room (the cap worked out again with its measured bytes). |
| `createBuildingLayer.anyLotHeightAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.askExposed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT ask every cell's masses of the map at the opening: only the cells the view draws one by one (their tile refined, `massCells.exposedCells`), the |
| `createBuildingLayer.askMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT mass the buildings on the page |
| `createBuildingLayer.books` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.capNow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The detail drawn within the room the ledger leaves (`MassCells.cap`): a level never landed counts as its kind's landed do on average, and before any o |
| `createBuildingLayer.capRaw` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.capTriangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.castInto` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.cellFine` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell near is wanted at full detail (L0) rather than its facade level (L1). |
| `createBuildingLayer.cellNear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.cellOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.census` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.chooseNow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.chooseRaw` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.chosenTriangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the levels chosen draw, triangles: each near cell's level (shown, or as its kind is reckoned until it lands), and every mass resident (the cells' |
| `createBuildingLayer.chunkOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.counted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's level come or gone: its bytes counted (here and in the ledger), the level shown chosen again. |
| `createBuildingLayer.covers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.cullDetails` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.cutCellsOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells a cut-open view reaches |
| `createBuildingLayer.cutOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a cell's entries read of the cut-open view (`near`, `entryOf`), exactly: DO NOT compare the spec by identity, the wiring hands a new one every fr |
| `createBuildingLayer.digestOfCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.dropKept` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.entriesFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.entriesOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.entryFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building as the workers are sent it: its key, its record's digest and its cell's ground digest (none of its record). |
| `createBuildingLayer.flagsFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.flyFlags` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.forcedCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells held near whatever the eye says: those cut open (`cutCells`), the one being edited (`dimmed`). |
| `createBuildingLayer.hideMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mass shown hidden from the colour pass where its box (in the world, `massCells.ts` cellMesh) is behind the walls. |
| `createBuildingLayer.hideOccluded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.keep` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.keepToBudget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Over the budget, the cells drawn far let go, those spanning least of the screen first (`rankOf`): their masses, on the GPU already, stand for them |
| `createBuildingLayer.keptFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether what is kept of a building was made for its record and the ground under it now; its summary (none when it said nothing). |
| `createBuildingLayer.landCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One frame's share of the cells: those drawn near with no detail become pending (HLOD streaming) |
| `createBuildingLayer.levelOfDetail` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.levelOfDetail.fitted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The levels chosen at scale 1, over the view's budget of triangles the farthest coarsened first (`capTriangles`); only when the masses alone pass it do |
| `createBuildingLayer.levelShown` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The level a cell shows: its detail where fine (or with no facade level yet), its facade level where not (or with no detail yet), none far (its masses) |
| `createBuildingLayer.levels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The tiles' (L2) and plates' (L3) masses, asked again only where a cell's buildings changed (`massLevels.ts`). |
| `createBuildingLayer.lodChanged` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.lodChosen` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The levels chosen, as a string: the scale, each near cell and whether at full detail, the cells held. |
| `createBuildingLayer.lotHeightAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.lotOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A kept building's lots' ground, made once from its record (read from the document, then let go) while what is kept was made for that record; null whil |
| `createBuildingLayer.near` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.occluderCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.present` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's levels shown or hidden as `levelShown` says, its masses where it shows none (its parts follow at `cullDetails`). |
| `createBuildingLayer.putKept` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.rank` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A landing's upload ranked by the pixels the masses standing for it leave out (`uploads.ts`). |
| `createBuildingLayer.rankOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far down the order the budget coarsens in a cell stands (`MassCells.cap`): the less a metre spans there, the sooner it goes; DO NOT rank it by its |
| `createBuildingLayer.refFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where a worker reads each record (`layerCells.ts` `recordRef`), found when its job is asked: the table's buffer and row while the record is the table' |
| `createBuildingLayer.refill` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.restoreOccluded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.revealOccluded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.room` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.roots` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setCutaway` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setDetailBudget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setDimmed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setMassBudget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setNight` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setPreview` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.setRuined` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.show` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's level landed shown; its parts drawn from the next `cullDetails`, at the levels the eye allows. |
| `createBuildingLayer.showSignature` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A shown building's signature body (`signature.ts`), its record read for it (the few signature buildings only); none for the rest. |
| `createBuildingLayer.shownAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where the batches of each level are shown, for `patchCell`. |
| `createBuildingLayer.sigOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A record's digest: its columns' when it is the document's, else worked out (a building drawn on its own). |
| `createBuildingLayer.slotNow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | An id's slot with its columns those of its record now (read from the document when the diary named it since); -1 when it is not in the document. |
| `createBuildingLayer.update` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createBuildingLayer.update.inCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building drawn in its cell: not the one the preview stands for, nor a ruin, nor the one left solid while the others fade. |
| `detailClass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `groupBoxReader` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A group's box in the world from its meshes' own boxes (an instanced part's, a mesh's geometry's put in place), kept while its children  are the same;  |
| `planHidden` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `playerCause` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a diary entry is the player's edit (`main.ts` mutateBuilt's `tool …`, undo, redo), not the growth (`zone growth`) nor a tile coming in. |

### `src/render/buildings/layerApi.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: DO NOT put the detail of every cell together again, and DO NOT hold a far cell's detail for a while)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingLayer` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BuildingPreviewInput` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT put the detail of every cell together again, and DO NOT hold a far cell's detail for a while |
| `BuildingRoot` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the layer draws, part by part: the masses and their far levels, the facade level, the cells in detail, their parts, the signature buildings, the  |
| `CutawaySpec` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `GroundDigestOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A content digest of everything the ground, the natural ground and the paving read in a rectangle (`BuildingLayer.update`). |

### `src/render/buildings/layerCells.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE CELLS OF THE BUILDINGS LAYER (`layer.ts`))_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `<anonymous class>.clear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `<anonymous class>.delete` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `<anonymous class>.set` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BATCH_CELL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Side of the cells the buildings are batched in: the world's one grid (`world/cells.ts`). |
| `COARSE_PARTS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Frozen tables, not state (`eslint-rules/no-module-state.js`). The parts drawn with the cell's shells, one batch a cell: window panes (each with its ro |
| `COVER_CELL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Side of the footprint index's buckets, world units (ten to a world cell, `CELL`). |
| `COVER_CPU_BUDGET` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The most the cover cells hold on the CPU (`ViewCells`): a few hundred cells of a dense town. |
| `COVER_PAD` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far past a building's stored box its cover reaches (its footprints grown by `PLANT_MARGIN`), with a margin. |
| `COVER_REACH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far round the eye the cover cells are derived ahead of the plants' questions. |
| `COVER_VERSION` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cover's generator: moves when what a cell holds is worked out otherwise. |
| `CellBatch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `bytes`: what a cell's batch and its parts (`partCells.ts`) send to the GPU. |
| `CellCache` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The buildings batched cell by cell, each cell its own meshes: a cell out of the view  A cell's batch and the entries it was made of (its coarse cell's |
| `CoverCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One cell of the cover: the polygons reaching it, flat (x, y pairs; polygon k's points from `starts[k]` to `starts[k + 1]`), filed by bucket. |
| `CoverIndex` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where the buildings stand, for the scenery's plant cull: derived cell by cell round the questions asked (`viewCells.ts`), never for the whole town |
| `CoverSource` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the cover index reads: the document's buildings and its diary. |
| `DETAIL_PARTS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every other part, filed by sub-cell and drawn by the error on the screen (`partCells.ts`). |
| `EMPTY_COVER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LevelKind` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's level: its detail or its facade level (`cellRequests.ts` `BatchKind`, named here so the cells' module imports nothing of the requests). |
| `PLANT_MARGIN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far past a wall a plant is still considered under the building. |
| `ResidentBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | THE BYTES THE CELLS' LEVELS HOLD ON THE GPU (`layer.ts` `counted`, `keepToBudget`), by level, and what each level measured when it last landed, kept o |
| `ResidentBytes.changed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `ResidentBytes.count` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level come (`bytes` over 0) or gone (0). |
| `ResidentBytes.forget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's buildings changed: what its levels measured no longer holds. |
| `ResidentBytes.landed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level landed, uploaded or not: what it measured. |
| `ResidentBytes.measured` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ResidentBytes.of` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a level holds or would hold, bytes: resident, else as measured when it last landed, else `typical`. |
| `ResidentBytes.resident` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ResidentBytes.total` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the levels hold now, bytes. |
| `STREAM_AHEAD` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Cells asked of the workers at once while the eye streams the town in (`landCells`, `layer.ts`), nearest first. |
| `boxGroundDigest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The ground of a box sampled as `groundDigest` samples a building's (6 by 6 points): the layer's, from the box its columns keep (`townColumns.ts`). |
| `bucketKey` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `cellGroup` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A group of the cells in `cache` (the coarse batches, or the detail ones): its triangles theirs, disposing it lets go of the group only. |
| `cellGroup.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells' batches belong to the cache: only this group is let go. |
| `cellGroup.setVisible` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `coverBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The bytes a cover cell holds: its arrays, and each bucket's entry. |
| `coverCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's cover from the buildings reaching it: a pure function of the cell and the document. |
| `coverPolygons` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's ground no plant stands on: its solid footprints grown by `PLANT_MARGIN`, and its footprint's rectangles (the cover index's and a tile's n |
| `createCoverIndex` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createCoverIndex.covers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createCoverIndex.follow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Brought up to the document: the cells its buildings' changes reached dropped (made again when asked), the rest kept. |
| `createCoverIndex.look` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The eye: the cells round it derived ahead of the plants' questions, `most` a call. |
| `dropCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `groundDigest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT work it out again here: every building the ground moved near turned its corners into the world again, 3 s of a 2 |
| `inCoverPolygon` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether (x, y) is in polygon `k` of a cell (`pointInPolygon` on the flat points, the same arithmetic). |
| `recordRef` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's batch out of its cache and its group, disposed.  Where the workers read a building's record (`layer.ts` `refOf`): its tile's shared table and |
| `watchedCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell cache that tells `told` each key that comes in (true) or goes (false). |

### `src/render/buildings/levelKeys.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE KEYS OF THE BUILDINGS' CELLS AND LEVELS: a cell "i,j", a 1,024 m tile "t:x,y", a 2,048 m plate "p:x,y" and the base over a plate's square "b:x,y" (`massBase.ts`) (`massLevels.ts`, `massCells.ts`, )_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `MassLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PLATE_CELLS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PLATE_SHIFT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The plates sit half a plate off the cells' origin: the map's middle plate is centred on it (`bounds.ts`); each holds four whole tiles. |
| `TILE_CELLS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Cells a tile and a plate are on a side (`world/cells.ts` CELL, 256 m: 1,024 m and 2,048 m, `bounds.ts` PLATE_SIZE). |
| `baseOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The base (the top level, never let go: `massBase.ts`) a cell lies under: its plate's square. |
| `cellIndex` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's column and row from its key ("i,j", `layer.ts` `cellOf`). |
| `levelCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells a level's key covers: its first column and row and its side, in cells. |
| `plateOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `tileOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The tile and the plate a cell (column, row) lies in, as keys ("t:x,y", "p:x,y"). |

### `src/render/buildings/lightSlots.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: DO NOT number the spaces in the order they are asked for (a table on the page, as this was))_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DIGITS` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint32_t fnv(` — dígitos do texto do número, que o jogo não usa | A whole number's digits, written over by each `hashWhole`. |
| `SPACE_SEEDS` | portado | `native/src/render/buildings/pane_rooms.cpp` → `SLOT_MASK = 0xffff` | Seeds there are (the old table's size: the glass's hash spreads them the same). |
| `hashWhole` | portado | `native/src/render/buildings/pane_rooms.cpp` → `uint32_t fnv(` | FNV-1a, 32 bits, of a whole number's decimal text, as `String(n)` writes it (no text made). |
| `spaceSeed` | portado | `native/src/render/buildings/pane_rooms.cpp` → `uint16_t slot_of(` | The seed of one space: building, floor, block, and the space's corner (to a decimetre): FNV-1a of "building:level:volume:x:y", hashed digit by digit,  |
| `spaceSeed.colon` | portado | `native/src/render/buildings/pane_rooms.cpp` → `uint32_t fnv(` |  |

### `src/render/buildings/louvreSlats.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/part_models, facade_parts, building_parts.gdshader

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas. _(no 4.0: THE SHUTTER LEAVES' SLATS DRAWN BY THEIR MATERIAL (`kit.ts` louvreGeometry): one box where nine slats were modelled, each pixel of it)_

**Motivo:** Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LOUVRE_BOX_HALF` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The box the slats are drawn in (`LOUVRE_SLATS_GLSL`): the frame's opening, its sides in the frame's inner faces (`louvreGeometry` leaves  them out), a |
| `LOUVRE_BOX_Z0,LOUVRE_BOX_Z1` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `LOUVRE_FRAME` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A louvred shutter leaf's slats (`louvreGeometry`, `LOUVRE_SLATS_GLSL`): nine, from y -0.42 every 0.105, about z -0.1, tilted -0.6 rad about x,  0.07 t |
| `LOUVRE_SLATS_GLSL` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL of the slats in their box (`louvreGeometry`): `louvreSlat(o, d, n)`, the first slat a ray from `o` along `d` (the model's axes) meets  inside the |
| `LOUVRE_VERTEX_BODY` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `LOUVRE_VERTEX_HEAD` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The vertex half of the slats (`louvreSlats`): the model's point, the ray to it from the camera in the model's axes (the light's way for a  shadow map: |
| `louvreMaterial` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The shutter leaves' material: its slats drawn in their box (`louvreSlats`), and its shadow's depth material the same (`partPose.ts`  depthPatch): the  |
| `louvreSlats` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The slats drawn in their box (`louvreGeometry`), on a part's material or its shadow's depth material (`colour`): a pixel of the box whose  ray meets n |

### `src/render/buildings/massBase.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A run of a base's cells whose vertices fit 16-bit indices (meshoptimizer's advice: half the index memory).)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BASE_CELL_STRIDE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Numbers a cell's entry holds in `BasePiece.cells`. |
| `BASE_VERTEX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Shorts a base vertex holds. |
| `BaseMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A square's base: its pieces on one grid. |
| `BasePiece` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A run of a base's cells whose vertices fit 16-bit indices (meshoptimizer's advice: half the index memory). |
| `BaseWriter` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A square's base put together from its cells' writers (each building into its own cell's, `cellOf`), on the square's grid; null with no cell. |
| `BaseWriter.cellOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The writer of the cell a building stands in (its position, as `layer.ts` cellOf files it). |
| `BaseWriter.cells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BaseWriter.done` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BaseWriter.done.q` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CURTAIN_GLASS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellWriter` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One cell's arrays as they are written: places in floats (three's axes) and a paint a vertex. |
| `CellWriter.index` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellWriter.paint` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A linear colour as one short: 5 bits of red, 6 of green, 5 of blue, in sRGB (the dark tints keep their steps). `massingMaterial.ts` reads it back. |
| `CellWriter.position` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellWriter.vertex` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A point of the plan at height `z`. |
| `CellWriter.vertices` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `GLASS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The glass seen from afar (`massingMaterial.ts` GLASS). |
| `LOT_AREA` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The smallest paved lot the base lays, m²: a yard under 10 m a side spans 2 px at the map's 0.2 px a metre, where the base is drawn; left to the finer  |
| `LOT_LIFT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A lot lies this far over the ground it is laid on with no floor to stand on (`massing.ts` emitLotPlates). |
| `PIECE_VERTICES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The most vertices a piece holds: every index of it fits 16 bits (65535 left out: WebGL 2's primitive restart). |
| `WALL_GLASS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The windows' mean over a wall from afar (`massingMaterial.ts`: the pane's share across times its height in the storey, 0.3 to 0.85, times 1.45): an or |
| `baseBuffers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every buffer of a base: handed over, not copied. |
| `baseBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The bytes a base holds on the GPU: its vertices and indices. |
| `emitBase` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's blocks and paved lots into its cell's writer; `floor` as its masses' (`massFloor`), undefined with no closed block. |
| `emitBase.ring` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `mix` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `paint` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A linear colour as one short: 5 bits of red, 6 of green, 5 of blue, in sRGB (the dark tints keep their steps). `massingMaterial.ts` reads it back. |
| `prism` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A block's prism: its roof (`top`, the ring shared with the walls' heads) in its colour, its walls in theirs from their foot ring. |
| `ringArea` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A ring's signed area in the plan (positive: anticlockwise). |
| `slab` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A flat ring at `z` (plan points, anticlockwise), its triangles facing up; returns its first vertex. |
| `toSrgb` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A linear channel in sRGB (IEC 61966-2-1), 0 to 1. |

### `src/render/buildings/massCells.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE BUILDINGS' FAR LEVEL, cell by cell: each cell of the buildings' grid)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Base` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The base over a plate's square (`massBase.ts`): planned once its square has buildings, its pieces once they come. |
| `BasePart` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A base's piece on the GPU: its mesh, and its cells' runs of indices (`BasePiece.cells`). |
| `Cell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `CellDrawn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What draws a cell of buildings now (`MassCells.census`): its detail, its masses, its tile's or plate's, the base's; 'empty' where none of its building |
| `DETAIL_REACH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `DETAIL_REACH_VISIBLE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A metre's pixels under which the 1.4 km reach forces no detail: a cell there spans too few pixels for any of it to be seen (the whole map at once, ~0. |
| `FACADE_ERROR` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The largest thing the facade level (L1, `emitChunk` `far`) leaves out, m: the deepest reveal it lays flat. |
| `FAR_ERROR` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The largest thing the far level leaves out, m: a balcony's slab (1.3 m) or a sill and its frame stand out of the wall |
| `FINE_PIXELS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The full detail (L0) while what the facade level lays flat, 0.3 m, spans this many pixels at the frame's own scale (~280 m at 1080 p). DO NOT take it  |
| `HORIZON_HALF` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `HYSTERESIS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell drawn near stays near down to this share of `NEAR_PIXELS`. |
| `LEVEL_CEILING` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LEVEL_FLOOR` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The vertical extent a tile's or a plate's box is tested against the view with (`levelInView`): below any ground, and as tall as a tower before its mas |
| `Level` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile or a plate: one mesh of its cells' masses, simplified as far as its `error` says (`massing.ts` `MassDetail`). |
| `MassCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `NEAR_PIXELS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The detail is drawn while what the far level leaves out spans this many pixels or more. |
| `STREET_EYE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | STREET-LEVEL HORIZON VIEWS: with the eye near the ground and looking along the streets |
| `Seen` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the eye reads of a box (`measureSeen`): a metre's pixels there, the pixels allowed, its distance across the plan; `scale`, the frame's error scal |
| `VIEW_COME` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell comes near only when its box, grown by this share of a cell, meets the view's frustum; a near cell stays near while its box grown by `VIEW_KEEP |
| `VIEW_KEEP` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `baseMesh` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A base's piece as a mesh (`massBase.ts`): its vertices one interleaved buffer of shorts (the place on its grid, read normalised; the paint), put in pl |
| `boxOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `cellMesh` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's mesh from its masses' arrays, taken as they came |
| `cellMesh.half` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Half floats where a vertex's numbers allow (`CellMass`): three uploads a Uint16Array as HALF_FLOAT where the attribute says so (r186 `WebGLAttributes. |
| `createMassCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `released`: a cell's or a level's masses let go, neither drawn nor wanted (or refused as they came): to be asked again once wanted (`layer.ts`).  `cur |
| `createMassCells.askable` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a tile or a plate let go for the budget may be asked again: the view meets it and wants it drawn whole (true for one not let go). DO NOT ask o |
| `createMassCells.books` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What it keeps, by book: cells, tiles and plates known, masses standing (`BuildingLayer.books`). |
| `createMassCells.cap` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT draw more detail than the budget holds (the brief: detail residency capped in bytes, falling back to L1): the farthest near cells fall back to  |
| `createMassCells.cap.sameRank` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells of the same rank as `movable[at]` (the same error and distance): stepped down together, never one of two alike. |
| `createMassCells.cellsLanded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether every cell of a tile has its masses: until then the tile drawn before stands for them (Cesium keeps a parent until its children load). |
| `createMassCells.census` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What draws each cell of buildings now, by key (the census of holes, `tests/perf/massBase.spec.ts`). |
| `createMassCells.choose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Which cells are drawn near (their detail) and which far (their masses), from the eye (map x/y, height) and the screen  - DO NOT give detail to a cell  |
| `createMassCells.choose.coarseWanted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether the view wants a level drawn in place of its children: judged by its error once its masses came, by what its level may leave out before. |
| `createMassCells.choose.decide` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.choose.look` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level meeting the view (grown by `VIEW_KEEP`): seen now. |
| `createMassCells.choose.wait` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level waited for: by the opening's picture too while it meets the frustum (`waitingInView`). One let go for the budget out of the view is not: it is |
| `createMassCells.coarse` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Drawn in place of its children (Cesium's replacement refinement). |
| `createMassCells.cover` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell under a tile or plate drawn in its place: far (its detail let go by the layer), its mass hidden; one with no mass stays near. |
| `createMassCells.coveredAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell's tile or plate stands in its place now. |
| `createMassCells.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.drawn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell has its masses on the GPU, or a tile or plate drawn in its place (`layer.ts` drawnOver): what stands for it before its detail; `current |
| `createMassCells.drawnBy` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What draws a cell other than the base: its tile or plate drawn in its place (with its masses), its detail shown near, its own masses; null for none. D |
| `createMassCells.drop` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.dropBase` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.dropMesh` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.ensure` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A plate is kept while it has a mesh or a tile of its own: its tiles are found through it (`choose`), so a tile makes its plate. |
| `createMassCells.evictLevels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The tiles' and plates' masses out of the view let go, the least recently seen first, until every mass of the town (cells, tiles, plates) holds `budget |
| `createMassCells.evicted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Its masses let go for the budget (`evictLevels`): out of the view, or in it and drawn by its children; asked again only once the view meets it and wan |
| `createMassCells.explain` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | TEMPORARY (DEV only, to be taken out): what `choose` reads of a cell from the last eye and view (`layer.ts` __budgetLog). |
| `createMassCells.exposedCells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells drawn one by one now (their tile refined, `choose`): the only cells whose own masses are asked (`layer.ts`). |
| `createMassCells.farTriangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The triangles drawn for what is not near: the tiles and plates shown and the far cells' masses (a near cell's masses, hidden under its detail or stand |
| `createMassCells.fine` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Drawn near and at full detail (L0); near and not fine: its facade level (L1). |
| `createMassCells.followView` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.forget` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.hold` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells the frame's budget holds back (`layer.ts` capTriangles): `far` never drawn near, `coarse` never at full detail, until let go there. DO NOT d |
| `createMassCells.inView` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.keys` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every cell there is (the whole town filed again, `layer.ts`). |
| `createMassCells.land` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's masses come: its mesh made of them (null: its buildings have no mass, the cell always near). |
| `createMassCells.landBase` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A base's prisms come (`massLevels.ts` landBase; null: none of its buildings stands from afar). |
| `createMassCells.landLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile's or a plate's masses come (`massLevels.ts`; null: none of its buildings has any): the old ones stood until now. |
| `createMassCells.letCellsGo` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile left unexposed by the frame's last choice: its cells' masses let go, neither drawn nor wanted. |
| `createMassCells.letTileGo` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile left unwanted by the frame's last choice: its masses let go, asked again once its plate refines. |
| `createMassCells.levelDrawn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a tile or a plate has its masses on the GPU (drawn or ready to be). |
| `createMassCells.levelInView` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.levelPixels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.levelSeen` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a tile or a plate met the view's frustum, grown by `VIEW_KEEP`, at the last `choose` (false before any `choose`; true for every level of a `ch |
| `createMassCells.levelWhole` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether the view wants a tile or a plate drawn whole (in place of its children) at the last `choose` (true for one not chosen yet). |
| `createMassCells.levelsOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.lowered` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.massBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The bytes a cell's masses hold on the GPU (0 before they came): what a level of it never landed is reckoned at (`layer.ts` capNow). |
| `createMassCells.massTriangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The triangles of a cell's masses (0 before they came): what it costs sent far (`layer.ts` capTriangles). |
| `createMassCells.measure` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.near` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell is drawn near; a cell with no mass (lawns only), or none known, always is. |
| `createMassCells.nearKeys` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cells drawn near now (what the streaming of the detail reads, `layer.ts` `landCells`). |
| `createMassCells.perMetreAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.pixels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The pixels `metres` spans at a cell from the last eye (`choose`): what its upload is ranked by (`uploads.ts`). |
| `createMassCells.pixelsAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.plan` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell whose masses are asked (new, or its buildings changed): kept as it stands until they come (`land`); `bottom`/`top` its buildings' heights till  |
| `createMassCells.planLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile or a plate that has buildings, its masses asked or not yet (`massLevels.ts` `plan`). |
| `createMassCells.remove` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell with no building left: gone. |
| `createMassCells.removeLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile or a plate with no building left: gone. |
| `createMassCells.setCoarse` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level drawn in place of its cells or not: its mesh shown, its cells covered or uncovered (16 or 64 looked up, on a change only). |
| `createMassCells.setDetailed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a cell's detail is shown (`layer.ts` streaming): a near cell keeps its mass shown until its detail lands |
| `createMassCells.setErrorScale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The frame's error scale for the next `choose` (`frameBudget.ts`): every error is allowed this many times the pixels it is allowed at 1. |
| `createMassCells.setExposed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile no longer refined (its plate or itself drawn whole): its cells' masses let go, neither drawn nor wanted (farTree.ts does the same with its node |
| `createMassCells.setNear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.setWanted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile no longer wanted (its plate drawn whole): its masses let go, asked again once its plate refines (`massLevels.ts` `release`). |
| `createMassCells.settle` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The frame's choosing is over (`layer.ts` levelOfDetail): what its last choice left unexposed or unwanted let go. |
| `createMassCells.shadowMasses` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A far sun cascade being drawn (`layer.ts` castInto): the cells drawn in detail whose masses can stand for them shown for the pass, their keys given (t |
| `createMassCells.showMass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's mass shown: far, or near with no detail to stand in for it; never under its tile or plate. |
| `createMassCells.square` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's square on the map. |
| `createMassCells.stand` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `createMassCells.syncBases` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The bases drawn under exactly the cells nothing finer draws (three's geometry groups): asked before each frame is drawn (`layer.ts` cullDetails). |
| `createMassCells.top` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The highest point of any cell's masses (the shadow's reach over the view, `renderer.ts`); -Infinity with none. |
| `createMassCells.wanted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile: wanted by the view (its plate refined), so asked of the workers (`massLevels.ts` `wanted`); a plate always is. |
| `keyOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's key from its column and row (`layer.ts` `cellOf`). |
| `levelClass` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The ledger's class of a level's mesh. |
| `measureSeen` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the eye reads of a box (plan x/y, top), as Cesium measures to a tile's bounding volume; the eye low, the farther the box the more error it may ma |
| `sameFrustum` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether two frusta are the same: every plane's normal and constant. |
| `sendUp` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mesh put on the GPU: registered in the ledger, sent ahead ranked by `priority` (never uploaded mid-frame; a cell of masses out of view kept its copy |
| `sphereOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |

### `src/render/buildings/massLevels.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE BUILDINGS' LEVELS ABOVE THE CELLS: a 1,024 m tile (L2) and a 2,048 m plate (L3) of masses, one mesh each (`massCells.ts` chooses them), and the base over each plate's square (the top level, `massB)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `KIND` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT ask a tile or a plate again for each cell an edit touched: the cells are touched, their levels asked once (`flush`) |
| `MassLevels.ask` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.asked` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.asking` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.away` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far a level is from the eye (`layer.ts`): the plates waiting are asked nearest first. |
| `MassLevels.changed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `MassLevels.clear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every request cancelled and every cell forgotten (a context restored, the layer gone): the next filing touches them all again. |
| `MassLevels.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `land`: a level's masses come (null: none of its buildings has any); `remove`: a level with no building left; `plan`: a level that has buildings (its  |
| `MassLevels.deferred` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile or a plate left waiting for now (`layer.ts`: out of the view while the curtain is up): asked by `pump` once it is not. |
| `MassLevels.dirty` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.edited` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.entries` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.flush` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every plate touched asked once (`PLATES_IN_FLIGHT` at a time, `pump`), made of its cells' entries; a tile touched asked once it is wanted (`pump`); a  |
| `MassLevels.hasAny` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.hole` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether nothing stands for a level yet (`layer.ts`: a plate with no masses of its own): asked in the far levels' band (`cellPool.ts` hole), before the |
| `MassLevels.landBase` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A base's prisms come (`massBase.ts`; null: none of its buildings stands from afar). |
| `MassLevels.listOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.outdated` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a level's buildings changed since its masses were made (asked and not come, or not asked yet): its mesh, if any, is of older buildings. |
| `MassLevels.pump` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The tiles changed since last asked that a view now wants (a plate refined): asked (each frame, `layer.ts`; as many as tiles changed, never the map). |
| `MassLevels.release` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile whose masses were let go, neither drawn nor wanted, or a plate's let go out of the view for the budget (`massCells.ts` evictLevels): asked agai |
| `MassLevels.stale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassLevels.touch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's buildings asked again (`entries`), or gone (null): its tile and plate to be asked again at `flush`; `edited`, by the player's edit (asked fir |
| `PLATES_IN_FLIGHT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT ask every plate of the map at once (the opening, a town filed whole): each job holds its entries (an object a building) until it is answered, s |

### `src/render/buildings/massing.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A BUILDING FROM AFAR: its blocks as they stand, one plain mesh, with the windows drawn by the shader)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ARCADE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | How far an arcade's wall steps back behind its columns (`buildingMesh.ts` ARCADE). |
| `ATTIC_H` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BURY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where the wall of a block on the ground starts: below the lowest ground under it, as the detailed plinth does. |
| `CELL_DETAIL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `DRAWN_PLANT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The roof plant drawn from afar (`emitRoofPlant`): the rest is under a metre across. |
| `EAVES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A pitched roof's overhang at the eaves (`buildingMesh.ts` EAVES). |
| `Facade` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A facade's pattern at a vertex: along, bay, storey height, first storey's height; and the glass share. |
| `GLASS_SHARE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The share of a bay its window takes across, by what the bay holds (a curtain wall is all glass). |
| `Grown` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A typed array written at its end, its room doubled as it fills: DO NOT gather numbers in a `number[]` (8 bytes each and its copies as it grows) for a  |
| `Grown.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Grown.done` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What was written, in an array of its own size. |
| `Grown.length` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Grown.push2` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Grown.push3` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Grown.push4` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Grown.room` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LOT_COLOURS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The colours of a lot's surface (`buildingMesh.ts` draws them in their finishes) |
| `MACHINE_ROOM_H` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A machine room over a lift core. |
| `MassDetail` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | WHAT A LEVEL OF THE MASSES LEAVES OUT: the cell's (all of it), a tile's (L2) and a plate's (L3, `massLevels.ts`) |
| `MassWriter` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.N` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A plan direction (local) and up, as a three normal. |
| `MassWriter.P` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A point of the building's plan at height z, in three's axes. |
| `MassWriter.asks` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A question only a level of boxes asks (`MassDetail.boxes`), kept as one this level was asked: a size another level would answer. |
| `MassWriter.chunk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.colour` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.error` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The tallest thing left out so far (`leaves`). |
| `MassWriter.f` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The building's frame once (`localToWorld` point by point turns the rotation each time; the same arithmetic). |
| `MassWriter.facade` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.face` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A convex polygon of points (three's axes), all with one normal and colour; `along` its points' metres along their face |
| `MassWriter.glazing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.index` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.kept` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The smallest thing asked about and kept so far (`leaves`; a box-less level's pitched roof too, `asks`): a coarser level whose `drop` is under it answe |
| `MassWriter.leaves` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a thing `size` m tall or deep is left out at this level; its size kept as the level's error when it is. |
| `MassWriter.normal` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassWriter.polygon` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A flat polygon's triangles (`earcut`'s, of `points`) with its points written once and shared, as `face` would lay each triangle: DO NOT write a slab's |
| `MassWriter.position` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `MassingChunk` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `NO_FACADE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PARAPET_H` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The parapet round a flat roof, and the attic band of the 'attic' dress (`buildingMesh.ts`). |
| `PARAPET_T` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PLATE_DETAIL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A plate's: under 8 m, the pitched roofs of houses and low blocks flattened too. |
| `Rgb` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Run` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One run of a storey (bays side by side with the same relief and glass), before the storeys alike are stacked into one. |
| `SAW_GLASS_RGB` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TERRACE_RGB` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TILE_DETAIL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A tile's: under 3 m (a machine room, `MACHINE_ROOM_H`; a parapet; an arcade's set-back, `ARCADE`). |
| `baseFloor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's floor as its base stands it (`massBase.ts`, the top level, drawn from afar): its lots' front pavement, no bay laid out (`frontFloorHeight |
| `box` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A box of the plan from z0 to z1: its four sides and its top. |
| `byStart` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Bays in the order they stand along their side. |
| `emitLotPlates` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The paved lots - car parks, yards, courts - as the detailed lots lay them |
| `emitMachineRooms` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The machine rooms over the lift cores, on the highest flat roof over each (`buildingMesh.ts`). |
| `emitMassing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's mass from afar, on the ground it stands on (the floor worked out as the detailed building's, `floorHeight`) |
| `emitMassingLevels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's masses at two levels, `coarse` leaving out at least what `fine` does: DO NOT walk the building again for the coarser where the finer's wa |
| `emitPolygonRoof` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A pitched roof over a shaped block: the roof's surface |
| `emitPolygonRoof.height` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `emitPolygonRoof.rect` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `emitRoof` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `emitRoof.wallOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `emitRoofPlant` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The plant on a flat roof a metre or more across (`roofPlan.ts`, the plan the detailed roof draws) |
| `emitVolume` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A block's walls from its bays as the detailed building lays them (`facadeBays`), off the layout every block of the body reads (`bayLayout`, worked out |
| `emitVolume.alike` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | DO NOT draw a quad a storey: the storeys alike, one above the other, are one wall whose windows the shader counts up from the lowest (`massingMaterial |
| `emitVolume.offsetOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The run's outward offset: a relief's push, an arcade's wall set back. Laid flat where the level leaves it out: the wall then runs on with its neighbou |
| `emitVolume.shareOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `floats` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `massColours` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's colours from afar: a material's tint in its own shade, times its finish's tone (`finishes.ts` `finishAlbedo`) when given. |
| `massFloor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's floor as the detailed one's (`buildingMesh.ts` `emitChunk`, `floorHeight`); undefined with no closed block. |
| `massWriter` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building's mass written out (`emitMassing`). |
| `rgbOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `shadeOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The building's own shade, as the detailed one has it (`buildingMesh.ts` `emitBuilding`). |
| `uints` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |

### `src/render/buildings/massingMaterial.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: The material of buildings seen from afar (`massing.ts`))_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BASE_PAINT_GLSL` | a portar | building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms | The base's colour (`massBase.ts` paint): 5-6-5 bits of sRGB in the vertex's fourth short, read back linear (IEC 61966-2-1). |
| `GLASS` | portado | `project/render/shaders/building_far.gdshader` → `vec3 glass = mix(` | The glass a window is seen as from afar: the detail's pane (`kit.ts` glass 0x7c8e98) scatters next to nothing (`glassRoom.ts`: its albedo times 0.06), |
| `HEAD` | portado | `project/render/shaders/building_far.gdshader` → `WIN_HEAD` |  |
| `MassingUniforms` | portado | `project/render/daylight.gd` → `rooms_awake` |  |
| `PULSE_GLSL` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `float pulse(` | GLSL `<prefix>Pulse(x, w, a0, a1)`: a pulse [a0, a1) repeated every 1 seen through a pixel whose footprint is w periods, and `<prefix>Footprint(cell)` |
| `ROOM_SEEN_GLSL` | portado | `project/render/shaders/building_far.gdshader` → `MEAN_SEEN` | The room seen through a pane by day, as the detail shows it once the pane is a few pixels (`glassRoom.ts` paneTiny's mean), lit by `roomDaylight`. |
| `SILL` | portado | `project/render/shaders/building_far.gdshader` → `WIN_SILL` | Where a window sits in its storey, from the floor (shares of the storey height). |
| `createMassingMaterial` | portado | `project/render/shaders/building_far.gdshader` → `shader_type spatial` | The masses' material; `base`, the top level's (`massBase.ts`): no normal and no facade of its own - each face shaded by its own slope (three's `flatSh |

### `src/render/buildings/partCells.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE PARTS OF ONE CELL OF BUILDINGS, EACH KIND DRAWN AT THE LEVEL ITS ERROR ON THE SCREEN ALLOWS)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BAR_PIXELS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A part made of bars (a frame's rails and stiles, a railing's, a brise's blades: its `pieces`) leaves the drawing once its bars span under this many pi |
| `Band` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One band of a kind's instances in the cell, by sub-cell. |
| `COARSEN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A level is left for a coarser one once that one's error is under this share of a pixel (a shadow under this share of a texel), and taken back at a who |
| `EVICT_AFTER` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The choices a run's mesh is kept after it leaves the drawing, before its buffers go: a run that comes back within a camera's pause keeps its mesh; one |
| `LIGHT_BACK` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `LIGHT_PIXELS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A shell's part is drawn with the far program (`kit.farShell`) once a metre spans under this many pixels where its run stands nearest (a finish's textu |
| `NONE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A sub-cell not drawn: every instance in it under a pixel. |
| `PART_ERROR_PX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Pixels a coarser level's error may span before a finer one is drawn. 1.5, not 1: the parts were 47 % of the district's triangles  (9.3 M of 20 M, `npm |
| `PartCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.bands` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.bounds` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.box` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.castInto` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Before a sun cascade is drawn: each band casts into it where its widest piece spans the cascade's texel, every instance of it at the  level that texel |
| `PartCell.choose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each sub-cell of each band at the coarsest level whose error is under a pixel, none where an instance is; shadows cast where a piece spans a texel. |
| `PartCell.chosenFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `filing`: the cell's parts by band and sub-cell, as its job filed them (`partFiling.ts`): DO NOT read an instance here, the page only wraps the buffer |
| `PartCell.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.drawFar` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.evictStale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.faded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.filing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.group` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.levelAt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.make` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.makeCaster` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.perMetre` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.posedLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runCount` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runFirst` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runLast` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runPixels` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runsOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.runsOf.triangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.setFaded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every kind drawn faded ("Ocultar outros") or not. |
| `PartCell.shadowLevel` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.tick` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.triangles` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartCell.warmMeshes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A mesh a band over the whole cell, made now to send its instances to the GPU ahead (`uploads.ts`): every run shares its buffers. |
| `PartView` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where the eye is (map x/y, height), the pixels a metre spans at a metre's distance (or everywhere, `orthoPx`) and a shadow texel's side, m (Infinity:  |
| `barsAcross` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The widest of a part's bars across (`PartScreen.pieces`, the middle side of each), at a sub-cell's scale. |
| `diagonal` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The diagonal of a box scaled. |
| `dropParts` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's parts out of `parts` and `root`, let go. |
| `largest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The largest of three lengths scaled. |
| `middle` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The middle of three lengths scaled: a piece's shadow is this wide from some side of the sun. |
| `widestBar` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A band's widest bar over all its sub-cells (`barsAcross`), metres: what the facade level's batch judges a kind of bars by (`farParts.ts`). |

### `src/render/buildings/partFiling.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A CELL'S PARTS FILED BY BAND AND SUB-CELL WHERE THE CELL IS MADE (`cellWork.ts`): the page is handed each band's buffers ready, never an instance to read (`partCells.ts` draws them))_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `<IfStatement>` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `BAND_BASE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The error a band's instances are grouped by starts at this, m, each band four times the last's. |
| `DETAIL_CELL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Side of the sub-cells a cell's parts are filed by (four a side): each part kind's level is chosen per sub-cell, or per quadrant or cell where they agr |
| `DRAW_TRIANGLES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a draw is taken to cost in triangles: a band is kept apart only if it draws as much (three's per-object work on the page, `projectObject`/`render |
| `FiledBand` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One band of a kind's instances in the cell, by sub-cell in Morton order. |
| `LEAVES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `PartFiling` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's parts, every kind's bands. |
| `SIDE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Sub-cells a side of a cell (the world's one grid, `layerCells.ts` BATCH_CELL): four, as `morton` is made for. |
| `bandOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The band of an instance whose kind's first coarser level leaves out `metres` of it (its size, for a kind of one level). |
| `bandSubCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Sub-cell `m`'s instances of a kind banded (`bands`), each band's count and largest scales in `perBand`; the band they all are (-1: mixed).  DO NOT loo |
| `copyBand` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A sub-cell's instances of `band` (after the merge, `into`) copied in at `at`; where the next goes. |
| `fileKind` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One kind's bands: one pass reads each instance's scales and band, the other copies; a sub-cell all of one band is copied whole. |
| `fileParts` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `details`: the cell's sub-cells (`DETAIL_CELL`) as the job made them, by key "column,row"; every kind's instances by band, each laid out by sub-cell i |
| `filingBuffers` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every buffer of a filing: handed over, not copied (MDN, `postMessage` transfer). |
| `filingBytes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What a filing sends to the GPU, in bytes: each band's instances, its colours and looks. |
| `morton` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A sub-cell's index in Morton order from its column and row in the cell. |
| `partBandings` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each kind's banding off the kit's screens as they are now (a small prop's simplified levels come in once the simplifier has loaded). |

### `src/render/buildings/partPose.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: A BUILDING PART'S PLACE AS THE GPU TAKES IT: a pose turned about the up axis - its place in three's axes, its turn's cosine and sine and its scale on three axes - 8 floats in place of a 16-float matri)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `MAIN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `POSED` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `POSE_A` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The pose's instanced attributes, as `posedMaterial` reads them (`cellInstances.ts` POSE_A, POSE_B): its first four floats, and the scale. |
| `POSE_B` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `POSE_DEPTH` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `POSE_FLOATS` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Floats a part's pose takes: x, y, z (three's axes), the turn's cosine; its sine, then the scale along the model's x, y and z. The turn kept as its cos |
| `POSE_MATRIX_GLSL` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | GLSL `mat4 partPoseMatrix(vec4 a, vec4 b)`: `poseMatrixInto` in GLSL, a = x, y, z, the turn's cosine; b = its sine, the scale. |
| `Posed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `letGoPosed` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A posed copy let go: the model's attributes and index taken off first, so three frees the poses' buffer only (r186 `WebGLGeometries` frees every attri |
| `partPoseDepth` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The shadow pass's material of a part's material: three's depth material drawn from the poses, one per material (r186 `WebGLShadowMap` copies its map a |
| `partPoseShader` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A vertex shader drawn from poses: three's `instanceMatrix` (r186 `WebGLProgram` declares it in the prefix; `project_vertex`, `worldpos_vertex`, `defau |
| `poseMatrices` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every pose of a batch as its matrix, 16 floats each: what a ruin is broken from (`fracture.ts`). |
| `poseMatrixInto` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A pose's matrix, column-major T * Ry * S, as its GLSL makes it (`partPoseShader`). |
| `posePart` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A material its instanced meshes draw from poses, patched once and for good, after its own patches (its non-instanced meshes draw as before): call it o |
| `posedCopy` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A model with a batch's poses as its instanced attributes, bound at instance `at`: the model's own attributes and index shared (three keeps one buffer  |

### `src/render/buildings/partWarm.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: One instance of every part and piece of furniture as the buildings' layer draws them, each caster with its posed shadow (`partPose.ts` `partPoseDepth`: the renderer's stand-in takes a mesh's own depth)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `partWarmMeshes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One instance of every part and piece of furniture as the buildings' layer draws them, each caster with its posed shadow (`partPose.ts` `partPoseDepth` |

### `src/render/buildings/parts.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/furniture

**Para o jogador:** Modelos dos móveis no corte, miniaturas das peças. _(no 4.0: Pictures of the things the galleries offer - a model, a window, a roof, a stair)_

**Motivo:** Sem prova por símbolo até agora. Depende do editor.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ELEMENT_PARTS` | a portar | render/buildings/furniture |  |
| `FRAME_SHARE_MS` | a portar | render/buildings/furniture | Milliseconds of a frame the pictures may take, after the first of the frame. |
| `IDLE_MS` | a portar | render/buildings/furniture | How long the second context is kept once the queue is empty. |
| `KIT_HASH` | a portar | render/buildings/furniture | The fingerprint of the code the pictures are made by (`cook-plugin.ts`): pictures are kept under it. |
| `MODEL_KEYS` | a portar | render/buildings/furniture |  |
| `PER_FRAME` | a portar | render/buildings/furniture | Pictures per frame: enough to fill a row, small enough to hold 60 fps. |
| `ROOF_PARTS` | a portar | render/buildings/furniture |  |
| `RUN_PARTS` | a portar | render/buildings/furniture | The traced runs are their element, seen as the thing they lay. |
| `SIZE` | a portar | render/buildings/furniture | Pictures of the things the galleries offer - a model, a window, a roof, a stair |
| `ThumbnailStudio` | a portar | render/buildings/furniture |  |
| `WALL_PARTS` | a portar | render/buildings/furniture | Which BayComponent a gallery id stands for: most carry their own name. |
| `__BUILDING_KIT_HASH__` | a portar | render/buildings/furniture |  |
| `blank` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio.dispose` | a portar | render/buildings/furniture | Releases the second GPU context at once. |
| `createThumbnailStudio.hold` | a portar | render/buildings/furniture | While held (the Builder in use) the studio keeps its context between galleries |
| `createThumbnailStudio.keptPictures` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio.release` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio.request` | a portar | render/buildings/furniture | Photographs the ids that are worth a picture, a few per frame, calling back with each batch. |
| `createThumbnailStudio.setUp` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio.shoot` | a portar | render/buildings/furniture |  |
| `createThumbnailStudio.step` | a portar | render/buildings/furniture |  |
| `partSample` | a portar | render/buildings/furniture | The record a part is photographed from |

### `src/render/buildings/partyWalls.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/party_walls

**Para o jogador:** A parede encostada no vizinho não é desenhada. _(no 4.0: PARTY WALLS: a bay inside a neighbour's block, back to back with its wall, is never seen, so it is not laid (as `geometry.ts` `exposedParts` leaves out what one block of a building covers of another).)_

**Motivo:** Sem prova por símbolo até agora. Pendência do CLAUDE.md.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Cover` | a portar | render/buildings/party_walls | One block of a neighbour as a wall beside it reads it: its plan in the world, its box, and the heights its walls (and plinth) stand between. |
| `PARTY_INSET` | a portar | render/buildings/party_walls |  |
| `PARTY_NUDGE` | a portar | render/buildings/party_walls | How far out of a bay's face its neighbour's mass is looked for: two walls on one lot line, never a gap a view could go into. |
| `PARTY_REACH` | a portar | render/buildings/party_walls | Two boxes this close may stand back to back. |
| `PARTY_STEP` | a portar | render/buildings/party_walls | A bay is looked at this often along its width, its ends this far in. |
| `PartyWalls` | a portar | render/buildings/party_walls | The buildings of one job read as one another's neighbours: only those the job is handed, so a neighbour that changes changes the job's entries and the |
| `PartyWalls.blocks` | a portar | render/buildings/party_walls |  |
| `PartyWalls.boxes` | a portar | render/buildings/party_walls |  |
| `PartyWalls.constructor` | a portar | render/buildings/party_walls | `known`: each building's floor as an earlier job measured it (`CellEntry.floor`), not measured again. |
| `PartyWalls.coversNear` | a portar | render/buildings/party_walls | The other buildings' blocks whose box touches this one's. |
| `PartyWalls.coversOf` | a portar | render/buildings/party_walls |  |
| `PartyWalls.floorOf` | a portar | render/buildings/party_walls | A building's ground floor as `emitChunk` lays it (its natural ground, `floorHeight`), worked out once for the job. |
| `PartyWalls.floors` | a portar | render/buildings/party_walls |  |
| `PartyWalls.floorsKnown` | a portar | render/buildings/party_walls | Each building's floor as the job knows it now, in the job's order: null with no closed block, undefined where none was asked. |
| `hiddenByCovers` | a portar | render/buildings/party_walls | Whether a stretch of wall - from (x, y) along (tx, ty) for `width`, facing (nx, ny), from z0 to z1, all in the world - stands inside the covers, every |

### `src/render/buildings/referenceModel.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/furniture

**Para o jogador:** Modelos dos móveis no corte, miniaturas das peças. _(no 4.0: A reference model in the Builder: a real building's 3D model)_

**Motivo:** Sem prova por símbolo até agora. Depende do editor.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CAPTURE_DENSITY` | a portar | render/buildings/furniture | Pixels per metre of the pictures a capture reads its colours from. |
| `CAPTURE_MAX` | a portar | render/buildings/furniture |  |
| `DEPTH_MATERIAL` | a portar | render/buildings/furniture | Depth as two 8-bit channels (an orthographic camera's depth is linear). |
| `ReferenceCapture` | a portar | render/buildings/furniture |  |
| `ReferenceInfo` | a portar | render/buildings/furniture |  |
| `ReferenceModel` | a portar | render/buildings/furniture |  |
| `ReferenceTarget` | a portar | render/buildings/furniture | A reference model in the Builder: a real building's 3D model |
| `View` | a portar | render/buildings/furniture |  |
| `captureModel` | a portar | render/buildings/furniture | Reads the triangles and the colours of a model (a copy of it in metres, three's axes). |
| `captureModel.at` | a portar | render/buildings/furniture |  |
| `captureModel.plan` | a portar | render/buildings/furniture |  |
| `captureModel.sample` | a portar | render/buildings/furniture |  |
| `createReferenceModel` | a portar | render/buildings/furniture |  |
| `createReferenceModel.apply` | a portar | render/buildings/furniture |  |
| `createReferenceModel.capture` | a portar | render/buildings/furniture | The model as a surveyor reads it, in the frame of where it stands (the building it is placed on, or the origin) |
| `createReferenceModel.fade` | a portar | render/buildings/furniture |  |
| `createReferenceModel.flip` | a portar | render/buildings/furniture |  |
| `createReferenceModel.load` | a portar | render/buildings/furniture |  |
| `createReferenceModel.placeOn` | a portar | render/buildings/furniture |  |
| `createReferenceModel.remove` | a portar | render/buildings/furniture |  |
| `createReferenceModel.toggle` | a portar | render/buildings/furniture |  |
| `createReferenceModel.turn` | a portar | render/buildings/furniture |  |
| `toSrgb` | a portar | render/buildings/furniture |  |

### `src/render/buildings/roofPlan.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_props, roof_plant, roof_terrace

**Para o jogador:** Ar-condicionado, antenas, vasos, varais, equipamentos de cobertura. _(no 4.0: WHAT STANDS ON A ROOF, planned once for both levels of a building: the detailed one)_

**Motivo:** Sem prova por símbolo até agora. 

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `MACHINE_ROOM_INSET` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | How far a machine room keeps inside its roof's edge: off the parapet (0.25 m thick) by 5 cm. |
| `PARASOL_HALF` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A parasol's half width and the height of its canopy over the deck. |
| `PARASOL_HIGH` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `PARASOL_LOW` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `ROOF_COLOURS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The colours of a roof's pieces. |
| `TANK_COLOURS` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Water tanks: the blue fibreglass of most roofs, the grey and the white of others, a sand-coloured one. |
| `machineRooms` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The machine rooms over the lift cores, each on the highest flat roof over its core and kept inside the parapet |
| `roofPlantOf` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The plant on a flat roof (`planRoofPlant`): none on a block of roof details of its own or a shaped one. |
| `terraceDeck` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A used roof terrace's deck, in the block's plan; null when the terrace is too small to be used. |
| `terraceParasols` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The centres of the parasols over a terrace's tables, along the middle of its deck. |
| `terraceSeed` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A used roof terrace's seeded choices (`buildingMesh.ts` `emitTerrace`): 0..1 by the building, the block and `k`. |

### `src/render/buildings/roomBehind.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: THE ROOM BEHIND A WINDOW, as the building's own interior has it (`interiorAt`, the rooms the cut-open view draws))_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FloorKind` | portado | `native/src/sim/buildings/interior/interior.h` → `enum FloorFinish` | A floor's finish: 0 wood, 1 stone, 2 porcelain tiles. |
| `HEAD_SHADE` | portado | `native/src/render/buildings/facade_bay.cpp` → `HEAD_SHADE = 0.8f` |  |
| `INSIDE` | portado | `native/src/render/buildings/pane_rooms.cpp` → `INSIDE = 0.5f` | How far inside the wall the side walls are looked for. |
| `MOST_DEEP` | portado | `native/src/render/buildings/pane_rooms.cpp` → `MOST_DEEP = 12.f` | The farthest a wall is looked for: past it the room is closed at this depth. |
| `MOST_WIDE` | portado | `native/src/render/buildings/pane_rooms.cpp` → `MOST_WIDE = 10.f` |  |
| `PANE_FRAME_AT` | portado | `native/src/render/buildings/shell.h` → `float frame = 0;` | Where a pane's frame is written among its ROOM_FLOATS (`PartRows.paneFrame`): its trim's r, g, b and the frame's thickness (0: no frame). |
| `PANE_KEY` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() | Numbers a pane is looked up by: its face's start, along and out directions, its place on the face and its ceiling. |
| `PANE_RECESS_AT` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `o.depth` — o vidro do jogo já fica no fundo do vão, com ombreiras de verdade | Where a pane's recess is written (`PartRows.paneRecess`): how deep the glass stands behind the wall's face when the pane is laid on the  face and draw |
| `PaneMemo` | portado | `native/src/render/buildings/pane_rooms.h` → `struct PaneRooms` | The rooms worked out for a storey's panes, by their nine numbers: equal as numbers is the same pane (0 and -0 alike, as their text was). |
| `PaneMemo.add` | portado | `native/src/render/buildings/pane_rooms.h` → `known;` | `room` kept for `key` (just looked up and not found). |
| `PaneMemo.bits` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.find` | portado | `native/src/render/buildings/pane_rooms.h` → `known;` | The index of the room kept for `key`, or -1. |
| `PaneMemo.hash` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.key` | portado | `native/src/render/buildings/pane_rooms.h` → `known;` |  |
| `PaneMemo.keys` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.place` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.rehash` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.rooms` | portado | `native/src/render/buildings/pane_rooms.h` → `known;` |  |
| `PaneMemo.slots` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.words` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `REVEAL_SHADE` | portado | `native/src/render/buildings/facade_bay.cpp` → `REVEAL = 0.68f` | The tones of an opening's reveals against its trim, as the shell lays them (`buildingMesh.ts` emitBay) and a recessed pane draws them  (`glassRoom.ts` |
| `ROOM_FLOATS` | portado | `native/src/render/buildings/shell.h` → `struct PaneRoom` | THE ROOM BEHIND A WINDOW, as the building's own interior has it (`interiorAt`, the rooms the cut-open view draws) |
| `ROOM_OWN` | portado | `native/src/render/buildings/shell.h` → `struct PaneRoom` | The room's own numbers (`roomsOfStorey`): the first twelve of a pane's ROOM_FLOATS. |
| `SILL_SHADE` | portado | `native/src/render/buildings/facade_bay.cpp` → `SILL_SHADE = 0.92f` |  |
| `firstHit` | portado | `native/src/render/buildings/pane_rooms.cpp` → `float first_hit(` | Where a ray (origin, unit direction) first crosses one of the segments (x0, y0, x1, y1 each), or `far`. |
| `floorKind` | portado | `native/src/sim/buildings/interior/interior.h` → `FloorFinish floor_finish(` | A storey's floor (`buildingMesh.ts` `emitInterior` and the windows alike): stone where people pass |
| `roomsOfStorey` | portado | `native/src/render/buildings/pane_rooms.cpp` → `PaneRoom room_at(` | The rooms of one block's storey seen from its facade |
| `roomsOfStorey.roomAt` | portado | `native/src/render/buildings/pane_rooms.cpp` → `PaneRoom room_at(` |  |
| `seen` | portado | `native/src/render/buildings/pane_rooms.cpp` → `interior.seen[f.kind]` | What a pane shows as its piece: the solid ones, that read as a box from across a room |

### `src/render/buildings/roomLamps.ts`

**Estado:** a portar · **Dono no jogo:** building_glass.gdshader, building_far.gdshader, render/buildings/pane_rooms

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar. _(no 4.0: THE COLOUR OF A LIT ROOM, by the room: one lamp colour for every window of the town read as a sheet of amber at night)_

**Motivo:** Sem prova por símbolo até agora. Defeitos registrados: chapa ondulada e torres sem janela acesa.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `HOME_MIX` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `lot < 0.36` | The share of homes' lit rooms by lamp: warm 2700 K, 3000 K, the kitchen's 3500 K, cool white. |
| `KELVIN_SRGB` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_2700` | THE COLOUR OF A LIT ROOM, by the room: one lamp colour for every window of the town read as a sheet of amber at night |
| `OFFICE_MIX` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `lot < 0.5` | Offices and shops: neutral 4000 K, daylight 5000 K, a warmer 3500 K in some. |
| `ROOM_LAMP_GLSL` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `vec3 room_lamp(` | GLSL: `roomLamp(lot, office)`, a lit room's lamp colour from its lot (0..1) and whether it is an office or a shop; `roomLampMean(office)`, the mix's m |
| `TV_SHARE` | portado | `project/render/shaders/building_pane.gdshaderinc` → `TV_SHARE = 0.12` | The share of a home's lit rooms that are lit by a television alone. |
| `lampVec` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_6500` | A black body's colour at `kelvin`, linear, of luminance 1, as GLSL. |
| `meanGlsl` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_HOME_MEAN` | A mix's mean lamp, as GLSL: what its rooms' lamps average to where many windows fall in a pixel. |
| `mixGlsl` | portado | `project/render/shaders/building_lamps.gdshaderinc` → `vec3 room_lamp(` |  |

### `src/render/buildings/shellBatch.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE CELLS' WALLS, ONE MULTI-DRAW A MATERIAL)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Batch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `IDENTITY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.add` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's shells (every mesh of `SHELL_MESH` in its batch) taken into the batches; the meshes let go. |
| `ShellBatch.batches` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.boxes` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.castShadow` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The far cascades take the buildings by their masses alone: the walls cast nothing into them. |
| `ShellBatch.casting` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.cells` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.detailGroup` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.ensure` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.facadeGroup` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.faded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.hideOccluded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The colour pass about to be drawn from `camera`: the shells of every cell shown whose box `hidden` says is behind the walls laid  (`render/occlusion.t |
| `ShellBatch.makeBatches` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.makeBatches.make` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.remove` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's shells taken out: its geometries (and their instances) deleted from their batch. |
| `ShellBatch.reset` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every instance let go (a lost context's cells asked again): the batches made again, their meshes put back in the groups. |
| `ShellBatch.restoreOccluded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The colour pass drawn: every shell drawn again from any camera. |
| `ShellBatch.scratch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.setFaded` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.setVisible` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A cell's level shown or hidden (a cell still on another level is not drawn, and its pieces cost nothing). |
| `ShellBatch.squeeze` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellBatch.visible` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `ShellSet` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The three attribute sets a cell's shells come in: the detail level's plain walls, the facade level's plain ones, and its runs  of bays with openings ( |
| `castChanged` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A batch whose drawn pieces changed (one added, taken out, shown or hidden): its `castVersion` stepped, read by the shadows' signature of their still c |
| `worldGeometry` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The mesh's shell as world vertices: the far level's positions come quantized on the cell's grid, the mesh's place putting them  in the world (`buildin |
| `worldPositions` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A shell's stored positions (four integers a vertex, normalized to the cell's grid) into world positions (three a vertex): the integers  read straight  |

### `src/render/buildings/shellParts.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/part_models, facade_parts, building_parts.gdshader

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas. _(no 4.0: THE SHELL'S PARTS: THE LOTS' AND ELEMENTS' BOXES, DRAWN INSTANCED WITH THE SHELL'S OWN LOOK)_

**Motivo:** Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BOX` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | `Emitter.box`'s faces from (-0.5, -0.5, -0.5) to (0.5, 0.5, 0.5), in its order: the four sides, then the top (the bottom is never seen). |
| `COVER_INSET` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | How far a paving's sides stand in from its top's edge (`Emitter.slab`): a wall or a gate on that edge keeps the face there. |
| `GRIME_REACH` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | How far above the ground floor a wall's weathering at the base reaches. |
| `GROUND_ATTRIBUTE` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The attribute of the ground a shell part's walls darken towards (`GRIME_REACH`), per instance. |
| `H` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `LOOK_FLOATS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Numbers an instance of a shell part carries besides its matrix and colour: its finish id, its wear and the ground its walls darken towards. |
| `MACRO_SCALE` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | World units over which a facade's tone drifts. |
| `NO_GROUND` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A ground so far down no wall darkens: what a part of a building with no ground floor takes (`Shell.ground` NaN). |
| `SHELL_PARTS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `SHELL_PART_BEGIN` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL after `begin_vertex`: a paving's sides stand `COVER_INSET` in from its top's edge whatever its size (its model's uv.x marks them). |
| `SHELL_PART_FACES` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Each shell part's model, face by face: the box; the rail of `emitFence` (its two sides, top and bottom, along u, no ends); the paving of `Emitter.slab |
| `SHELL_PART_KINDS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |
| `SHELL_PART_PARS` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL of the shell's vertex shader drawn instanced (`finishes.ts`): `macroNoiseInto` to the bit (Math.imul is a uint product, >>> a uint shift). |
| `SHELL_UV_GLSL` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL: `shellUv`, from the vertex in the world and its normal there (three's axes). |
| `ShellPartFace` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A face of a shell part's model, as `Emitter.box` (or `emitFence`) laid it: its corners in order and its normal; `inset`: a paving's side (`COVER_INSET |
| `ShellPartKind` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The kinds drawn by the shell's material: a box (sides and top), a fence's rail (sides, top and bottom), a paving (a box whose sides stand in),  a box  |
| `Unit` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A point of a shell part's model in its own plan: along (u, the building's x), across (v, its y) and up (w), each -0.5..0.5. |
| `glsl` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A number as a GLSL float literal. |
| `lattice` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The lattice value of `macroNoiseInto` from a corner's three terms (`Math.imul(i / 0, 0x27d4eb2d)`, the j's and the k's), 0..1: integers in, so no numb |
| `macroNoiseInto` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | Smooth 3D value noise, 0..1, for tone that drifts over a whole facade: the point read from `io[0..2]`, the noise written in `io[3]`.  DO NOT pass the  |
| `shellPartError` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The screen error of a shell part's top-only level, model units, from an instance's scales [x, y, z] at `o`: the smaller side of the largest face its s |
| `shellPartVertex` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL after `worldpos_vertex`: what `Shell.face` lays on each vertex, from the vertex in the world: the finish's planar uv (along the face, up it), the |
| `shellUv` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A shell vertex's planar uv, metres along its face and up it, from its place and its face's normal in three's axes (x, y up, -z north): `SHELL_UV_GLSL` |

### `src/render/buildings/signature.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/civic*, render/buildings/signature

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas). _(no 4.0: Signature buildings: a building of the tower kit drawn with parts of its own)_

**Motivo:** Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DESIGNS` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Design` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `FACADE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `FacadeKind` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `LOOKS` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Mat` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A material's name: the shared ones of `makeMaterials`, or a facade made by `facade`. |
| `Painter` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Collects parts by material, in metres of the building's frame. |
| `Parts.Y` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts.box` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A box from (x0, y0, z0) to (x1, y1, z1), UVs in metres so textures tile at their real size. |
| `Parts.bush` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A shrub on the ground. |
| `Parts.byMat` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts.cone` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A clipped conical shrub (a cypress in a pot), as either side of the door. |
| `Parts.cylinder` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts.cypress` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A cypress standing in the ground. |
| `Parts.decal` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A painted panel - a sign, a clock, a crest - on a face: 'front' looks -y, 'left' -x, 'right' +x. |
| `Parts.dome` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A dome (half a sphere, `squash` its height over its radius). |
| `Parts.ellipse` | a portar | sim/buildings/b40/civic*, render/buildings/signature | An upright elliptic prism (a rounded tower, a slab edge round it), UVs in metres round and up. |
| `Parts.flag` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A flagpole with its flag (painted by `paint`). |
| `Parts.gable` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A triangular prism: a gable roof |
| `Parts.gable.Yf` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts.hip` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A hipped roof (a frustum on a rectangle, `top` its ridge fraction; 0 a pyramid). |
| `Parts.lamp` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A street lamp: a post and a glowing lantern. |
| `Parts.mirror` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Draws mirrored front to back about y = mirror / 2 (the back face), or null. |
| `Parts.plant` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A pot with a bushy plant. |
| `Parts.prism` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A regular prism of `seg` sides (3 a triangle, 4 a square, 6, 8...), turned `rot` radians, UVs in metres. |
| `Parts.push` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Parts.pushRaw` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Adds a ready geometry (a cone, a spire) in metres of three's frame. |
| `Parts.rail` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A railing panel along x (or y) at height z: an alpha-cut screen of bars. |
| `Parts.steps` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A flight of `n` steps rising back from the front edge `y0` (towards +y). |
| `Parts.tree` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A tree: a trunk and a full crown. |
| `Parts.windowFront` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A window or glazed door on a face looking -y, at depth y: a dark frame round warm glass. |
| `Parts.windowSide` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A window on a side face (looking -x or +x) at x. |
| `TowerLook` | a portar | sim/buildings/b40/civic*, render/buildings/signature | How one of the towers of the reference sheets is made. |
| `archDoor` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `archOpening` | a portar | sim/buildings/b40/civic*, render/buildings/signature | An open arch in a stone face: a belfry's or a loggia's, dark inside. |
| `badge` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `balconyMid` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Image 1: balconies on both wings, a timber column with the stairs, a stone base, a roof terrace. |
| `balustrade` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A balustrade along x at height z: a rail, balusters, a plinth. |
| `bank` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 8. A bank: a stone block, a portico of four fluted columns under a pediment with its name, gold doors, a banner, planters. |
| `banner` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `brazilFlag` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `buildSignature` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The body of a signature building, placed in the world: `floor` is the height of its ground floor |
| `buildSignature.solid` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The faces another part covers, or that lie on the ground facing down, taken out before the parts are merged (`hiddenFaces.ts`): a step inside the next |
| `canvasTexture` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `cemetery` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 13. A cemetery: a stone wall with iron railings and an iron gate, gravel paths, hedged plots of graves, mausoleums, cypresses, a chapel. |
| `church` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 7. A church: a stone nave under a tiled gable roof, a rose window and arched portal, a bell tower with a tiled dome, an apse. |
| `cityHallBrick` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 1. A town hall in red brick and stone: arched windows, a pedimented centre, a slate roof with dormers, a clock tower under a copper dome. |
| `cityHallWhite` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 5. A grand white town hall: a portico of columns with a pediment, a tall clock tower and cupola, corner pavilions, banners and statues. |
| `clockFace` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `colonnade` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Columns along x on a portico: `n` of them between x0 and x1 at depth y, on `z`, `h` tall. |
| `cubeBalcony` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 5. Concrete with orange-framed windows, glass box balconies standing out in a chequer. |
| `decoSpire` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 2. An art deco skyscraper: setbacks of beige stone with gold strips, pinnacles, a golden crown and spire. |
| `disposeSignature` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `drawsSignature` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Whether this module draws the building's body. |
| `facade` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A facade drawn as a picture of its bays - panes and mullions, spandrels, punched windows in a wall |
| `facade.archWindow` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `facade.brickWall` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `facade.glass` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Daylight glass: dark, reflective, a faint warmth from the rooms behind. |
| `facadeMaterialKey` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `facadeUses` | a portar | sim/buildings/b40/civic*, render/buildings/signature | What a facade use key lays: its style and the uv rhythm baked into its pieces. |
| `facades` | a portar | sim/buildings/b40/civic*, render/buildings/signature | One material a facade style: the UV rhythm (bays, storeys, base) is baked into each piece's uv when it is pushed, so a building does not need a materi |
| `fireStation` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 10. A fire station: three red bay doors, its name and crests, a red brick command block, a training tower with stairs, an apron. |
| `forum` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 6. A courthouse: a stone block of tall windows, a portico of four columns under a pediment with the scales, flags, planters. |
| `garageDoor` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `helipad` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `hexTerracotta` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 2. A hexagon in terracotta with arched windows, white string courses, a lantern on top. |
| `hospital` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A hospital: a glass centre in a white frame, wings, an emergency canopy, a helipad, plant. |
| `hospitalA` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `hospitalB` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `ironGate` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `lettering` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Lettering on a panel: `bg` null for letters alone. |
| `loftBrick` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 4. Brick lofts: black shopfronts with awnings, big black-framed windows, terraces stepping back, a water tower. |
| `makeMaterials` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `makeMaterials.ashlar` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Surfaces that read as stone, concrete, roofing, tiles: never flat colour (flat light planes blow out to white). |
| `makeMaterials.chevrons` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Art Deco spandrel: gilded chevrons on dark bronze. |
| `makeMaterials.courses` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `makeMaterials.decoPanel` | a portar | sim/buildings/b40/civic*, render/buildings/signature | An Art Deco gilded panel: a stepped sunburst and zigzags. |
| `makeMaterials.rusticated` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Rusticated stone: 0.6 m courses with deep shadowed joints, long blocks. |
| `makeMaterials.seams` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A standing-seam copper roof: vertical seams 0.5 m apart. |
| `makeMaterials.speckle` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `makeMaterials.std` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `materials` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `octCopper` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 7. An octagon of glass behind copper fins, narrowing at the top under a copper spire. |
| `pinkCorner` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 9. Pink flats on an L with a round corner, white-framed windows, corner balconies with black rails. |
| `plainFlag` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `podium2` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A two-storey podium in `mat`, glazed all round, its canopy, planters and trees; returns its top. |
| `police` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 11. A police station: grey concrete, a black central block with the badge and name, louvres, a garage of patrol cars behind a gate. |
| `redCross` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `roofPlant` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Plant on a flat roof: a plant room, units, a tank. |
| `roofRing` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A flat roof's parapet in `mat` from z0 to z1 round the rectangle, and the membrane deck inside it at `deck` |
| `roseWindow` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `scales` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `school` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 9. A school: two storeys of brick with white-framed windows, a stone entrance with a clock and its name, a flag, a playground. |
| `seedOf` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A seed from a name (FNV-1a): the same pictures in every run. |
| `stackedBlocks` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 8. Blocks of four floors stacked out of line, black and white in turn. |
| `stepGarden` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 3. A ziggurat of terraces every four floors, white ribbon windows, gardens and glass rails on each step. |
| `stripedFlag` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `towerOf` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A tower of the reference sheet: podium, shaft, crown and roof, in its own look. |
| `towerOf.shaft` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `triangleDark` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 10. A triangle of dark glass, white frame bands, a narrower crown and a mast. |
| `twinNavy` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 6. Twin navy towers with gold bands, joined by a glass sky bridge. |
| `twistGreen` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 1. A square tower turning a little at every floor, teal glass, white slab edges. |
| `waveWhite` | a portar | sim/buildings/b40/civic*, render/buildings/signature | 4. White slab balconies that wave in and out, floor after floor, glass rails, blue glass behind. |

### `src/render/buildings/thinParts.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/part_models, facade_parts, building_parts.gdshader

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas. _(no 4.0: A geometry seen from both sides: its faces and a copy of them turned the other way (winding and normals reversed), indexed (DO NOT `toNonIndexed` it first: each instance of a far railing or brise drew)_

**Motivo:** Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CARD` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The cards' patterns: a window frame, a roof's railing, a balcony railing's front and its sides. |
| `CARD_ATTRIBUTE` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A part's card (`kit.ts` cards): which pattern its single face draws, per vertex (0 on every other geometry: WebGL's default for a missing attribute). |
| `CARD_GLSL` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | GLSL: whether a card's point lies on one of its bars, each at least a pixel wide (`fw`, the coordinate a pixel spans): a bar thinner than a pixel is d |
| `prefilterThinParts` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | The thin parts' materials pre-filtered (see below), set once on the kit's materials. |
| `twoSided` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader | A geometry seen from both sides: its faces and a copy of them turned the other way (winding and normals reversed), indexed (DO NOT `toNonIndexed` it f |
| `twoSided.at` | a portar | render/buildings/part_models, facade_parts, building_parts.gdshader |  |

### `src/render/buildings/townColumns.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: WHAT THE BUILDINGS LAYER KEEPS OF EACH BUILDING, AS NUMBERS IN COLUMNS BY ITS ID (`layer.ts`): its digests, its place and box, its cell and what it flies; never its record (report "Arquitetura para ci)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FILED` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Filed in the town (`TownIndex`), in the cell `filedI`, `filedJ`. |
| `FLIES` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | It flies a flag (`buildingMesh.ts` `fliesFlag`): its summary is asked even far (the flags). |
| `KEPT` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the workers emitted of it is kept (`keptSig`, `keptDigest`). |
| `KNOWN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Set once the record's numbers are in its columns (`file`); cleared when the diary names it (`stale`). |
| `SHOWN` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Shown by the layer (not hidden by the preview nor a ruin): its flags read from what is kept of it. |
| `SIGNATURE` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A signature building (`signature.ts` `drawsSignature`): its body drawn with parts of its own. |
| `TownColumns` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.cellOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The coarse cell an id is filed in (`BATCH_CELL`), from its anchor. |
| `TownColumns.clear` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every id let go (another document, the whole town filed again). |
| `TownColumns.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.drop` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | An id gone from the document: its slot handed back. |
| `TownColumns.file` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A record's numbers put in its columns (what was kept of it and whether it is shown stay). |
| `TownColumns.filedI` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The cell it is filed in (`FILED`): where it stood when last filed, whatever its record says now. |
| `TownColumns.filedJ` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.flags` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.forEach` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every id held, with its slot. |
| `TownColumns.groundMaxX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.groundMaxY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.groundMinX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The box of the ground it reads (`storedBounds` grown by its module twice: `layerCells.ts` `groundDigest`). |
| `TownColumns.groundMinY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.has` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.height` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.idOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The id held at a slot. |
| `TownColumns.ids` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The id held at each slot. |
| `TownColumns.keptDigest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.keptSig` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | What the kept summary was made of (`KEPT`): its record's and its ground's digests. |
| `TownColumns.known` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether an id's numbers are those of its record now (filed since the diary last named it). |
| `TownColumns.massSig` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.maxX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.maxY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.members` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.minX` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Its stored box (`storedBounds`), and its height over its foot. |
| `TownColumns.minY` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.set` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.sig` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Its record's digest, with and without its wear. |
| `TownColumns.size` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.slotOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The slot of an id; -1 when it has none. |
| `TownColumns.slots` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownColumns.stale` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The record of an id changed (the diary named it): its numbers read again when next asked. |
| `TownColumns.x` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The anchor (the cell is worked out of it). |
| `TownColumns.y` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `memberDigest` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each member object's digest, once (`TownColumns.members`): a record's members are shared with other buildings (`intern.ts`, `SharedMembers`:  a storey |
| `recordDigests` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The digest of a record with its wear and without it (`massing.ts` reads no wear): what its detail and its masses are made of.  DO NOT walk the whole r |

### `src/render/buildings/townIndex.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: The buildings the layer shows, by id and by coarse cell, kept from one change to the next: ids, never records (`townColumns.ts`).)_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CellBatches` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Where a cell's batches are kept and shown. |
| `TownIndex` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The buildings the layer shows, by id and by coarse cell, kept from one change to the next: ids, never records (`townColumns.ts`). |
| `TownIndex.byCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownIndex.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownIndex.file` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownIndex.has` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `TownIndex.reset` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every shown building filed again (the whole town: the opening, a map opened); `shown`, their slots (their columns filed). |
| `TownIndex.take` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A building as it is now taken in (`slot`: its columns filed; -1: gone, or not shown); the cells it left and entered marked in `dirty`. |
| `patchCell` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One cell's batches swapped for those landed for it (`cellRequests.ts`), its parts with them; none |
| `signatureBodies` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The signature buildings' bodies |
| `signatureBodies.keepOnly` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Every body let go but those of the ids `keep` keeps. |
| `signatureBodies.remove` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `signatureBodies.set` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | `sig`: its record's digest (`townColumns.ts`): the body made again only when it or its floor changed. |
| `signatureBodies.top` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The highest point of any body (`massCells.ts` `top` for the rest); -Infinity with none. |

### `src/render/canyonLight.ts`

**Estado:** não se aplica · **Dono no jogo:** render_world, city_world.gd

**Para o jogador:** Montagem do renderizador do 4.0 (fatia).

**Motivo:** Pontos de encaixe; luz de cânion é de landscape/pós.


### `src/render/carBody.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/render/destruction.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência.

**Motivo:** Funcionalidade demolition da fila.


### `src/render/flatAtlas.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/fogLayer.ts`

**Estado:** a portar · **Dono no jogo:** -

**Para o jogador:** -

**Motivo:** Sem prova por símbolo até agora. Classificado ao ler.


### `src/render/gardenLists.ts`

**Estado:** a portar · **Dono no jogo:** render/trees/tree_place (b40::lot_trees)

**Para o jogador:** Árvores e canteiros nos lotes. _(no 4.0: DO NOT hand the gardens every planted building's list at every change)_

**Motivo:** Sem prova por símbolo até agora. Pendência do CLAUDE.md: árvores do lote.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GardenLists` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `Site` | a portar | render/trees/tree_place (b40::lot_trees) | A planted building: its plants digested, its bank, and its plants on the ground (once placed) with the ground change they follow. DO NOT keep its reco |
| `gardenLists` | a portar | render/trees/tree_place (b40::lot_trees) | `ground`: where and when the drawn ground changed |
| `gardenLists.check` | a portar | render/trees/tree_place (b40::lot_trees) | A placed site the ground moved under since it was placed: placed again. |
| `gardenLists.follow` | a portar | render/trees/tree_place (b40::lot_trees) | The whole store read only the first time or when the diary cannot say which buildings changed (a map loaded or cleared). |
| `gardenLists.groundMoved` | a portar | render/trees/tree_place (b40::lot_trees) | @rebuild The banks a change of the ground since the last look reached; every placed bank only when the record cannot say where. |
| `gardenLists.plantKeyOf` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `gardenLists.take` | a portar | render/trees/tree_place (b40::lot_trees) | A building's record taken in. |
| `gardenLists.takeAll` | a portar | render/trees/tree_place (b40::lot_trees) | @rebuild Every building of the store and every site taken in again: the first time, or when what changed cannot be said. |
| `gardenLists.update` | a portar | render/trees/tree_place (b40::lot_trees) | The lists to place and to take away since the last call, null when none. |

### `src/render/gardens.ts`

**Estado:** a portar · **Dono no jogo:** render/trees/tree_place (b40::lot_trees)

**Para o jogador:** Árvores e canteiros nos lotes. _(no 4.0: DO NOT go back to rebuilding the whole town's gardens at every building.)_

**Motivo:** Sem prova por símbolo até agora. Pendência do CLAUDE.md: árvores do lote.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GARDEN_SLACK` | a portar | render/trees/tree_place (b40::lot_trees) | How far past a building's bounds its garden's placings may stand: a hedge's clipped piece, a flower bed's clump (both within its element, rounded). |
| `GardenCells` | a portar | render/trees/tree_place (b40::lot_trees) | THE GARDENS HELD ROUND THE VIEW (`propCells.ts`): placed cell by cell from the buildings of the document, near whole, far as their pictures. |
| `GardenChange` | a portar | render/trees/tree_place (b40::lot_trees) | The buildings' garden lists to place and to take away (`gardenLists.ts`): a list is placed once and taken away by the same object. |
| `GardenPlant` | a portar | render/trees/tree_place (b40::lot_trees) | A plant of a building's garden, in the world: a tree, a shrub, a run of hedge or a flower bed. |
| `Gardens` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `KIND_KEYS` | a portar | render/trees/tree_place (b40::lot_trees) | The kinds of plant, in the order of the gardens' kinds: the trees by species, then the bushes. |
| `KIND_OF` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `POSE` | a portar | render/trees/tree_place (b40::lot_trees) | Floats a plant's pose takes (`YAW_POSE`): its place in the scene's frame (x, y, z), its yaw and its scale on three axes. |
| `Placement` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardenCells` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardenCells.cull` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardenCells.dispose` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardenCells.follow` | a portar | render/trees/tree_place (b40::lot_trees) | The diary's changes of the buildings and the ground since the last look (when the ground is settled: a stroke over). |
| `createGardenCells.setMap` | a portar | render/trees/tree_place (b40::lot_trees) | DO NOT drop the cull here unconditionally |
| `createGardenCells.source` | a portar | render/trees/tree_place (b40::lot_trees) | The buildings and the ground the gardens are placed from: the cells placed from another store all dropped. |
| `createGardens` | a portar | render/trees/tree_place (b40::lot_trees) | Every building's garden in one layer, given its lists (`gardenLists.ts`): what a test of the whole map places to compare. |
| `createGardens.cull` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardens.dispose` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardens.exclude` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `createGardens.setMap` | a portar | render/trees/tree_place (b40::lot_trees) | DO NOT drop the cull here unconditionally |
| `createGardens.setNear` | a portar | render/trees/tree_place (b40::lot_trees) | A plant's level follows its size on the screen (`cellInstances.ts`). |
| `createGardens.update` | a portar | render/trees/tree_place (b40::lot_trees) | DO NOT hand it the town's lists to find what changed: that was a pass over the town per building grown. |
| `gardenKinds` | a portar | render/trees/tree_place (b40::lot_trees) | The gardens' kinds (`KIND_KEYS`): each plant's body and leaf cards near, its picture far, chosen by its size on the screen. |
| `gardenKinds.kind` | a portar | render/trees/tree_place (b40::lot_trees) | A long hedge in steps, each on the ground under it, not one box at its middle's height. |
| `gardenPack` | a portar | render/trees/tree_place (b40::lot_trees) | One garden's plants placed, by plant kind (`KIND_KEYS`): each one's pose (DO NOT keep 16-float matrices of every plant of the town), tint and largest  |
| `gardenPack.pack` | a portar | render/trees/tree_place (b40::lot_trees) |  |
| `gardenPacksAbout` | a portar | render/trees/tree_place (b40::lot_trees) | The plants of the gardens standing about a rectangle (at least every one in it): the buildings whose bounds (their elements' too) meet it, each one's  |
| `gardenPictures` | a portar | render/trees/tree_place (b40::lot_trees) | Each garden kind's picture among the scenery's (`KIND_KEYS` order). |
| `gardenPlants` | a portar | render/trees/tree_place (b40::lot_trees) | The plants of every building's garden, in the world, on the ground they stand on. |
| `gardenSpecies` | a portar | render/trees/tree_place (b40::lot_trees) | A garden tree's species: mostly broadleaf, some conifers, now and then a flowering ipê. |

### `src/render/gpuLedger.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GpuLedger.roomFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | src/render/gpuLedger.ts:62-66 method GpuLedger.roomFor: the feature's own |

### `src/render/groundChanges.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GroundChanges.touches` | não se aplica | core, JobPool, RenderWorld | src/render/groundChanges.ts:38-54 method GroundChanges.touches: the feature's own |
| `isRect` | não se aplica | core, JobPool, RenderWorld | src/render/groundChanges.ts:72-72 function isRect: the feature's own |
| `overlaps` | não se aplica | core, JobPool, RenderWorld | src/render/groundChanges.ts:74-74 function overlaps: the feature's own |

### `src/render/groundShape.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/groundShapePool.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/interiorSolids.ts`

**Estado:** outro porte · **Dono no jogo:** Porte 8 (vista do interior)

**Para o jogador:** Ver dentro dos prédios: a camada Interior e o corte do prédio acima de um andar. _(no 4.0: THE SOLIDS OF A VEHICLE'S INTERIOR, BY NAME.)_

**Motivo:** Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build. A aba Interior do Construir fica no build e chama a visão pelo RenderWorld.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CabinParts` | outro porte | Porte 8 (vista do interior) | The cabin's parts as they are added, each named; merged by the caller in the same order. |
| `CabinParts.add` | outro porte | Porte 8 (vista do interior) | Adds parts of one kind, belonging to `seats` (a seat, several, or none). |
| `CabinParts.geometries` | outro porte | Porte 8 (vista do interior) |  |
| `CabinParts.named` | outro porte | Porte 8 (vista do interior) |  |
| `CabinParts.solids` | outro porte | Porte 8 (vista do interior) | The solids as index ranges of the merge of `geometries` and then `after` (a closed interior's door cards): one a piece. |
| `CabinParts.solids.take` | outro porte | Porte 8 (vista do interior) |  |
| `InteriorSolid` | outro porte | Porte 8 (vista do interior) | One closed piece of the cabin (a box, an extruded profile, a bar), named |
| `InteriorSolidKind` | outro porte | Porte 8 (vista do interior) | THE SOLIDS OF A VEHICLE'S INTERIOR, BY NAME. |

### `src/render/lotLattice.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays

**Para o jogador:** Editar lotes: dividir, juntar, acrescentar, apagar, definir frente; camada dos lotes. _(no 4.0: DO NOT go back to triangulating each lot and laying every vertex on the ground)_

**Motivo:** Sem prova por símbolo até agora. Aba Zonas › Lotes está 'em construção'.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `AA_MARGIN` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | How far past a cell an edge is still listed in it, beyond its stroke's half width |
| `DASH` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A dashed line: dash and gap, metres. |
| `FLAG_BUTT` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `FLAG_DASHED` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `FLAG_FILL` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `FLAG_INSIDE` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `FLAG_LEVEL` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `FLAG_STROKE` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `GAP` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LOT_CELL` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Metres a side of a lattice cell: a few shapes each where lots are smallest. |
| `LatticeShape` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | One thing drawn on the ground. |
| `LotLattice` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.add` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape's records written into every cell it reaches; the cells rewritten at `flush`. |
| `LotLattice.alloc` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.blocks` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.cellShapes` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.cellsPerSide` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.cellsWritten` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Cells whose records were written since made (a probe of the work an edit costs). |
| `LotLattice.clear` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Every shape gone, the texture empty. |
| `LotLattice.compact` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.constructor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.data` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The texture's texels, four uint32 each; replaced when it grows. |
| `LotLattice.dirty` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.flush` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The cells shapes were added to or taken from, written again in drawing order. |
| `LotLattice.grow` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.live` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.mark` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.placed` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.records` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.remove` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape taken out of the cells it was in. |
| `LotLattice.reorder` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape held moved in the drawing order (`order`): its cells written again in the new order, its records kept (they do not depend on the order). The s |
| `LotLattice.reserve` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.rows` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.setEntry` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.setTable` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.size` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.streams` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.tableTexels` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.takeUpload` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What to send to the GPU since the last call: everything  DO NOT send each range as written: a big edit wrote hundreds to thousands of one-texel entrie |
| `LotLattice.tilesPerSide` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.top` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.wholeChanged` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.writeCell` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.written` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.x0` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotLattice.y0` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `Placed` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape as the lattice keeps it: the records it wrote, by cell. |
| `REFERENCES` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Points tried as a cell's reference, metres from its centre |
| `REFERENCE_CLEAR` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The least a reference keeps from every edge (metres): its side is never in doubt. |
| `Stream` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Where a cell's stream lies. |
| `TEXTURE_WIDTH` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Texels a row of the texture. |
| `TILE_CELLS` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Cells a side of a tile: the index is kept only where there are shapes. |
| `UPLOAD_GAP` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Ranges of one row closer than this many texels go up as one (`takeUpload`). |
| `UPLOAD_RANGES` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | More ranges than this after joining: the whole texture goes up, one call. |
| `f32` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `floatBits` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `halfBits` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | IEEE half float bits of `v` (round to nearest even; no NaN). |
| `insideEdges` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Even-odd: whether (px, py) is inside the ring the edges close (half-open crossing rule). |
| `segmentDistance` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Distance from point (px, py) to segment (ax, ay)-(bx, by). |
| `segmentNearBox` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Whether segment (ax, ay)-(bx, by) comes within `r` of the box [x0, y0]-[x1, y1]. |
| `segmentNearBox.clip` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `u32` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |

### `src/render/lotOverlay.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays

**Para o jogador:** Editar lotes: dividir, juntar, acrescentar, apagar, definir frente; camada dos lotes. _(no 4.0: DO NOT go back to geometry laid on the ground per lot; DO NOT hand every lot over again when one changes)_

**Motivo:** Sem prova por símbolo até agora. Aba Zonas › Lotes está 'em construção'.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `<ExpressionStatement>` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `<ExpressionStatement>@59` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `BAND` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape's order: its band (polygons, lines, dots), its group's rank, its place in the group. |
| `CELL_SHIFT` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The world's lattice cells each way a level's key holds (`createLotOverlay` `cellKey`): past the world's reach (`world/bounds.ts` WORLD_HALF, 2^25 m) i |
| `CELL_SIDE` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `EMPTY` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The lattice's texture when there is nothing: an integer texture still, for the `usampler2D`. |
| `FACING_UP` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A building's surface facing up at least this much (world normal's y): a wall's never is. |
| `FRAME_HALF` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Half the lattice's smallest frame: a plate and the ring round it (6,144 m a side), wherever its middle is on the plate |
| `GROUND_KINDS` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What moves the ground a built lot's level is read from (`surfaceHeightAt`): its level is read again there. |
| `Keyed` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What one shape of a group is, its key: the same key, the same shape; `band` 0 polygons, 1 lines, 2 dots. |
| `LEVEL_REACH` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | How far above or below a built lot's ground level a building's level surface is still its yard (metres) |
| `LOT_PARS` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `LotOverlayGroup` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | One group's shapes and its place in the drawing order: in each of the polygons, the lines and the dots, a higher rank over a lower. |
| `LotOverlayInput` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | DO NOT go back to geometry laid on the ground per lot; DO NOT hand every lot over again when one changes |
| `LotOverlayShapes` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | What is drawn of one group. |
| `MAX_RANK` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A group's rank stays under this: every shape's order is then an exact double. |
| `RANK` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `UNIFORMS` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Shared by every ground material (`applyLotOverlay`), set by the overlay: the lattice's texture and its frame |
| `applyLotOverlay` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Makes a material colour the lots into itself |
| `createLotOverlay` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | `groundAt`: the ground's height (`surfaceHeightAt`), read once for each built lot, at its middle. |
| `createLotOverlay.cellKey` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A lattice cell's key on the world's own grid of `LOT_CELL` (not the frame's: a level read stays filed when the frame moves). |
| `createLotOverlay.cellOf` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `createLotOverlay.dispose` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `createLotOverlay.forgetLevels` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The levels read where the ground moved, forgotten: the groups that read them ('all' with no rectangles). |
| `createLotOverlay.leaves` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Whether a named group laid is not wholly in the frame: only then the frame is stood again over them all (the groups held are in it). |
| `createLotOverlay.levelOf` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A shape's ground level: the ground at its points' mean (a built lot's: under its building, its pad), to the centimetre. |
| `createLotOverlay.remake` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A group's shapes laid again (gone when `group` is undefined): a shape with the same key kept as it is. |
| `createLotOverlay.set` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Draws `input` (null: no lots) into the ground from the next frame. |
| `createLotOverlay.standFrame` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The frame stood again over the lots drawn when they left it, on the plate under their middle (`world/bounds.ts`): every shape laid anew in it. |
| `createLotOverlay.upload` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The lattice's writes sent to its texture: in place, or a texture of the new size. |
| `frameHalfFor` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | Half the frame stood on a plate's centre (cx, cy) that holds [x0, y0, x1, y1]: the smallest by whole plates, never one sized to a map |
| `frameUniforms` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The shader's frame set to a lattice's: its corner, its cells and its tiles a side. |
| `groupBox` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | The box round a group's shapes; null for none. |
| `groupBox.grow` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays |  |
| `rgba` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A colour (0xRRGGBB, sRGB) and an alpha as the lattice keeps them: RGBA8, red lowest. |
| `shapesOf` | a portar | sim/commands/lot_commands, project/game/tools/lot_edit_tool.gd, render/overlays | A group's shapes, keyed, in drawing order: the polygons, then the lines, then the dots. |

### `src/render/materials.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/mesh/farBlockBatches.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona. _(no 4.0: THE ROAD BLOCKS' FAR LEVELS DRAWN IN BATCHES, ONE A LOOK (material, order, shadows, attributes))_

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ADOPTED` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Batch` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `Entry` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | One far level adopted: where its geometry is in its batch, and the geometry stored there (another one on the mesh: stored again). |
| `FarBlockBatches` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.adopt` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A block's far level drawn by its look's batch from now on: never drawn itself, its geometry copied there at the next `sync`. |
| `FarBlockBatches.batchFor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.batches` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.constructor` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.dispose` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.ensure` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.entries` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.group` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.refill` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The batches' buffers lost with the WebGL context: every level stored again from its arrays (`gpuOnly.ts` refillArrays gave them back). |
| `FarBlockBatches.release` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.rootOf` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.shownIn` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.store` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs |  |
| `FarBlockBatches.store.gone` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Taken out of the batch with its geometry: let go by the residency or retired with its block. |
| `FarBlockBatches.sync` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Each far level's instance shown as the level is (in the scene, its parents shown), its geometry stored again where the mesh has a new  one (`restoreFa |
| `batchedFar` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | Whether a far level is drawn by its batch (`FarBlockBatches.adopt`): nothing to upload or warm for it. |
| `letGoArrays` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | A geometry's arrays let go once in its batch, their size kept as `gpuOnly.ts` keeps it (`memoryCounters.ts`, `gpuLedger.ts`). |
| `uncounted` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | The batch's storage left out of the memory counters (`memoryCounters.ts`): its bytes are counted on the far levels it holds, as  `gpuOnly.ts` keeps th |

### `src/render/mesh/hiddenFaces.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `dropFacing` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | src/render/mesh/hiddenFaces.ts:99-129 function dropFacing: the feature's own |

### `src/render/mesh/roadResidency.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.


### `src/render/networkFarRoads.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/occlusion.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/playEffects.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência.

**Motivo:** Funcionalidade demolition da fila.


### `src/render/postprocess.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/renderer.ts`

**Estado:** não se aplica · **Dono no jogo:** render_world, city_world.gd

**Para o jogador:** Montagem do renderizador do 4.0 (fatia).

**Motivo:** Pontos de encaixe; luz de cânion é de landscape/pós.


### `src/render/roadSurfaces.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/roomLamps.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/interior

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte. _(no 4.0: The room lights of the floors cut open (the building tool's cutaway))_

**Motivo:** Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `RoomLamps` | a portar | sim/buildings/interior | The room lights of the floors cut open (the building tool's cutaway) |
| `RoomLamps.floors` | a portar | sim/buildings/interior |  |
| `RoomLamps.lamps` | a portar | sim/buildings/interior |  |

### `src/render/scenery.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL. _(no 4.0: Grass tufts and wildflowers, shown only at close zoom.)_

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Scenery` | não se aplica | core, JobPool, RenderWorld | src/render/scenery.ts:346-360 interface Scenery: the feature's own |

### `src/render/sharedGround.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `groundAround` | não se aplica | core, JobPool, RenderWorld | src/render/sharedGround.ts:101-109 function groundAround: the feature's own |

### `src/render/surfaceTextures.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `surfaceMaps` | não se aplica | core, JobPool, RenderWorld | src/render/surfaceTextures.ts:84-89 function surfaceMaps: the feature's own |

### `src/render/terrain.ts`

**Estado:** não se aplica · **Dono no jogo:** core, JobPool, RenderWorld

**Para o jogador:** Infraestrutura do navegador e do WebGL.

**Motivo:** Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem.


### `src/render/torusWindow.ts`

**Estado:** a portar · **Dono no jogo:** render/wear + road.gdshader

**Para o jogador:** Ruas e calçadas envelhecem com o uso: trilhas de pneu escurecem, viram rachadura e buraco; o piso mancha onde se anda. _(no 4.0: A SQUARE WINDOW OF TEXELS OVER THE UNBOUNDED WORLD, kept in a texture that wraps round (the toroidal update of clipmaps: Tanner, Migdal & Jones, "The Clipmap", SIGGRAPH 1998; Asirvatham & Hoppe, GPU G)_

**Motivo:** Sem prova por símbolo até agora. Escopo explícito do porte.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `TexelBand` | a portar | render/wear + road.gdshader | A band of world texels, inclusive: columns gx0..gx1, rows gy0..gy1. |
| `TorusWindow` | a portar | render/wear + road.gdshader |  |
| `TorusWindow.bandOf` | a portar | render/wear + road.gdshader | A world rectangle's texels on the window (null: none of it is on it). |
| `TorusWindow.constructor` | a portar | render/wear + road.gdshader |  |
| `TorusWindow.follow` | a portar | render/wear + road.gdshader | Centres the window on (x, y) once it strayed more than `hold` texels: the bands of texels that came in (to write), or null when it stays. |
| `TorusWindow.indexAt` | portado | `native/src/render/wear/wear_field.cpp` → `int wrap(int v, int size)` | The texel index (row by row) of a world point, or -1 off the window. |
| `TorusWindow.ox` | a portar | render/wear + road.gdshader | The world texel of the window's low corner (west, south). |
| `TorusWindow.oy` | a portar | render/wear + road.gdshader |  |
| `TorusWindow.span` | a portar | render/wear + road.gdshader |  |
| `TorusWindow.texel` | a portar | render/wear + road.gdshader | The texture's column (or row) of world texel `g`. |
| `TorusWindow.whole` | a portar | render/wear + road.gdshader |  |

### `src/render/uploads.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/building_renderer*, render_world, chunk_jobs

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

**Motivo:** Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `reserveWarm` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | src/render/uploads.ts:424-433 function reserveWarm: the feature's own |
| `warmFull` | a portar | render/buildings/building_renderer*, render_world, chunk_jobs | src/render/uploads.ts:418-420 function warmFull: the feature's own |

### `src/render/vehicleModels.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/render/wear.ts`

**Estado:** a portar · **Dono no jogo:** render/wear + road.gdshader

**Para o jogador:** Ruas e calçadas envelhecem com o uso: trilhas de pneu escurecem, viram rachadura e buraco; o piso mancha onde se anda. _(no 4.0: Wear: streets and footways that age with use (the player's order of 2026-10-05).)_

**Motivo:** Sem prova por símbolo até agora. Escopo explícito do porte.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CELL` | portado | `project/data/render/streets.json` → `field_cell_m` |  |
| `HOLD` | portado | `native/src/render/wear/wear_field.cpp` → `HOLD_SHARE` | How far the view goes from the window's middle before it follows (texels): an eighth of it, the texels that leave cleared. |
| `RANGE_STRIDE` | não se aplica | `native/src/render/wear/wear_field.cpp` → `texture_2d_update` — detalhe do envio parcial do three; o Godot envia a camada inteira | three r186 counts a data texture's update ranges in RGBA components whatever its format (`WebGLTextures.js` updateTexture, "only RGBA supported"): 4 a |
| `RES` | portado | `native/src/render/wear/wear_field.h` → `RES = LAYER_CELLS * LAYERS_AXIS` |  |
| `UPLOAD_EVERY` | portado | `project/data/render/streets.json` → `upload_every_s` | Seconds between uploads of the field to the GPU. |
| `WEAR_RES` | portado | `native/src/render/wear/wear_field.h` → `RES = LAYER_CELLS * LAYERS_AXIS` | Texels across the window round the view: 5 m each over 10,240 m, the reach the wear had (DO NOT shrink it without the player's yes on shots: wear past |
| `WearField` | portado | `native/src/render/wear/wear_field.h` → `class WearField` |  |
| `applyWear` | portado | `project/render/shaders/wear.gdshaderinc` → `vec3 wear_asphalt(` | Makes a road surface material show the wear field: `channel` 0 reads the wheels |
| `createWearField` | portado | `native/src/render/wear/wear_field.h` → `bool init(` | DO NOT hold a texture over a map: the world has none; the wear lives in a window that wraps round (`torusWindow.ts`) |
| `createWearField.add` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::add(` |  |
| `createWearField.dispose` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::free(` |  |
| `createWearField.feet` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::wear(` | A walker at (x, y), for `dt` seconds. |
| `createWearField.follow` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::follow(` | The window kept round where the view looks: wear is only kept near it (the traffic it shows is the micro window's). |
| `createWearField.random` | portado | `native/src/render/wear/wear_field.cpp` → `float WearField::random(` |  |
| `createWearField.tick` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::upload(` | Uploads the field when it is due. |
| `createWearField.touch` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::touch(` |  |
| `createWearField.wheels` | portado | `native/src/render/wear/wear_field.cpp` → `void WearField::wear(` | A vehicle at (x, y) heading `angle`, `width` across its wheels, for `dt` seconds. |

### `src/sim/agents/bay.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/buildingOrder.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/kerbBays.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lorryDocks.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lorryTraffic.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lotBays.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes. _(no 4.0: Null: the record has no parts at all (`elements` absent).)_

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lotDoors.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lotNav.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/lotTraffic.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/parking.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/agents/trafficBuildings.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/ambient/ambientIndex.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/ambient/demand.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/host/simClient.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/sim/host/simHost.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/ui/about.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/ui/buildStamp.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador. _(no 4.0: The build stamp: branch, short commit and date of what is running)_

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BAKED` | não se aplica | project/ui, GameFlow |  |
| `BuildStamp` | não se aplica | project/ui, GameFlow | The build stamp: branch, short commit and date of what is running |
| `__BUILD_STAMP__` | não se aplica | project/ui, GameFlow |  |
| `formatBuildStamp` | não se aplica | project/ui, GameFlow |  |
| `listeners` | não se aplica | project/ui, GameFlow |  |
| `onBuildStamp` | não se aplica | project/ui, GameFlow |  |
| `refreshBuildStamp` | não se aplica | project/ui, GameFlow | Re-reads the stamp from the development server (the About dialog opened): a fast-forward shows without a reload. |
| `stamp` | não se aplica | project/ui, GameFlow |  |

### `src/ui/builder/catalog.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The Builder's tool catalogue: seven tabs, each showing all it offers at once, in short sections.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BUILDER_CATALOG` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Every tab with its tools, flat: the shape the tests and the wiring read. |
| `BUILDER_TAB_SPECS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderCategoryId` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderField` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | One precise, editable number of the current selection, for the inspector. |
| `BuilderSection` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderSelectionInfo` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What the inspector shows for the current selection. |
| `BuilderShelf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a section of a tab holds: tools, or one of the panel's own shelves. |
| `BuilderTabSpec` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderToolKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderToolSpec` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `DRAW_ACTIONS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a drawn shape does: a new building, a block joined beside the selection, one on its roof, or a cut out of it. |
| `DRAW_SHAPES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The shape a draw tool draws. |
| `FACADE_SCOPES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where an opening or a pattern is applied. |
| `OPENING_COMPONENTS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The BayComponent an opening tool places. |
| `PlanShapeId` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The plans a closed outline can be recognised as, and drawn as. |
| `TIER_SHAPES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The plan a "block shape" action gives the selected block. |
| `action` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `mode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `tabSpec` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/ui/builder/icons.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The Builder's icon set: one stroked 24x24 glyph per tool and per top-bar action)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuilderIconId` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `FALLBACK` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `P` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `TAB_ICON` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The seven tabs of Construction, drawn with the glyph of what they hold. |
| `builderIcon` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The glyph body for an id, ready to drop inside an `<svg>`. |
| `builderIconSvg` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A complete inline SVG for an id. |

### `src/ui/builder/workspace.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The Builder's side of the interface: its state)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuilderActions` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderState` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BuilderWorkspace` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ChromeMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `SNAP_MODES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `el` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.inside` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | See inside as it is now, and each change of it (a building opened by a click on the map too). |
| `initBuilderWorkspace.onInside` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  |  |
| `initBuilderWorkspace.refresh` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.relabel` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.renderInspector` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.setMode` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Which half of the game the container is driving. |
| `initBuilderWorkspace.setPresetThumbnails` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.showGallery` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Opens a tab. |
| `initBuilderWorkspace.showInside` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | Shows the see-inside control as it now is (opened by a click on the map). |
| `initBuilderWorkspace.subscribe` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `initBuilderWorkspace.thumbnail` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/ui/i18n/index.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `parseDecimal` | não se aplica | project/ui, GameFlow | src/ui/i18n/index.ts:90-93 function parseDecimal: the feature's own |

### `src/ui/materialSwatch.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.


### `src/ui/overlay/builderGrid.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: DO NOT grow a second layer of gizmos here)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `drawBuilderGrid` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT grow a second layer of gizmos here |

### `src/ui/overlay/buildingOverlay.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The building tool's hairlines on the overlay canvas)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingOverlayInput` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building tool's hairlines on the overlay canvas |
| `GLYPH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `RELIEF` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The push-pull handle of a face region, in the second accent. |
| `SELECTION_FAINT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The selected volume's roof edge: present, but quieter than the base line. |
| `drawBuildingOverlay` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `drawBuildingOverlay.ring` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `drawHandle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/ui/planSwatch.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.


### `src/ui/v2/shell.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.


### `src/view/viewport.ts`

**Estado:** não se aplica · **Dono no jogo:** project/ui, GameFlow

**Para o jogador:** Montagem da interface e laboratório do navegador.

**Motivo:** Interface é da funcionalidade interface; laboratório é ferramenta do 4.0.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `flatViewport.rotate` | não se aplica | project/ui, GameFlow | src/view/viewport.ts:76-78 method flatViewport.rotate: the feature's own |

### `src/world/bounds.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `insideWorld` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/bounds.ts:84-85 function insideWorld: the feature's own |

### `src/world/buildingGates.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: THE BUILDINGS' ENTRANCES a street's parking keeps clear of: from the records alone (no document, no network), so the document and the tile store read them without a cycle through the streets)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CAR_GATE_WIDTH` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The narrowest gate a car goes through (a person's is 1.2 m); `sim/agents/parking.ts` `CAR_GATE`. |
| `entrancesOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | One building's entrances: its car gates and the edges of its parking, worked out once a record (records are replaced, never changed: `identityMemo`) - |
| `gatesOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Every entrance of some buildings (`entrancesOf`), four numbers each (x0, y0, x1, y1): what a tile keeps of its buildings for its streets' parking (`Ti |
| `workOutEntrances` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Each record's entrances, worked out once (`entrancesOf`: records are never changed, only replaced). |

### `src/world/buildings/architecture.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: ARCHITECTURAL STYLES for the grown buildings, as a facade grammar.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ArchStyle` | a portar | sim/buildings/b40/* |  |
| `Composition` | a portar | sim/buildings/b40/* | What a composition was given, for the parts of the building that follow it (the door, the dress). |
| `ERA_AFFINITY` | a portar | sim/buildings/b40/* |  |
| `Era` | a portar | sim/buildings/b40/* | The ERA of a quarter of the city: its buildings were mostly put up in one period, so they share a family of styles |
| `FOR` | a portar | sim/buildings/b40/* | The styles each kind of building is drawn from, with their weights. |
| `GroundRole` | a portar | sim/buildings/b40/* |  |
| `STYLES` | a portar | sim/buildings/b40/* |  |
| `StyleKey` | a portar | sim/buildings/b40/* |  |
| `StylePaints` | a portar | sim/buildings/b40/* | A building's paints within its style: the skin, the trim that goes with it. |
| `between` | a portar | sim/buildings/b40/* |  |
| `columnComponent` | a portar | sim/buildings/b40/* | The component of column `i` of `n` in a rhythm counted from the ends in (symmetric). |
| `columnsFor` | a portar | sim/buildings/b40/* | How many columns a face of `length` units is shared into on a module of `module` metres |
| `composeFacades` | a portar | sim/buildings/b40/* | Composes a building's solid blocks in a style |
| `paintsOf` | a portar | sim/buildings/b40/* |  |
| `paintsOf.light` | a portar | sim/buildings/b40/* | A light frame on a light wall disappears: on a pale skin, a style with a dark trim on its list prefers it. |
| `pick` | a portar | sim/buildings/b40/* |  |
| `stoneBaseOf` | a portar | sim/buildings/b40/* | The stone of the base of a style that has one: the skin's tone, a harder finish, a shade darker. |
| `stoneBaseOf.ch` | a portar | sim/buildings/b40/* |  |
| `styleFor` | a portar | sim/buildings/b40/* | The style of a building of `kind`: its quarter's era weighs the styles (`ERA_AFFINITY`) |
| `tint` | a portar | sim/buildings/b40/* | The colour of a skin, nudged a little in lightness: two buildings of one paint are never the same. |
| `tint.ch` | a portar | sim/buildings/b40/* |  |
| `tones` | a portar | sim/buildings/b40/* |  |

### `src/world/buildings/blocks.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: A building is blocks put together like bricks, and the blocks are never changed by being put together.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `polygonArea` | a portar | sim/buildings/b40/* |  |
| `resolve` | a portar | sim/buildings/b40/* |  |
| `resolve.asSolid` | a portar | sim/buildings/b40/* |  |
| `resolve.levelsMeet` | a portar | sim/buildings/b40/* |  |
| `resolve.top` | a portar | sim/buildings/b40/* |  |
| `resolveBlocks` | a portar | sim/buildings/b40/* | A building is blocks put together like bricks, and the blocks are never changed by being put together.  Each record resolved, once (a stored record is |
| `simplePieces` | a portar | sim/buildings/b40/* | A ring with holes as simple rings: split in two through the first hole. |
| `volumeOf` | a portar | sim/buildings/b40/* |  |

### `src/world/buildings/blueprints.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Reusable building definitions: everything a building is except where it stands.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ABOVE_STREET` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What an access component becomes on a storey above the street. |
| `BLUEPRINTS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The built-in presets, in palette order. Names are `building.preset.<key>`. |
| `Blueprint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `BlueprintBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Reusable building definitions: everything a building is except where it stands. |
| `DEFAULT_GROUND_HEIGHT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `DEFAULT_PALETTE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `DEFAULT_STOREY_HEIGHT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `apartments` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `bayCount` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Whole bays along a length, at about one module each. |
| `bays` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | `n` default modules, world units: how the presets below are measured. |
| `blueprintByKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The use each preset is filed under in the palette. |
| `bodyOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A blueprint's body with a building's placement stripped off. |
| `courtyard` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Four individually editable wings around an open, ground-level courtyard. |
| `courtyard.wing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `defaultFacade` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The default facade of one storey, by use and level. |
| `defaultRoof` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `defaultStorey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `factory` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `generateBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The neutral starting block: no category, just a mass of `w x d` units and `storeys` storeys with windows all round |
| `generateBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The parametric generator: a single volume `w x d` world units and `storeys` storeys |
| `house` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `instantiate` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A building record for a blueprint, placed so the middle of its FRONT edge (the street side) is at `anchor`. |
| `mixedBlock` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `office` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `placeBody` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT copy the body for a ghost: every pointer move wrote the whole body to JSON and read it back to move it. |
| `rowhouse` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `shop` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `storeyUse` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The use a storey of a building of `use` has at `level`. |
| `tower` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `upperStoreyFrom` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A storey for above `source`, in the building's own style rather than a category's: the same facade, with every way in |
| `upperStoreyFrom.lift` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `warehouse` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `withCore` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/world/buildings/cameraSolids.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings (consulta) + camera_motion.gd

**Para o jogador:** A câmera não atravessa prédios. _(no 4.0: THE LOWEST THE CAMERA'S EYE MAY STAND OVER THE BUILDINGS, a continuous function of the map point)_

**Motivo:** Sem prova por símbolo até agora. Verificar na câmera atual.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CELL` | a portar | sim/buildings (consulta) + camera_motion.gd | The grid the volumes are filed in, world units a cell. |
| `CLEARANCE` | a portar | sim/buildings (consulta) + camera_motion.gd | How far over a roof the eye keeps, world units. |
| `CameraSolids` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `CameraSolidsKeeper` | a portar | sim/buildings (consulta) + camera_motion.gd | The town's solids as filed for the camera |
| `GroundSince` | a portar | sim/buildings (consulta) + camera_motion.gd | Where and when the ground the buildings stand on changed (the diary's ground kinds, `render/renderer.ts`). |
| `OCCLUDER_MARGIN` | a portar | sim/buildings (consulta) + camera_motion.gd | How far the occluder's plan is drawn in from the volume's: the reliefs pushed into its faces, and a margin. |
| `OccluderVisit` | a portar | sim/buildings (consulta) + camera_motion.gd | An occluder as its building's frame: the building's centre and turn, the box in that frame, the walls' bottom and top (world height). |
| `PAGE` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `PAGE_BITS` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `REACH` | a portar | sim/buildings (consulta) + camera_motion.gd | How far round a volume its floor eases down to the building's foot, world units: the camera's radius |
| `Rect` | a portar | sim/buildings (consulta) + camera_motion.gd | A world rectangle: [minX, minY, maxX, maxY]. |
| `STRIDE` | a portar | sim/buildings (consulta) + camera_motion.gd | DO NOT go back to an object per volume and a set per building: 29 MB of a city of 5 400. |
| `S_BX,S_BY,S_COS,S_SIN,S_X0,S_Y0,S_X1,S_Y1,S_FOOT,S_TOP` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `S_NEXT` | a portar | sim/buildings (consulta) + camera_motion.gd | The slot of the next solid volume of the same building (-1: its last): a building's volumes a chain from its first (`SolidFiling.first`). |
| `S_WALL0,S_WALL1,S_INSET` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling` | a portar | sim/buildings (consulta) + camera_motion.gd | The solid volumes filed by cell (their slots in `SolidPages`), each building's taken out and filed again on its own. |
| `SolidFiling.add` | a portar | sim/buildings (consulta) + camera_motion.gd | `b`'s solid volumes filed, standing on `foot`. |
| `SolidFiling.cells` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.clear` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.constructor` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.filedIds` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.first` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.floorAt` | a portar | sim/buildings (consulta) + camera_motion.gd | The lowest the eye may stand at (x, y), world height; -Infinity away from every building. |
| `SolidFiling.forEachIn` | a portar | sim/buildings (consulta) + camera_motion.gd | Every solid volume filed in the cells over a map rectangle, once, as its building's frame |
| `SolidFiling.forEachInAny` | a portar | sim/buildings (consulta) + camera_motion.gd | Every solid volume filed in the cells over any of the rectangles, once for all of them. |
| `SolidFiling.forEachOccluder` | a portar | sim/buildings (consulta) + camera_motion.gd | Every volume over a map rectangle that surely hides what stands behind it (`render/occlusion.ts`): the box of its walls drawn in by its reliefs, never |
| `SolidFiling.pages` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidFiling.remove` | a portar | sim/buildings (consulta) + camera_motion.gd | The building's solids taken out of their cells. |
| `SolidFiling.roofAt` | a portar | sim/buildings (consulta) + camera_motion.gd | The roof over (x, y) when the point is inside a solid volume's plan (its highest, no margin round it), world height |
| `SolidFiling.visitCells` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.clear` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.epoch` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.free` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.give` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.next` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.page` | a portar | sim/buildings (consulta) + camera_motion.gd | The page holding `slot`, read at `at(slot) + S_*`. |
| `SolidPages.pages` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.stamps` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `SolidPages.take` | a portar | sim/buildings (consulta) + camera_motion.gd | A slot for one volume, from the free list or a new one (a page added when it is the first of one). |
| `SolidPages.visitOnce` | a portar | sim/buildings (consulta) + camera_motion.gd | True the first time `slot` is met in this walk. |
| `SolidPages.walk` | a portar | sim/buildings (consulta) + camera_motion.gd | A new walk: no slot visited in it yet. |
| `SolidVisit` | a portar | sim/buildings (consulta) + camera_motion.gd | A solid volume as its building's frame: the building's centre and turn, the volume's box in that frame, its top. |
| `at` | a portar | sim/buildings (consulta) + camera_motion.gd | Where a slot's numbers start in its page. |
| `cameraSolidsKeeper` | a portar | sim/buildings (consulta) + camera_motion.gd | DO NOT file the town again for one building |
| `cameraSolidsKeeper.follow` | a portar | sim/buildings (consulta) + camera_motion.gd | Takes in the buildings and the ground; true when the filing changed since the last `follow`. |
| `cameraSolidsKeeper.solids` | a portar | sim/buildings (consulta) + camera_motion.gd | The filing, brought up to the last `follow` on the first ask after it. |
| `cellKey` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `forEachCellOf` | a portar | sim/buildings (consulta) + camera_motion.gd | The cells a volume's world box grown by the reach touches (its numbers at `o` in `page`), each told once. |
| `lowestEye` | a portar | sim/buildings (consulta) + camera_motion.gd | The lowest the eye may stand at (x, y) over `list`, its floor easing down over `reach` round each volume. |
| `overlaps` | a portar | sim/buildings (consulta) + camera_motion.gd |  |
| `smoothstep` | a portar | sim/buildings (consulta) + camera_motion.gd |  |

### `src/world/buildings/cityBuildings.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/civic*, render/buildings/signature

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas). _(no 4.0: The city's buildings - homes, public services, shops, places to eat and go out, places of work and of leisure)_

**Motivo:** Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BRICK` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `BlockSpec` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `CITY_BUILDINGS` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `CONCRETE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `CREAM` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `CityBuilding` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `CityCategory` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `GLASS` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A model being put together, block by block. |
| `Model.block` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.build` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.constructor` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.core` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.cores` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.el` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.elements` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.groundHeight` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.heights` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.look` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.lot` | a portar | sim/buildings/b40/civic*, render/buildings/signature | An open block: a lot of grass, paving, gravel, sand or water. |
| `Model.materials` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.ring` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A fence (or wall) all round a rectangle, in pieces. |
| `Model.row` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A row of trees (or any part) from (x0,y0) to (x1,y1), `n` of them. |
| `Model.storeyHeight` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `Model.volumes` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `OCTAGON` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The cross of a stepped tower, normalised. |
| `PANEL` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `PLASTER` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `RED` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `RETAINING_STONE` | a portar | sim/buildings/b40/civic*, render/buildings/signature | A retaining wall's stone, and the flights cut through one (`world/growth/lotPlan.ts` terraces) |
| `SLATE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `STONE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `TILE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `WHITE` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `cityBuilding` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `isRetainingStone` | a portar | sim/buildings/b40/civic*, render/buildings/signature | Whether a material is the retaining wall's stone. |
| `make` | a portar | sim/buildings/b40/civic*, render/buildings/signature | ------------------------------------------------------------ homes |
| `mat` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |

### `src/world/buildings/decayWatch.ts`

**Estado:** outro porte · **Dono no jogo:** ports/demolition

**Para o jogador:** Prédio que quebra em pedaços, ataques, efeitos e decadência. _(no 4.0: DO NOT read `change.ids ?? []`: after a load the new town never ran down and the old one stayed due.)_

**Motivo:** Funcionalidade demolition da fila.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `createDecayWatch` | outro porte | ports/demolition | DO NOT read `change.ids ?? []`: after a load the new town never ran down and the old one stayed due. |
| `createDecayWatch.dueOf` | outro porte | ports/demolition |  |
| `createDecayWatch.file` | outro porte | ports/demolition |  |
| `createDecayWatch.nextTenth` | outro porte | ports/demolition |  |

### `src/world/buildings/docks.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: A works' lorry docks: the wide bays its loading yard is laid with, each square in front of a loading door of the hall)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LORRY_DOCK` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A stall wider than this is a lorry's, not a car's (`sim/agents/parking.ts`). |
| `LorryDock` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A works' lorry docks: the wide bays its loading yard is laid with, each square in front of a loading door of the hall |
| `STALL` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `docksOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `isDock` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `lorryDocks` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's lorry docks, worked out when its traffic record is sent (`sim/agents/trafficBuildings.ts`); the worker keeps them, the page does not. |
| `ringDistance` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |

### `src/world/buildings/elements.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Free parts of a building.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BOUNDARY_KINDS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The lot's boundary and its posts: runs meet each other at joints by design. |
| `BayRef` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A bay of a face, as the pointer picked it. |
| `CROWN_FROM` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ELEMENT_DEFAULTS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Sizes an element starts with, world units: [w, d, h]. |
| `FACING_NORMAL` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `FOLLOWS_GROUND` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Parts that stand on the land itself and follow it |
| `FOLLOW_STEP` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The longest step of a run that follows the ground. |
| `Footprint` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `GROUND_DRESSING` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Parts that dress a lot rather than raise a building. |
| `MAX_ELEMENT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `MEET` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far two parts may run into each other: a contact, a footing set into paving. |
| `MIN_ELEMENT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `ON_FACADE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Parts hung on a facade rather than standing on the ground. |
| `PLANT_KINDS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Planting, which grows under a tree's crown. |
| `RAMP_RUN_PER_RISE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A ramp's slope: one in twelve, the usual limit for a wheelchair ramp. |
| `SURFACE_KINDS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Laid on the ground to stand things on (`lotPlan.ts` FLAT_KINDS): a surface, not a footprint. |
| `TRUNK` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A tree's trunk, and where its crown starts as a share of its height. |
| `elementAt` | portado | `native/src/sim/commands/body_parts.cpp` → `std::string op_add_part_at(` | An element of `kind` at a free point of the local frame, on the ground (a slab at `z`). |
| `elementClash` | portado | `native/src/sim/commands/body_parts.cpp` → `const Volume *clash(` | The first volume an element would stand inside of, or null. Touching is allowed. `spans`: the building's `massSpans`, when the building is unchanged s |
| `elementRing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The world ring of an element's plan. |
| `elementsAgainstBay` | portado | `native/src/sim/commands/body_parts.cpp` → `std::string op_add_part(` | Candidate placements of an element of `kind` against a picked bay, best first. |
| `elementsAgainstBayAxis` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `elementsAgainstBayAxis.at` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A point on the face line, `out` in front of it and `a` along it. |
| `elementsMeet` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Whether two parts of a lot stand in each other (Etapa 5a) |
| `foliage` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Foliage: a shrub grows into a hedge (`elementsMeet`). |
| `followPieces` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A long run that follows the ground in steps of at most `FOLLOW_STEP`; anything else whole. |
| `footA,footB` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `footprintOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `groundElements` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | World rings of the elements that stand on the ground: footprint, for the road and neighbour tests. |
| `heightsMeet` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Heights running into each other by more than a contact (a flush joint of two plates is a contact). |
| `isEarthwork` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The yard's earthworks - a flight, a ramp, a retaining wall in its stone |
| `isStructure` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Laid with the lot's structure (its boundary, its earthworks): dressing in its way is cleared. |
| `massSpans` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Each block's heights as `elementClash` reads them, its floor then its top (two numbers a block, in the order of `b.volumes`; a lot, a cut or a clip le |
| `onGround` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Whether an element's foot stands on the ground (rather than on a floor, or hung on a wall). |
| `partGroups` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The objects a lot's parts make: parts that touch (by the same contact `unsupportedElements` reads) are one thing |
| `partGroups.find` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `planDepth` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How deep two turned rectangles run into each other (separating axes); <= 0 when apart. |
| `planReach` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The radius of a circle round every footprint of a part (`footprintOf`, its own or a trunk's).  The radius round a part's centre its footprint stays wi |
| `reachAlong` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A footprint's half extent along the axis (ux, uy). |
| `runFor` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The run a stair or a ramp needs for its rise. |
| `snap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `stairSteps` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Steps of a stair: every riser at most `STEP_RISE`. |
| `takeElementId` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A new element's id, and the building's counter moved past it. |
| `thinRun` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A thin run of the boundary: a plant grows through it (`elementsMeet`). |
| `unsupportedElements` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The parts nothing holds up: a carport roof whose posts were refused, a pergola whose posts a retaining wall cleared |
| `unsupportedElements.heldByMass` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Hung on the building's wall, or laid on one of its roofs or floors |
| `unsupportedElements.onLand` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `unsupportedElements.square` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/world/buildings/facadePatterns.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: A coherent facade is one command; individual bays remain fully editable.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FacadeTarget` | a portar | sim/buildings/b40/* |  |
| `USE` | a portar | sim/buildings/b40/* |  |
| `applyFacadePattern` | portado | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_pattern(` | A coherent facade is one command; individual bays remain fully editable. |
| `updateFacadeGeometry` | portado | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_face(` | A face's proportions are authored data; reducing its bay count resamples edits. |

### `src/world/buildings/flags.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/flags + props.gdshader

**Para o jogador:** Bandeiras em mastros de prédios públicos, ao vento. _(no 4.0: Flags: a generic, customizable flag flown from a mast (a spire, a lantern's mast, a flagpole).)_

**Motivo:** Sem prova por símbolo até agora. Item próprio do render.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `DEFAULT_FLAG` | a portar | render/buildings/flags + props.gdshader |  |
| `FLAG_COLOURS` | a portar | render/buildings/flags + props.gdshader | The colours a flag's fields are picked from in the Builder. |
| `FLAG_PATTERNS` | a portar | render/buildings/flags + props.gdshader | Flags: a generic, customizable flag flown from a mast (a spire, a lantern's mast, a flagpole). |
| `FlagDesign` | a portar | render/buildings/flags + props.gdshader |  |
| `FlagPattern` | a portar | render/buildings/flags + props.gdshader |  |
| `isFlagPattern` | a portar | render/buildings/flags + props.gdshader |  |
| `legacyFlag` | a portar | render/buildings/flags + props.gdshader | The designs of the first, fixed flags, kept so old maps fly the same flags. |
| `migrateFlagDesign` | a portar | render/buildings/flags + props.gdshader | A design read from a save, or null if it is not one. |

### `src/world/buildings/floors.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Nothing is evicted for room: DO NOT cap it.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingFloors` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Nothing is evicted for room: DO NOT cap it. |
| `buildingFloors` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | `boxOf`: a building's bounds grown by how far its floor reads (`FLOOR_READ_REACH`), `reach` that growth. |
| `buildingFloors.floorOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The building's ground floor on the ground and the paving as they stand. |
| `buildingFloors.follow` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Forgets the floors whose reach (`boxOf`) a change of the ground since the last `follow` touches; where the diary cannot say, those the regions the cal |
| `buildingFloors.forget` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Forgets one building's floor (its site worked out again). |
| `buildingFloors.forgetAll` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Forgets every floor (the roads or the land moved everywhere). |

### `src/world/buildings/footprints.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: Polygon outlines are normalized to the volume's bounding rectangle.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `AREA_EPS` | a portar | sim/buildings/b40/* | Two footprints overlapping by less than this area (a square of some 1.3 mm) only touch |
| `EdgeFrame` | a portar | sim/buildings/b40/* |  |
| `KnownFrames` | a portar | sim/buildings/b40/* | A block's side frames as last worked out, and the numbers they were worked out from (the editor's operations move a block in place, `opScalePlan`). |
| `asPolygon` | a portar | sim/buildings/b40/* |  |
| `boxesApart` | a portar | sim/buildings/b40/* | Whether two rings' boxes are strictly apart (touching boxes are clipped: their rings may share an edge). |
| `containsPoint` | portado | `native/src/sim/buildings/b40/plan_area.cpp` → `bool contains_point(` |  |
| `convexOverlap` | a portar | sim/buildings/b40/* | The area two convex rings share: `a` clipped by each edge of `b` in turn, the area of what is left (either winding). |
| `coveredArea` | a portar | sim/buildings/b40/* | The area of `ring` that lies over `supports`, the supports' own overlaps counted once: summed one by one where `ring` and every support are convex and |
| `edgeFrame` | a portar | sim/buildings/b40/* | DO NOT make a side's frame (and the block's ring) again each time it is asked, by side and storey: kept on the block, worked out again only when its p |
| `framesOf` | a portar | sim/buildings/b40/* |  |
| `isConvex` | a portar | sim/buildings/b40/* | Whether a ring turns one way only (collinear points allowed). |
| `localFootprint` | a portar | sim/buildings/b40/* |  |
| `offsetRing` | a portar | sim/buildings/b40/* | Small miter offsets for selection outlines and ground clearance. |
| `overlapArea` | portado | `native/src/sim/buildings/b40/plan_area.cpp` → `float overlap_area(` | The area two rings share. DO NOT clip two rings whose boxes are apart (a building's boundary piece against every ring of its neighbours, each a polygo |
| `polygonArea` | a portar | sim/buildings/b40/* |  |
| `roofDetailRing` | a portar | sim/buildings/b40/* |  |
| `roofPartFits` | a portar | sim/buildings/b40/* |  |
| `sideFrame` | a portar | sim/buildings/b40/* |  |
| `supportShare` | portado | `native/src/sim/buildings/b40/plan_area.cpp` → `float support_share(` | The share (0..1) of `ring`'s area that lies over `supports`. |
| `supportedBy` | a portar | sim/buildings/b40/* |  |
| `validOutline` | a portar | sim/buildings/b40/* | Polygon outlines are normalized to the volume's bounding rectangle. |
| `volumeSides` | a portar | sim/buildings/b40/* |  |

### `src/world/buildings/foundation.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: How a building meets the ground.)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ACCESS_COMPONENTS` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Components that are a way in, on level 0.  Doors only: a shopfront is a window on the street, not a way in. |
| `ENTRANCE_LOOKS` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The highest paving any entrance opens onto, or -Infinity.  How far out from an access its paving is looked for, nearest first. |
| `ENTRANCE_REACH` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far in front of an entrance paving still counts as the street it opens onto. |
| `Entrance` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Extension point: a way into the building on its ground floor. |
| `FLOOR_READ_REACH` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The farthest `floorHeight` reads the ground or the paving past a building's bounds |
| `FootprintGround` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ground under a building: its lowest and highest point, absolute and about each block's own floor. |
| `Foundation` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `GroundAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `LANDING_RISE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | DO NOT lay the landing at the floor itself. |
| `MAX_PLINTH` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The steepest site a building may be placed on |
| `NO_PAVING` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `PLINTH_BURY` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The plinth runs this far below the lowest ground, so no slope shows under it. |
| `PLINTH_MIN` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ground floor stands at least this far above the highest ground under it |
| `PavedAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Height of a PAVED surface at a point - a footway, a carriageway - or NaN where there is none. |
| `RECESS_BACK` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Wall kept between a recessed flight and the back of its volume. |
| `SCAN_STEP` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Sampling step when looking for the edge of the paving in front of an entrance. |
| `STEP_RISE` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `STEP_RUN` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `THRESHOLD` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Height of a ground floor's threshold over the pavement it opens onto. |
| `adjacentEntrance` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `atLevel` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The paving at the level of land at `land` |
| `entranceLook` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `entrancePaving` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ground storey's bays packed (`groundBays`): no bay object made, as the floor is asked for by frame. |
| `flightRun` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Plan length of a flight of `steps`: one tread each, and the top one doubled as a landing. |
| `floorHeight` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The absolute ground-floor height of a building, uncached. Same rule as `foundationOf`. |
| `floorOver` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ground floor's height. |
| `foundationOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `frontFloorHeight` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A ground floor's height as seen from afar: the pavement along its lots' open edges, else its plinth over the highest ground - the door's pavement and  |
| `isAccess` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `lotFront` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The highest pavement along the open edges of a building's lots |
| `mergeEntrances` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Side-by-side ways in become ONE wide flight. |
| `pavingEdge` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Distance from a facade to the first paving in front of it, or Infinity if there is none within `limit` |
| `pavingEdge.paved` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `sampleFootprint` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Samples the ground over every ground volume, at about half-module spacing. |
| `stepsFor` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Steps needed to climb `rise`; a threshold under two risers needs none. |

### `src/world/buildings/geometry.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: Every spatial question about a building, answered from the record.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BayOf` | a portar | sim/buildings/b40/* | What one exposed bay (or piece of one) of one storey is wherever its building stands. |
| `Bounds` | a portar | sim/buildings/b40/* |  |
| `BuildingFrame` | a portar | sim/buildings/b40/* | DO NOT call `localToWorld` point by point in a building's loops |
| `DEFAULT_PITCH` | a portar | sim/buildings/b40/* | Default roof pitches, degrees. |
| `EPS` | a portar | sim/buildings/b40/* | Two lengths closer than this (0.04 mm) are the same one (shared walls, touching volumes). |
| `FacadeBay` | a portar | sim/buildings/b40/* | One exposed bay (or piece of one) of one storey: everything a mesh builder or a picker needs. |
| `Frame` | a portar | sim/buildings/b40/* | What a building's frame is read from: its origin and rotation (a worker is sent no more, `sim/agents/lotBays.ts`). |
| `GB_COMPONENT` | a portar | sim/buildings/b40/* |  |
| `GB_X,GB_Y,GB_NX,GB_NY,GB_Z,GB_SIDE,GB_WIDTH` | a portar | sim/buildings/b40/* |  |
| `GRID` | a portar | sim/buildings/b40/* | The step every horizontal dimension snaps to in the editor: half a metre. |
| `GROUND_BAYS` | a portar | sim/buildings/b40/* | Each record's ground storey bays (`GroundBays`), by the record itself: a stored record is never changed in place |
| `GROUND_BAY_STRIDE` | a portar | sim/buildings/b40/* | Numbers per bay in a `GroundBays` table, and where each lies. |
| `GroundBays` | a portar | sim/buildings/b40/* | The ground storey's bays (level 0) of a building, packed |
| `LocalBay` | a portar | sim/buildings/b40/* | The same bay in the building's own frame (`bayLayout`): its point and normal local. |
| `MIN_BEARING` | a portar | sim/buildings/b40/* | The share of a block's plan that must bear on the blocks under it. |
| `MIN_SIZE` | a portar | sim/buildings/b40/* | The smallest a volume may be on either side. |
| `NO_SPANS` | a portar | sim/buildings/b40/* | No spans. |
| `Rhythm` | a portar | sim/buildings/b40/* | What a building's bays and floors are laid by besides its blocks: the bay module across, the storey heights up (`levelHeight`). |
| `SIDE_NORMAL` | a portar | sim/buildings/b40/* | Outward unit normal of a side, in the local frame. |
| `STORED_BOUNDS` | a portar | sim/buildings/b40/* |  |
| `bayCentreLocal` | a portar | sim/buildings/b40/* | Local centre of bay `index` on the facade line of `side`. |
| `bayLayout` | a portar | sim/buildings/b40/* | A building's bays in its own frame, worked out once for its blocks list and rhythm: the records sharing them (`intern.ts`, a tile's decoded members, a |
| `bayWidth` | a portar | sim/buildings/b40/* | Width of every bay of a side: the side shared evenly. |
| `baysOn` | a portar | sim/buildings/b40/* | Bays on a side: as many as fit at about one module each, at least one. |
| `buildingBounds` | a portar | sim/buildings/b40/* | World bounding box of a building: its volumes and its elements. |
| `buildingFrame` | a portar | sim/buildings/b40/* |  |
| `buildingHeight` | a portar | sim/buildings/b40/* | The highest point of the building above its ground floor. |
| `closedBlocks` | a portar | sim/buildings/b40/* | A building's closed blocks (an open one is a lot, laid on the ground): one list for every record with the same blocks, the blocks themselves when none |
| `coveredSpans` | a portar | sim/buildings/b40/* | Stretches of a side, in its along coordinate, that another volume stands against on `level` |
| `elementRect` | a portar | sim/buildings/b40/* | An element's plan, local frame: [x0, y0, x1, y1]. `w` runs across its facing, `d` along it. |
| `exposedParts` | a portar | sim/buildings/b40/* | The parts of [a0, a1] no span covers. |
| `facadeBays` | a portar | sim/buildings/b40/* | A building's bays in the world (`level`: that storey's alone), from its layout turned and moved to where it stands; a new list each time, kept by no o |
| `footprintBox` | a portar | sim/buildings/b40/* | Local bounding box of the footprint, world units. |
| `footprintCentre` | a portar | sim/buildings/b40/* | World centre of the footprint's bounding box. |
| `footprintRects` | a portar | sim/buildings/b40/* | World corner rings of every ground volume: the building's footprint. |
| `frameToLocal` | a portar | sim/buildings/b40/* | A world point in the building's frame, as `worldToLocal` gives it. |
| `frameToWorld` | a portar | sim/buildings/b40/* |  |
| `frameX` | a portar | sim/buildings/b40/* | World x of a local point, as `localToWorld` gives it. |
| `frameY` | a portar | sim/buildings/b40/* | World y of a local point, as `localToWorld` gives it. |
| `groundBayComponent` | a portar | sim/buildings/b40/* | The component of a packed ground bay. |
| `groundBays` | a portar | sim/buildings/b40/* |  |
| `groundProjections` | a portar | sim/buildings/b40/* | World rings of the projections that stand on the ground (from the first storey of a ground volume) |
| `groundVolumes` | a portar | sim/buildings/b40/* | Volumes standing on the ground. Everything else stands on these. |
| `inheritStoredBounds` | a portar | sim/buildings/b40/* | `to`'s boxes are `from`'s: the same building in every respect its box reads (its wear changed alone, `BuildingStore.wear`). |
| `insideSpans` | a portar | sim/buildings/b40/* | The stretches of an edge (its along coordinate) that run strictly inside a polygon |
| `isMass` | a portar | sim/buildings/b40/* | A block that is mass (solid or exclusive), not a cut or a clip of others. |
| `isSupported` | a portar | sim/buildings/b40/* | Whether a volume above the ground has something under every part of it |
| `isSupported.bears` | a portar | sim/buildings/b40/* |  |
| `layoutsOf` | a portar | sim/buildings/b40/* | Each blocks list's layouts, one per rhythm it was asked with: what `bayLayout` keeps. |
| `letGoLayout` | a portar | sim/buildings/b40/* | A blocks list's layouts let go now (`buildingMesh.ts` `letGoEmitted`): asked again, they are worked out again, the same. |
| `levelElevation` | a portar | sim/buildings/b40/* | Height of the floor of level `L` above the building's ground floor. |
| `levelHeight` | a portar | sim/buildings/b40/* | Height of level `L`, world units. |
| `liftAt` | a portar | sim/buildings/b40/* | The lift of the block a point of the plan is in, on `level`: where a floor's furniture, partitions and cores stand |
| `localDirToWorld` | a portar | sim/buildings/b40/* | A local direction turned into the world. |
| `localToWorld` | a portar | sim/buildings/b40/* |  |
| `occupiesLevel` | a portar | sim/buildings/b40/* | Whether a volume has a storey on `level`. |
| `partlyCoveredSpans` | a portar | sim/buildings/b40/* | Stretches of a side another block stands against over PART of storey `level`'s height |
| `planOverlap` | a portar | sim/buildings/b40/* | Whether two volumes share floor area in plan (touching is not sharing). |
| `projectionRect` | a portar | sim/buildings/b40/* | The plan rectangle a relief's region occupies in front of its face, local frame, world units: [x0, y0, x1, y1]. |
| `reliefAt` | a portar | sim/buildings/b40/* | The relief a bay of a storey is in, if any (the last one listed wins). |
| `reliefPushes` | a portar | sim/buildings/b40/* | DO NOT ask `reliefAt` for each bay: a side's every bay and storey has its relief's depth written once, the reliefs in their order so the last listed w |
| `rhythmOf` | a portar | sim/buildings/b40/* |  |
| `ridgeAlongX` | portado | `native/src/sim/buildings/b40/roof_shape.h` → `inline bool ridge_along_x(` | Whether a gable or hip ridge runs along the local x axis: as set, else along the longer side. |
| `roofHeightAt` | portado | `native/src/sim/buildings/b40/roof_shape.h` → `inline float roof_height_at(` | Height over the eaves at one local roof point, shared by roof parts. |
| `roofRise` | portado | `native/src/sim/buildings/b40/roof_shape.h` → `inline float roof_rise(` | How far a volume's roof rises above its top floor, world units. |
| `roofSlope` | portado | `native/src/sim/buildings/b40/roof_shape.h` → `inline float roof_slope(` | A volume's roof pitch as rise over run. |
| `sameRhythm` | a portar | sim/buildings/b40/* | Whether a building has this rhythm: every number the same (`Object.is`), level by level. |
| `sawtoothRun` | a portar | sim/buildings/b40/* | The run of one sawtooth: two modules, or the whole depth if less. |
| `shedFall` | a portar | sim/buildings/b40/* | The side a shed roof falls towards. |
| `sideLength` | a portar | sim/buildings/b40/* | Length of one side of a volume. |
| `sideStart` | a portar | sim/buildings/b40/* | Local start of a side (where its along coordinate is 0) and its along direction. |
| `solidFootprints` | a portar | sim/buildings/b40/* | World corner rings of the BUILT ground volumes: what a person cannot walk through. |
| `solidSpan` | a portar | sim/buildings/b40/* | The heights a block stands against, above the building's ground floor: from its floor to its eaves |
| `spansAgainst` | a portar | sim/buildings/b40/* |  |
| `standsAgainst` | a portar | sim/buildings/b40/* | How much of storey `level` of block `v` another block `o` stands against, by height |
| `storedBounds` | a portar | sim/buildings/b40/* | `buildingBounds` of a record held by the store, worked out once per record and margin. |
| `topLevel` | a portar | sim/buildings/b40/* | Highest level any volume reaches (one past the top storey). |
| `volumeCorners` | a portar | sim/buildings/b40/* | The four world corners of a volume, anticlockwise in the local frame. |
| `volumeElevation` | a portar | sim/buildings/b40/* | Height of the floor of a block's level `level` above the building's ground floor, its lift included. |
| `volumeHeight` | a portar | sim/buildings/b40/* | Height of a volume's top (eaves), above the building's ground floor, its lift included. |
| `volumeLift` | a portar | sim/buildings/b40/* | How far a block's own floor stands from the building's ground floor (`Volume.lift`). |
| `volumeRectLocal` | a portar | sim/buildings/b40/* | Local rectangle of a volume, world units: [x0, y0, x1, y1]. |
| `workOutLayout` | a portar | sim/buildings/b40/* | Every bay that is an OUTSIDE wall, in the building's own frame: its blocks and rhythm alone decide it. |
| `worldToLocal` | a portar | sim/buildings/b40/* |  |
| `worldToLocalAt` | a portar | sim/buildings/b40/* | `worldToLocal` for a point the pointer gives (a click). |

### `src/world/buildings/grid.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: THE BUILDINGS BY PLACE (`BuildingStore.near`): a grid of cells, each holding the buildings whose box)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingGrid` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.base` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.cellAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.cells` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Every building, in the store's order (a load, a clear, a whole store taken over). `boxOf`: its box as the store files it (`storedBounds`), the same fo |
| `BuildingGrid.delete` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.file` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.gone` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.inOrder` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.insert` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A building new to the store: last in its order. |
| `BuildingGrid.near` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The ids of the buildings whose box (or anchor) may reach the rectangle, in the store's order. |
| `BuildingGrid.nearAny` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT ask `near` rectangle by rectangle for many |
| `BuildingGrid.nextRank` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.over` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.ownCell` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.placeOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.placed` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.placedOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.unfile` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingGrid.update` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A building's record replaced: it keeps its place in the order. |
| `CELL` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Side of a cell, world units: as wide as the largest of the common buildings. |
| `Filed` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | What the grid files: a building, or what its neighbours read of one (`neighbours.ts`), with its anchor. |
| `PACKED_IDS` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A rank and an id packed in one number (`BuildingGrid.inOrder`): ids below 2^32, ranks below 2^20, the two within a double's 53 exact bits. |
| `PACKED_RANKS` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `PLACED_RANK,PLACED_I0,PLACED_I1,PLACED_J0,PLACED_J1` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Placed` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where a building is filed: its rank in the store's order, then the columns and rows of the cells its box reaches (`PLACED_*`). DO NOT keep its cells'  |
| `forKeys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Calls `visit` with the key of every cell a building's place reaches, columns outer, rows inner. |
| `keyOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |

### `src/world/buildings/handles.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Where the edit handles of a selected building stand, in world 3D.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CORNER_OUT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far the move, turn and scale handles stand off the footprint's corner. |
| `HANDLE_OUT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `Handle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `HandleKind` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Where the edit handles of a selected building stand, in world 3D. |
| `HandleRegion` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A rectangle of bays and storeys of one face (see `editor/buildings.ts`, `FaceRegion`). |
| `LIFT_OUT` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far the lift handle stands off its block's corner (the scale handle's 0.8 m). |
| `buildingHandles` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | `nearest` orders the four footprint corners |
| `buildingHandles.footOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `liftNear` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The lift of the ground block nearest a corner of the footprint's box (local frame) |

### `src/world/buildings/interior.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/interior

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte. _(no 4.0: What is inside a building, floor by floor)_

**Motivo:** Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FLIGHT_RISE` | a portar | sim/buildings/interior | The highest a riser may be, and the least and the most a tread is deep (the 17 cm and 28-30 cm of a house's stair). |
| `FLIGHT_TREAD` | a portar | sim/buildings/interior |  |
| `FLIGHT_TREAD_MIN` | a portar | sim/buildings/interior |  |
| `FLIGHT_WIDTH` | a portar | sim/buildings/interior | A flight's width: a house's stair, less where the blocks meet along a shorter line. |
| `FURNITURE_KINDS` | portado | `native/src/sim/buildings/interior/tables.h` → `enum FurnitureKind` |  |
| `FURNITURE_SIZE` | portado | `project/data/render/interiors.json` → `"sizes"` | Sizes in metres: across, deep, high. |
| `FloorInterior` | portado | `native/src/sim/buildings/interior/interior.h` → `struct FloorInterior` |  |
| `Furnished` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `types_;` — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | What a building's floors are furnished from besides its blocks (`furnishFloor`, `deriveSpaces`, `flatPlan`, `levelFlights`), its floors furnished so f |
| `Furniture` | portado | `native/src/sim/buildings/interior/interior.h` → `struct Furniture` |  |
| `FurnitureKind` | portado | `native/src/sim/buildings/interior/tables.h` → `enum FurnitureKind` |  |
| `ITEM_CELL` | portado | `native/src/sim/buildings/interior/plan.cpp` → `ITEM_CELL` | The side of the cells the pieces of a floor are filed by (`Plan.free`). |
| `LAMP_KINDS` | a portar | sim/buildings/interior | The pieces that give light: the room lights are where these are. |
| `LevelFlight` | a portar | sim/buildings/interior | A flight of stairs between two blocks of a building standing at floors of their own (a split level, `Volume.lift`) |
| `NONE_FILED` | não se aplica | `native/src/sim/buildings/interior/plan.cpp` → `filed_.find` — evitava alocação em JS; não existe em C++ | A piece's half extents across the floor, with the gap kept round it (`Plan.free`).  A cell no piece is filed in (`Plan.free`): DO NOT make an empty li |
| `Partition` | portado | `native/src/sim/buildings/interior/interior.h` → `struct Partition` | A thin wall between rooms, from (x0,y0) to (x1,y1). |
| `Plan` | portado | `native/src/sim/buildings/interior/plan.h` → `class Plan` | One block's floor, being furnished: where things may stand, and what stands. |
| `Plan.alongBack` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::along_back(` | Pieces along the back wall (or the left/right), facing into the room. |
| `Plan.alongSide` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::along_side(` |  |
| `Plan.constructor` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::Plan(` |  |
| `Plan.cores` | portado | `native/src/sim/buildings/interior/plan.h` → `cores_;` | Index loops: DO NOT walk these with an iterator, one made for each piece tried (`free` is asked for every place a piece is tried). |
| `Plan.corner` | não se aplica | `native/src/sim/buildings/interior/plan.cpp` → `bool Plan::inside` — evitava alocação em JS; não existe em C++ |  |
| `Plan.d` | portado | `native/src/sim/buildings/interior/plan.h` → `float depth()` |  |
| `Plan.filed` | portado | `native/src/sim/buildings/interior/plan.h` → `filed_;` |  |
| `Plan.free` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::free(` | Whether a piece of this size fits at (x, y): inside the walls, clear of the cores and what stands. |
| `Plan.grid` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::grid(` | A grid of `kind` filling a rectangle, `gx` x `gy` apart, all facing `angle`. |
| `Plan.items` | portado | `native/src/sim/buildings/interior/plan.h` → `items;` |  |
| `Plan.put` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::put(` | Living room: the TV on the side wall, the sofa facing it, a table by the window, the kitchen along the corridor wall. |
| `Plan.ring` | portado | `native/src/sim/buildings/interior/plan.h` → `ring_;` |  |
| `Plan.w` | portado | `native/src/sim/buildings/interior/plan.h` → `float width()` |  |
| `Plan.wall` | portado | `native/src/sim/buildings/interior/plan.h` → `void wall(float ax` | A wall from (u0, s0) to (u1, s1). |
| `Plan.wallAcross` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::wall_across(` | A wall across x at `y`, with a doorway at `door` (local x). |
| `Plan.wallAlong` | portado | `native/src/sim/buildings/interior/plan.cpp` → `Plan::wall_along(` |  |
| `Plan.walls` | portado | `native/src/sim/buildings/interior/plan.h` → `walls;` |  |
| `Plan.x0` | portado | `native/src/sim/buildings/interior/plan.h` → `x0 = 0` |  |
| `Plan.x1` | portado | `native/src/sim/buildings/interior/plan.h` → `x1 = 0` |  |
| `Plan.y0` | portado | `native/src/sim/buildings/interior/plan.h` → `y0 = 0` |  |
| `Plan.y1` | portado | `native/src/sim/buildings/interior/plan.h` → `y1 = 0` |  |
| `RIGHT` | portado | `native/src/sim/buildings/interior/plan.h` → `RIGHT_ANGLE` |  |
| `RoomCell` | portado | `native/src/sim/buildings/interior/plan.h` → `struct RoomCell` | One room off a corridor, as `roomsFloor` hands it to be furnished. |
| `backRoom` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `float back_room` | A room across the back of the floor, `depth` deep, behind a wall with a door near its end: a stockroom, a kitchen. |
| `cells` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void cells` |  |
| `classRoom` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void class_room` | A classroom: the board on the side wall, desks and chairs facing it, the teacher's desk. |
| `classrooms` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void classrooms` |  |
| `cutOpen` | outro porte | Porte 8 (vista do interior) — Decisão da coordenadora, a pedido do usuário: sessão própria, que parte do convert/build.  | The building as drawn cut open at `level` |
| `defaultFunction` | portado | `native/src/sim/buildings/interior/interior.cpp` → `purpose_of` |  |
| `deskRows` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void desk_rows` | Rows of desks facing the windows, from `s0` in to `s1` (the corridor side by default). |
| `dining` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void dining` |  |
| `face` | portado | `native/src/sim/buildings/interior/plan.h` → `inline float face` | The way a piece faces, as a direction in the floor's own axes (+y is the back). |
| `flatFloor` | portado | `native/src/sim/buildings/interior/interior_homes.cpp` → `bool flat_floor` | A floor of flats, from its plan (`spaces.ts`) |
| `flightBox` | a portar | sim/buildings/interior | A flight's footprint, local, a hand's breadth round it: [x0, y0, x1, y1]. |
| `floorType` | portado | `native/src/sim/buildings/interior/interior.cpp` → `Interior::type_of` | DO NOT furnish every storey of a tower: a floor is furnished from what it reads of its level alone - ground or not, its height, the cores through it,  |
| `furnish` | portado | `native/src/sim/buildings/interior/interior.cpp` → `void furnish(Plan` | Furnishes one block's floor according to what the building is for. |
| `furnishFloor` | portado | `native/src/sim/buildings/interior/interior.cpp` → `Interior::at` |  |
| `furnishedFor` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `types_;` — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | DO NOT key them by the record (each emission's body is a new one on the same blocks, `emitChunk`) nor cap them: kept on the blocks list, let go with i |
| `furnishedOf` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `levels_;` — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | Each blocks list's furnished floors, one set per rest of the building it was asked with. |
| `furnishingOf` | a portar | sim/buildings/interior | A floor's furniture as stored pieces: what an edit starts from. |
| `homeUnits` | portado | `native/src/sim/buildings/interior/interior_homes.cpp` → `void home_units` | Homes (or hotel rooms) along the floor, each a few rooms with their furniture. |
| `hotelRoom` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void hotel_room` | A hotel room: the bed, a desk by the window, a wardrobe, a bathroom by the corridor. |
| `houseFloor` | portado | `native/src/sim/buildings/interior/interior_homes.cpp` → `void house_floor` | A floor of a house: walls on the bays' boundaries, so no wall cuts a window. |
| `houseFloor.stairs` | portado | `native/src/sim/buildings/interior/interior_homes.cpp` → `FK_STAIRWELL` | A flight to the floor above (its foot at the front), and the opening it arrives through. |
| `interiorAt` | portado | `native/src/sim/buildings/interior/interior.h` → `const FloorInterior &at(int level)` | One floor of a building, inside: furnished once for its blocks, the rest it reads and its type (the records sharing them share it; a building emitted, |
| `isFurnitureKind` | a portar | sim/buildings/interior |  |
| `itemCell` | portado | `native/src/sim/buildings/interior/plan.cpp` → `cell_key(` | A cell's key, a small integer (cells within 32 km of the block's frame): DO NOT key past 2^30, every key a heap number made for each lookup. |
| `kitchen` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void kitchen` | A kitchen behind `wall`: cooking along the back, a prep table. |
| `letGoInterior` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `class Interior` — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | A blocks list's furnished floors let go now (`buildingMesh.ts` `letGoEmitted`: a job's building emitted, its furniture not read again by the job): ask |
| `levelFlights` | a portar | sim/buildings/interior | The flights between the blocks that have a storey on `level` at different heights and stand side by side |
| `lobby` | portado | `native/src/sim/buildings/interior/interior_homes.cpp` → `void lobby` |  |
| `officeRoom` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void office_room` | An office room: rows of desks from the windows back, chairs behind them, a shelf on the side wall |
| `placed` | a portar | sim/buildings/interior | A placed piece at its kind's size. |
| `roomLights` | portado | `native/src/sim/buildings/interior/interior.cpp` → `void room_lights` | A ceiling light over each group of furniture: a room's worth. |
| `roomsFloor` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `bool rooms_floor` | A floor as rooms off a corridor, like the flats (`spaces.flatPlan`) |
| `roomsFloor.P` | portado | `native/src/sim/buildings/interior/plan.cpp` → `RoomCell::put` |  |
| `roomsFloor.wall` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `r.wall(0, D` | A wall from (u0, s0) to (u1, s1). |
| `shopsAround` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void shops_around` |  |
| `wardRoom` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void ward_room` | A ward: beds along the side walls, heads to the wall, a chair by each. |
| `wards` | portado | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void wards` |  |

### `src/world/buildings/intern.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: THE BUILDINGS' RECORDS SHARE WHAT THEY HAVE ALIKE (hash-consing): one storey, facade or material object for every building)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BITS` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A number's bits, to hash it without making its text (DO NOT `String(v)`: a string for every number of every building loaded). |
| `Plain` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A record's member made of plain data (numbers, strings, booleans, null, arrays and objects of those). |
| `SharedMembers` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | RECORDS' MEMBERS SHARED AMONG THEM, A BATCH AT A TIME, in place (hash-consing as `intern`, with tables of its own): what records written to a table ha |
| `SharedMembers.batchStart` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.buckets` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.callStart` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.canon` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.code` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.end` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The batch over: the members met twice in it kept for the next (`keeps`), the others let go. |
| `SharedMembers.end.keep` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.hashes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.hits` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.ids` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Each canonical object's number: what a parent's key is made of (its children are canonical, so equal means the same object). |
| `SharedMembers.keepChildren` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.keeps` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether a member is kept for the next batch (`share`). |
| `SharedMembers.keyCodes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.local` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.next` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.share` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The records' members made canonical, in place, against those kept and each other. Several lists may make one batch (a city's records, then the same mi |
| `SharedMembers.shareRecord` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SharedMembers.sharedAgain` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `WORDS` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `collected` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT keep the entry of an object collected (its WeakRef, its bucket, its hash's key): a bucket was only swept when its own hash came again, so every |
| `hashString` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `hashValue` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `ids` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Each canonical object's number: what a parent's key is made of (its children are canonical, so equal means the same object). |
| `intern` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The canonical object equal to `value` (its children made canonical first): the one already shared, or `value` itself. |
| `internMembers` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A record's object members made canonical, in place (the record itself stays its own: its id, place and wear). |
| `internedEntries` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Entries of the table now, and hashes: what the tests read. |
| `letGoInterned` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A canonical object collected: its entry let go, and its hash's bucket with it once empty. |
| `mix` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Inside the hash: 32-bit FNV-1a over the key's pieces. |
| `nextId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `sameShallow` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether two nodes with canonical children are the same: their primitives equal, their children the same objects. |
| `table` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The canonical objects by their shallow key's hash; held weakly, a shape no building uses any more goes. |

### `src/world/buildings/items.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: THE STORE'S BUILDINGS: A RECORD TABLE (`TableRows`, decoded when read) WITH THE RECORDS WRITTEN SINCE OVER IT. A `Map` to its readers, in a `Map`'s order (a key set again keeps its place, one deleted )_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingItems` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | THE STORE'S BUILDINGS: A RECORD TABLE (`TableRows`, decoded when read) WITH THE RECORDS WRITTEN SINCE OVER IT. A `Map` to its readers, in a `Map`'s or |
| `BuildingItems.[Symbol.iterator]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.[Symbol.toStringTag]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.added` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.before` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Told a key's value before it is written (undefined: the key was not there); null: nobody listens. |
| `BuildingItems.clear` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Every record let go, the table with them. |
| `BuildingItems.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.copy` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A copy that reads the same table and holds the same records written (`BuildingStore.shareFrom`): its cost the records written, never the table. |
| `BuildingItems.delete` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.entries` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.forEach` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.get` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The table under the records written (null: none, every record written here). |
| `BuildingItems.gone` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.has` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.inBase` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.keys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.over` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.owns` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether the record of `key` was written here (not the table's own). |
| `BuildingItems.replaced` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.sameUnwritten` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether `key` reads the very same record here and in `other`, known without decoding it: both over one table, neither wrote it. |
| `BuildingItems.set` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.size` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.table` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.tableRef` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where a worker reads the record of `key` when it is the table's own (not written here): the table's buffer and its row, nothing decoded; null otherwis |
| `BuildingItems.values` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingItems.writes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | What was written over the table, in the order `BuildingItems.over` puts it back (the save's sections, the simulation's map). |

### `src/world/buildings/lotLibrary.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: The lots made in the lot lab (`lots/*.json`, 2026-10-05): each a building of the tower kit with its whole lot)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LOT_NAMES` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The names of the lots, in a stable order. |
| `loadLot` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Reads one lot, or null when there is none of that name. |
| `loaded` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The lots read so far, by name (`loadedLot`). |
| `loadedLot` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A lot already read (`loadLot`), or null: for what must answer at once (a picture of it). |
| `loaders` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The lots made in the lot lab (`lots/*.json`, 2026-10-05): each a building of the tower kit with its whole lot |
| `lotKey` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The key a lot goes by in the builder's models. |
| `nameOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |

### `src/world/buildings/lotTemplate.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: A lot kept in the lot lab's library (`?lab=lots`, `editor/lotLab.ts`, `lot-library-plugin.ts`))_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LotTemplate` | a portar | sim/buildings/b40/* | A lot kept in the lot lab's library (`?lab=lots`, `editor/lotLab.ts`, `lot-library-plugin.ts`) |

### `src/world/buildings/lots.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: The surface of a building's open lot - its car park, its yard - as it is laid)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `KERB_RISE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Rise of a lot's edge over the pavement it meets. |
| `LOOK_OUT` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far outside a lot's edge the street is looked for. |
| `LotSlope` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A lot's surface as plain numbers, copied to a worker as they are |
| `LotSurface` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The surface of a building's open lot - its car park, its yard - as it is laid |
| `PavingAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The pavement height at a world point, or NaN where there is none. |
| `clamp01` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `lotHeightAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | DO NOT change its arithmetic for one side only |
| `lotSurfaces` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |

### `src/world/buildings/materials.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: What a building is made of.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingMaterials` | a portar | sim/buildings/b40/* |  |
| `FINISHES` | a portar | sim/buildings/b40/* |  |
| `FINISH_COLOUR` | a portar | sim/buildings/b40/* | Each finish's own colour, as its swatch shows it: brick is brick-red before it is painted. |
| `FLAT_ROOFS` | a portar | sim/buildings/b40/* | Flat roofs as a town has them: tar-dark membrane, grey membrane, white reflective sheet, beige gravel |
| `FLAT_ROOF_MATERIAL` | a portar | sim/buildings/b40/* | A flat roof's default: a smooth membrane, whatever the palette (a panel grid read as a floor). |
| `Finish` | a portar | sim/buildings/b40/* |  |
| `GENERIC_SLAB` | a portar | sim/buildings/b40/* | The slab the generator gives a tower or an office block (`procedural.ts`, `towerKit.ts`) |
| `MATERIAL_SLOTS` | a portar | sim/buildings/b40/* | Surfaces a building-wide material can be set for. |
| `MaterialSlot` | a portar | sim/buildings/b40/* |  |
| `MaterialSpec` | a portar | sim/buildings/b40/* |  |
| `MaterialTarget` | a portar | sim/buildings/b40/* | Where a material is being set. |
| `PALETTE_MATERIALS` | a portar | sim/buildings/b40/* |  |
| `PLINTH_MATERIAL` | a portar | sim/buildings/b40/* |  |
| `PaletteMaterials` | a portar | sim/buildings/b40/* | The colour schemes a building starts with (`Building.palette`) |
| `STYLES` | a portar | sim/buildings/b40/* | Factory styles: a coherent set of materials for a whole building, applied in one click and freely changed afterwards |
| `VolumeMaterials` | a portar | sim/buildings/b40/* |  |
| `applyMaterial` | portado | `native/src/sim/commands/body_finish.cpp` → `std::string op_paint(` | Sets (or with `null` clears) the material of a target, in place. |
| `applyMaterial.tidyVolume` | a portar | sim/buildings/b40/* |  |
| `applyStyle` | a portar | sim/buildings/b40/* | Applies a style to a whole building (clearing per-volume and per-face overrides). |
| `applyStyle.state` | a portar | sim/buildings/b40/* | The storeys' own paint too (`wallMaterial` reads it first) |
| `isFinish` | a portar | sim/buildings/b40/* |  |
| `isMaterialSpec` | a portar | sim/buildings/b40/* |  |
| `isPitched` | a portar | sim/buildings/b40/* |  |
| `materialAt` | a portar | sim/buildings/b40/* | The material a target currently resolves to (what the palette shows as chosen). |
| `migrateBuildingMaterials` | a portar | sim/buildings/b40/* |  |
| `migrateMaterial` | a portar | sim/buildings/b40/* | A stored material, repaired: a valid spec, or undefined. |
| `migrateVolumeMaterials` | a portar | sim/buildings/b40/* |  |
| `paletteOf` | a portar | sim/buildings/b40/* |  |
| `plinthMaterial` | a portar | sim/buildings/b40/* |  |
| `roofMaterial` | a portar | sim/buildings/b40/* |  |
| `spec` | a portar | sim/buildings/b40/* |  |
| `trimMaterial` | a portar | sim/buildings/b40/* |  |
| `wallMaterial` | a portar | sim/buildings/b40/* |  |

### `src/world/buildings/neighbours.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: A STANDING BUILDING AS ITS NEIGHBOURS READ IT: what a building checked or grown beside it asks of it (`validate.ts` `validateOn`, `world/growth/zoning.ts`) - where it stands, what it puts on the groun)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Neighbour` | a portar | sim/buildings/b40/* |  |
| `NeighbourSource` | a portar | sim/buildings/b40/* | Where a neighbour is asked for: the buildings by place (`BuildingStore.near`, a superset). |
| `Obstacle` | a portar | sim/buildings/b40/* | A ring a building puts on the ground (world units) and its box. |
| `RecordNeighbour` | a portar | sim/buildings/b40/* | A stored record read as a neighbour, each answer worked out when first asked. |
| `RecordNeighbour.bounds` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.constructor` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.footprints` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.id` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.obstacles` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.projections` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.reach` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.signature` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.storeys` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.top` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.x` | a portar | sim/buildings/b40/* |  |
| `RecordNeighbour.y` | a portar | sim/buildings/b40/* |  |
| `groundOf` | a portar | sim/buildings/b40/* | What a stored building puts on the ground - its volumes, projections and ground elements - as world rings with their boxes, once per record (a record  |
| `neighbourOf` | a portar | sim/buildings/b40/* | A stored record as its neighbours read it, made once per record (the page's side). |
| `ringBox` | a portar | sim/buildings/b40/* | A ring's box. |
| `settledNeighbour` | a portar | sim/buildings/b40/* | Every answer worked out now, the record not kept (the growth's worker, `editor/growthReplica.ts`). |
| `signatureOf` | a portar | sim/buildings/b40/* | Each record's signature, worked out once (a record is replaced when its building changes). |
| `storeNeighbours` | a portar | sim/buildings/b40/* | The buildings of a store by place, read as neighbours (the page's side). |
| `storeNeighbours.near` | a portar | sim/buildings/b40/* |  |
| `storeysOf` | a portar | sim/buildings/b40/* | The top of a building's tallest built block, in storeys. |

### `src/world/buildings/pads.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: The ground graded to every building)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BOUND_SLACK` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Rounding allowed between a ring's distance and its bounds' (`shapeAt`) |
| `BuildingPads` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `CELL_LISTS_KEPT` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `FILE_CELLS` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Cells a side of the squares the platforms' ids are filed under. |
| `Grading` | a portar | sim/buildings/foundation, render/buildings/facade_emit | What `shapeOverPads` works in, point after point, owned by the platforms that grade (`buildingPads`, `padShaper`): each ring's bound and its distance  |
| `Grading.answer` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `Grading.bounds` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `Grading.distanceOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The distance from (x, y) to ring `at` of the walk (`ring`), measured once for both walks of `shapeOverPads`. |
| `Grading.measured` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `Grading.ringAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `Grading.shaped` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `HYPOT_TIE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Squares of an edge's distance this close to the least (relative) have their `Math.hypot` taken: far wider than the rounding of either, so the edge `Ma |
| `LOT_PLATE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far a paved lot's plate stands over the graded ground under it. |
| `NO_IDS` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `NO_PADS` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `PADS_KEPT` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platforms kept worked out (`townPads` with a resolver), and the cells' lists of them kept put together: what a shaping round the view reads again  |
| `PAD_BATTER` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Slope of the bank: horizontal run per unit of height. |
| `PAD_CELL` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Side of the cells the town's platforms are filed under (`townPads`). |
| `PAD_REACH` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Furthest a bank may reach from its platform. |
| `POOL_SINK` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far water stands below its lot's edge: a pool's or a pond's rim. |
| `POOL_TERRAIN_CLEARANCE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Clearance for water over the triangulated terrain between grid corners. |
| `Pad` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `PadLevel` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platform's height under a ring, as plain numbers (`padLevelAt`) |
| `PadShapeCut` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The town's platforms a shaping of the ground can read inside some boxes, as plain data |
| `TownPads` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The town's platforms, one put in, changed or taken out by its building's id (`put`). |
| `buildingPads` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The pads of every building. |
| `padCellKey` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `padLevelAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A platform's height under one of its rings at a point (a car park falls with its street). |
| `padOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | One building's platform (its blocks' and its open lots' rings, levels and bounds); null for one with none. |
| `padShaper` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platforms of a cut (`TownPads.cut`) graded as the town grades them |
| `padShaper.shapeAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The graded height at a point, and how fully it replaces the land (0 or 1): one answer reused, read before the next call. |
| `padsWork` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The work `shapeAt` did: rings measured (`ringDistance` calls). |
| `ringDistance` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Distance from a point to a closed ring: 0 inside it.  DO NOT take `Math.hypot` of every edge again (it ran for each edge of each ring at each corner o |
| `shapeOverPads` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The graded height at a point from the platforms filed under its cell, in their order |
| `townPads` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platforms filed under every grid cell their box reaches: a corner of the ground asks only the ones over it.  DO NOT hold every building's platform |
| `townPads.cut` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platforms a point inside `boxes` can read, as plain data for the ground's worker (`padShaper`). |
| `townPads.forEachCell` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Each cell a box reaches. |
| `townPads.forget` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `townPads.idsIn` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ids whose box reaches cell (cx, cy), in its square's order; null for none. |
| `townPads.keep` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `townPads.listOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `townPads.padAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | An id's platform: kept, or worked out again. |
| `townPads.put` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's platform now (null: it has none, or is gone). |
| `townPads.shapeAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The graded height at a point, and how fully it replaces the land (0 or 1): one answer reused, read before the next call. |
| `townPads.shapeBounds` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Made when asked (a shaping of the whole map), never kept: an object a building. |
| `townPads.squareOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `townPads.squaresOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The squares a box reaches. |

### `src/world/buildings/pick.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: A ray, in WORLD axes: x and y on the map, z up.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingHit` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `OPEN_PLATE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How thick an open block's plate is picked: its kerb and its edge, under its surface. |
| `Ray3` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A ray, in WORLD axes: x and y on the map, z up. |
| `heightOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How high a building reaches over its ground floor: its tallest volume with its roof, and the parts standing on it |
| `pickBuilding` | portado | `native/src/sim/commands/body_pick.cpp` → `BodyHit pick_body(` | The nearest building face under a ray. |
| `rayNearBox` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `slab` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Ray against an axis-aligned box: entry distance and the axis entered through. |

### `src/world/buildings/procedural.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: Buildings made to measure for a lot, never the same twice.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `AWNINGS` | a portar | sim/buildings/b40/* |  |
| `BRICKS` | a portar | sim/buildings/b40/* |  |
| `CLAY` | a portar | sim/buildings/b40/* | The clay tile roofs alone: a colonial or a bungalow roof is ceramic. |
| `Envelope` | a portar | sim/buildings/b40/* | What the lot asks of the building. |
| `HARD` | a portar | sim/buildings/b40/* |  |
| `HouseForm` | a portar | sim/buildings/b40/* |  |
| `MAX_ROOF_RISE` | a portar | sim/buildings/b40/* | The most a roof rises over its eaves, metres: a gable or a hip about a storey |
| `MadeBuilding` | a portar | sim/buildings/b40/* |  |
| `PITCH` | a portar | sim/buildings/b40/* | Pitches drawn, degrees, as built: clay tiles 20-35 |
| `ROOF_TILES` | a portar | sim/buildings/b40/* | Pitched roofs: clay tiles in their reds and browns, slate, fibre-cement and metal sheet. |
| `Rect` | a portar | sim/buildings/b40/* |  |
| `Signature` | a portar | sim/buildings/b40/* | What the eye tells two buildings apart by. |
| `WORKS_ROOFS` | a portar | sim/buildings/b40/* | A works hall's panel roofs (`works`). |
| `backDoorOn` | a portar | sim/buildings/b40/* | A back door on the rear of the block at the back, near the middle. |
| `baysOf` | a portar | sim/buildings/b40/* | The columns a face of a block is shared into (as composed, else on the 3 m module). |
| `between` | a portar | sim/buildings/b40/* |  |
| `bodySignature` | a portar | sim/buildings/b40/* | The signature of any building, grown or not, read off its record as the eye reads it |
| `bodySignature.token` | a portar | sim/buildings/b40/* |  |
| `doorAt` | a portar | sim/buildings/b40/* | Moves the door of a block's front to the bay holding `x` (units, local). |
| `dressBuilding` | a portar | sim/buildings/b40/* | After the facades are composed: the entrance |
| `finish` | a portar | sim/buildings/b40/* | Finishes a body: pitched roofs given their pitch, the facades composed in the style (`architecture.ts`) |
| `flats` | a portar | sim/buildings/b40/* | Flats: a bar, an L or a U round a courtyard, stepped at the top; a tower on a podium when high. |
| `half` | a portar | sim/buildings/b40/* | Rounds to the 0.5 m the Builder's grid uses. |
| `hang` | a portar | sim/buildings/b40/* | Adds a facade part (a canopy, a marquise) unless it would cut into the building. |
| `house` | a portar | sim/buildings/b40/* | A house, in one of six forms chosen by case on the envelope and then at random among those that fit |
| `houseRoof` | a portar | sim/buildings/b40/* | The roof a house style builds, by its weights: a colonial house's parapet, a bungalow's tiles, a modern house's slab. |
| `lightness` | a portar | sim/buildings/b40/* | Lightness, 0..1, of a colour. |
| `madeToMeasure` | a portar | sim/buildings/b40/* | A building for an envelope `W` x `D` metres of a lot, for a zone's use and density. |
| `nudge` | a portar | sim/buildings/b40/* | A colour nudged in hue, saturation and lightness, a little |
| `nudge.ch` | a portar | sim/buildings/b40/* |  |
| `office` | a portar | sim/buildings/b40/* | An office building: glass, a base and a tower stepped back as it rises, or a slab. |
| `otherSkin` | a portar | sim/buildings/b40/* | A second skin of the same style, for the other half of a pair side by side (never a block stacked on another). |
| `pickOf` | a portar | sim/buildings/b40/* |  |
| `roofOf` | a portar | sim/buildings/b40/* | A roof material, nudged. |
| `roofShape` | a portar | sim/buildings/b40/* | Pitch and ridge of a pitched roof, drawn: a gable end to the street one time in three |
| `setbacks` | a portar | sim/buildings/b40/* | Recursive setbacks (CityEngine's `RecursiveSetbacks`) |
| `shop` | a portar | sim/buildings/b40/* | A street shop: shopfronts on the ground floor, flats or offices above, a stockroom behind. |
| `signatureDistance` | a portar | sim/buildings/b40/* | How unlike two signatures are, 0 (the same building) to about 4 |
| `signatureOf` | a portar | sim/buildings/b40/* | The signature of a body made: its form, its main wall and roof, its height. |
| `towerShape` | a portar | sim/buildings/b40/* | A tower's plan, normalized to its block: undefined |
| `works` | a portar | sim/buildings/b40/* | A works: an office on the street corner, the hall behind it with its loading doors at the back. |

### `src/world/buildings/recordTable.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: A TILE'S BUILDINGS AS ONE TYPED TABLE IN SHARED MEMORY (CLAUDE.md "Architecture": per-tile typed tables read by stateless jobs): columns for what is read without the record (id, place, rotation), each)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FORMAT` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Bumped when the layout below changes. |
| `HEAD` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Header words: magic, format, rows, strings, string units, shapes, shape keys, nodes, stream bytes. |
| `Layout` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where each section starts, from the header's counts. |
| `MAGIC` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordRef` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where a building's record is read from: a table and its row (posted to a worker as the table's buffer and the row). |
| `RecordTable` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A table read back from its buffer (posted or shared): its columns are views on it; its records are decoded by a `TableReader`. |
| `RecordTable.at` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | `keepShared`: a member written as a node in one batch is that node in the batches after (the members shared across batches, `intern.ts` SharedMembers) |
| `RecordTable.ids` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.reader` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A reader of this table: what it decodes (strings, shapes, shared members) is kept for its own life only (a job's). `weak`: its shared members kept onl |
| `RecordTable.rotations` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.row` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The row holding a building's id; -1 when it is not in the table. |
| `RecordTable.rowOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.rows` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.xs` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RecordTable.ys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A growable byte stream the encoder writes the members into. |
| `Stream.bytes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.f64` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.i32` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.length` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.room` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.room.grow` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Doubled while small; past 64 MB a quarter more (a city's table, `TableWriter`: DO NOT leave up to half of a buffer of hundreds of MB empty). |
| `Stream.u32` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.u8` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `Stream.view` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `T_NULL,T_FALSE,T_TRUE,T_U8,T_I32,T_F64,T_STR,T_ARR,T_OBJ,T_NODE` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT keep a reader past the job it was made for: a stateless job decodes what it reads and lets it go; the shared members it decoded are one object  |
| `TableReader.collected` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | `keepShared`: a member written as a node in one batch is that node in the batches after (the members shared across batches, `intern.ts` SharedMembers) |
| `TableReader.decoded` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.nodeAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.nodes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.pos` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.record` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A row's record: a fresh object of its own, its shared members the reader's. |
| `TableReader.rootAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.shape` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.shapeKeys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.shapeStart` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.shapes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.skip` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.strStart` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.strUnits` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.string` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.strings` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.value` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A shared member is written where it is first met, behind its node's mark; later meetings write only the mark. |
| `TableReader.view` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableReader.weakNodes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A TABLE WRITTEN A BATCH OF ROWS AT A TIME (a city generated off the page, `editor/smallCityMap.ts` buildCityTable): every string and object shape once |
| `TableWriter.add` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Rows written after those before; a member shared between rows of this batch written once, as a node. |
| `TableWriter.add.body` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.add.number` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.add.value` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A shared member is written where it is first met, behind its node's mark; later meetings write only the mark. |
| `TableWriter.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | `keepShared`: a member written as a node in one batch is that node in the batches after (the members shared across batches, `intern.ts` SharedMembers) |
| `TableWriter.finish` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The table as written so far, in a buffer of its own (shared where the thread is isolated, `allocate`). |
| `TableWriter.idAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The id of a row written. |
| `TableWriter.ids` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.keepOnly` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Of the nodes kept across batches (`keepShared`), only those `keep` keeps (`SharedMembers.keeps`): DO NOT keep every node ever written, they grew with  |
| `TableWriter.keptNodes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.nodeAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.pos` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.read` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A row written, decoded: a fresh object of its own, its shared members its own too. |
| `TableWriter.rootAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.rows` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.shapeId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.shapeList` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.shapeNames` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.shapes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.stream` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.stringId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.stringList` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.strings` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.turns` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.value` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A shared member is written where it is first met, behind its node's mark; later meetings write only the mark. |
| `TableWriter.xs` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableWriter.ys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `align8` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `allocate` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Shared memory where the page is cross-origin isolated (and in workers and tests); a plain buffer otherwise (only read on its own thread).  DO NOT trus |
| `eachRecord` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Records read from their refs one at a time, as `readRecords` reads them: DO NOT hold them all where each is used once - what is worked out of a record |
| `encodeRecords` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The rows written as one table: their values as JSON would keep them (undefined members left out, in arrays null), numbers exact. |
| `isRecordTable` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether a buffer holds a record table of this format: written once when encoded (`encodeRecords`), never again - a job process's thread keeps the copy |
| `layout` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `layout.take` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `readRecords` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Records read from their refs, one reader a table for the batch (a job's): the shared members of a tile decoded once. |
| `readRow` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A row's record read on the page, knowing its row: a job is handed the row, never the record (`refOf`). |
| `refOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where a worker reads a record: its tile's row, or a table of its own (an edited or new building, or one opened by the cut). |
| `refs` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Each record's place in a table: its tile's (`tableRecords`) or its own, written once when first asked (a record is replaced, never edited). |
| `sharedObjects` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Every object reached more than once from the rows (a member shared by `intern.ts`): written once, as a node. |
| `sharedObjects.walk` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `tableBytes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A table's bytes as a store keeps them: a plain buffer, copied when the table is in shared memory. |
| `tableIn` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A table's bytes in a buffer of their own (shared where the thread is isolated, `allocate`): one read back from a store, which keeps plain buffers only |
| `tableRecords` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT read a whole city's table this way on the page (`world/buildings/tableRows.ts` reads a row when asked): every record decoded and kept is ~8 KB  |

### `src/world/buildings/reference.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: A reference model's geometry as the Builder reads it (`editor/fromReference.ts`) and as the renderer samples it)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Point3` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A point of the model, metres: x and y on the plan, z up. |
| `ReferenceSampler` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A colour the model shows at a point of its surface, seen along the normal, or null (nothing there). |
| `Triangle` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | One triangle of the model's surface. |

### `src/world/buildings/roofPlant.ts`

**Estado:** a portar · **Dono no jogo:** render/buildings/facade_props, roof_plant, roof_terrace

**Para o jogador:** Ar-condicionado, antenas, vasos, varais, equipamentos de cobertura. _(no 4.0: WHAT STANDS ON A FLAT ROOF, chosen and packed per building.)_

**Motivo:** Sem prova por símbolo até agora. 

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CATALOGUE` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `CELL` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | Grid cell of the packing, and the clear way kept round each piece. |
| `CLEAR` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `EDGE` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | The parapet's margin: nothing stands against the roof's edge. |
| `Kind` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `RoofPlantItem` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `RoofPlantKind` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `RoofSite` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `USE` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `planRoofPlant` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `planRoofPlant.free` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `planRoofPlant.take` | a portar | render/buildings/facade_props, roof_plant, roof_terrace |  |
| `stream` | a portar | render/buildings/facade_props, roof_plant, roof_terrace | A small seeded stream (mulberry32): the same roof, the same plant. |

### `src/world/buildings/sections.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: A STORE'S BUILDINGS AS BUFFERS, NEVER AS OBJECTS (the save's sections, the simulation's map, the undo of a whole map): the table they were opened from, as it is (shared, never copied), and what was wr)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingSections` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A STORE'S BUILDINGS AS BUFFERS, NEVER AS OBJECTS (the save's sections, the simulation's map, the undo of a whole map): the table they were opened from |
| `eachSectionRecord` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Every record of the sections in the store's order, one at a time (a worker's copy: `sim/agents/trafficBuildings.ts`): DO NOT hold them all - what is w |
| `eachSectionRef` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where each record of the sections is read, in the store's order (`eachSectionRecord`'s order): a row of the table, of the records replaced or of those |
| `itemsOfSections` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The store's items read back from their sections: the table read when asked (`TableRows`), the records written decoded now (few). |
| `itemsOfSections.written` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `sectionsBytes` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The bytes the sections hold. |
| `sectionsOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The sections of a store's buildings: the table shared as it is, only the records written encoded (`encodeRecords`). A map that is no table (a working  |
| `sectionsSize` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | How many buildings the sections hold. |
| `sectionsSize.rows` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `tableOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |

### `src/world/buildings/serialize.ts`

**Estado:** não se aplica · **Dono no jogo:** sim/persistence/save_*

**Para o jogador:** Leitura de saves JSON antigos do 4.0. _(no 4.0: The stored shape of a building is the model itself, as plain JSON, with `schema` naming its version.)_

**Motivo:** O jogo não abre saves do 4.0.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Loose` | não se aplica | sim/persistence/save_* |  |
| `MIGRATED` | não se aplica | sim/persistence/save_* | The buildings this thread's `migrateBuilding` made: already current (records are replaced, never edited). |
| `Scale` | não se aplica | sim/persistence/save_* | Lengths are stored in world units since schema 2; schema 1 stored whole cells of `module`. |
| `SerializedBuilding` | não se aplica | sim/persistence/save_* | The stored shape of a building is the model itself, as plain JSON, with `schema` naming its version. |
| `finite` | não se aplica | sim/persistence/save_* |  |
| `height` | não se aplica | sim/persistence/save_* |  |
| `int` | não se aplica | sim/persistence/save_* |  |
| `isRecord` | não se aplica | sim/persistence/save_* |  |
| `isSerializedBuildings` | não se aplica | sim/persistence/save_* | Boundary check used by `editor/persistence.ts`: an array, or absent. |
| `migrateBuilding` | não se aplica | sim/persistence/save_* | Any stored building, of any schema this build knows, as a current one: DO NOT migrate one of its own again (a new object each time, every derived prod |
| `migrateCore` | não se aplica | sim/persistence/save_* |  |
| `migrateElement` | não se aplica | sim/persistence/save_* | Elements came with schema 2: world units. |
| `migrateFacade` | não se aplica | sim/persistence/save_* |  |
| `migrateRelief` | não se aplica | sim/persistence/save_* | Reliefs came with schema 2: their depth is always in world units. |
| `migrateSpace` | não se aplica | sim/persistence/save_* |  |
| `migrateStorey` | não se aplica | sim/persistence/save_* |  |
| `migrateVolume` | não se aplica | sim/persistence/save_* |  |
| `num` | não se aplica | sim/persistence/save_* |  |
| `size` | não se aplica | sim/persistence/save_* |  |

### `src/world/buildings/siteGrading.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: THE BUILDINGS' SITES AS THE GROUND WAS LAST GRADED TO THEM: each building's platform)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `SiteBox` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A world box: [minX, minY, maxX, maxY]. |
| `SiteGrading` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `siteGrading` | a portar | sim/buildings/foundation, render/buildings/facade_emit | `groundAt`: the ground a building stands on before its own grading; `pavedAt`: the footway's height; `apron` |
| `siteGrading.boxAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `siteGrading.changedSites` | a portar | sim/buildings/foundation, render/buildings/facade_emit | What changed among the buildings since the ground was graded: the box of each site added, removed or changed |
| `siteGrading.changedSites.grade` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `siteGrading.changedSites.take` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's platform as worked out elsewhere (the ground's worker, `groundShape.ts` `padsOfAsk`), on its floor as the floors keep it: as `refresh` wo |
| `siteGrading.follow` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The platforms whose bank (`bankBox`, `reach` past the bounds) a change of what they read (the floor's ground, the paving) since the last `follow` reac |
| `siteGrading.forget` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The roads or the land moved: every platform is worked out afresh, and every site read again as graded. |
| `siteGrading.rebuild` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The town's platforms put together from each building's (`pads`); null for a town with none. |
| `siteGrading.refresh` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's platform worked out again (the roads round it moved), on its floor as the floors keep it (follow them first). |
| `siteGrading.resolvePad` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's platform worked out again by its id (the town keeps a bounded number of them, `pads.ts` townPads). |
| `siteGrading.siteKeyOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `siteGrading.take` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A building's platform as worked out elsewhere (the ground's worker, `groundShape.ts` `padsOfAsk`), on its floor as the floors keep it: as `refresh` wo |

### `src/world/buildings/spaces.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/interior

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte. _(no 4.0: Extension point for interiors and occupants.)_

**Motivo:** Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CORRIDOR` | portado | `native/src/sim/buildings/interior/plan.cpp` → `CORRIDOR = 1.6f` | Corridor width, and the depth from which a floor has homes on both sides. |
| `DOUBLE_LOADED` | portado | `native/src/sim/buildings/interior/plan.cpp` → `DOUBLE_LOADED = 11.f` |  |
| `FlatCell` | portado | `native/src/sim/buildings/interior/plan.h` → `struct FlatCell` | One home on a residential floor, and how it lies. |
| `FlatPlan` | portado | `native/src/sim/buildings/interior/plan.h` → `struct FlatPlan` |  |
| `KIND` | não se aplica | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` — cache e tipo de espaço do 4.0; o jogo guarda só o retângulo do espaço |  |
| `StoreySpaces` | portado | `native/src/sim/buildings/interior/interior.cpp` → `floor->spaces` | Extension point for interiors and occupants. |
| `deriveSpaces` | portado | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` | A building's spaces, worked out once for its blocks list, use and rhythm: DO NOT key them by the record (every emission's body is a new record, `emitC |
| `flatPlan` | portado | `native/src/sim/buildings/interior/plan.cpp` → `FlatPlan flat_plan(` | A residential floor of one block as homes: a corridor down its long axis |
| `spacesOf` | não se aplica | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` — cache e tipo de espaço do 4.0; o jogo guarda só o retângulo do espaço | Each blocks list's spaces, one per use and rhythm it was asked with (`workOutSpaces` reads the blocks, the use, the module and the storey heights alon |
| `workOutSpaces` | portado | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` |  |

### `src/world/buildings/spilledItems.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: The records decoded from the table kept as objects, the newest read last: a grown building's neighbours are read again and again while its block grows (`growth/zoning.ts` neighbourStoreys, nearSignatu)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `RECENT` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The records decoded from the table kept as objects, the newest read last: a grown building's neighbours are read again and again while its block grows |
| `SpilledItems` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | THE BUILDINGS OF A CITY BEING GROWN, THE OLDER ONES IN A TABLE (`TableWriter`), NOT AS OBJECTS (`editor/smallCityMap.ts` buildCityTable): a `Map` to t |
| `SpilledItems.[Symbol.iterator]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.[Symbol.toStringTag]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.clear` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | `shared`: what the records spilled have alike (`SharedMembers`), the batch ended by whoever passes it (`endBatch`) - the page's table shares it (`edit |
| `SpilledItems.decoded` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.delete` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.endBatch` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The batch of members shared ended (`SharedMembers.end`): the table keeps as nodes only the members kept. |
| `SpilledItems.entries` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.forEach` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.get` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | How many records are held as objects (not spilled). |
| `SpilledItems.gone` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.has` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.held` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.heldCount` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.keys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.over` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.ownShared` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.recent` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.rowOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.set` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.shared` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.size` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.spill` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | All but the newest `keep` records held moved into the table, in the order they were added; returned, for whoever writes them elsewhere too (the page's |
| `SpilledItems.spilled` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.spilledLive` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.table` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `SpilledItems.values` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |

### `src/world/buildings/splitLevel.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: A building that follows its hillside)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FILL_WEIGHT` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A fill costs this much more than a cut of the same depth. |
| `GAIN` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The least a step must save in mean cut and fill under the back to be built. |
| `GARAGE_BAY` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The narrowest bay a car's garage door is set in: a 2.4 m door and its jambs. |
| `MIN_PART` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The shallowest a part of a split block may be. |
| `ON_EDGE` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far a block's side may stand from the envelope's side and still look onto the side strip. |
| `SlopeSite` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `SplitLevel` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `clone` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `doorOn` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Whether storey 0 of a block has a way in on `side`. |
| `garageUnder` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A garage in the lower ground floor of a house stepped a whole storey down its hillside, on the side its drive runs down |
| `isAccess` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `liftStep` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The step a block's floor is snapped to when it is dragged |
| `opSetLift` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Sets the floor of block `id` to stand `lift` above (negative: below) the building's ground floor |
| `ownerOf` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The block a part hung on a face belongs to: the one whose face, on its side, is nearest. |
| `snap` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `snapLift` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A lift snapped to the nearest half storey (`liftStep`) and held within `MAX_LIFT`. |
| `splitBlock` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Splits block `v` across its depth at local `y` |
| `splitBlock.keep` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `stepToSlope` | a portar | sim/buildings/foundation, render/buildings/facade_emit | Steps the back of a body (local frame, its volumes in place on the lot) to the ground under it. |
| `stepToSlope.backDoor` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `stepToSlope.blind` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A storey with no openings on the faces that look onto a side strip laid higher than it. |
| `stepToSlope.costAt` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `stepToSlope.onEdge` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |
| `withoutBays` | a portar | sim/buildings/foundation, render/buildings/facade_emit | A storey's facade with the overrides of `drop` sides' bays taken out (their bay count changes with the block). |

### `src/world/buildings/store.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: Two records the same in every field but their wear (`decay`), compared field by field, no copy and no text.)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingStore` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT copy a record on the way in again "to be safe": the deep copy of every record added or replaced |
| `BuildingStore.add` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | @perItem Stores a new building under a fresh id. |
| `BuildingStore.all` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.below` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.changed` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.changedFor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The ids of the buildings changed since `cursor` last read this store (`BuildingStore.cursor()`); null: read them all |
| `BuildingStore.changedSince` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The ids of the buildings changed after `serial` (a `revision` read before), from the diary |
| `BuildingStore.clear` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | `journal`: the document's diary; a store on its own keeps one of its own. |
| `BuildingStore.copyAllocator` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Keeps id allocation monotonic across a clone (see `RoadDoc.clone`). |
| `BuildingStore.cursor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.dropAll` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The buildings of a tile taken out of the page's document (`RoadDoc.unloadTile`): one diary entry. |
| `BuildingStore.forkFrom` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A working copy over another store (`RoadDoc.fork`): its records read through, each written here only |
| `BuildingStore.get` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The serial of the latest change of the buildings in the diary (their shape, place or use; not their wear). |
| `BuildingStore.grid` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.has` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.idAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The id a building made at a place takes from its loaded tile's range (`RoadDoc.tileIdAt`); null: this store's allocator. |
| `BuildingStore.idList` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Every building's id, in the order of `all()`, no record decoded (the table's id column, then the ids written after it). |
| `BuildingStore.ids` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.internShared` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Whether a record's members alike with other buildings' are shared with them (`intern.ts`): DO NOT for a store thrown away once written out (the tile g |
| `BuildingStore.items` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.leaveBelow` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.load` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Loads stored buildings, repairing or dropping what cannot be read. |
| `BuildingStore.loadPrepared` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Loads buildings a worker prepared (`editor/tileStore.ts` `prepareTile`: each migrated and interned there, the tile's shapes shared through the message |
| `BuildingStore.loadSections` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Loads a map's buildings from their table (`editor/preparedCity.ts`, the binary save `world/docBinary.ts`): DO NOT decode them - the table is the store |
| `BuildingStore.near` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The buildings whose box (or anchor) may reach the rectangle, in the order of `all()` |
| `BuildingStore.nearAny` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The buildings `meets` keeps for one of `rects`, asked of those within `pad` of it, each once, in the store's order |
| `BuildingStore.nextId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.placeAll` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A tile's buildings put in the page's document, or taken out (`RoadDoc.loadTile`): one diary entry for all of them.  `plain`: read from a tile's table  |
| `BuildingStore.put` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | @perItem Replaces an existing building's record (same id). |
| `BuildingStore.remove` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | @perItem Runs once a building removed: nothing it reaches may walk the map. |
| `BuildingStore.replaceWith` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT go back to clearing the store and the grid and writing every record again |
| `BuildingStore.replaceWith.take` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.restore` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Puts back a building as it was, or takes it away (`undefined`): undo and redo (`world/docImage.ts`). |
| `BuildingStore.restoreNextId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The id the allocator hands out next, set back by undo. |
| `BuildingStore.revision` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.sections` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The buildings as buffers (`BuildingSections`): the table shared, the records written encoded - the save's, the simulation's, a whole map's undo. |
| `BuildingStore.settled` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.shareFrom` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The same records as another store (they are never changed in place) |
| `BuildingStore.size` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.spillToTable` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The buildings kept from now on as a table that their older records are spilled into (`SpilledItems.spill`), read back when asked: a city grown off the |
| `BuildingStore.table` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.tableRef` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Where a worker reads a building's record without decoding it on the page (`recordTable.ts` `RecordRef`): its table's buffer and row while the record i |
| `BuildingStore.takeFork` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Takes back a working copy over this store (`forkFrom`): the records it deleted, replaced |
| `BuildingStore.takeWhole` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A map opened, or a whole map put back by undo (`RoadDoc.replaceWhole`): the other store's buildings taken as they are, one change of the whole map - D |
| `BuildingStore.toJSON` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT copy the records out again: every record is plain JSON data already and never changed in place (OWNERSHIP above), and the undo's whole image (` |
| `BuildingStore.toText` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Written when asked (a whole-map undo image, an export), never kept: one pass over the records, not a text per record and a join of thousands (MDN JSON |
| `BuildingStore.watch` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Told each building's record before it is written (an edit being recorded, `RoadDoc.beginEdit`); null: nobody listens. |
| `BuildingStore.wear` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | @perItem DO NOT write a wear change with `put` |
| `BuildingStore.wearRevision` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `BuildingStore.withWatch` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `rectOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT answer null for it: one such record made the whole entry (every building of an edit) the whole map. |
| `sameButWear` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Two records the same in every field but their wear (`decay`), compared field by field, no copy and no text. |

### `src/world/buildings/streetDoors.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/foundation, render/buildings/facade_emit

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões. _(no 4.0: The doors of a building that open straight onto the street: a shop or a bar built up to the footway)_

**Motivo:** Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `PEOPLE_DOORS` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The ways a person goes in and out by (a garage's or a loading bay's door is a vehicle's). |
| `STEP_OUT` | a portar | sim/buildings/foundation, render/buildings/facade_emit | How far out from the door its foot is looked for, clear of the building and its lots. |
| `StreetDoor` | a portar | sim/buildings/foundation, render/buildings/facade_emit | The doors of a building that open straight onto the street: a shop or a bar built up to the footway |
| `streetDoors` | a portar | sim/buildings/foundation, render/buildings/facade_emit |  |

### `src/world/buildings/tableRows.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer. _(no 4.0: THE BUILDINGS OF A RECORD TABLE AS A READ-ONLY MAP BY ID, EACH RECORD DECODED WHEN READ (report "Arquitetura para cidades grandes", "A conta de memória"): the table is the source of truth, a record is)_

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `RECENT` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The records read by id kept alive, newest last: the selection, the inspector and the tools read the same few again and again. |
| `TableRows` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | DO NOT keep every record decoded (`tableRecords` did: ~8 KB of heap a building, the table ~1.3 KB outside it). A record read is the same object while  |
| `TableRows.[Symbol.iterator]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.[Symbol.toStringTag]` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.alive` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.buffer` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.collected` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.entries` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.forEach` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.get` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.has` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.held` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | How many rows' records are decoded and held somewhere now (the LRU of `get` among them): what the table saves is what is not. |
| `TableRows.idAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The id of a row. |
| `TableRows.keys` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.onRead` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | Told each record decoded (the store marks it settled: `BuildingStore.settled`). |
| `TableRows.reader` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.recent` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.record` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | A row's record: the one held somewhere if any, else decoded now. |
| `TableRows.rowOf` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | The row of an id; -1 when the table has none. |
| `TableRows.size` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.sortedIds` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.sortedRows` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.table` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `TableRows.values` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |

### `src/world/buildings/towerKit.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/civic*, render/buildings/signature

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas). _(no 4.0: The tower kinds: the buildings drawn by parts of their own)_

**Motivo:** Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `TOWER_KINDS` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `TowerKind` | a portar | sim/buildings/b40/civic*, render/buildings/signature |  |
| `signatureKind` | a portar | sim/buildings/b40/civic*, render/buildings/signature | The signature a building is drawn with, or null for the shared facade kit. |

### `src/world/buildings/types.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/b40/*

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas. _(no 4.0: The modular building model.)_

**Motivo:** Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BAY_COMPONENTS` | a portar | sim/buildings/b40/* | Replaceable facade components, one per bay. |
| `BUILDING_FUNCTIONS` | a portar | sim/buildings/b40/* | What a building is FOR, in the city: where people live, work, study, buy, eat, are treated, are buried. |
| `BUILDING_SCHEMA` | a portar | sim/buildings/b40/* | The schema written on every stored building. |
| `BUILDING_USES` | a portar | sim/buildings/b40/* |  |
| `BayComponent` | a portar | sim/buildings/b40/* |  |
| `BlockMode` | a portar | sim/buildings/b40/* |  |
| `Building` | a portar | sim/buildings/b40/* |  |
| `BuildingElement` | a portar | sim/buildings/b40/* | A free part of a building: a flight of stairs, a ramp, a pillar, a canopy, a free-standing wall, a slab. |
| `BuildingFunction` | a portar | sim/buildings/b40/* |  |
| `BuildingId` | a portar | sim/buildings/b40/* |  |
| `BuildingIdBrand` | a portar | sim/buildings/b40/* |  |
| `BuildingUse` | a portar | sim/buildings/b40/* |  |
| `CORE_KINDS` | a portar | sim/buildings/b40/* |  |
| `Core` | a portar | sim/buildings/b40/* | Extension point: a vertical circulation shaft, a module square whose corner is at `(x, y)` in the local frame. |
| `CoreKind` | a portar | sim/buildings/b40/* |  |
| `DECAY_GRACE` | a portar | sim/buildings/b40/* | City minutes a building stays as new before it starts to age: five days. |
| `DECAY_SPAN` | a portar | sim/buildings/b40/* | City minutes from the first stains to falling apart, without maintenance: thirty days. |
| `DEFAULT_MODULE` | a portar | sim/buildings/b40/* |  |
| `DRESS_CROWNS` | a portar | sim/buildings/b40/* |  |
| `DRESS_LINES` | a portar | sim/buildings/b40/* | The horizontal articulation of a block, as an architectural style draws it |
| `ELEMENT_KINDS` | a portar | sim/buildings/b40/* | Free parts a building can be given besides its volumes. |
| `ElementKind` | a portar | sim/buildings/b40/* |  |
| `FACADE_PATTERNS` | a portar | sim/buildings/b40/* |  |
| `Facade` | a portar | sim/buildings/b40/* | A facade: the storey's default component, optional whole-side overrides, and single-bay overrides keyed `"side:index"`. |
| `FacadeGeometry` | a portar | sim/buildings/b40/* | Measured facade rhythm on one physical face of a volume. |
| `FacadePattern` | a portar | sim/buildings/b40/* |  |
| `FaceId` | a portar | sim/buildings/b40/* | Any edge of an authored footprint. Cardinal `Side` is for oriented parts. |
| `LOT_SURFACES` | a portar | sim/buildings/b40/* | What an open block is laid with. |
| `LotSurface` | a portar | sim/buildings/b40/* |  |
| `MAX_ELEMENTS` | a portar | sim/buildings/b40/* | A whole property - walls, gates, paths, car park, garden, cornices - fits. |
| `MAX_GROUND_HEIGHT` | a portar | sim/buildings/b40/* |  |
| `MAX_LIFT` | a portar | sim/buildings/b40/* | The farthest a block's own floor stands above or below the building's ground floor. |
| `MAX_MODULE` | a portar | sim/buildings/b40/* |  |
| `MAX_PITCH` | a portar | sim/buildings/b40/* |  |
| `MAX_PROJECTION` | a portar | sim/buildings/b40/* |  |
| `MAX_RECESS` | a portar | sim/buildings/b40/* | Deepest a recess may go into a volume, and furthest a projection may stand out. |
| `MAX_SIZE` | a portar | sim/buildings/b40/* | Widest a volume may be on either axis, world units. |
| `MAX_STOREYS` | a portar | sim/buildings/b40/* |  |
| `MAX_STOREY_HEIGHT` | a portar | sim/buildings/b40/* |  |
| `MAX_TERRACE` | a portar | sim/buildings/b40/* | The farthest a terrace of a lot stands above or below its building's floor. |
| `MAX_VOLUMES` | a portar | sim/buildings/b40/* |  |
| `MIN_MODULE` | a portar | sim/buildings/b40/* |  |
| `MIN_PITCH` | a portar | sim/buildings/b40/* |  |
| `MIN_STOREY_HEIGHT` | a portar | sim/buildings/b40/* |  |
| `PALETTE_COUNT` | a portar | sim/buildings/b40/* |  |
| `PlacedFurniture` | a portar | sim/buildings/b40/* | One piece of furniture or a light placed by the player, in the building's local frame. |
| `ROOF_DETAIL_KINDS` | a portar | sim/buildings/b40/* |  |
| `ROOF_KINDS` | a portar | sim/buildings/b40/* |  |
| `Relief` | a portar | sim/buildings/b40/* | A region of one face pushed in or out |
| `RoofDetail` | a portar | sim/buildings/b40/* |  |
| `RoofDetailKind` | a portar | sim/buildings/b40/* |  |
| `RoofKind` | a portar | sim/buildings/b40/* |  |
| `SIDES` | a portar | sim/buildings/b40/* |  |
| `SPACE_KINDS` | a portar | sim/buildings/b40/* |  |
| `Side` | a portar | sim/buildings/b40/* | A face of a volume, in the building's local frame: 0 front (local -y, the street side), 1 right (+x), 2 back |
| `Space` | a portar | sim/buildings/b40/* | Extension point: a subdivision of a storey (a flat, a shop, a corridor). |
| `SpaceKind` | a portar | sim/buildings/b40/* |  |
| `Storey` | a portar | sim/buildings/b40/* |  |
| `Volume` | a portar | sim/buildings/b40/* | A rectangular block, standing on level `base`: a rectangle of the local frame, world units. |
| `VolumeDress` | a portar | sim/buildings/b40/* |  |
| `asBuildingId` | a portar | sim/buildings/b40/* |  |
| `bayKey` | a portar | sim/buildings/b40/* |  |
| `cloneBuilding` | a portar | sim/buildings/b40/* | A deep, independent copy, as JSON would make it (`jsonCopy`). |
| `componentAt` | portado | `native/src/sim/buildings/b40/types.h` → `inline uint8_t bay_at(` | The component a facade puts in one bay. |
| `decayOf` | a portar | sim/buildings/b40/* | How run-down a building built at `builtAt` is at `now`, in tenths. |
| `generatedBuiltAt` | a portar | sim/buildings/b40/* | DO NOT stamp every generated building at the same minute: the whole city would age at once (a diary entry and a tile write for every building, the sam |
| `isBayComponent` | a portar | sim/buildings/b40/* |  |
| `isBuildingUse` | a portar | sim/buildings/b40/* |  |
| `isElementKind` | a portar | sim/buildings/b40/* |  |
| `isFacadePattern` | a portar | sim/buildings/b40/* |  |
| `isRoofDetailKind` | a portar | sim/buildings/b40/* |  |
| `isRoofKind` | a portar | sim/buildings/b40/* |  |
| `isSide` | a portar | sim/buildings/b40/* |  |
| `jsonCopy` | a portar | sim/buildings/b40/* | What `JSON.parse(JSON.stringify(value))` makes, without the text: DO NOT go back to the round trip, every tile's buildings copied through a string of  |
| `settleBuilding` | a portar | sim/buildings/b40/* | DO NOT turn this back into a copy (`cloneBuilding`)  `intern`: its members alike with other buildings' shared with them (`intern.ts`); false where the |
| `settleMember` | a portar | sim/buildings/b40/* |  |
| `settleValue` | a portar | sim/buildings/b40/* |  |
| `volumeById` | a portar | sim/buildings/b40/* |  |
| `volumeTop` | a portar | sim/buildings/b40/* | The level a volume's roof sits on (one past its top storey). |

### `src/world/buildings/validate.ts`

**Estado:** a portar · **Dono no jogo:** sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores. _(no 4.0: Why a building cannot stand where it is.)_

**Motivo:** Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BUILDING_WORLD_MARGIN` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Nothing the player builds may come closer than this to the world's reach (`bounds.ts` WORLD_HALF): there is no map rim. |
| `BuildingProblem` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Why a building cannot stand where it is. |
| `JUNCTION_TOUCH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Area a footprint may share with a junction's carriageway and kerb and still only touch it, square units (0.16 m2) |
| `JunctionRing` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A junction's kerb ring as the checks read it (`core/ring.ts` `Ring`). |
| `PARTY_WALL` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | How far two buildings' volumes may run into each other at a shared party wall, world units (20 cm). |
| `ROAD_CLEARANCE` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Gap kept between a footprint and the back of a footway: next to nothing. |
| `ROAD_CONTACT_EPS` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Only floating-point noise at an exactly touching road edge, never a visible overlap. |
| `RoadReach` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a check of a building against the roads reads of the network (`touchesRoad`, the growth's corner test): the roads and junctions filed by place, e |
| `RoadReachRibbon` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A road as the checks read it: its profile's widths and its whole centreline (`SegmentRibbon`). |
| `SiteContext` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `SiteReads` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | What a check of a building on its site reads: the buildings round it, the roads, the ground. The document's (`siteOf` a `SiteContext`) on the page; th |
| `TOUCH` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Overlap two footprints may have and still count as touching (terraces). |
| `boundsOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `lineOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A road's full centreline as points, once per ribbon (ribbons are never changed, only replaced). |
| `pointSegmentDistance` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `polygonsOverlap` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Whether two simple polygons overlap (share interior area, roughly). |
| `polylineDistance` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Shortest distance from an open polyline to a closed polygon (0 inside). |
| `ringOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | A junction's kerb ring as points, once per ring (a junction solved again is a new one). |
| `ringsBox` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The box round several rings. |
| `segmentsCross` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `segmentsCross.o` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui |  |
| `siteOf` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The document's site as the checks read it. |
| `structuralProblem` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | Structural checks: need no world at all. |
| `structuralProblem.storeyOk` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The storey height, then each level's own past the ground floor (a list read in place: no list made a building checked). |
| `touchesRoad` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | DO NOT go back to a grid of every road and junction of the map made again at each network revision (`RoadContacts`) |
| `validateBuilding` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | The first reason `b` cannot stand, or null; `ignore` is the building being edited (not its own obstacle). |
| `validateOn` | a portar | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui | `validateBuilding` on a site as the checks read it (`SiteReads`): the page's document, or the growth's worker's. |

### `src/world/changes.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `rectsAround` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/changes.ts:141-145 function rectsAround: the feature's own |

### `src/world/cityGen/plan.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem.

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).


### `src/world/cityGen/squares.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem.

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).


### `src/world/doc.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `RoadDoc.constructor` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/doc.ts:299-303 constructor RoadDoc.constructor: the feature's own |
| `RoadDoc.lot` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/doc.ts:317-319 method RoadDoc.lot: the feature's own |
| `RoadDoc.lotAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/doc.ts:325-327 method RoadDoc.lotAt: the feature's own |
| `RoadDoc.nextLotId` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal |  |
| `RoadDoc.tileIdAt` | a portar | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal | src/world/doc.ts:1492-1499 method RoadDoc.tileIdAt: the feature's own |

### `src/world/docBinary.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/world/groundPatch.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: Row by row, `cols` a row.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GroundPatch` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | src/world/groundPatch.ts:30-38 interface GroundPatch: the feature's own |
| `PatchMiss` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | src/world/groundPatch.ts:51-55 class PatchMiss: the feature's own |
| `latticeHeight` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | src/world/groundPatch.ts:18-27 function latticeHeight: the feature's own |
| `patchGround` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | src/world/groundPatch.ts:58-65 function patchGround: the feature's own |
| `sampleGroundPatch` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | src/world/groundPatch.ts:41-48 function sampleGroundPatch: the feature's own |

### `src/world/growth/grownRows.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A TILE'S GROWN BUILDINGS PACKED AND MADE AGAIN (`rows.ts`): the generator packs what it grew (`packGrown`), every reader of a tile's buildings takes them made again from their lots (`tileBuildingsOf`))_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FLAT` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The flat land a generated tile is grown on (`wholeMapTile.ts`): what its buildings are made again on. |
| `choiceOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Row `k`'s choices. |
| `grownRecords` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The rows made records again on their lots, stamped as the generator stamped them (built at the city's first minute, new); a row of another generator,  |
| `packGrown` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Buildings grown, each with its lot and its choices, packed in columns. |
| `tileBuildingsOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every building of a tile: its records and its grown rows made again. |
| `warned` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `withGrownBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The tile with its grown rows made records (for whoever reads `buildings` whole: a write's bakes, a tile prepared for the page). |

### `src/world/growth/lotBoundaryRuns.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: DO NOT go back to testing every sample against every part: that was O(samples x parts) per side)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CoverBox` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A part's box, as the old test read it: `centre`, a boundary part covering where `/px - cx/ <= hw && /py - cy/ <= hd` |
| `FIRST,STEP` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `firstWhere` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The first index of `ts` (ascending) at which `holds` is true, `ts.length` when never: `holds` false then true. |
| `openRunsAlong` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The open stretches along an edge from 0 to `len`: the samples run along `along` |

### `src/world/growth/lotPlan.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A grown lot, composed the way a plot in a real street is)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ALLEY_COBBLES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `BOUNDARY_RESERVE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Elements kept free for a lot's boundary, which is laid after everything else. |
| `BOUNDARY_SET` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `BackUse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `CONCRETE_PATH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `DOCK` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lorry's dock: wider than a car's bay (`sim/agents/parking.ts` LORRY_DOCK, 3.2 m). |
| `DRIVE,HOUSE_DRIVE,FOOTPATH,TRUCK_LANE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `FLAT_KINDS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Parts a car drives over: laid in a drive as anywhere. |
| `FrontUse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `GARAGE_APRON` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Depth of the paved apron behind a house where a car turns into its garage under the back. |
| `GARAGE_GRADE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The drive of a house down (or up) to a garage in its lower ground floor (`world/buildings/splitLevel.ts` `garageUnder`) |
| `Lot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotKind` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `PARK_SLOPE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The steepest a car park's stalls are laid: 1 in 20. |
| `PAVERS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `PAVERS_WARM` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `RAMP_LANDING` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far a car park's or a loading yard's drive may climb or fall from the street to the yard |
| `RETAINING` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A retaining wall's stone, and the flights cut through one. |
| `STALL,AISLE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `STEP_RISE_M` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `STEP_RUN_M` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `STONE_PATH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `SidePlan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `SideUse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `TERRACE_MAX_RISERS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `TERRACE_MIN` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A yard falls or rises at least this much (m) before it is terraced; risers and treads of its steps; the most risers. |
| `YARD_FULL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A loading yard deep enough for a lorry's approach to its docks (`planLot`, industry). |
| `apart` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `Math.hypot(dx, dy) > r`, the same answer: the square settles it clear of `r` by far more than hypot's error, hypot asked only near `r` (its result a  |
| `between` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `driveSide` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The side of the lot its drive runs down, if it has one. |
| `furnishLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Lays the plan on a body whose building already stands in its envelope |
| `furnishLot.X,Y` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.flightBack` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A flight `width` wide from the street floor's level at `y` to the back level beyond it, at `x`. |
| `furnishLot.frontAt` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Along the street the boundary stands as close to the footway as a part may (`touchesRoad`: 2 cm), whatever its thickness |
| `furnishLot.frontRun` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.furnishFree` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.isCorner` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.lift` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Everything laid since (`fromVolume`, `fromElement`) raised (negative: lowered) `rise` metres |
| `furnishLot.near` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Not set apart by the circles round their footprints: the test `elementsMeet` makes first, the same sum. |
| `furnishLot.piece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A piece that meets something (a porch, a step) is split, and only the part that really meets it is left out |
| `furnishLot.probe` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.reachOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.retain` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A retaining wall in stone from (x0, y0) to (x1, y1) |
| `furnishLot.stepYard` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The yard from `y0` back to `y1` laid as a terrace `fall` metres above (or below) the floor, rounded to whole risers |
| `furnishLot.stepYard.clear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The steps' place and the wall's line cleared of the garden's parts. |
| `furnishLot.stepYard.flightClear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The flight lands on open lawn, as near the building's middle as it can: never in the pool, on a path or a paved corner |
| `furnishLot.stepYard.inYard` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.streetSide` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A side on a second street (a corner lot) is closed as the front is: a low wall, a fence on it, a railing.  Set in from the edge by its own thickness,  |
| `furnishLot.streetSide.at` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `furnishLot.strip` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | ---- the strips beside the building: what part of each is drive, and what path |
| `garageRamp` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `half` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `loadingDoors` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The loading doors on the ground floor's back faces that stand at `back` (local y, the yard's front edge) |
| `lotKind` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `planLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The plan of a lot `W` x `D` metres for a kind of building. |
| `planLot.parkingRows` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Parking behind: rows the depth allows after `front` and a building of `minBuilding`. |
| `planLot.sides` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `yardRamp` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |

### `src/world/growth/rows.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: THE BUILDINGS A TILE'S GENERATOR GREW, AS ROWS (the city-game's record: a few numbers a building and a versioned generator): each the choices `designOnLot` made on its lot (`zoning.ts` `GrownChoice`),)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GrownRows` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | THE BUILDINGS A TILE'S GENERATOR GREW, AS ROWS (the city-game's record: a few numbers a building and a versioned generator): each the choices `designO |
| `selectRows` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The rows of the buildings `keep` names, in their order (a tile written again with the grown buildings no edit touched, `RoadDoc.tileRecords`). |

### `src/world/growth/version.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: The version of the growth (`zoning.ts` `designOnLot`, `lotPlan.ts`, `buildings/procedural.ts`: the yards, walls and facades of grown buildings), stamped on each building grown and on a tile's grown ro)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LOT_PLAN_VERSION` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The version of the growth (`zoning.ts` `designOnLot`, `lotPlan.ts`, `buildings/procedural.ts`: the yards, walls and facades of grown buildings), stamp |

### `src/world/growth/zoning.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A lot's side in metres, to the millimetre, for the plans made in metres (`planLot`, `madeToMeasure`, `furnishLot`).)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BOUNDARY_KINDS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `BOUNDARY_PARTS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `CANDIDATES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Grows one building on a zoned lot without one (`world/lots.ts`): the lot is planned first - front, sides, back - and the building made for the envelop |
| `GROW_ON_REACH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How near the last building grown an open lot must be to grow next (the street filling along). |
| `GrownChoice` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | WHAT A GROWN BUILDING IS (the city-game's rule: a building is a few numbers and a versioned generator, `LOT_PLAN_VERSION`): the choices `designOnLot`  |
| `GrowthSite` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A site a building is grown on: what its checks read, and the id the building will take (its draft's). |
| `JOINT` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LIKE_REACH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far round a lot the buildings it should not look like are read: the 100 m of the rule it answers (`tests/world/variety.spec.ts`; at 90 m a twin 95 |
| `LOT_READ_MARGIN` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far round a lot what it is built and checked against is read: its ground, its roads, the buildings it meets. |
| `LotPick` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot a building grows on next, and the stream it is made with. |
| `STREET_MARGIN` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far beyond a footway's edge a point still counts as on the street, for a corner lot (`cornerOf`). |
| `TRIES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A handful of tries, each a building made: a few milliseconds apiece (`designOnLot`). |
| `Tried` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A try's body as made: its parts, the plan as laid before it was carried to the street, the lot in its frame, the candidates made and kept, the floor r |
| `TrySite` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One try's site on a lot: the lot as the try takes it (its width and depth, world units and metres), its front middle set back by the try's front margi |
| `apart` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far `a` is from `b`. |
| `commitGrown` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A building grown on its lot written in the document: the record added (as checked, `designOnLot`), the lot naming it (`OpenLots.grew`). Returns its id |
| `cornerOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The side of a lot (`left`: along -u from the front's middle) with a street just beyond it over most of its depth: a corner lot. Undefined for none, or |
| `cornerOf.onStreet` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The ribbons filed round the point (`Network.ribbonsMeeting`): a ribbon within its footway and the margin of it has its box, which holds the footway, w |
| `densitiesOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot's densities tried in turn: its own, then each lower. |
| `designOnLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The building grown on a lot (`pickLot`), checked on its site, or null when nothing fits it. Writes nothing: it reads the site (`GrowthSite`) - the doc |
| `designOnLot.floorOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `fitToLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A grown building made to its lot's own shape (the player, 2026-10-05: "buildings must follow the shape of the lots"). The plan was laid on the lot's f |
| `fitToLot.asOutline` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `fitToLot.clip` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Surfaces clipped to the lot; volumes of the building itself kept where they stand on it. |
| `fitToLot.inside` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `fitToLot.insideAt` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `fitToLot.sound` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A surface cut to a sliver (under 2 m across, or an outline the model cannot hold) is left out: kept, it made the whole building invalid and the lot wa |
| `frameOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot's frame (`lotBuildFrame`) with its axes: `u` along the front, `n` back into it. |
| `growOnLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Grows one building on a zoned lot without one (`world/lots.ts`), here on the page: the lot picked (`pickLot`), the building designed for it on the doc |
| `growthReachOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where a lot's growth reads the document (`designOnLot`): `lot`, the lot and its frame with a margin - its ground, the roads and the buildings its trie |
| `growthSiteOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The document's site for a building grown now. |
| `lotStream` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The stream the next building is made with, its first draw the open lot drawn at random (of `count`): seeded by the id the building will take, so a cit |
| `meets` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether segment a-b meets segment (cx, cy)-(dx, dy), touching counted. |
| `metresOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot's side in metres, to the millimetre, for the plans made in metres (`planLot`, `madeToMeasure`, `furnishLot`). |
| `nearSignatures` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The buildings round a point: each one's signature (worked out once per record, `neighbours.ts`) and how far it stands. |
| `nearestLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The open lot nearest the last building grown, within `GROW_ON_REACH` (the street fills along), or null: none so near. `near`: the open lots that may b |
| `neighbourStoreys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Storeys of the buildings beside a lot's front (within its width and a little), tallest block each. |
| `pickLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot the next building grows on (`growOnLot`): the open lot nearest the last building grown, when one is near (the street fills along), a lot at ra |
| `placed` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A grown building checked on its site, its boundary meeting the neighbours': where a piece of its wall or fence stands against a neighbour's own buildi |
| `quarterOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The street side and the quarter a lot belongs to, for its building's style (`world/buildings/architecture.ts`): `character` the same for the lots of o |
| `quarterOf.hash` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `reachStreet` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A plan set back `front` from its lot's front (a try's margin, `growOnLot`) carried out to the street: each surface along the plan's front - the foreco |
| `reachStreet.lay` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `rectInside` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether a volume's rectangle lies strictly inside a ring: a corner inside it and no side of it meeting or touching a side of the ring (a ring wholly i |
| `replayOnLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The record `designOnLot` made on a lot, made again from its choices (`GrownChoice`) alone: the same try with the same stream, the same candidate kept, |
| `streetFloor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The ground floor a grown building on a lot will have: its street's paving along the lot's front (`street`, a try's points, `trySites`), and a threshol |
| `streetPointsOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every point a lot's growth reads its street's paving at (`streetFloor`): what the page reads for the growth's worker. |
| `tryOnLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One try on a lot made whole (`designOnLot`): the plan laid, the candidates made and one kept, the yards furnished, carried to the street and fitted to |
| `trySites` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `trySites.local` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `unlikeness` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How unlike its surroundings a building would be: its least distance in looks to any of them, a building farther away counting as more unlike (a twin a |

### `src/world/lotRecord.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: The boundary, counter-clockwise (the inside on the left of each side): any polygon)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Lot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `insideLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |

### `src/world/lotRings.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A LOT'S RING, pure geometry (`lots.ts`))_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `hull` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `hull.cross` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `lotCentre` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The boundary's area centroid (the corners' mean for a degenerate one). |
| `orientedBox` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The smallest-area rectangle round a convex hull (rotating its edges): centre, long axis u, short axis v, lengths. |
| `simplifyRing` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A ring with the points that lie within `tol` of the line through their neighbours taken out, again and again |

### `src/world/lotStore.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: Where a lot lies: the box round its corners.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LOT_CELL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Side of the cells the lots are filed in, world units: a few lots wide |
| `LotEdit` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One change of some lots as the store applies it (`LotStore.apply`). |
| `LotStore` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT hand the lots out as an array the callers write into, nor search one by id or by place |
| `LotStore.addKeys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Adds land to what is already cut into lots (`lots.ts` `applyLots`) |
| `LotStore.all` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every lot, in the lots' order: what saves, copies or makes the whole map again reads; nothing that asks about a place. |
| `LotStore.append` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.apply` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One change of the lots, written in the diary as ONE entry with every id it touched and where they were and are. |
| `LotStore.at` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot whose shape holds `p` (the first in the lots' order when lots overlap), or undefined. |
| `LotStore.beforeKeys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.below` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.cells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.changedFor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The ids of the lots changed since `cursor` last read this store; null: read them all |
| `LotStore.changedSince` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The ids of the lots changed after `serial` (a `revision` read before), from the diary; null when it cannot say which |
| `LotStore.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `journal`: the document's diary; a store on its own keeps one of its own. |
| `LotStore.cursor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.drop` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.dropKeys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A tile's keys taken out of the page's document (`RoadDoc.unloadTile`): one entry over its square. |
| `LotStore.filing` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.forkFrom` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A working copy over another store (`RoadDoc.fork`) |
| `LotStore.get` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The serial of the latest change of the lots in the diary. |
| `LotStore.has` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.hasKey` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.idAt` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The id a lot made at a place takes from its loaded tile's range (`RoadDoc.tileIdAt`); null: this store's allocator. |
| `LotStore.ids` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.in` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lots whose box meets `rect`, in increasing id order: a superset of those whose shape reaches it |
| `LotStore.items` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.keyList` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.keySet` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.keys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.keysRewritten` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Moves when the keys are changed other than by keys added at the end (`addKeys`): a follower sends the list whole then, else only the new tail. |
| `LotStore.keysShared` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.leaveBelow` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.list` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | @rebuild The lots as an array, in their order, made again only after a change: for the readers that still take an array |
| `LotStore.listed` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.load` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lots and keys of a map read (`RoadDoc.fromJSON`): the whole store, entries without ids. |
| `LotStore.nextId` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.nextOrder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.order` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.orderOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot's place in the order (`ORDER` above): a lot put last gets a number greater than any before. |
| `LotStore.ownKeys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.recordingMap` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.replaceWith` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT compare the lots as text (`JSON.stringify` of the whole list) |
| `LotStore.reset` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.restore` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Puts lots back as they were, or takes them away (`undefined`): undo and redo (`world/docImage.ts`), all in one entry. |
| `LotStore.restoreNextId` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The id allocator set back (undo, `RoadDoc.restore`). |
| `LotStore.revision` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.shareFrom` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The same records as another store (they are never changed in place) |
| `LotStore.size` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotStore.spliceKeys` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A stretch of the keys put back as it was (undo, `RoadDoc.restore`): `list.splice`, the set kept with it. |
| `LotStore.takeFork` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Takes back a working copy over this store (`forkFrom`): the lots it took away, replaced (in place) and put last |
| `LotStore.takeWhole` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Another map's lots in place of these (`RoadDoc.replaceWhole`): its records taken as they are, the whole map written changed - DO NOT compare them lot  |
| `LotStore.toJSON` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every lot, its corners copied (the file, `RoadDoc.serialized`). |
| `LotStore.watch` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Told each lot before it is written, and before the keys are (an edit being recorded, `RoadDoc.beginEdit`); null: nobody. |
| `lotBox` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where a lot lies: the box round its corners. |

### `src/world/lots.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: Lots: the land cut into plots the player zones and buildings grow on, as a town's cadastre is. - A closed block)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CROSSED_AREA` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Area a road takes off a lot under which it counts as crossing it: less is the paving's own outline round the lot, not a road through it. |
| `Candidate` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `FIT_CELL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Side of the grid the proposed lots are filed in: a few lots wide. |
| `FiledLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A proposed lot filed by place, with its place in the plan's order. |
| `LAND_CLEARANCE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far a planned lot stands back from the paving: a hair, so what grows on it is never on the footway |
| `LOT_DEPTH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Depth of a lot along an open street. |
| `LOT_FRONTAGE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The frontage a lot aims at: a block's rows and a street's strip are cut into equal lots of about this width |
| `LandLabels` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The land's pieces for one plan, labelled where asked: each piece flooded once, from the first of its cells asked |
| `LandLabels.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.flood` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.label` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The piece a cell was labelled in, or -1. |
| `LandLabels.labelUnder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The piece a point's cell was labelled in, never flooding: -1 when it is paved or not labelled. |
| `LandLabels.labels` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.lastKey` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.lastTile` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.pieceAt` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The piece of land holding a cell, flooded the first time it is asked; -1 for a paved cell. |
| `LandLabels.pieceUnder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The piece under a point (`pieceAt`). |
| `LandLabels.pieces` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandLabels.scanTile` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every piece with a cell in a tile flooded, each told to `visit` as it is made. |
| `LandLabels.tileOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT ask the paving of the whole map for each plan: the land's raster in tiles, each made once and asked again only where the network changed |
| `LandPaving.clear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.forget` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.lastBits` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.lastKey` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.paved` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether raster cell (ci, cj) is paved: its centre on a carriageway, a footway or a junction's plate (`markPaved`). |
| `LandPaving.tile` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LandPaving.tiles` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The tiles whose cells may have changed, and their neighbours' |
| `LandPiece` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A piece of land: cells joined side to side, none paved. |
| `LotCut` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | New lots for land with none yet, and the lots the paving now covers. |
| `LotPlanner` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT plan the whole map for each edit: the plan kept by block and by street, each planned again where the network or the lots changed |
| `LotPlanner.around` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The outline round the point, read over the reach and a straight back's length more |
| `LotPlanner.blockPlan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.blockPlan.at` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.blockPlan.frame` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The block's frame for a point: along its length (s) and across it (t). |
| `LotPlanner.blockPlan.rowSpans` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A row's lots, as spans along the block: the row cut in equal columns, except where stored lots already stand in it |
| `LotPlanner.byId` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.dropped` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.filed` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.fitFree` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.install` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.lotCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.netState` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.nextId` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.pavedTest` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.paving` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.plan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.planAll` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.planChanged` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.planChanged.put` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.result` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.run` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.run.enqueue` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.seen` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.slots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.sourceCells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.streetPlan` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.streetPlan.open` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | No block: wider or deeper than `MAX_BLOCK`, or joined to land that is. |
| `LotPlanner.uninstall` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.unknown` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `LotPlanner.update` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The plan for the land as it is now: planned again only where the network and the lots changed since the last. |
| `MAX_BLOCK` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Land wider or deeper than this is open land, never a block: it bounds every flood of the land (a tile of the world's grid). |
| `MIN_LOT` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `PAD` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far round a street's box its land is asked whether it is paved: a lot's depth and a street's width more. |
| `PAVING_MARGIN` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Room round a lot's box the paving is read over: past the clearance, so the cut inside the box is the whole map's. |
| `PlanOrder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A place in the plan's order: blocks by their first cell, row by row, then streets by id (the order the whole map is planned in). |
| `PlanSource` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A block or a street kept in the plan. |
| `PlanWork` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A block or a street to plan: a block by its piece (or, null, by its first cell asked again), a street by its id. |
| `Planned` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What a block or a street plans: its land's keys and the lots it proposes. |
| `PlannedLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot the plan proposes, with its box. |
| `RASTER` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `SIDES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A cell's four neighbours. |
| `SMALL_PIECE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far a piece of land too small for a block reaches at most: fewer cells than a block's least area holds. |
| `STRAIGHT_BACK` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | An outline edge at least this long is a straight back of a footway; shorter ones are a corner's curve. |
| `STREET_GROW` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far past a street's filed bounds its strip reads the land and the lots (`streetPlan`'s reach, with a margin). |
| `TILE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Side of a tile of the land's raster, in raster cells (128 m). |
| `TILE_SIDE` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `TWO_ROWS` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A block deeper than this is cut in two rows, back to back. |
| `addPolygonLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A new lot of any shape: the corners the player clicked, in order. |
| `applyLots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Stores a plan: the covered lots removed, the new ones added, the land's keys kept. |
| `blockWork` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The work of planning a piece of land as a block, or null when it is no block (open, or too small). |
| `cellIndex` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A cell's place in its tile. |
| `clipLeft` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The part of a polygon on the left of the line a -> b (Sutherland-Hodgman against one half-plane). |
| `clipLeft.side` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `compareOrder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `coveredBy` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether the paving covers a stored lot: its centre, or a corner a little in from it. |
| `curveLotSide` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Bends the side of a lot from corner `a` to corner `b` through `through`: a curve |
| `curveLotSide.curve` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `cutLines` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The cut lines of a cut on a lot, each as two points: the preview the tool draws. |
| `cutLines.P` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `cutLotsByRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A ROAD EDIT CUTS THE LOTS IT CROSSES (`main.ts` mutateBuilt, beside the buildings it razes): each lot the changed paving (`rects`; null, every lot) ta |
| `cutSpans` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Each span cut into equal parts as near `target` long as a whole number of them allows. |
| `deleteLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `facingCorners` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The corners in the order a building reads its lot: front-start to front-end with the back on the LEFT of that direction |
| `footwayEdgesNear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The outline of the paving a building may stand against round a box: the union of the footways (`pavingNear`) |
| `forTiles` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Calls `visit` with every tile a rectangle reaches. |
| `forgetLotPlans` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A network's planner let go (`planLots` makes it again when asked): a city generated once (`editor/smallCityMap.ts`), whose land is planned once while  |
| `freeSpans` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The parts of `from..to` not under any of `taken`, each at least `min` long. |
| `isLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `joinLots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Joins two lots that share a side into one: the two boundaries walked round, the shared stretch left out. |
| `landLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A proposed lot (four corners, the front first) cut back to the land: the paving taken off |
| `lerp` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `lotArea` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `lotBuildFrame` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where a building can stand on a lot |
| `lotFrame` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot's frame for a building: front middle, facing angle (along the front), width and depth. |
| `lotPlanWork` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The work of the lots' plans: raster tiles made, land cells flooded, blocks and streets planned. A count for the scale specs. |
| `lotRect` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The rectangle from `a` to `b` square to `angle` (a street's direction), counter-clockwise; null when too small. |
| `lotRect.p` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `lotSnapper` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The lot snap: a point near a street is put on the outer edge of its footway as drawn |
| `meets` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `meetsInside` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `moveLotCorner` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Moves a lot corner to `to`, and every other lot's corner standing on the same point with it |
| `near` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A map with no street keeps the lot as drawn; one with streets, none near it, cuts it against nothing |
| `nearerStreet` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | For a one-row block: whether the street is nearer its -v side than its +v side, at this column. |
| `onLand` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot drawn over the street, cut back to the land: what falls on the paving is taken off, so the lot |
| `overlapDeep` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether a corner or the middle of one polygon is well inside the other (the separating-axis test is only for convex shapes). |
| `overlapDeep.inner` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `pavingNear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT go back to the union of the whole map's footways, made again at each network revision |
| `planLots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT plan the whole map again after an edit: the land the diaries say changed is planned again, the rest kept |
| `plannedStreet` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether a street's land is planned: a ground street with footways. |
| `planners` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Each network's planner, following that network's changes and its document's lots (`LotPlanner`). |
| `rayExit` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far a ray from inside a polygon runs before it leaves it. |
| `reachesTiles` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether a rectangle reaches one of these tiles. |
| `sameMade` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether two plans propose the same lots, corner for corner. |
| `sameRect` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `setLotFront` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Makes one side of a lot its front - the side on the street, which the building grown there faces |
| `spanOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `splitLot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Cuts a lot by `cut`; false when nothing worth a lot is left on both sides. |
| `squareCorners` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A lot cut back to the footways, its corners made square |
| `streetWork` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `tileKey` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `tileRect` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `withFront` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A piece of a cut lot with its front set: the side lying along the old front, or else the side nearest it |
| `withFront.off` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `workFor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `zoneLots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Zones lots (or clears them, `zone` null). |

### `src/world/nearestRibbon.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: The first box searched round a point, u; each search after it four times as wide.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FIRST_REACH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The first box searched round a point, u; each search after it four times as wide. |
| `LAST_REACH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Wider than any map: the last box holds every road. |
| `nearestRibbon` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT walk every road of the map for the street nearest a point: every lot drawn did, for each of its sides. |

### `src/world/openLots.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: THE OPEN LOTS (`RoadDoc.openLots`))_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CELL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Side of a cell of the centres' grid, world units: about the reach of the street filling along. |
| `LotsHost` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What it reads of the document (`RoadDoc.openLots`), handed over by it |
| `MIN_ROOM` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The least room the slots are laid with. |
| `OpenLots` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.at` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The `k`-th open lot (from 0), in the lots' order. Asked after `count`. |
| `OpenLots.byBuilding` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.cellOfSlot` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.cells` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.clear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.constructor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.count` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How many lots are open, `refused` set aside. |
| `OpenLots.cursor` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.file` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.findLast` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.flag` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.grew` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `lot` took the building `id`: written in the document (`cause`, where), the index kept in step. |
| `OpenLots.holes` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.lastGrown` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Where the building grown last stands (the highest id standing on a lot), or null. Asked after `count`. |
| `OpenLots.lastId` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.lastStale` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.name` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.near` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The open lots whose centre may be within `reach` of `at` (a superset), in the lots' order. Asked after `count`. |
| `OpenLots.open` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.openCount` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.place` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.rebuild` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.records` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.refuse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `lot` set aside: nothing fits it (`refused`, the set last asked with). |
| `OpenLots.refused` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.refusedSize` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.room` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.seenBuildings` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.slotOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.slotOrder` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.sync` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.top` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.tree` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `OpenLots.unname` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `cellOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The cell of the centres' grid a lot's centre lies in. |
| `keyOf` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |

### `src/world/parkingLayout.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.


### `src/world/poleLines.ts`

**Estado:** outro porte · **Dono no jogo:** ports/traffic, ports/street-furniture

**Para o jogador:** Carros e caminhões usando vagas e docas dos lotes; postes.

**Motivo:** Só fatias; o dono é o tráfego/mobiliário.

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Placed` | outro porte | ports/traffic, ports/street-furniture | Every ring that can count at some centre, with the run of columns it may count in. |
| `markPlates` | outro porte | ports/traffic, ports/street-furniture | src/world/poleLines.ts:384-472 function markPlates: the feature's own |
| `markPlates.cx` | outro porte | ports/traffic, ports/street-furniture |  |
| `markPlates.cy` | outro porte | ports/traffic, ports/street-furniture |  |
| `markPlates.place` | outro porte | ports/traffic, ports/street-furniture |  |
| `onPaving` | outro porte | ports/traffic, ports/street-furniture | src/world/poleLines.ts:481-484 function onPaving: the feature's own |

### `src/world/rasterGrid.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: A raster over the plane and the states of its cells, shared by the paved raster)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LAND` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A raster cell's state in the label array: land not asked about (no street near), and land of no piece yet. |
| `NEAR` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A raster cell's state: within a street's reach, to be asked whether paved (`markPaved` turns it to `PAVED` or `LAND`). |
| `PAVED` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A raster cell's state: paved. |
| `RasterGrid` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | A raster: `W` x `H` cells of side `step` from (`minX`, `minY`); cell (i, j)'s centre is `minX + (i + 0.5) * step`, `minY + (j + 0.5) * step`. |
| `firstTrue` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The first index in [lo, hi) where a predicate false then true along the range (monotone) turns true |
| `gridRect` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The rectangle holding every cell centre of a raster. |

### `src/world/rescale.ts`

**Estado:** a portar · **Dono no jogo:** sim/buildings/building_store, sim/persistence, sim/commands/edit_journal

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

**Motivo:** Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas.


### `src/world/solids.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem.

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).


### `src/world/zoneGrid.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: The zoning grid: the land along every street, cut into cells the player zones and buildings grow on.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BUCKET` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Spatial hash bucket edge: one cell. |
| `OVERLAP_SLACK` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How far two cells may run into each other and still count as apart: rounding, and cells that only share an edge. |
| `SUB_CELL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The subgrid a cell is trimmed on where a whole cell does not fit (1 m). |
| `SWEEP_SLACK` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | How much wider than a street's reach its capsules are scanned: far more than any rounding. |
| `Street` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Each street's columns and the cell maker for them, first; the cells are then laid a row at a time across every street |
| `ZONE_CELL` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZONE_DEPTH` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZoneCell` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZoneGrid` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `bucketKey` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `buildZoneGrid` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Builds the grid of every ground street that carries pedestrians. |
| `insideQuad` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `key` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `markPaved` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Every `NEAR` cell of `label` whose centre is paved turned to `PAVED`, the other `NEAR` cells to `LAND` |
| `markRoads` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `onRoad` (`zoneGrid.ts`) on the raster |
| `pavedTester` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | What land may not be built on: any ground road's carriageway and footway |
| `pavedTester.onPlate` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The paving as drawn - carriageways, kerbs, footways and every junction's corners |
| `pavedTester.onRoad` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | DO NOT bucket every ribbon of the map for each tester: every zone stroke and lot plan paid for the whole map first. |
| `quadsOverlap` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether two quads overlap by more than `slack`, by the separating-axis test |
| `zoneGridSteps` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | `buildZoneGrid` a street at a time: it yields after each street, so the grid can be laid over several frames |
| `zoneGridSteps.at` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Cell at (segment, side, column, row), or undefined where the grid has none. |
| `zoneGridSteps.cellAt` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The cell whose quad holds a world point. |
| `zoneGridSteps.cellsNear` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Cells whose centre lies within `radius` of a point. |
| `zoneGridSteps.inset` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Tested a tenth of the way in, so a cell whose edge lies along the paving's edge |
| `zoneGridSteps.lay` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `zoneGridSteps.taken` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Whether a cell would overlap one already laid by more than a sliver. |

### `src/world/zones.ts`

**Estado:** a portar · **Dono no jogo:** sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots

**Para o jogador:** Crescimento: lotes pelas ruas zonadas, plano do lote, prédios que nascem. _(no 4.0: Authored land-use area. Buildings generated from it remain ordinary editable buildings.)_

**Motivo:** Sem prova por símbolo até agora. Conferir casos de borda (lote de esquina, garagem em encosta).

| Declaração | Estado | Dono / motivo | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ZONE_DENSITIES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZONE_USES` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | Authored land-use area. Buildings generated from it remain ordinary editable buildings. |
| `Zone` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZoneDensity` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `ZoneMark` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | One zoned cell of the street grid |
| `ZoneUse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `isZoneDensity` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `isZoneMark` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `isZoneUse` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `zoneDensityName` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots |  |
| `zoneUseName` | a portar | sim/simulation/growth, city_growth, sim/citygen, sim/lots, b40/lot_*, open_lots | The game's own string for a use or a density read from a map (DO NOT keep the parsed one: a string of its own a lot, ~30 B each). |
