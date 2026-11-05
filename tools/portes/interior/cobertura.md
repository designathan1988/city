# Cobertura do rastreamento: interior (vista de dentro dos prédios)

Uma linha por arquivo e por declaração própria de `feature.md` (fonte: `converter/out/features/interior/boundary.json`; gerada por `evidence/cobertura/gerar.py`). Regra desta tabela: `portado` e `já existia` só existem com arquivo e símbolo do jogo, em `provas_interior.py` (minhas) ou em `ports/build/evidence/cobertura/provas.py` (herdadas do porte build, de que este clone parte), e o gerador confere cada um no clone: símbolo que não está no arquivo aborta a geração. `outro porte` diz a funcionalidade da fila que é dona; `não se aplica` traz o motivo; `a portar` é o que é meu e ainda falta, com o motivo. As declarações que o 4.0 corta em pedaços de arquivos grandes (`sliced`, como `emitInterior` de `buildingMesh.ts`) estão na seção final.

Arquivos: 82. Declarações próprias: 1338. Pedaços listados: 9 (de 109).

| Estado | Declarações |
|---|---|
| outro porte | 1160 |
| já existia | 120 |
| não se aplica | 37 |
| portado | 20 |
| a portar | 1 |

Pedaços: não se aplica 3, portado 6.

**A portar (meus, sem prova):** `src/editor/buildingTool.ts#BuildingTool.interiorPreview`

### `src/buildingsWiring.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingWiring` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingWiringDeps` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building tool's share of the composition root. |
| `PRIMITIVE_OF_TOOL` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The basic shape each tool of the Draw tab throws. |
| `READY` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Every tool the catalogue lists, all of them wired to the engine.  Every tool the catalogue lists is wired to the engine. |
| `createBuildingWiring` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.activate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.afterGesture` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | After a gesture: if a creating tool just made what it makes, it is put down. |
| `createBuildingWiring.afterRoadEdit` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | After a road edit, inside the same undo step: the road-wins rule over what changed since `before`, the network's state read just before the  |
| `createBuildingWiring.backToSelect` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Puts down whatever is in hand: the pointer selects again. |
| `createBuildingWiring.beforeDraw` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Before each draw: hands the preview to the renderer. |
| `createBuildingWiring.buildReference` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Builds the reference where it stands, in place of the building it was placed on. |
| `createBuildingWiring.bulldozeAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The bulldozer: demolishes the building under a screen point, if any. |
| `createBuildingWiring.cancelOperation` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Right button: cancels the operation in progress, if any. |
| `createBuildingWiring.clearArming` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Choosing a tool puts the previous one away: only one thing may be taking the pointer at a time |
| `createBuildingWiring.copyStyle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.deactivate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.defaultSplitFloor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.drawGizmos` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The Builder's own layer: the construction grid, on the selected building's floor while the grid is on. |
| `createBuildingWiring.drawOverlay` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.floorInHand` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The floor the floor chip shows: the picked face's storey, or the ground. |
| `createBuildingWiring.hintKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The sentence the hint bar shows: the plan being drawn wins, otherwise the tool the tray has chosen. |
| `createBuildingWiring.hintKey@1206` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The sentence the hint bar shows: the plan being drawn wins, otherwise the tool the tray has chosen. |
| `createBuildingWiring.insideClick` | portado | `project/game/interior/interior_controller.gd` → `func _unhandled_input(` — o interruptor, o prédio no meio da vista ou o duplo clique; enquadra o corpo e o andar | A click with nothing in hand: opens (or switches) the building seen inside; true if taken. |
| `createBuildingWiring.inspectorFields` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The precise numbers of the current selection, for the inspector. |
| `createBuildingWiring.key` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Returns true when the tool used the key. |
| `createBuildingWiring.languageChanged` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.loadReference` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.materialLine` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.notify` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A transient answer to an action, shown by the interface as every answer of the game. |
| `createBuildingWiring.openInside` | portado | `project/game/interior/interior_controller.gd` → `func _open(` — o interruptor, o prédio no meio da vista ou o duplo clique; enquadra o corpo e o andar | A building opened from inside at its ground floor (the player went in), or closed. |
| `createBuildingWiring.planActionOfTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Wing, stack and cut are drags too: a rectangle against the selection. |
| `createBuildingWiring.pointerDown` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.pointerMove` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.pointerUp` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.referenceAction` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The reference's own actions; true when `id` was one. |
| `createBuildingWiring.referenceTarget` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Where the reference stands: on the selected building, centred on its plan. |
| `createBuildingWiring.refresh` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The workspace follows the tool at once, not a frame later: the panels are cheap to re-render |
| `createBuildingWiring.requestThumbnails` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Photographs the parts a gallery is about to show, a few per frame. |
| `createBuildingWiring.restored` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | After an undo, a redo or a load. |
| `createBuildingWiring.runTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Runs an action tool, arms a mode tool, or opens a gallery (the workspace's job). |
| `createBuildingWiring.selectionName` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | What the inspector is looking at, in one line. |
| `createBuildingWiring.setField` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | One inspector number, applied to the model. |
| `createBuildingWiring.setFree` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Alt held frees a drag from the grid |
| `createBuildingWiring.shapeOfTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The shape a draw tool draws, when it is a drag shape. |
| `createBuildingWiring.state` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `createBuildingWiring.tellInside` | portado | `project/game/interior/interior_controller.gd` → `func _refresh(` — o interruptor, o prédio no meio da vista ou o duplo clique; enquadra o corpo e o andar | DO NOT add a second bar for it: a floating one |

### `src/core/derivedCache.ts`

**Para o jogador:** Infraestrutura do navegador e do WebGL.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `readDerivedAll` | não se aplica | core, JobPool, RenderWorld — Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem. | src/core/derivedCache.ts:107-126 function readDerivedAll: the feature's own |

### `src/editor/blueprintLibrary.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BlueprintLibrary` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/blueprintLibrary.ts:21-49 class BlueprintLibrary: the feature's own |
| `BlueprintLibrary.kept` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BlueprintLibrary.list` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BlueprintLibrary.remove` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Removes one blueprint, the rest as they were; false when nothing could be written. |
| `BlueprintLibrary.save` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Saves a body under a name (the one of that name replaced); returns the stored blueprint, or null when it was not saved. |

### `src/editor/buildingFloors.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingFloors` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingFloors.cursor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingFloors.floorOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingFloors.floors` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingFloors.follow` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `KINDS` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | DO NOT key the floors on a global revision (`${doc.revision}:${doc.terrainRevision}:${buildings.revision}`) |

### `src/editor/buildingPlans.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `PRIMITIVES` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `PlanShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `Primitive` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The basic shapes thrown at a building: a plan and a roof. |
| `UpperMassPlacement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Creates an editable shaped volume on a roof. Values are world units. |
| `addPlanMass` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Adds a mass to the same building at ground or on an existing roof. |
| `addShapedUpperMass` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `addShapedUpperMass.atScale` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `bevelPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Bevels corners of a plan: each corner becomes two, `distance` back along its two sides (never past half a side). |
| `changePlanVertex` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Inserting/deleting a vertex invalidates edge-specific decorations. |
| `changePlanVertex.remap` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `changePlanVertex.remapSides` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `movePlanEdge` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `movePlanVertex` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `offsetPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Offsets the selected volume's whole plan; preserves edge identities. |
| `setVolumePlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Store a ring in a volume's bounding frame. Existing edge overrides stay indexed. |
| `shapeBody` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `shapePoints` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Editable polygon starters. Curved forms are deliberately explicit vertices. |
| `shapePoints.points` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/editor/buildingProfile.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `reshapeTier` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Turns a tier into another editable plan while mapping facade work by face direction. |
| `reshapeTier.nearest` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `reshapeTier.replicate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `splitVolumeAtFloor` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_split_at_floor(` (porte build) | Splits one continuous mass at a floor boundary. |

### `src/editor/buildingRoofs.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `SIZE` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `addRoofDetail` | já existia | `native/src/sim/commands/body_finish.cpp` → `std::string op_add_gear(` (porte build) | A roof part belongs to one mass and follows every building edit. |
| `removeRoofDetail` | já existia | `native/src/sim/commands/body_finish.cpp` → `std::string op_remove_gear(` (porte build) |  |
| `updateRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/editor/buildingSnap.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BUILDING_REACH` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far a building still lends its rotation to one being placed. |
| `EDGE_SNAP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How close an edge must come to another to be made flush. |
| `FootprintSize` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The size of a footprint, measured from its anchor. |
| `PlacementSnap` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Where a footprint goes when the pointer is at `cursor`: to a road first |
| `ROAD_REACH` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far from a road a footprint still snaps to it, beyond its own depth. |
| `cornerFlush` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | At a corner, the side of the footprint next to the crossing road slid onto the back of that road's footway |
| `flush` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Makes the footprint's edges flush with a neighbour of the same bearing: along the local x axis always |
| `footprintSize` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `frontAnchor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The front-centre anchor that puts the footprint's CENTRE at `centre`. |
| `nearestBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `snapPlacement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | @perInput Runs once a pointer move (the snap under the cursor): nothing it reaches may walk the map. |
| `snapToRoad` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | DO NOT walk `net.ribbons` here: this runs on every pointer move. |

### `src/editor/buildingTool.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BaySelection` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingModelTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingPreview` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingSelection` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.activeModelTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.addSetback` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.addStoreys` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.addUpperShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.addWing` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.applyFacadeGrammar` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.applyStyle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Dresses the whole selected building in a factory style. |
| `BuildingTool.applyToSelectedBay` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Applies the armed component to the selected bay, with the current scope. |
| `BuildingTool.armComponent` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armCore` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Arms (or with the same kind again, disarms) a free element to place on the selected building. |
| `BuildingTool.armFurniture` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armModelTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armPrimitive` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.armed` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The kind of free element the palette has armed: the pointer places one. |
| `BuildingTool.backPoint` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.beginHandleDrag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.beginShapeDrag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.bevelBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Bevels every corner of the selected block, or only the picked corner in point mode. |
| `BuildingTool.blockLift` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far the selected block's floor stands from the building's ground floor (`Volume.lift`), world units. |
| `BuildingTool.blockOffset` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The selected block's centre, as an offset from its reference block's centre (world units). |
| `BuildingTool.blueprintKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.body` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | What a click in place mode builds. |
| `BuildingTool.builderHintKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A key for the workspace's hint sentence: `hint.builder.<key>` in the dictionaries. |
| `BuildingTool.buildingAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building under a screen point, or null. |
| `BuildingTool.cancelOperation` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Right button, or Escape: ends whatever the tool is doing, staying in the Builder. |
| `BuildingTool.cancelPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.carried` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.changeVertex` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.chooseBlueprint` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.chooseShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.clearArmingFor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.click` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.clipboard` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.component` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The component the picker has armed: a click on a bay puts it there. |
| `BuildingTool.constructor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.copyBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A copy of the selected block, set beside it on its free side (or on top when it stands alone) |
| `BuildingTool.copySelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.coreKind` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A core in hand (lift or stair), or 'remove': the next click on the cut floor places or removes one. |
| `BuildingTool.currentMaterial` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The material the current target is built in. |
| `BuildingTool.cutAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.cutDir` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.cutLevel` | portado | `native/src/sim/buildings/interior/cut.h` → `struct CutView` — o andar do corte vai no CityState (InteriorView::cut) | The interior view: the selected building drawn cut open above this level, or null. |
| `BuildingTool.cutPreview` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.cutRecord` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.cutView` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The way the camera looks (world, horizontal), set by the wiring each frame. |
| `BuildingTool.cyclePalette` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.deactivate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The tool is put away: no ghost, no gesture, no hover. The selection stays. |
| `BuildingTool.deleteRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.deleteSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.detachBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Takes the selected block - and what stands on it - out of the building into a building of its own, in the same place |
| `BuildingTool.drag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.dragging` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.dropPrimitive` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.duplicateSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.editFurniture` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.editFurniture.snap` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.editFurniture.under` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The piece under the pointer: the nearest whose footprint holds it. |
| `BuildingTool.endPaintStroke` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The stroke is over: the next press starts a new one. |
| `BuildingTool.endShapeDrag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Releases the drag: the shape is built where the ghost stands. |
| `BuildingTool.extrudeFace` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Extrudes the picked face along its normal: positive out, negative in (push/pull). |
| `BuildingTool.extrudeFaceBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Extrudes the picked face as a NEW block: a brick the face's width and the block's height, set against it |
| `BuildingTool.faceInHand` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.faceRegion` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The picked rectangle of bays and storeys of one face, or null. |
| `BuildingTool.faceRegion.clampTo` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.finishElementRun` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.finishPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Validates the actual building on the terrain, then commits one undo step. |
| `BuildingTool.floorOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Ground-floor height: cached for stored buildings, measured for a preview. |
| `BuildingTool.floors` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.focusSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.free` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Alt held: drags follow the pointer freely, without snapping to the grid. |
| `BuildingTool.freeHand` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Nothing is in hand: the pointer selects, and drags what it selected. |
| `BuildingTool.furnitureAngle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.furnitureKind` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Furnishing, as in The Sims: a piece (or a light) in hand is put down where the cut floor is clicked |
| `BuildingTool.generate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The generator: a neutral block of the current parameters. |
| `BuildingTool.ghostAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.groupWith` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Agrupar: every volume of another building moves into this record, brought into its frame; the emptied record goes. |
| `BuildingTool.handleAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.handles` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The handles of the selection, in world 3D. Only in edit mode. |
| `BuildingTool.handles.nearest` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Move and rotate go on the two footprint corners nearest the viewer. |
| `BuildingTool.hover` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.hoverElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.hoverHandle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.hoverPlace` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Recomputes the placement ghost for the pointer at `world`. |
| `BuildingTool.hoverPrimitive` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.hoversOnly` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | @perInput Runs once a pointer move: nothing it reaches may walk the map.  Whether a pointer move now only hovers (`pointerMove`: no bay zone |
| `BuildingTool.insertStorey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | One more storey in the selected mass, a copy of storey `at`, put in at `at` (below it) or `at + 1` (above it) |
| `BuildingTool.insetFace` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Insets the picked face: its bays, all but a frame of one bay at each end and the top and bottom storeys |
| `BuildingTool.interiorPreview` | a portar | Integração (aba Interior do Construir + camada Interior) — A aba Interior do Construir é da Porte 2 e chama a visão: `CityStore.dispatch(&"set_interior", ...)` já aceita prédio e andar; ligar a aba a ela só se faz com build e interior juntos (integração, depois de build). | The selected building cut open at the interior level, drawn solid in place of the stored one. |
| `BuildingTool.judge` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.judged` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.judgedCursor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.key` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Returns true when the tool used the key. |
| `BuildingTool.lastPaintKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.lastScreen` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.lastWorld` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.massMoveArmed` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The Move block tool: the press takes hold of the selected mass itself. |
| `BuildingTool.materialScope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.materialTarget` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The surface a material pick paints now, or null (a face scope needs a clicked facade). |
| `BuildingTool.measure` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | What the gesture in progress measures, for the overlay to show beside it: a length or a depth |
| `BuildingTool.mirrorSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Mirrors the selected building left to right, in place. |
| `BuildingTool.mode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.moveRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.offsetBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Grows (or with a negative distance shrinks) the selected block's whole plan. |
| `BuildingTool.onCutFloor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.onSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.paint` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Paints the current target: a new finish keeps its colour, a new colour keeps its finish. |
| `BuildingTool.paintBrush` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.paintStroke` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.params` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.paste` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.pathKind` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A run of fence, wall or paving, traced as a path |
| `BuildingTool.pick` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.pickAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building face under a screen point, if any (the bulldozer asks). |
| `BuildingTool.placeCore` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.placeDrawn` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.placeElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.placeHere` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planAction` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planArea` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planCursor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planHeight` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.planPoints` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.pointMode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Point mode: every corner of the selected block is a handle, boxes included. |
| `BuildingTool.pointOnPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.pointerDown` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Returns true when the tool used the press. |
| `BuildingTool.pointerMove` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.pointerUp` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.preview` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.primitive` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A basic shape in hand: the next click drops it on a roof, against a wall, or on the ground. |
| `BuildingTool.primitiveDraft` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.primitiveDraft.ring` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Read before the drag is put away: the ring is drawn from its start, and read afterwards it was empty |
| `BuildingTool.primitiveDraft.shape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.problem` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The last problem a gesture reported, for the overlay's label. |
| `BuildingTool.referenceOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The block a block is measured from: the one it stands on |
| `BuildingTool.reliefDepth` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far the picked region is pushed now (0 when flush or nothing is picked). |
| `BuildingTool.removeElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.removeVolume` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.repeatElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Repeats the selected element in a row along the building. |
| `BuildingTool.report` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.reshapeTier` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.resize` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.roofDetailKind` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.rotateSelected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.rotation` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Rotation the player has asked for; snapping may override it. |
| `BuildingTool.scope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.selectBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Selects a building just made elsewhere (built from a 3D reference) |
| `BuildingTool.selectFloor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Points the face tools at one floor of the selected volume. |
| `BuildingTool.selectRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.selectVolume` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.selected` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.selectedElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The selected free element, if one is. |
| `BuildingTool.selectedRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.selection` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A stair laid along the path climbs the floor being edited |
| `BuildingTool.serial` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setBlockBase` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The level the selected block starts on. |
| `BuildingTool.setBlockLift` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Raises (or, negative, lowers) the selected block's floor to `lift` above the building's ground floor, as typed |
| `BuildingTool.setBlockMode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How the selected block combines with the others: solid (null), void, intersect or exclusive. |
| `BuildingTool.setBlockOffset` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Puts the selected block's centre at an offset from its reference block's centre. |
| `BuildingTool.setCutLevel` | portado | `project/game/interior/interior_controller.gd` → `func _move_floor(` |  |
| `BuildingTool.setFacadeGeometry` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setFunction` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | What the selected building is for (null: nothing in particular). |
| `BuildingTool.setGroundHeight` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setLevelHeight` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setMaterialScope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setMode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setPaintBrush` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Sets the swatch carried by the canvas paint tool; the edit happens on a face click. |
| `BuildingTool.setParameter` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setPointMode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setPreview` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setRelief` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Pushes the picked region out (positive) or in (negative); 0 flattens it. |
| `BuildingTool.setRoof` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setRoofDetailFlag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setRoofDetailFlagDesign` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The selected spire's or lantern's flag design: its pattern and colours. |
| `BuildingTool.setRoofDetailHeight` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setRoofShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Changes the selected volume's roof pitch, ridge or fall. |
| `BuildingTool.setScope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setStage` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setStoreys` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.setVolumeSize` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The selected block's width and depth (world units, null keeps one), about its centre |
| `BuildingTool.shapeDragAction` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.shapeDragAngle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.shapeDragRing` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.shapeDragShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.shapeDragSpan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.shapeDragStart` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The Draw tools: a preset footprint sized by dragging on the ground. |
| `BuildingTool.shapeDragZ` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.showDraft` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.splitAtFloor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.stage` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.startElementRun` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.startPlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.sync` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Drops a selection whose building or volume no longer exists (after an undo). |
| `BuildingTool.turnBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Turns the selected block about its own centre by `degrees` (the block, not the building). |
| `BuildingTool.turnFurniture` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A quarter turn for the piece in hand, or for the one being moved. |
| `BuildingTool.turnRoofDetail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.updateElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Resizes or turns the selected element. |
| `BuildingTool.updatePlanPreview` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.updateShapeDrag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.useBody` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Places from an arbitrary body (a user blueprint, the clipboard). |
| `BuildingTool.verdict` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `BuildingTool.zone` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A facade zone being dragged: one side of one block, from a bay to a bay. |
| `BuildingToolMode` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `CreatorTool` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `DRAG_START_PIXELS` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `Drag` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `LONG_PRESS_MS` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A press held this long without moving counts as a long press. |
| `MaterialScope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | What a material pick paints: the whole building, the selected volume, one face of it, or its roof. |
| `PICK_BOTTOM` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `PICK_PAD` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far round the ray's track a step looks: a building is picked on its own volumes, inside its stored box. |
| `PICK_REACH` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far along the ground the ray is walked: the deepest ground a view takes in (a low view zoomed out, `isoViewport.ts`), the world having n |
| `PICK_STEP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far along the map a step of the pick's walk reaches (the store's grid cell, `world/buildings/grid.ts`). |
| `PICK_TOP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The highest a roof can stand (the tallest building on the highest land) and the lowest ground: a ray is walked between them. |
| `PREVIEW_ID` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `PlaceParameters` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Parameters of the generated block (the sliders); lengths in world units. |
| `PlanAction` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `ROTATE_STEP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `ToolHost` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `ToolView` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building tool, as a state machine in world coordinates. |
| `now` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `pickAlongRay` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | DO NOT copy `buildings.all()` here: every move of the pointer over the map copied and tested the whole city (audit A9). |
| `pickAlongRay.fresh` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `placeAt` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Near a street it faces the street, as a new building does. |
| `stripId` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/editor/buildings.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingContext` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:76-76 type BuildingContext: the feature's own |
| `BuildingParameters` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:306-311 interface BuildingParameters: the feature's own |
| `EditResult` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:78-83 interface EditResult: the feature's own |
| `ElementPatch` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:988-988 type ElementPatch: the feature's own |
| `FAIL_MISSING` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:85-85 variable FAIL_MISSING: the feature's own |
| `FUSE_GAP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:714-714 variable FUSE_GAP: the feature's own |
| `FacadeScope` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:344-344 type FacadeScope: the feature's own |
| `FaceRegion` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:911-917 interface FaceRegion: the feature's own |
| `MIRROR_SIDE` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:1026-1026 variable MIRROR_SIDE: the feature's own |
| `RELIEF_STEP` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:920-920 variable RELIEF_STEP: the feature's own |
| `RoofShape` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:952-957 interface RoofShape: the feature's own |
| `UNCHANGED` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:86-86 variable UNCHANGED: the feature's own |
| `addBuildingRecord` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:500-509 function addBuildingRecord: the feature's own |
| `blockSnap` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:749-769 function blockSnap: the feature's own |
| `boxesMeet` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:863-864 function boxesMeet: the feature's own |
| `buildingsIn` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:613-620 function buildingsIn: the feature's own |
| `clearBuildingsAfterEdit` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:597-601 function clearBuildingsAfterEdit: the feature's own |
| `clearBuildingsOnRoads` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:576-594 function clearBuildingsOnRoads: the feature's own |
| `clipRing` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:867-872 function clipRing: the feature's own |
| `copyStoreys` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:170-184 function copyStoreys: the feature's own |
| `cutRelief` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:923-936 function cutRelief: the feature's own |
| `deleteBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:536-538 function deleteBuilding: the feature's own |
| `duplicateBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:556-573 function duplicateBuilding: the feature's own |
| `editBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:512-521 function editBuilding: the feature's own |
| `everyBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:604-610 function everyBuilding: the feature's own |
| `groupInto` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:623-670 function groupInto: the feature's own |
| `normaliseAngle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:474-480 function normaliseAngle: the feature's own |
| `opAddElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:982-986 function opAddElement: the feature's own |
| `opAddSetback` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_add_setback(` (porte build) | src/editor/buildings.ts:242-276 function opAddSetback: the feature's own |
| `opAddWing` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_add_wing(` (porte build) | src/editor/buildings.ts:187-239 function opAddWing: the feature's own |
| `opFuseElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:675-711 function opFuseElement: the feature's own |
| `opMirror` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:1029-1100 function opMirror: the feature's own |
| `opMirror.flipSide` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `opMoveBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:717-746 function opMoveBlock: the feature's own |
| `opRemoveElement` | já existia | `native/src/sim/commands/body_parts.cpp` → `std::string op_remove_part(` (porte build) | src/editor/buildings.ts:1015-1022 function opRemoveElement: the feature's own |
| `opRemoveVolume` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_remove_volume(` (porte build) | src/editor/buildings.ts:279-297 function opRemoveVolume: the feature's own |
| `opRepeatElement` | já existia | `native/src/sim/commands/body_parts.cpp` → `std::string op_repeat_part(` (porte build) | src/editor/buildings.ts:1103-1125 function opRepeatElement: the feature's own |
| `opResize` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_resize(` (porte build) | src/editor/buildings.ts:147-167 function opResize: the feature's own |
| `opRotate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:433-447 function opRotate: the feature's own |
| `opScalePlan` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:450-472 function opScalePlan: the feature's own |
| `opSetComponent` | já existia | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_bay(` (porte build) | src/editor/buildings.ts:377-430 function opSetComponent: the feature's own |
| `opSetComponentZone` | já existia | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_zone(` (porte build) | src/editor/buildings.ts:347-374 function opSetComponentZone: the feature's own |
| `opSetLevelHeight` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:329-340 function opSetLevelHeight: the feature's own |
| `opSetParameters` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:313-326 function opSetParameters: the feature's own |
| `opSetParameters.set` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `opSetRelief` | já existia | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_relief(` (porte build) | src/editor/buildings.ts:939-950 function opSetRelief: the feature's own |
| `opSetRoof` | já existia | `native/src/sim/commands/body_finish.cpp` → `std::string op_set_roof(` (porte build) | src/editor/buildings.ts:299-304 function opSetRoof: the feature's own |
| `opSetRoofShape` | já existia | `native/src/sim/commands/body_finish.cpp` → `std::string op_roof_shape(` (porte build) | src/editor/buildings.ts:960-977 function opSetRoofShape: the feature's own |
| `opSetStoreys` | já existia | `native/src/sim/commands/body_mass.cpp` → `std::string op_set_storeys(` (porte build) | src/editor/buildings.ts:114-124 function opSetStoreys: the feature's own |
| `opUpdateElement` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:991-1013 function opUpdateElement: the feature's own |
| `opUpdateElement.size` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Snapped to the grid unless the player holds Alt; free, to the centimetre. |
| `polygonBounds` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:874-886 function polygonBounds: the feature's own |
| `removeVolume` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:541-553 function removeVolume: the feature's own |
| `replaceBuilding` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:524-534 function replaceBuilding: the feature's own |
| `rideWith` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:102-111 function rideWith: the feature's own |
| `shiftBays` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:127-144 function shiftBays: the feature's own |
| `simplePieces` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:889-905 function simplePieces: the feature's own |
| `snapLength` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:99-99 function snapLength: the feature's own |
| `storeyTemplate` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:91-96 function storeyTemplate: the feature's own |
| `weldInto` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/editor/buildings.ts:805-860 function weldInto: the feature's own |

### `src/editor/fromReference.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ReferenceBlock` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `ReferenceOptions` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `TOUCH` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `Top` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `blocksFromTriangles` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The model's blocks, from its upward faces. |
| `blocksFromTriangles.absorb` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `blocksFromTriangles.overlaps` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `buildFromReference` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building the reference shows; null when the model has no block big enough to build. |
| `buildFromReference.height` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `buildFromReference.level` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `buildFromReference.stoneOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `buildFromReference.wallColour` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | 2. Colour: what the walls show, a storey at a time, on the blocks standing on the ground. |
| `buildFromReference.wallColour.med` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The median of each channel: the wall, not its windows or shadows. |
| `convexHull` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `convexHull.chain` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `convexHull.cross` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `distance` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `luminance` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `normalisedOutline` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A block's hull in its own box, 0..1, without points closer than a few centimetres. |
| `normalisedOutline.clamp` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `toHex` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/jobs/jobPool.ts`

**Para o jogador:** Infraestrutura do navegador e do WebGL.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `warmJobWorkers` | não se aplica | core, JobPool, RenderWorld — Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem. | src/jobs/jobPool.ts:537-539 function warmJobWorkers: the feature's own |

### `src/main.ts`

**Para o jogador:** A montagem do 4.0.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `mapSlider.end` | outro porte | Montagem do jogo (play.gd, GameFlow) — Vizinhança; o jogo monta pela cena de jogo. | src/main.ts:1666-1666 method mapSlider.end: the feature's own |
| `mapSlider.set` | outro porte | Montagem do jogo (play.gd, GameFlow) — Vizinhança; o jogo monta pela cena de jogo. | src/main.ts:1659-1664 method mapSlider.set: the feature's own |

### `src/render/buildings/buildingMesh.ts`

**Para o jogador:** Toda a geometria do prédio: paredes, vãos, telhados, lote, escadas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `insideRing` | portado | `native/src/sim/buildings/b40/plan_area.cpp` → `Ring ring_of(const Volume &v)` — o contorno do volume que vira o piso do andar aberto | src/render/buildings/buildingMesh.ts:2364-2371 function insideRing: the feature's own |
| `lotGroundOf.heightAt` | outro porte | render/buildings/facade_*, roof_*, lot_*, shell (a portar lá) — Sem prova por símbolo até agora. Emissores já no jogo; o que falta (interior no corte, escadas abertas, relógio, bandeira) é tratado por item. | src/render/buildings/buildingMesh.ts:2355-2360 method lotGroundOf.heightAt: the feature's own |
| `meshesOf.setVisible` | outro porte | render/buildings/facade_*, roof_*, lot_*, shell (a portar lá) — Sem prova por símbolo até agora. Emissores já no jogo; o que falta (interior no corte, escadas abertas, relógio, bandeira) é tratado por item. | src/render/buildings/buildingMesh.ts:4128-4130 method meshesOf.setVisible: the feature's own |
| `signatureFloorOf` | outro porte | render/buildings/facade_*, roof_*, lot_*, shell (a portar lá) — Sem prova por símbolo até agora. Emissores já no jogo; o que falta (interior no corte, escadas abertas, relógio, bandeira) é tratado por item. | src/render/buildings/buildingMesh.ts:3699-3703 function signatureFloorOf: the feature's own |

### `src/render/buildings/cellArrays.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `cellBytes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/cellArrays.ts:333-345 function cellBytes: the feature's own |

### `src/render/buildings/cellPool.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CellPool` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/cellPool.ts:158-396 class CellPool: the feature's own |
| `CellPool.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellPool.forgetFloors` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT keep the floor of a building gone from the document (a tile let go as the eye walked on): the floors kept follow what is loaded, not  |
| `CellPool.isRunning` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a job is still waiting or being worked on (one done on the page never is). |
| `CellPool.supersede` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A job a newer one for its cell replaces (`cancel`). |
| `CellPool.warm` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The job system's workers made now, ahead of the first job. |
| `FloorColumns.retain` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/cellPool.ts:150-155 method FloorColumns.retain: the feature's own |

### `src/render/buildings/cellRequests.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BatchKind` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's detail (L0) or its facade level (L1, `emitChunk` `far`). |
| `CellRequests` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.asked` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.asking` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.clear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every request forgotten and every batch not taken disposed (a context restored, the layer gone). |
| `CellRequests.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `admit`: whether a level landed is still wanted within the GPU's room, its bytes measured (refused: disposed, never uploaded); `rank`: its u |
| `CellRequests.drop` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell no longer wanted (drawn far before it landed): its request cancelled, its batches if landed disposed. |
| `CellRequests.dropMass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's masses no longer wanted (it has no building left). |
| `CellRequests.holeRoom` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether another hole may be asked past the limits (a full upload queue, `layer.ts` STREAM_AHEAD): fewer than `MOST_HOLES` asked or on their  |
| `CellRequests.holes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.holesInFlight` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.isAsked` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell has been asked for and not landed yet. |
| `CellRequests.land` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.landed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.mass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's masses from afar made of `entries` (`cellWork.ts` 'mass'), handed to `done` when they come |
| `CellRequests.masses` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.massing` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.summaries` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.summarize` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the page keeps of these far buildings (their flags), with no batch made. |
| `CellRequests.take` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's batches taken to be shown: the layer's to dispose from now on. |
| `CellRequests.waiting` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `CellRequests.want` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `cell`'s batches made of `entries`, once landed and (with `upload`) on the GPU; null while they are made |
| `LandedCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's batches as landed: its shells, panes and furniture, and its parts by sub-cell (`partCells.ts`). |
| `Landing` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MOST_HOLES` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The most holes (`want` with `hole`) asked past a full upload queue at once, until their meshes are on the GPU: what that queue grows by at a |
| `disposeLanded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT dispose first: a mesh still queued was drawn after and its geometry went back to the GPU with no owner. |
| `meshesIn` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every mesh of a landed cell, but its shells: those go into the multi-draw batch when the cell is shown, and their arrays are still  read the |

### `src/render/buildings/chunk.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `PackedEntries` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/chunk.ts:50-54 interface PackedEntries: the feature's own |
| `sameEntries` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/chunk.ts:46-47 function sameEntries: the feature's own |
| `samePacked` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/chunk.ts:56-61 function samePacked: the feature's own |
| `unpack` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/chunk.ts:64-67 function unpack: the feature's own |

### `src/render/buildings/farParts.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FarPartBatch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.add` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.boxes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.castShadow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The far cascades take the buildings by their masses alone: the batch casts nothing (`layer.ts` `castInto`). |
| `FarPartBatch.casting` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.cells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.clear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every instance let go (a lost context's cells asked again): the buffers kept, nothing drawn until cells land. |
| `FarPartBatch.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.drawn` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.faded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.group` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.grow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.kinds` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.remove` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.setFaded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Its instances drawn at the coarsest level the far view was made for. |
| `FarPartBatch.sync` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells of `cells` the view draws at their facade level (`shown`) are in the batch: those that came are copied in, those gone taken out. |
| `FarPartBatch.tellBoxes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `FarPartBatch.trianglesOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The triangles the batch draws of a cell, for the view's budget. |
| `FarPartBatch.view` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Each kind drawn only while its pieces span a pixel where the nearest cell stands: a balcony a kilometre off is under one, and only its cost  |
| `KindBatch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | One kind over every facade cell: its master buffers, its mesh, and each cell's run. |
| `Run` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's instances of one kind in the batch: a contiguous run of the kind's buffers. |
| `dirty` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The run's poses and colours sent to the GPU, its range only. DO NOT clear the ranges a change already added: every cell added or taken out t |
| `reachPoint` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |

### `src/render/buildings/flagLayer.ts`

**Para o jogador:** Bandeiras em mastros de prédios públicos, ao vento.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `createFlagLayer.dispose` | outro porte | render/buildings/flags + props.gdshader (a portar lá) — Sem prova por símbolo até agora. Item próprio do render. | src/render/buildings/flagLayer.ts:163-168 method createFlagLayer.dispose: the feature's own |

### `src/render/buildings/frameBudget.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FrameScale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/frameBudget.ts:57-64 class FrameScale: the feature's own |
| `FrameScale.fit` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The levels chosen at the scale that fits the budget: `count` chooses at a scale and says their triangles. |
| `LandedMeasure` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/frameBudget.ts:70-142 class LandedMeasure: the feature's own |
| `LandedMeasure.changed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `LandedMeasure.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.forget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell gone from the town (its tile left the active region): its measures let go, out of the means too (DO NOT keep every cell the camera ev |
| `LandedMeasure.landed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.of` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.othersOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.ratios` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.set` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.size` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.sums` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.unweigh` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LandedMeasure.weightOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What a cell weighs whatever its level (`layer.ts`: its masses' triangles, always at hand), 0 when unknown: a level never landed, or let go,  |
| `LandedMeasure.weights` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `fitErrorScale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/frameBudget.ts:41-54 function fitErrorScale: the feature's own |
| `letBackFitting` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/frameBudget.ts:23-37 function letBackFitting: the feature's own |
| `letGoPassed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/frameBudget.ts:11-20 function letGoPassed: the feature's own |

### `src/render/buildings/glassRoom.ts`

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FRAME_OUT` | já existia | `project/render/shaders/building_pane.gdshaderinc` → `FRAME_OUT_M` (porte build) | How far in front of its pane a frame's middle stands (\`buildingMesh.ts\` \`glazed\`: the frame put 3 cm out of the glass). |
| `FRAME_T` | já existia | `project/render/shaders/building_pane.gdshaderinc` → `FRAME_SHARE` (porte build) | The kit's window frame (\`kit.ts\` frameGeometry, its material): its bars in the unit square (border \`t\`, a mullion and a transom 0.8 t wi |
| `PANE_FRAME_GLSL` | já existia | `project/render/shaders/building_pane.gdshaderinc` → `float pane_frame(` (porte build) | GLSL: the share of the pixel a pane's frame covers and the normal of the face of it seen (view space), drawn on the pane itself.  Near, each |
| `PANE_FRAME_LOOK` | não se aplica | `project/render/shaders/building_pane.gdshaderinc` → `mix(0.0, 0.05, frame)` (porte build) — valores escritos direto no shader; lin() era conversão de hex do JS |  |
| `PANE_RECESS_GLSL` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` (porte build) — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | GLSL, first in the pane's colour: a pane laid on the wall's face (`buildingMesh.ts` glazedRecessed, its glass `vRoomE.x` behind it). The  ey |
| `PANE_RECESS_LIGHT_GLSL` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` (porte build) — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | GLSL, after the frame: the light at the point met in a recess (`PANE_RECESS_GLSL`), on a side, the glass or the frame's face.  The sun reach |
| `REVEAL_LOOK` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `jl{` (porte build) — o recuo pintado no shader serve ao vidro deitado na parede; no jogo o vão tem geometria real | How an opening's reveals are drawn (`PANE_RECESS_GLSL`), as the shell's trim finishes are (`bake/finishRecipes.ts`: render, stone and  concr |
| `roomBehindGlass` | já existia | `project/render/shaders/building_pane.gdshaderinc` → `void pane(vec2 uv` (porte build) | The window panes' material: the room behind each pane drawn by interior mapping |
| `roomBehindGlass.lin` | não se aplica | `project/render/shaders/building_pane.gdshaderinc` → `room_floor(` (porte build) — valores escritos direto no shader; lin() era conversão de hex do JS |  |

### `src/render/buildings/gpuBatchedMesh.ts`

**Para o jogador:** Infraestrutura do navegador e do WebGL.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GpuBatchedMesh.colourHidden` | não se aplica | core, JobPool, RenderWorld — Sem comportamento próprio para o jogador; o motor e o núcleo C++ já fazem. | src/render/buildings/gpuBatchedMesh.ts:80-80 field GpuBatchedMesh.colourHidden: the feature's own |

### `src/render/buildings/kit.ts`

**Para o jogador:** As peças da fachada: caixilhos, grades, venezianas, brises, toldos; e a agenda das luzes externas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `createBuildingKit.dispose` | outro porte | render/buildings/part_models, facade_parts, building_parts.gdshader (a portar lá) — Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos. | src/render/buildings/kit.ts:724-727 method createBuildingKit.dispose: the feature's own |
| `createBuildingKit.setGhostValid` | outro porte | render/buildings/part_models, facade_parts, building_parts.gdshader (a portar lá) — Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos. | src/render/buildings/kit.ts:718-723 method createBuildingKit.setGhostValid: the feature's own |
| `createBuildingKit.setNight` | outro porte | render/buildings/part_models, facade_parts, building_parts.gdshader (a portar lá) — Sem prova por símbolo até agora. Aro hoje tampa o vidro (Shell::box instanciada ignora skip): é a causa dos dois defeitos. | src/render/buildings/kit.ts:697-717 method createBuildingKit.setNight: the feature's own |

### `src/render/buildings/layer.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CAST_RADIUS_SHARE` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | SHADOW CASTERS BY THEIR SIZE IN THE CASCADE (Unreal's r.Shadow.RadiusThreshold, 0.03 at its epic scalability to 0.06 at low: a caster  whose |
| `DETAIL_CLASSES` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The ledger's classes of the cells' detail (L0) and facade level (L1): what the room left under the cap is shared by. |
| `GAME_DETAIL_MEMORY_MB` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The most the cells' detail may hold on the GPU as the game sets it (MB): a large city keeps its far levels and masses for the  whole map, bu |
| `Kept` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the page keeps of a building emitted in a worker beyond the digests it was made for (`TownColumns` `keptSig`, `keptDigest`): its summar |
| `PlanTest` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A world box (three's axes) as the plan's (x, y, height) to the occlusion's test. |
| `createBuildingLayer` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.admit` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's level landed: uploaded only if the view still wants it at that level within the room (the cap worked out again with its measured by |
| `createBuildingLayer.anyLotHeightAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.askExposed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT ask every cell's masses of the map at the opening: only the cells the view draws one by one (their tile refined, `massCells.exposedCe |
| `createBuildingLayer.askMass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT mass the buildings on the page |
| `createBuildingLayer.books` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.capNow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The detail drawn within the room the ledger leaves (`MassCells.cap`): a level never landed counts as its kind's landed do on average, and be |
| `createBuildingLayer.capRaw` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.capTriangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.castInto` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.cellFine` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell near is wanted at full detail (L0) rather than its facade level (L1). |
| `createBuildingLayer.cellNear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.cellOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.census` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.chooseNow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.chooseRaw` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.chosenTriangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the levels chosen draw, triangles: each near cell's level (shown, or as its kind is reckoned until it lands), and every mass resident ( |
| `createBuildingLayer.chunkOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.counted` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's level come or gone: its bytes counted (here and in the ledger), the level shown chosen again. |
| `createBuildingLayer.covers` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.cullDetails` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.cutCellsOf` | portado | `native/src/sim/buildings/interior/view.h` → `bool cuts(int id` — quais prédios a vista alcança (um alvo ou o raio); o 4.0 diz células, o jogo diz chunks | The cells a cut-open view reaches |
| `createBuildingLayer.cutOf` | portado | `native/src/render/buildings/building_bodies.cpp` → `cut_body(` — a chave do corte (prédio, andar, direção em oito passos) é a chave do cache dos corpos cortados | What a cell's entries read of the cut-open view (`near`, `entryOf`), exactly: DO NOT compare the spec by identity, the wiring hands a new on |
| `createBuildingLayer.digestOfCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.dropKept` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.entriesFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.entriesOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.entryFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A building as the workers are sent it: its key, its record's digest and its cell's ground digest (none of its record). |
| `createBuildingLayer.flagsFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.flyFlags` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.forcedCells` | portado | `native/src/render/buildings/building_renderer.h` → `bool cut = false;` — o chunk com prédio aberto é desenhado perto a qualquer distância; o nível médio e a malha distante saem do caminho | The cells held near whatever the eye says: those cut open (`cutCells`), the one being edited (`dimmed`). |
| `createBuildingLayer.hideMass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A mass shown hidden from the colour pass where its box (in the world, `massCells.ts` cellMesh) is behind the walls. |
| `createBuildingLayer.hideOccluded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.keep` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.keepToBudget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Over the budget, the cells drawn far let go, those spanning least of the screen first (`rankOf`): their masses, on the GPU already, stand fo |
| `createBuildingLayer.keptFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether what is kept of a building was made for its record and the ground under it now; its summary (none when it said nothing). |
| `createBuildingLayer.landCells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | One frame's share of the cells: those drawn near with no detail become pending (HLOD streaming) |
| `createBuildingLayer.levelOfDetail` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.levelOfDetail.fitted` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The levels chosen at scale 1, over the view's budget of triangles the farthest coarsened first (`capTriangles`); only when the masses alone  |
| `createBuildingLayer.levelShown` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The level a cell shows: its detail where fine (or with no facade level yet), its facade level where not (or with no detail yet), none far (i |
| `createBuildingLayer.levels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The tiles' (L2) and plates' (L3) masses, asked again only where a cell's buildings changed (`massLevels.ts`). |
| `createBuildingLayer.lodChanged` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.lodChosen` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The levels chosen, as a string: the scale, each near cell and whether at full detail, the cells held. |
| `createBuildingLayer.lotHeightAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.lotOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A kept building's lots' ground, made once from its record (read from the document, then let go) while what is kept was made for that record; |
| `createBuildingLayer.near` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.occluderCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.present` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's levels shown or hidden as `levelShown` says, its masses where it shows none (its parts follow at `cullDetails`). |
| `createBuildingLayer.putKept` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.rank` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A landing's upload ranked by the pixels the masses standing for it leave out (`uploads.ts`). |
| `createBuildingLayer.rankOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | How far down the order the budget coarsens in a cell stands (`MassCells.cap`): the less a metre spans there, the sooner it goes; DO NOT rank |
| `createBuildingLayer.refFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Where a worker reads each record (`layerCells.ts` `recordRef`), found when its job is asked: the table's buffer and row while the record is  |
| `createBuildingLayer.refill` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.restoreOccluded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.revealOccluded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.room` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.roots` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.setCutaway` | portado | `native/src/sim/commands/interior_commands.cpp` → `const char *set_interior_view(` — comando sob a trava exclusiva do mapa; marca os pedaços pelo canal view_chunks do ChangeLog |  |
| `createBuildingLayer.setDetailBudget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.setDimmed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.setMassBudget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.setNight` | já existia | `project/render/daylight.gd` → `func set_hour(` — a hora e a noite chegam aos prédios pelo Daylight (janelas acesas já eram do porte build) |  |
| `createBuildingLayer.setPreview` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.setRuined` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.show` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's level landed shown; its parts drawn from the next `cullDetails`, at the levels the eye allows. |
| `createBuildingLayer.showSignature` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A shown building's signature body (`signature.ts`), its record read for it (the few signature buildings only); none for the rest. |
| `createBuildingLayer.shownAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Where the batches of each level are shown, for `patchCell`. |
| `createBuildingLayer.sigOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A record's digest: its columns' when it is the document's, else worked out (a building drawn on its own). |
| `createBuildingLayer.slotNow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | An id's slot with its columns those of its record now (read from the document when the diary named it since); -1 when it is not in the docum |
| `createBuildingLayer.update` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createBuildingLayer.update.inCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A building drawn in its cell: not the one the preview stands for, nor a ruin, nor the one left solid while the others fade. |
| `detailClass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `groupBoxReader` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A group's box in the world from its meshes' own boxes (an instanced part's, a mesh's geometry's put in place), kept while its children  are  |
| `planHidden` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `playerCause` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a diary entry is the player's edit (`main.ts` mutateBuilt's `tool …`, undo, redo), not the growth (`zone growth`) nor a tile coming  |

### `src/render/buildings/layerApi.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingLayer` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `BuildingPreviewInput` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT put the detail of every cell together again, and DO NOT hold a far cell's detail for a while |
| `BuildingRoot` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the layer draws, part by part: the masses and their far levels, the facade level, the cells in detail, their parts, the signature build |
| `CutawaySpec` | portado | `native/src/sim/buildings/interior/view.h` → `struct InteriorView` — o jogo guarda a vista no CityState (um prédio ou um raio), sem salvar |  |
| `GroundDigestOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A content digest of everything the ground, the natural ground and the paving read in a rectangle (`BuildingLayer.update`). |

### `src/render/buildings/layerCells.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `<anonymous class>.clear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `<anonymous class>.delete` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `<anonymous class>.set` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `BATCH_CELL` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Side of the cells the buildings are batched in: the world's one grid (`world/cells.ts`). |
| `COARSE_PARTS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Frozen tables, not state (`eslint-rules/no-module-state.js`). The parts drawn with the cell's shells, one batch a cell: window panes (each w |
| `COVER_CELL` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Side of the footprint index's buckets, world units (ten to a world cell, `CELL`). |
| `COVER_CPU_BUDGET` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The most the cover cells hold on the CPU (`ViewCells`): a few hundred cells of a dense town. |
| `COVER_PAD` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | How far past a building's stored box its cover reaches (its footprints grown by `PLANT_MARGIN`), with a margin. |
| `COVER_REACH` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | How far round the eye the cover cells are derived ahead of the plants' questions. |
| `COVER_VERSION` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cover's generator: moves when what a cell holds is worked out otherwise. |
| `CellBatch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `bytes`: what a cell's batch and its parts (`partCells.ts`) send to the GPU. |
| `CellCache` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The buildings batched cell by cell, each cell its own meshes: a cell out of the view  A cell's batch and the entries it was made of (its coa |
| `CoverCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | One cell of the cover: the polygons reaching it, flat (x, y pairs; polygon k's points from `starts[k]` to `starts[k + 1]`), filed by bucket. |
| `CoverIndex` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Where the buildings stand, for the scenery's plant cull: derived cell by cell round the questions asked (`viewCells.ts`), never for the whol |
| `CoverSource` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the cover index reads: the document's buildings and its diary. |
| `DETAIL_PARTS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every other part, filed by sub-cell and drawn by the error on the screen (`partCells.ts`). |
| `EMPTY_COVER` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `LevelKind` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's level: its detail or its facade level (`cellRequests.ts` `BatchKind`, named here so the cells' module imports nothing of the reques |
| `PLANT_MARGIN` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | How far past a wall a plant is still considered under the building. |
| `ResidentBytes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | THE BYTES THE CELLS' LEVELS HOLD ON THE GPU (`layer.ts` `counted`, `keepToBudget`), by level, and what each level measured when it last land |
| `ResidentBytes.changed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `ResidentBytes.count` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A level come (`bytes` over 0) or gone (0). |
| `ResidentBytes.forget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's buildings changed: what its levels measured no longer holds. |
| `ResidentBytes.landed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A level landed, uploaded or not: what it measured. |
| `ResidentBytes.measured` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ResidentBytes.of` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What a level holds or would hold, bytes: resident, else as measured when it last landed, else `typical`. |
| `ResidentBytes.resident` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ResidentBytes.total` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What the levels hold now, bytes. |
| `STREAM_AHEAD` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Cells asked of the workers at once while the eye streams the town in (`landCells`, `layer.ts`), nearest first. |
| `boxGroundDigest` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The ground of a box sampled as `groundDigest` samples a building's (6 by 6 points): the layer's, from the box its columns keep (`townColumns |
| `bucketKey` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `cellGroup` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A group of the cells in `cache` (the coarse batches, or the detail ones): its triangles theirs, disposing it lets go of the group only. |
| `cellGroup.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells' batches belong to the cache: only this group is let go. |
| `cellGroup.setVisible` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `coverBytes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The bytes a cover cell holds: its arrays, and each bucket's entry. |
| `coverCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's cover from the buildings reaching it: a pure function of the cell and the document. |
| `coverPolygons` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A building's ground no plant stands on: its solid footprints grown by `PLANT_MARGIN`, and its footprint's rectangles (the cover index's and  |
| `createCoverIndex` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createCoverIndex.covers` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createCoverIndex.follow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Brought up to the document: the cells its buildings' changes reached dropped (made again when asked), the rest kept. |
| `createCoverIndex.look` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The eye: the cells round it derived ahead of the plants' questions, `most` a call. |
| `dropCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `groundDigest` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT work it out again here: every building the ground moved near turned its corners into the world again, 3 s of a 2 |
| `inCoverPolygon` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether (x, y) is in polygon `k` of a cell (`pointInPolygon` on the flat points, the same arithmetic). |
| `recordRef` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's batch out of its cache and its group, disposed.  Where the workers read a building's record (`layer.ts` `refOf`): its tile's shared |
| `watchedCells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell cache that tells `told` each key that comes in (true) or goes (false). |

### `src/render/buildings/levelKeys.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `baseOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/levelKeys.ts:15-15 function baseOf: the feature's own |
| `cellIndex` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/levelKeys.ts:18-21 function cellIndex: the feature's own |
| `plateOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/levelKeys.ts:13-13 function plateOf: the feature's own |
| `tileOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/levelKeys.ts:12-12 function tileOf: the feature's own |

### `src/render/buildings/massCells.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Base` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:96-96 interface Base: the feature's own |
| `BasePart` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:94-94 interface BasePart: the feature's own |
| `Cell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:44-61 interface Cell: the feature's own |
| `CellDrawn` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:99-99 type CellDrawn: the feature's own |
| `DETAIL_REACH` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:28-28 variable DETAIL_REACH: the feature's own |
| `DETAIL_REACH_VISIBLE` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:30-30 variable DETAIL_REACH_VISIBLE: the feature's own |
| `FACADE_ERROR` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:21-21 variable FACADE_ERROR: the feature's own |
| `FAR_ERROR` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:19-19 variable FAR_ERROR: the feature's own |
| `FINE_PIXELS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:25-25 variable FINE_PIXELS: the feature's own |
| `HORIZON_HALF` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:33-33 variable HORIZON_HALF: the feature's own |
| `HYSTERESIS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:27-27 variable HYSTERESIS: the feature's own |
| `LEVEL_CEILING` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:39-39 variable LEVEL_CEILING: the feature's own |
| `LEVEL_FLOOR` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:38-38 variable LEVEL_FLOOR: the feature's own |
| `Level` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:64-91 interface Level: the feature's own |
| `MassCells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:130-214 interface MassCells: the feature's own |
| `NEAR_PIXELS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:23-23 variable NEAR_PIXELS: the feature's own |
| `STREET_EYE` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:32-32 variable STREET_EYE: the feature's own |
| `Seen` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:227-227 interface Seen: the feature's own |
| `VIEW_COME` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:35-35 variable VIEW_COME: the feature's own |
| `VIEW_KEEP` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:36-36 variable VIEW_KEEP: the feature's own |
| `baseMesh` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:105-128 function baseMesh: the feature's own |
| `boxOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:216-216 function boxOf: the feature's own |
| `cellMesh` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:244-275 function cellMesh: the feature's own |
| `cellMesh.half` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Half floats where a vertex's numbers allow (`CellMass`): three uploads a Uint16Array as HALF_FLOAT where the attribute says so (r186 `WebGLA |
| `createMassCells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:280-1072 function createMassCells: the feature's own |
| `createMassCells.askable` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a tile or a plate let go for the budget may be asked again: the view meets it and wants it drawn whole (true for one not let go). DO |
| `createMassCells.books` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What it keeps, by book: cells, tiles and plates known, masses standing (`BuildingLayer.books`). |
| `createMassCells.cap` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | DO NOT draw more detail than the budget holds (the brief: detail residency capped in bytes, falling back to L1): the farthest near cells fal |
| `createMassCells.cap.sameRank` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells of the same rank as `movable[at]` (the same error and distance): stepped down together, never one of two alike. |
| `createMassCells.cellsLanded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether every cell of a tile has its masses: until then the tile drawn before stands for them (Cesium keeps a parent until its children load |
| `createMassCells.census` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What draws each cell of buildings now, by key (the census of holes, `tests/perf/massBase.spec.ts`). |
| `createMassCells.choose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Which cells are drawn near (their detail) and which far (their masses), from the eye (map x/y, height) and the screen  - DO NOT give detail  |
| `createMassCells.choose.coarseWanted` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether the view wants a level drawn in place of its children: judged by its error once its masses came, by what its level may leave out bef |
| `createMassCells.choose.decide` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.choose.look` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A level meeting the view (grown by `VIEW_KEEP`): seen now. |
| `createMassCells.choose.wait` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A level waited for: by the opening's picture too while it meets the frustum (`waitingInView`). One let go for the budget out of the view is  |
| `createMassCells.cover` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell under a tile or plate drawn in its place: far (its detail let go by the layer), its mass hidden; one with no mass stays near. |
| `createMassCells.coveredAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell's tile or plate stands in its place now. |
| `createMassCells.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.drawn` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell has its masses on the GPU, or a tile or plate drawn in its place (`layer.ts` drawnOver): what stands for it before its detail |
| `createMassCells.drawnBy` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | What draws a cell other than the base: its tile or plate drawn in its place (with its masses), its detail shown near, its own masses; null f |
| `createMassCells.drop` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.dropBase` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.dropMesh` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.ensure` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A plate is kept while it has a mesh or a tile of its own: its tiles are found through it (`choose`), so a tile makes its plate. |
| `createMassCells.evictLevels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The tiles' and plates' masses out of the view let go, the least recently seen first, until every mass of the town (cells, tiles, plates) hol |
| `createMassCells.exposedCells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells drawn one by one now (their tile refined, `choose`): the only cells whose own masses are asked (`layer.ts`). |
| `createMassCells.farTriangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The triangles drawn for what is not near: the tiles and plates shown and the far cells' masses (a near cell's masses, hidden under its detai |
| `createMassCells.fine` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Drawn near and at full detail (L0); near and not fine: its facade level (L1). |
| `createMassCells.followView` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.forget` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.hold` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells the frame's budget holds back (`layer.ts` capTriangles): `far` never drawn near, `coarse` never at full detail, until let go there |
| `createMassCells.inView` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.keys` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every cell there is (the whole town filed again, `layer.ts`). |
| `createMassCells.land` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's masses come: its mesh made of them (null: its buildings have no mass, the cell always near). |
| `createMassCells.landBase` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A base's prisms come (`massLevels.ts` landBase; null: none of its buildings stands from afar). |
| `createMassCells.landLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile's or a plate's masses come (`massLevels.ts`; null: none of its buildings has any): the old ones stood until now. |
| `createMassCells.letCellsGo` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile left unexposed by the frame's last choice: its cells' masses let go, neither drawn nor wanted. |
| `createMassCells.letTileGo` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile left unwanted by the frame's last choice: its masses let go, asked again once its plate refines. |
| `createMassCells.levelDrawn` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a tile or a plate has its masses on the GPU (drawn or ready to be). |
| `createMassCells.levelInView` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.levelPixels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.levelSeen` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a tile or a plate met the view's frustum, grown by `VIEW_KEEP`, at the last `choose` (false before any `choose`; true for every leve |
| `createMassCells.levelWhole` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether the view wants a tile or a plate drawn whole (in place of its children) at the last `choose` (true for one not chosen yet). |
| `createMassCells.levelsOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.lowered` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.massBytes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The bytes a cell's masses hold on the GPU (0 before they came): what a level of it never landed is reckoned at (`layer.ts` capNow). |
| `createMassCells.massTriangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The triangles of a cell's masses (0 before they came): what it costs sent far (`layer.ts` capTriangles). |
| `createMassCells.measure` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.near` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell is drawn near; a cell with no mass (lawns only), or none known, always is. |
| `createMassCells.nearKeys` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The cells drawn near now (what the streaming of the detail reads, `layer.ts` `landCells`). |
| `createMassCells.perMetreAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.pixels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The pixels `metres` spans at a cell from the last eye (`choose`): what its upload is ranked by (`uploads.ts`). |
| `createMassCells.pixelsAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.plan` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell whose masses are asked (new, or its buildings changed): kept as it stands until they come (`land`); `bottom`/`top` its buildings' hei |
| `createMassCells.planLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile or a plate that has buildings, its masses asked or not yet (`massLevels.ts` `plan`). |
| `createMassCells.remove` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell with no building left: gone. |
| `createMassCells.removeLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile or a plate with no building left: gone. |
| `createMassCells.setCoarse` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A level drawn in place of its cells or not: its mesh shown, its cells covered or uncovered (16 or 64 looked up, on a change only). |
| `createMassCells.setDetailed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a cell's detail is shown (`layer.ts` streaming): a near cell keeps its mass shown until its detail lands |
| `createMassCells.setErrorScale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The frame's error scale for the next `choose` (`frameBudget.ts`): every error is allowed this many times the pixels it is allowed at 1. |
| `createMassCells.setExposed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile no longer refined (its plate or itself drawn whole): its cells' masses let go, neither drawn nor wanted (farTree.ts does the same wit |
| `createMassCells.setNear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.setWanted` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile no longer wanted (its plate drawn whole): its masses let go, asked again once its plate refines (`massLevels.ts` `release`). |
| `createMassCells.settle` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The frame's choosing is over (`layer.ts` levelOfDetail): what its last choice left unexposed or unwanted let go. |
| `createMassCells.shadowMasses` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A far sun cascade being drawn (`layer.ts` castInto): the cells drawn in detail whose masses can stand for them shown for the pass, their key |
| `createMassCells.showMass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's mass shown: far, or near with no detail to stand in for it; never under its tile or plate. |
| `createMassCells.square` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's square on the map. |
| `createMassCells.stand` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `createMassCells.syncBases` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The bases drawn under exactly the cells nothing finer draws (three's geometry groups): asked before each frame is drawn (`layer.ts` cullDeta |
| `createMassCells.top` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The highest point of any cell's masses (the shadow's reach over the view, `renderer.ts`); -Infinity with none. |
| `createMassCells.wanted` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile: wanted by the view (its plate refined), so asked of the workers (`massLevels.ts` `wanted`); a plate always is. |
| `keyOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:42-42 function keyOf: the feature's own |
| `levelClass` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:102-102 function levelClass: the feature's own |
| `measureSeen` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:230-241 function measureSeen: the feature's own |
| `sameFrustum` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:1075-1081 function sameFrustum: the feature's own |
| `sendUp` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:220-224 function sendUp: the feature's own |
| `sphereOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massCells.ts:217-217 function sphereOf: the feature's own |

### `src/render/buildings/massLevels.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `KIND` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massLevels.ts:11-11 variable KIND: the feature's own |
| `MassLevels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massLevels.ts:16-180 class MassLevels: the feature's own |
| `MassLevels.ask` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.asked` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.away` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | How far a level is from the eye (`layer.ts`): the plates waiting are asked nearest first. |
| `MassLevels.changed` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Called whenever what it answers may change (`layer.ts` levelOfDetail chooses the levels again then). |
| `MassLevels.clear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every request cancelled and every cell forgotten (a context restored, the layer gone): the next filing touches them all again. |
| `MassLevels.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `land`: a level's masses come (null: none of its buildings has any); `remove`: a level with no building left; `plan`: a level that has build |
| `MassLevels.deferred` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile or a plate left waiting for now (`layer.ts`: out of the view while the curtain is up): asked by `pump` once it is not. |
| `MassLevels.dirty` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.edited` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.entries` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.flush` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every plate touched asked once (`PLATES_IN_FLIGHT` at a time, `pump`), made of its cells' entries; a tile touched asked once it is wanted (` |
| `MassLevels.hasAny` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.hole` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether nothing stands for a level yet (`layer.ts`: a plate with no masses of its own): asked in the far levels' band (`cellPool.ts` hole),  |
| `MassLevels.landBase` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A base's prisms come (`massBase.ts`; null: none of its buildings stands from afar). |
| `MassLevels.listOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.outdated` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Whether a level's buildings changed since its masses were made (asked and not come, or not asked yet): its mesh, if any, is of older buildin |
| `MassLevels.pump` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The tiles changed since last asked that a view now wants (a plate refined): asked (each frame, `layer.ts`; as many as tiles changed, never t |
| `MassLevels.release` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A tile whose masses were let go, neither drawn nor wanted, or a plate's let go out of the view for the budget (`massCells.ts` evictLevels):  |
| `MassLevels.stale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `MassLevels.touch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's buildings asked again (`entries`), or gone (null): its tile and plate to be asked again at `flush`; `edited`, by the player's edit  |
| `PLATES_IN_FLIGHT` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/massLevels.ts:13-13 variable PLATES_IN_FLIGHT: the feature's own |

### `src/render/buildings/partCells.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BAR_PIXELS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:29-29 variable BAR_PIXELS: the feature's own |
| `Band` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:48-79 interface Band: the feature's own |
| `COARSEN` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:21-21 variable COARSEN: the feature's own |
| `EVICT_AFTER` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:34-34 variable EVICT_AFTER: the feature's own |
| `LIGHT_BACK` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:32-32 variable LIGHT_BACK: the feature's own |
| `LIGHT_PIXELS` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:31-31 variable LIGHT_PIXELS: the feature's own |
| `NONE` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:27-27 variable NONE: the feature's own |
| `PART_ERROR_PX` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:25-25 variable PART_ERROR_PX: the feature's own |
| `PartCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:103-517 class PartCell: the feature's own |
| `PartCell.bands` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.bounds` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.box` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.castInto` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Before a sun cascade is drawn: each band casts into it where its widest piece spans the cascade's texel, every instance of it at the  level  |
| `PartCell.choose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Each sub-cell of each band at the coarsest level whose error is under a pixel, none where an instance is; shadows cast where a piece spans a |
| `PartCell.chosenFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `filing`: the cell's parts by band and sub-cell, as its job filed them (`partFiling.ts`): DO NOT read an instance here, the page only wraps  |
| `PartCell.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.drawFar` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.evictStale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.faded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.filing` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.group` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.levelAt` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.make` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.makeCaster` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.perMetre` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.posedLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runFirst` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runLast` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runPixels` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runsOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.runsOf.triangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.setFaded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every kind drawn faded ("Ocultar outros") or not. |
| `PartCell.shadowLevel` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.tick` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.triangles` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `PartCell.warmMeshes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A mesh a band over the whole cell, made now to send its instances to the GPU ahead (`uploads.ts`): every run shares its buffers. |
| `PartView` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:37-44 interface PartView: the feature's own |
| `barsAcross` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:84-88 function barsAcross: the feature's own |
| `diagonal` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:96-96 function diagonal: the feature's own |
| `dropParts` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:520-526 function dropParts: the feature's own |
| `largest` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:82-82 function largest: the feature's own |
| `middle` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:98-101 function middle: the feature's own |
| `widestBar` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partCells.ts:90-94 function widestBar: the feature's own |

### `src/render/buildings/partFiling.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `filingBytes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/partFiling.ts:205-209 function filingBytes: the feature's own |

### `src/render/buildings/parts.ts`

**Para o jogador:** Modelos dos móveis no corte, miniaturas das peças.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ELEMENT_PARTS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:44-44 variable ELEMENT_PARTS: the feature's own |
| `FRAME_SHARE_MS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:29-29 variable FRAME_SHARE_MS: the feature's own |
| `IDLE_MS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:31-31 variable IDLE_MS: the feature's own |
| `KIT_HASH` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:135-135 variable KIT_HASH: the feature's own |
| `MODEL_KEYS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:55-55 variable MODEL_KEYS: the feature's own |
| `PER_FRAME` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:27-27 variable PER_FRAME: the feature's own |
| `ROOF_PARTS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:40-42 variable ROOF_PARTS: the feature's own |
| `RUN_PARTS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:47-53 variable RUN_PARTS: the feature's own |
| `SIZE` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:25-25 variable SIZE: the feature's own |
| `ThumbnailStudio` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:122-131 interface ThumbnailStudio: the feature's own |
| `WALL_PARTS` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:34-38 variable WALL_PARTS: the feature's own |
| `__BUILDING_KIT_HASH__` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `blank` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:57-61 function blank: the feature's own |
| `createThumbnailStudio` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:137-322 function createThumbnailStudio: the feature's own |
| `createThumbnailStudio.hold` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | While held (the Builder in use) the studio keeps its context between galleries |
| `createThumbnailStudio.keptPictures` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createThumbnailStudio.release` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createThumbnailStudio.request` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | Photographs the ids that are worth a picture, a few per frame, calling back with each batch. |
| `createThumbnailStudio.setUp` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createThumbnailStudio.shoot` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createThumbnailStudio.step` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `partSample` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | src/render/buildings/parts.ts:64-120 function partSample: the feature's own |

### `src/render/buildings/partyWalls.ts`

**Para o jogador:** A parede encostada no vizinho não é desenhada.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Cover` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | One block of a neighbour as a wall beside it reads it: its plan in the world, its box, and the heights its walls (and plinth) stand between. |
| `PARTY_INSET` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. |  |
| `PARTY_NUDGE` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | How far out of a bay's face its neighbour's mass is looked for: two walls on one lot line, never a gap a view could go into. |
| `PARTY_REACH` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | Two boxes this close may stand back to back. |
| `PARTY_STEP` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | A bay is looked at this often along its width, its ends this far in. |
| `PartyWalls` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | The buildings of one job read as one another's neighbours: only those the job is handed, so a neighbour that changes changes the job's entri |
| `PartyWalls.blocks` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. |  |
| `PartyWalls.boxes` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. |  |
| `PartyWalls.constructor` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | `known`: each building's floor as an earlier job measured it (`CellEntry.floor`), not measured again. |
| `PartyWalls.coversNear` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | The other buildings' blocks whose box touches this one's. |
| `PartyWalls.coversOf` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. |  |
| `PartyWalls.floorOf` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | A building's ground floor as `emitChunk` lays it (its natural ground, `floorHeight`), worked out once for the job. |
| `PartyWalls.floors` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. |  |
| `PartyWalls.floorsKnown` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | Each building's floor as the job knows it now, in the job's order: null with no closed block, undefined where none was asked. |
| `hiddenByCovers` | outro porte | render/buildings/party_walls (a portar lá) — Sem prova por símbolo até agora. Pendência do CLAUDE.md. | Whether a stretch of wall - from (x, y) along (tx, ty) for `width`, facing (nx, ny), from z0 to z1, all in the world - stands inside the cov |

### `src/render/buildings/referenceModel.ts`

**Para o jogador:** Modelos dos móveis no corte, miniaturas das peças.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CAPTURE_DENSITY` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | Pixels per metre of the pictures a capture reads its colours from. |
| `CAPTURE_MAX` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `DEPTH_MATERIAL` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | Depth as two 8-bit channels (an orthographic camera's depth is linear). |
| `ReferenceCapture` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `ReferenceInfo` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `ReferenceModel` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `ReferenceTarget` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | A reference model in the Builder: a real building's 3D model |
| `View` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `captureModel` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | Reads the triangles and the colours of a model (a copy of it in metres, three's axes). |
| `captureModel.at` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `captureModel.plan` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `captureModel.sample` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.apply` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.capture` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. | The model as a surveyor reads it, in the frame of where it stands (the building it is placed on, or the origin) |
| `createReferenceModel.fade` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.flip` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.load` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.placeOn` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.remove` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.toggle` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `createReferenceModel.turn` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |
| `toSrgb` | outro porte | render/buildings/furniture (a portar lá) — Sem prova por símbolo até agora. Depende do editor. |  |

### `src/render/buildings/roomBehind.ts`

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FloorKind` | já existia | `native/src/sim/buildings/interior/interior.h` → `enum FloorFinish` (porte build) | A floor's finish: 0 wood, 1 stone, 2 porcelain tiles. |
| `HEAD_SHADE` | já existia | `native/src/render/buildings/facade_bay.cpp` → `HEAD_SHADE = 0.8f` (porte build) |  |
| `INSIDE` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `INSIDE = 0.5f` (porte build) | How far inside the wall the side walls are looked for. |
| `MOST_DEEP` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `MOST_DEEP = 12.f` (porte build) | The farthest a wall is looked for: past it the room is closed at this depth. |
| `MOST_WIDE` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `MOST_WIDE = 10.f` (porte build) |  |
| `PANE_FRAME_AT` | já existia | `native/src/render/buildings/shell.h` → `float frame = 0;` (porte build) | Where a pane's frame is written among its ROOM_FLOATS (`PartRows.paneFrame`): its trim's r, g, b and the frame's thickness (0: no frame). |
| `PANE_KEY` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() | Numbers a pane is looked up by: its face's start, along and out directions, its place on the face and its ceiling. |
| `PANE_RECESS_AT` | não se aplica | `native/src/render/buildings/facade_bay.cpp` → `o.depth` (porte build) — o vidro do jogo já fica no fundo do vão, com ombreiras de verdade | Where a pane's recess is written (`PartRows.paneRecess`): how deep the glass stands behind the wall's face when the pane is laid on the  fac |
| `PaneMemo` | já existia | `native/src/render/buildings/pane_rooms.h` → `struct PaneRooms` (porte build) | The rooms worked out for a storey's panes, by their nine numbers: equal as numbers is the same pane (0 and -0 alike, as their text was). |
| `PaneMemo.add` | já existia | `native/src/render/buildings/pane_rooms.h` → `known;` (porte build) | `room` kept for `key` (just looked up and not found). |
| `PaneMemo.bits` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.find` | já existia | `native/src/render/buildings/pane_rooms.h` → `known;` (porte build) | The index of the room kept for `key`, or -1. |
| `PaneMemo.hash` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.key` | já existia | `native/src/render/buildings/pane_rooms.h` → `known;` (porte build) |  |
| `PaneMemo.keys` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.place` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.rehash` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.rooms` | já existia | `native/src/render/buildings/pane_rooms.h` → `known;` (porte build) |  |
| `PaneMemo.slots` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `PaneMemo.words` | não se aplica | `native/src/render/buildings/pane_rooms.cpp` → `uint64_t mix(` (porte build) — tabela hash feita à mão em JS; o jogo usa unordered_map com a chave de mix() |  |
| `REVEAL_SHADE` | já existia | `native/src/render/buildings/facade_bay.cpp` → `REVEAL = 0.68f` (porte build) | The tones of an opening's reveals against its trim, as the shell lays them (`buildingMesh.ts` emitBay) and a recessed pane draws them  (`gla |
| `ROOM_FLOATS` | já existia | `native/src/render/buildings/shell.h` → `struct PaneRoom` (porte build) | THE ROOM BEHIND A WINDOW, as the building's own interior has it (`interiorAt`, the rooms the cut-open view draws) |
| `ROOM_OWN` | já existia | `native/src/render/buildings/shell.h` → `struct PaneRoom` (porte build) | The room's own numbers (`roomsOfStorey`): the first twelve of a pane's ROOM_FLOATS. |
| `SILL_SHADE` | já existia | `native/src/render/buildings/facade_bay.cpp` → `SILL_SHADE = 0.92f` (porte build) |  |
| `firstHit` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `float first_hit(` (porte build) | Where a ray (origin, unit direction) first crosses one of the segments (x0, y0, x1, y1 each), or `far`. |
| `floorKind` | já existia | `native/src/sim/buildings/interior/interior.h` → `FloorFinish floor_finish(` (porte build) | A storey's floor (`buildingMesh.ts` `emitInterior` and the windows alike): stone where people pass |
| `roomsOfStorey` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `PaneRoom room_at(` (porte build) | The rooms of one block's storey seen from its facade |
| `roomsOfStorey.roomAt` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `PaneRoom room_at(` (porte build) |  |
| `seen` | já existia | `native/src/render/buildings/pane_rooms.cpp` → `interior.seen[f.kind]` (porte build) | What a pane shows as its piece: the solid ones, that read as a box from across a room |

### `src/render/buildings/roomLamps.ts`

**Para o jogador:** O vidro mostra o cômodo (paredes, piso, móvel, cortina, persiana), acende de noite pela hora e com a lâmpada do cômodo; de longe as janelas seguem acesas e sem cintilar.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `HOME_MIX` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `lot < 0.36` (porte build) | The share of homes' lit rooms by lamp: warm 2700 K, 3000 K, the kitchen's 3500 K, cool white. |
| `KELVIN_SRGB` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_2700` (porte build) | THE COLOUR OF A LIT ROOM, by the room: one lamp colour for every window of the town read as a sheet of amber at night |
| `OFFICE_MIX` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `lot < 0.5` (porte build) | Offices and shops: neutral 4000 K, daylight 5000 K, a warmer 3500 K in some. |
| `ROOM_LAMP_GLSL` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `vec3 room_lamp(` (porte build) | GLSL: `roomLamp(lot, office)`, a lit room's lamp colour from its lot (0..1) and whether it is an office or a shop; `roomLampMean(office)`, t |
| `TV_SHARE` | já existia | `project/render/shaders/building_pane.gdshaderinc` → `TV_SHARE = 0.12` (porte build) | The share of a home's lit rooms that are lit by a television alone. |
| `lampVec` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_6500` (porte build) | A black body's colour at `kelvin`, linear, of luminance 1, as GLSL. |
| `meanGlsl` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `LAMP_HOME_MEAN` (porte build) | A mix's mean lamp, as GLSL: what its rooms' lamps average to where many windows fall in a pixel. |
| `mixGlsl` | já existia | `project/render/shaders/building_lamps.gdshaderinc` → `vec3 room_lamp(` (porte build) |  |

### `src/render/buildings/shellBatch.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Batch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `IDENTITY` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.add` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's shells (every mesh of `SHELL_MESH` in its batch) taken into the batches; the meshes let go. |
| `ShellBatch.batches` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.boxes` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.castShadow` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The far cascades take the buildings by their masses alone: the walls cast nothing into them. |
| `ShellBatch.casting` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.cells` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.detailGroup` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.dispose` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.ensure` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.facadeGroup` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.faded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.hideOccluded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The colour pass about to be drawn from `camera`: the shells of every cell shown whose box `hidden` says is behind the walls laid  (`render/o |
| `ShellBatch.makeBatches` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.makeBatches.make` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.remove` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's shells taken out: its geometries (and their instances) deleted from their batch. |
| `ShellBatch.reset` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every instance let go (a lost context's cells asked again): the batches made again, their meshes put back in the groups. |
| `ShellBatch.restoreOccluded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The colour pass drawn: every shell drawn again from any camera. |
| `ShellBatch.scratch` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.setFaded` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.setVisible` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A cell's level shown or hidden (a cell still on another level is not drawn, and its pieces cost nothing). |
| `ShellBatch.squeeze` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellBatch.visible` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `ShellSet` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The three attribute sets a cell's shells come in: the detail level's plain walls, the facade level's plain ones, and its runs  of bays with  |
| `castChanged` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A batch whose drawn pieces changed (one added, taken out, shown or hidden): its `castVersion` stepped, read by the shadows' signature of the |
| `worldGeometry` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The mesh's shell as world vertices: the far level's positions come quantized on the cell's grid, the mesh's place putting them  in the world |
| `worldPositions` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A shell's stored positions (four integers a vertex, normalized to the cell's grid) into world positions (three a vertex): the integers  read |

### `src/render/buildings/signature.ts`

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas).

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `buildSignature` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | src/render/buildings/signature.ts:1755-1799 function buildSignature: the feature's own |
| `buildSignature.solid` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | The faces another part covers, or that lie on the ground facing down, taken out before the parts are merged (`hiddenFaces.ts`): a step insid |
| `disposeSignature` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | src/render/buildings/signature.ts:1801-1803 function disposeSignature: the feature's own |
| `makeMaterials` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | src/render/buildings/signature.ts:199-358 function makeMaterials: the feature's own |
| `makeMaterials.ashlar` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | Surfaces that read as stone, concrete, roofing, tiles: never flat colour (flat light planes blow out to white). |
| `makeMaterials.chevrons` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | Art Deco spandrel: gilded chevrons on dark bronze. |
| `makeMaterials.courses` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. |  |
| `makeMaterials.decoPanel` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | An Art Deco gilded panel: a stepped sunburst and zigzags. |
| `makeMaterials.rusticated` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | Rusticated stone: 0.6 m courses with deep shadowed joints, long blocks. |
| `makeMaterials.seams` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | A standing-seam copper roof: vertical seams 0.5 m apart. |
| `makeMaterials.speckle` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. |  |
| `makeMaterials.std` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. |  |

### `src/render/buildings/townColumns.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `TownColumns` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/townColumns.ts:54-178 class TownColumns: the feature's own |
| `TownColumns.cellOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The coarse cell an id is filed in (`BATCH_CELL`), from its anchor. |
| `TownColumns.clear` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every id let go (another document, the whole town filed again). |
| `TownColumns.drop` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | An id gone from the document: its slot handed back. |
| `TownColumns.file` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A record's numbers put in its columns (what was kept of it and whether it is shown stay). |
| `TownColumns.has` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownColumns.idOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The id held at a slot. |
| `TownColumns.members` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownColumns.set` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownColumns.size` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownColumns.slotOf` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The slot of an id; -1 when it has none. |
| `TownColumns.stale` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The record of an id changed (the diary named it): its numbers read again when next asked. |
| `memberDigest` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/townColumns.ts:26-33 function memberDigest: the feature's own |
| `recordDigests` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/buildings/townColumns.ts:38-52 function recordDigests: the feature's own |

### `src/render/buildings/townIndex.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CellBatches` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Where a cell's batches are kept and shown. |
| `TownIndex` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The buildings the layer shows, by id and by coarse cell, kept from one change to the next: ids, never records (`townColumns.ts`). |
| `TownIndex.byCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownIndex.constructor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownIndex.file` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownIndex.has` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `TownIndex.reset` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every shown building filed again (the whole town: the opening, a map opened); `shown`, their slots (their columns filed). |
| `TownIndex.take` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | A building as it is now taken in (`slot`: its columns filed; -1: gone, or not shown); the cells it left and entered marked in `dirty`. |
| `patchCell` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | One cell's batches swapped for those landed for it (`cellRequests.ts`), its parts with them; none |
| `signatureBodies` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The signature buildings' bodies |
| `signatureBodies.keepOnly` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | Every body let go but those of the ids `keep` keeps. |
| `signatureBodies.remove` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. |  |
| `signatureBodies.set` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | `sig`: its record's digest (`townColumns.ts`): the body made again only when it or its floor changed. |
| `signatureBodies.top` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | The highest point of any body (`massCells.ts` `top` for the rest); -Infinity with none. |

### `src/render/gpuLedger.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `GpuLedger.roomFor` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/gpuLedger.ts:62-66 method GpuLedger.roomFor: the feature's own |

### `src/render/interiorSolids.ts`

**Para o jogador:** Os sólidos do interior de um VEÍCULO: painel, console, bancos, volante, pedais, forro das portas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CabinParts` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. | The cabin's parts as they are added, each named; merged by the caller in the same order. |
| `CabinParts.add` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. | Adds parts of one kind, belonging to `seats` (a seat, several, or none). |
| `CabinParts.geometries` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. |  |
| `CabinParts.named` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. |  |
| `CabinParts.solids` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. | The solids as index ranges of the merge of `geometries` and then `after` (a closed interior's door cards): one a piece. |
| `CabinParts.solids.take` | não se aplica | - — Interior de veículo, não vista de dentro dos prédios; no jogo é a cabine dos agentes (`render/agents/agent_cabin.cpp`, `agent_parts.cpp`). A Porte 2 o entregou à Porte 8 por engano de nome. |  |
| `InteriorSolid` | não se aplica | `native/src/render/agents/agent_cabin.cpp` → `void seat_shell(` — sólidos do interior de VEÍCULOS (painel, bancos, volante): a cabine dos agentes, não a vista de dentro dos prédios | One closed piece of the cabin (a box, an extruded profile, a bar), named |
| `InteriorSolidKind` | não se aplica | `native/src/render/agents/agent_cabin.cpp` → `void lining(` — sólidos do interior de VEÍCULOS (painel, bancos, volante): a cabine dos agentes, não a vista de dentro dos prédios | THE SOLIDS OF A VEHICLE'S INTERIOR, BY NAME. |

### `src/render/roomLamps.ts`

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `RoomLamps` | portado | `native/src/render/buildings/interior_lamps.cpp` → `std::vector<Vector3> interior_lamps(` — as lâmpadas do andar visto, no mundo, as mais próximas do centro do lote primeiro | The room lights of the floors cut open (the building tool's cutaway) |
| `RoomLamps.floors` | não se aplica | `native/src/render/buildings/interior_lamps.cpp` → `body.level_z(level)` — cache de altura por prédio do 4.0; o jogo soma Body::level_z na hora, sem cache |  |
| `RoomLamps.lamps` | portado | `native/src/render/buildings/interior_lamps.cpp` → `std::vector<Vector3> interior_lamps(` — as lâmpadas do andar visto, no mundo, as mais próximas do centro do lote primeiro |  |

### `src/render/uploads.ts`

**Para o jogador:** Streaming e níveis de detalhe dos prédios: invisível ao jogador quando funciona.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `reserveWarm` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/uploads.ts:424-433 function reserveWarm: the feature's own |
| `warmFull` | outro porte | render/buildings/building_renderer*, render_world, chunk_jobs (a portar lá) — Sem prova por símbolo até agora. Candidato: chunks near/mid/far, JobPool, MultiMesh em bandas, orçamentos por quadro. | src/render/uploads.ts:418-420 function warmFull: the feature's own |

### `src/ui/builder/workspace.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `initBuilderWorkspace.inside` | portado | `project/game/interior/interior_controller.gd` → `var _state: Dictionary` — a última leitura do núcleo (ligado, prédio, andar), que a barra e as luzes de cômodo seguem | src/ui/builder/workspace.ts:341-341 method initBuilderWorkspace.inside: the feature's own |
| `initBuilderWorkspace.onInside` | portado | `project/game/interior/interior_controller.gd` → `interior_revision` — o controlador relê o núcleo quando a revisão muda | src/ui/builder/workspace.ts:342-344 method initBuilderWorkspace.onInside: the feature's own |
| `initBuilderWorkspace.relabel` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/ui/builder/workspace.ts:355-358 method initBuilderWorkspace.relabel: the feature's own |
| `initBuilderWorkspace.setPresetThumbnails` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/ui/builder/workspace.ts:360-363 method initBuilderWorkspace.setPresetThumbnails: the feature's own |
| `initBuilderWorkspace.showGallery` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/ui/builder/workspace.ts:352-354 method initBuilderWorkspace.showGallery: the feature's own |
| `initBuilderWorkspace.showInside` | portado | `project/ui/interior_bar.gd` → `func show_state(` | src/ui/builder/workspace.ts:337-340 method initBuilderWorkspace.showInside: the feature's own |
| `initBuilderWorkspace.subscribe` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/ui/builder/workspace.ts:365-368 method initBuilderWorkspace.subscribe: the feature's own |
| `initBuilderWorkspace.thumbnail` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/ui/builder/workspace.ts:369-369 method initBuilderWorkspace.thumbnail: the feature's own |

### `src/ui/labels.ts`

**Para o jogador:** Escolhas das ferramentas, amostras de material e de planta, rótulos.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `nodeCountLabel` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/labels.ts:18-18 function nodeCountLabel: the feature's own |
| `roadCountLabel` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/labels.ts:17-17 function roadCountLabel: the feature's own |

### `src/ui/materialSwatch.ts`

**Para o jogador:** Escolhas das ferramentas, amostras de material e de planta, rótulos.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BASE` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |
| `CACHE` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | Drawn once per finish: the tray asks for these on every redraw. |
| `draw` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |
| `draw.line` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |
| `materialSwatch` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |
| `shade` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | A darker shade of a hex colour, for mortar, seams and shadow. |
| `shade.mix` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |

### `src/ui/overlay/builderGrid.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `drawBuilderGrid` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | DO NOT grow a second layer of gizmos here |

### `src/ui/overlay/buildingOverlay.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingOverlayInput` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The building tool's hairlines on the overlay canvas |
| `GLYPH` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `RELIEF` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The push-pull handle of a face region, in the second accent. |
| `SELECTION_FAINT` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The selected volume's roof edge: present, but quieter than the base line. |
| `drawBuildingOverlay` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `drawBuildingOverlay.ring` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `drawHandle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/ui/planSwatch.ts`

**Para o jogador:** Escolhas das ferramentas, amostras de material e de planta, rótulos.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CACHE` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | Drawn once per shape and size. |
| `OUTLINES` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | Outline points per shape, in a 0..1 box. |
| `planSwatch` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. |  |

### `src/ui/roads/catalogPanel.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `allCatalogRoads` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:33-35 function allCatalogRoads: the feature's own |
| `catalogStrip` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:65-153 function catalogStrip: the feature's own |
| `markPicked` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:156-165 function markPicked: the feature's own |
| `nameOf` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:28-28 function nameOf: the feature's own |
| `pickRoad` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:46-50 function pickRoad: the feature's own |
| `pickedRoad` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:38-43 function pickedRoad: the feature's own |
| `sameRoad` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/catalogPanel.ts:29-30 function sameRoad: the feature's own |

### `src/ui/roads/cutWalls.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `setCutWalls` | não se aplica | - — Corte de ESTRADA (talude ou muro), não o corte de prédio; é da fila Vias (elevação). | src/ui/roads/cutWalls.ts:13-17 function setCutWalls: the feature's own |

### `src/ui/roads/furnitureChoice.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `setFurnitureChosen` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/furnitureChoice.ts:18-22 function setFurnitureChosen: the feature's own |

### `src/ui/roads/keys.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `bindableKey` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/keys.ts:63-65 function bindableKey: the feature's own |
| `resetRoadKeys` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/keys.ts:80-83 function resetRoadKeys: the feature's own |
| `roadKey` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/keys.ts:58-60 function roadKey: the feature's own |
| `save` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/keys.ts:53-56 function save: the feature's own |
| `setRoadKey` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/keys.ts:68-78 function setRoadKey: the feature's own |

### `src/ui/roads/keysPanel.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `closeRoadKeys` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `open` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | THE ROAD TOOL'S KEYS, in a small panel beside the tool's options  The open panel, and what lets go of its key listener when it closes (MDN,  |
| `openRoadKeys` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `openRoadKeys.onKey` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | While a chip waits, the next key is its, before the game sees it. |
| `openRoadKeys.render` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `roadKeysOpen` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |

### `src/ui/roads/money.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `balanceTip` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/money.ts:16-18 function balanceTip: the feature's own |

### `src/ui/roads/profileEditor.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `profileEditorOpen` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/ui/roads/profileEditor.ts:158-158 function profileEditorOpen: the feature's own |

### `src/ui/roads/profileSwatch.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `ASPHALT` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `BUS` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `CARS` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `CYCLE` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `FOOTWAY` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `GRASS` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `KERB` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `MEDIAN_GRASS` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `WHITE` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `YELLOW` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `profileSwatch` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |

### `src/ui/toolChoices.ts`

**Para o jogador:** Escolhas das ferramentas, amostras de material e de planta, rótulos.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `keep` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:19-25 function keep: the feature's own |
| `setCloudMode` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:131-133 function setCloudMode: the feature's own |
| `setElementKind` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:120-122 function setElementKind: the feature's own |
| `setElementMode` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:112-112 function setElementMode: the feature's own |
| `setFogErase` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:70-72 function setFogErase: the feature's own |
| `setGullyErase` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:98-100 function setGullyErase: the feature's own |
| `setPaintKind` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:190-193 function setPaintKind: the feature's own |
| `setPoleLampMode` | outro porte | Mobiliário da rua (postes) — Poste de rua, não lâmpada de cômodo. | src/ui/toolChoices.ts:46-49 function setPoleLampMode: the feature's own |
| `setPoleToolMode` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:61-63 function setPoleToolMode: the feature's own |
| `setRoadGridShown` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:227-231 function setRoadGridShown: the feature's own |
| `setRoadWidth` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:203-205 function setRoadWidth: the feature's own |
| `setStreetscapeKind` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:34-37 function setStreetscapeKind: the feature's own |
| `setTreeKind` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:86-86 function setTreeKind: the feature's own |
| `setTreeMode` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:84-84 function setTreeMode: the feature's own |
| `setZoneColoursShown` | outro porte | Interface e Construir — Entra por vizinhança do HUD; nada da vista de dentro. | src/ui/toolChoices.ts:220-220 function setZoneColoursShown: the feature's own |

### `src/ui/v2/shell.ts`

**Para o jogador:** O HUD do 4.0: barra de cima, camadas, simulação, câmera, ajuda e menu.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BIOME_SWATCHES` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The biomes, painted or chosen for the whole map (`world/ecology.ts`). |
| `Category` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `ICON` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `PAINT_SWATCH` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The colour of each paintable ground, for its button. |
| `SWATCHES` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `ShellDeps` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `ShellGame` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `ShellGameValues` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | What the shell needs from the rest of the game.  The game's state as the shell reads it (`main.ts` `gameState`) |
| `ShellHandle` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | What the game asks of the shell. |
| `ShellReadouts` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The map's own numbers the old status bar showed (`main.ts`): its roads and nodes, the zoom |
| `ShellRoadClass` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A road class, as the upgrade tool shows it. |
| `ShellSlider` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A number the player sets with a slider: its range, and how the game reads and takes it. |
| `ShellStatus` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The city's numbers, as `main.ts` computes them for the top bar and the simulation menu. |
| `ShellTransitTool` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `ShellZonePatch` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | What a lot or zone setting is changed to (`main.ts` `setZone`). |
| `TransitToolKind` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | What the public transport tool is set to and does, as the panel shows it (`editor/transitTools.ts`). |
| `button` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `el` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `hexOf` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.BARRIER_ICON` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.addToBar` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | Puts a control of the game's own in the top bar, before Help (F9's button, `ui/healthPanel.ts`). |
| `mountShell.ask` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.card` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A thing: its picture, its name. |
| `mountShell.cardRows` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The rows of cards a catalog draws in the strip (`roads/catalogPanel.ts`). |
| `mountShell.categoryOf` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.choices` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.closeDropdown` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.closePop` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.cutShort` | não se aplica | `project/ui/interior_bar.gd` → `func show_state(` — detecta texto cortado com reticências no DOM para dar a dica inteira; o tema do jogo não corta | Whether a control's own text is cut short (an ellipsis): its tip then says it whole. |
| `mountShell.dockName` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.dropdown` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.elementRows` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The element brush's rows: lay or erase, its settings for the kind in hand, and clearing them all. |
| `mountShell.fadeAll` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.fadeEdges` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A row that scrolls sideways - the strip, a catalog's row of cards - fades out at the side with more in it |
| `mountShell.fillPop` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.fitDrawer` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The asset panel stands centred in the room between the tool options (left) and the selection (right), never over either |
| `mountShell.group` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A cluster of controls in the row; its name is the tooltip, not a heading. |
| `mountShell.isEffect` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.keyList` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.landTabOf` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.later` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.mapAction` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | An action on the whole map, asking first: clearTerrain, clearGullies, clearFog, clearTrees, clearElements, scatterClouds, clearClouds. |
| `mountShell.nameShown` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | Whether a control's name is on screen: an icon's button keeps its name in a span clipped to nothing |
| `mountShell.namedRow` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A row whose short name stands at the left and whose whole name is its tooltip and its accessible name. |
| `mountShell.note` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | What a tool does: in the tooltip of its drawer, never as a paragraph. |
| `mountShell.numberText` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.openLandTab` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.openTool` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | Opens a tool's panel with that tool in hand (the road inspector's eyedropper). |
| `mountShell.orow` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | One row of the tool options: a short name at the left, its controls at the right - the name beside what it names |
| `mountShell.picture` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | ------------------------------------------------------------ construction |
| `mountShell.popBody` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.range` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.range.show` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.readoutValue` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.readyOf` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.render` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderBuild` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderLandscape` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderLandscape.brushRows` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderLandscape.run` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderLandscape.terrainCard` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderModels` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderModels.fill` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderRoads` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | ------------------------------------------------------------ roads |
| `mountShell.renderSection` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.renderSimple` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | ------------------------------------------------------------ simple tools |
| `mountShell.renderTransit` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | ------------------------------------------------------------ public transport |
| `mountShell.renderZones` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.row` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A catalog's row of cards spans the panel's flexible track, which gives the panel no width of its own |
| `mountShell.section` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.seeInside` | portado | `project/ui/hud_popover.gd` → `func _interior_row(` |  |
| `mountShell.seg` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A choice among a few, as a row of segments (all in view); the menu redrawn with the new one lit. |
| `mountShell.showStatus` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The HUD: the city's numbers as the game computes them |
| `mountShell.sigOf` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.stepper` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A stepper: less, the value, more. |
| `mountShell.stopReadouts` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.subhead` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A divider naming what follows: the settings of the whole map, after the brush's. |
| `mountShell.swatchArt` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncHistory` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncImpossible` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncInside` | portado | `project/ui/interior_bar.gd` → `func show_state(` |  |
| `mountShell.syncMetrics` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncMoney` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncRanges` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | Shows every slider on screen as the game has its number now (not the one being dragged). |
| `mountShell.syncReadouts` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncSide` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncSky` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.syncSpeed` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.tab` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | ------------------------------------------------------------ helpers A mode of the tool: its icon, its name in the tooltip. |
| `mountShell.terrainModeNow` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.textInput` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A text box in the options; the keys typed stay in it (the game's shortcuts do not see them). |
| `mountShell.tipCheck` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A control rebuilt or removed under the pointer takes its tooltip with it. |
| `mountShell.toggle` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.tool` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.toolButton` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.toolButton.run` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.toolKeyed` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A tool's key was pressed (`editor/gameKeys.ts`), after the tool machine took it or put it down |
| `mountShell.toolKeys` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | Each tool's key and its name, one row each (not "1 Z H T O B I" on one key cap). |
| `mountShell.updateHint` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.useBarrier` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.useElement` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.useTerrain` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | The terrain tool in hand (picked only when it is not: picked again it is put away), then its mode. |
| `mountShell.useTool` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |
| `mountShell.verb` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. | A verb: an icon and its name, in a row. |
| `svg` | outro porte | Interface (HUD, camadas, simulação, câmera, ajuda, menu) — A linha da camada Interior e a barra do andar são minhas e têm prova (seeInside, syncInside); o resto do HUD é da fila Interface. |  |

### `src/ui/v2/transitLine.ts`

**Para o jogador:** A linha de transporte no HUD.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `transitLineRow` | outro porte | Transporte (linhas) — Vizinhança do HUD. | DO NOT build this row with innerHTML: a name `<img src=x onerror=…>` ran its script when the panel opened. |

### `src/view/viewport.ts`

**Para o jogador:** Montagem da interface e laboratório do navegador.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `flatViewport.rotate` | não se aplica | project/ui, GameFlow — Interface é da funcionalidade interface; laboratório é ferramenta do 4.0. | src/view/viewport.ts:76-78 method flatViewport.rotate: the feature's own |

### `src/world/buildings/blueprints.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `blueprintByKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/blueprints.ts:321-323 function blueprintByKey: the feature's own |
| `bodyOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/blueprints.ts:326-330 function bodyOf: the feature's own |

### `src/world/buildings/cityBuildings.ts`

**Para o jogador:** Prédios-assinatura e torres de forma própria (igreja, hospital, escola, prefeitura, torres torcidas).

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `cityBuilding` | outro porte | sim/buildings/b40/civic*, render/buildings/signature (a portar lá) — Sem prova por símbolo até agora. Cívicos básicos existem; assinaturas ainda não. | src/world/buildings/cityBuildings.ts:339-341 function cityBuilding: the feature's own |

### `src/world/buildings/elements.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BayRef` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/elements.ts:272-277 interface BayRef: the feature's own |
| `elementAt` | já existia | `native/src/sim/commands/body_parts.cpp` → `std::string op_add_part_at(` (porte build) | src/world/buildings/elements.ts:423-428 function elementAt: the feature's own |
| `elementsAgainstBay` | já existia | `native/src/sim/commands/body_parts.cpp` → `std::string op_add_part(` (porte build) | src/world/buildings/elements.ts:282-293 function elementsAgainstBay: the feature's own |
| `elementsAgainstBayAxis` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/elements.ts:297-420 function elementsAgainstBayAxis: the feature's own |
| `elementsAgainstBayAxis.at` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A point on the face line, `out` in front of it and `a` along it. |
| `runFor` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/elements.ts:139-142 function runFor: the feature's own |
| `snap` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/elements.ts:279-279 function snap: the feature's own |
| `takeElementId` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | src/world/buildings/elements.ts:262-267 function takeElementId: the feature's own |

### `src/world/buildings/facadePatterns.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FacadeTarget` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. |  |
| `USE` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. |  |
| `applyFacadePattern` | já existia | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_pattern(` (porte build) | A coherent facade is one command; individual bays remain fully editable. |
| `updateFacadeGeometry` | já existia | `native/src/sim/commands/body_facade.cpp` → `std::string op_set_face(` (porte build) | A face's proportions are authored data; reducing its bay count resamples edits. |

### `src/world/buildings/footprints.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `supportedBy` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/footprints.ts:138-142 function supportedBy: the feature's own |

### `src/world/buildings/geometry.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `bayCentreLocal` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:498-502 function bayCentreLocal: the feature's own |
| `buildingHeight` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:798-802 function buildingHeight: the feature's own |
| `footprintCentre` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:199-202 function footprintCentre: the feature's own |
| `planOverlap` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:289-291 function planOverlap: the feature's own |
| `reliefAt` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:433-441 function reliefAt: the feature's own |
| `worldToLocalAt` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/geometry.ts:68-70 function worldToLocalAt: the feature's own |

### `src/world/buildings/handles.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CORNER_OUT` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far the move, turn and scale handles stand off the footprint's corner. |
| `HANDLE_OUT` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `Handle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `HandleKind` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Where the edit handles of a selected building stand, in world 3D. |
| `HandleRegion` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A rectangle of bays and storeys of one face (see `editor/buildings.ts`, `FaceRegion`). |
| `LIFT_OUT` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How far the lift handle stands off its block's corner (the scale handle's 0.8 m). |
| `buildingHandles` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | `nearest` orders the four footprint corners |
| `buildingHandles.footOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `liftNear` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The lift of the ground block nearest a corner of the footprint's box (local frame) |

### `src/world/buildings/interior.ts`

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `FLIGHT_RISE` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | The highest a riser may be, and the least and the most a tread is deep (the 17 cm and 28-30 cm of a house's stair). |
| `FLIGHT_TREAD` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. |  |
| `FLIGHT_TREAD_MIN` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. |  |
| `FLIGHT_WIDTH` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | A flight's width: a house's stair, less where the blocks meet along a shorter line. |
| `FURNITURE_KINDS` | já existia | `native/src/sim/buildings/interior/tables.h` → `enum FurnitureKind` (porte build) |  |
| `FURNITURE_SIZE` | já existia | `project/data/render/interiors.json` → `"sizes"` (porte build) | Sizes in metres: across, deep, high. |
| `FloorInterior` | já existia | `native/src/sim/buildings/interior/interior.h` → `struct FloorInterior` (porte build) |  |
| `Furnished` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `types_;` (porte build) — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | What a building's floors are furnished from besides its blocks (`furnishFloor`, `deriveSpaces`, `flatPlan`, `levelFlights`), its floors furn |
| `Furniture` | já existia | `native/src/sim/buildings/interior/interior.h` → `struct Furniture` (porte build) |  |
| `FurnitureKind` | já existia | `native/src/sim/buildings/interior/tables.h` → `enum FurnitureKind` (porte build) |  |
| `ITEM_CELL` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `ITEM_CELL` (porte build) | The side of the cells the pieces of a floor are filed by (`Plan.free`). |
| `LAMP_KINDS` | portado | `native/src/render/buildings/interior_lamps.cpp` → `FK_CEILING_LAMP` | The pieces that give light: the room lights are where these are. |
| `LevelFlight` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | A flight of stairs between two blocks of a building standing at floors of their own (a split level, `Volume.lift`) |
| `NONE_FILED` | não se aplica | `native/src/sim/buildings/interior/plan.cpp` → `filed_.find` (porte build) — evitava alocação em JS; não existe em C++ | A piece's half extents across the floor, with the gap kept round it (`Plan.free`).  A cell no piece is filed in (`Plan.free`): DO NOT make a |
| `Partition` | já existia | `native/src/sim/buildings/interior/interior.h` → `struct Partition` (porte build) | A thin wall between rooms, from (x0,y0) to (x1,y1). |
| `Plan` | já existia | `native/src/sim/buildings/interior/plan.h` → `class Plan` (porte build) | One block's floor, being furnished: where things may stand, and what stands. |
| `Plan.alongBack` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::along_back(` (porte build) | Pieces along the back wall (or the left/right), facing into the room. |
| `Plan.alongSide` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::along_side(` (porte build) |  |
| `Plan.constructor` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::Plan(` (porte build) |  |
| `Plan.cores` | já existia | `native/src/sim/buildings/interior/plan.h` → `cores_;` (porte build) | Index loops: DO NOT walk these with an iterator, one made for each piece tried (`free` is asked for every place a piece is tried). |
| `Plan.corner` | não se aplica | `native/src/sim/buildings/interior/plan.cpp` → `bool Plan::inside` (porte build) — evitava alocação em JS; não existe em C++ |  |
| `Plan.d` | já existia | `native/src/sim/buildings/interior/plan.h` → `float depth()` (porte build) |  |
| `Plan.filed` | já existia | `native/src/sim/buildings/interior/plan.h` → `filed_;` (porte build) |  |
| `Plan.free` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::free(` (porte build) | Whether a piece of this size fits at (x, y): inside the walls, clear of the cores and what stands. |
| `Plan.grid` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::grid(` (porte build) | A grid of `kind` filling a rectangle, `gx` x `gy` apart, all facing `angle`. |
| `Plan.items` | já existia | `native/src/sim/buildings/interior/plan.h` → `items;` (porte build) |  |
| `Plan.put` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::put(` (porte build) | Living room: the TV on the side wall, the sofa facing it, a table by the window, the kitchen along the corridor wall. |
| `Plan.ring` | já existia | `native/src/sim/buildings/interior/plan.h` → `ring_;` (porte build) |  |
| `Plan.w` | já existia | `native/src/sim/buildings/interior/plan.h` → `float width()` (porte build) |  |
| `Plan.wall` | já existia | `native/src/sim/buildings/interior/plan.h` → `void wall(float ax` (porte build) | A wall from (u0, s0) to (u1, s1). |
| `Plan.wallAcross` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::wall_across(` (porte build) | A wall across x at `y`, with a doorway at `door` (local x). |
| `Plan.wallAlong` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `Plan::wall_along(` (porte build) |  |
| `Plan.walls` | já existia | `native/src/sim/buildings/interior/plan.h` → `walls;` (porte build) |  |
| `Plan.x0` | já existia | `native/src/sim/buildings/interior/plan.h` → `x0 = 0` (porte build) |  |
| `Plan.x1` | já existia | `native/src/sim/buildings/interior/plan.h` → `x1 = 0` (porte build) |  |
| `Plan.y0` | já existia | `native/src/sim/buildings/interior/plan.h` → `y0 = 0` (porte build) |  |
| `Plan.y1` | já existia | `native/src/sim/buildings/interior/plan.h` → `y1 = 0` (porte build) |  |
| `RIGHT` | já existia | `native/src/sim/buildings/interior/plan.h` → `RIGHT_ANGLE` (porte build) |  |
| `RoomCell` | já existia | `native/src/sim/buildings/interior/plan.h` → `struct RoomCell` (porte build) | One room off a corridor, as `roomsFloor` hands it to be furnished. |
| `backRoom` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `float back_room` (porte build) | A room across the back of the floor, `depth` deep, behind a wall with a door near its end: a stockroom, a kitchen. |
| `cells` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void cells` (porte build) |  |
| `classRoom` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void class_room` (porte build) | A classroom: the board on the side wall, desks and chairs facing it, the teacher's desk. |
| `classrooms` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void classrooms` (porte build) |  |
| `cutOpen` | portado | `native/src/sim/buildings/interior/cut.cpp` → `Body cut_open(const Body &b, int level, Point2 view)` — volumes acima do andar saem, pavimentos truncados, entrada refeita; função pura do corpo, em sim | The building as drawn cut open at `level` |
| `defaultFunction` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `purpose_of` (porte build) |  |
| `deskRows` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void desk_rows` (porte build) | Rows of desks facing the windows, from `s0` in to `s1` (the corridor side by default). |
| `dining` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void dining` (porte build) |  |
| `face` | já existia | `native/src/sim/buildings/interior/plan.h` → `inline float face` (porte build) | The way a piece faces, as a direction in the floor's own axes (+y is the back). |
| `flatFloor` | já existia | `native/src/sim/buildings/interior/interior_homes.cpp` → `bool flat_floor` (porte build) | A floor of flats, from its plan (`spaces.ts`) |
| `flightBox` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | A flight's footprint, local, a hand's breadth round it: [x0, y0, x1, y1]. |
| `floorType` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `Interior::type_of` (porte build) | DO NOT furnish every storey of a tower: a floor is furnished from what it reads of its level alone - ground or not, its height, the cores th |
| `furnish` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `void furnish(Plan` (porte build) | Furnishes one block's floor according to what the building is for. |
| `furnishFloor` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `Interior::at` (porte build) |  |
| `furnishedFor` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `types_;` (porte build) — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | DO NOT key them by the record (each emission's body is a new one on the same blocks, `emitChunk`) nor cap them: kept on the blocks list, let |
| `furnishedOf` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `levels_;` (porte build) — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | Each blocks list's furnished floors, one set per rest of the building it was asked with. |
| `furnishingOf` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | A floor's furniture as stored pieces: what an edit starts from. |
| `homeUnits` | já existia | `native/src/sim/buildings/interior/interior_homes.cpp` → `void home_units` (porte build) | Homes (or hotel rooms) along the floor, each a few rooms with their furniture. |
| `hotelRoom` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void hotel_room` (porte build) | A hotel room: the bed, a desk by the window, a wardrobe, a bathroom by the corridor. |
| `houseFloor` | já existia | `native/src/sim/buildings/interior/interior_homes.cpp` → `void house_floor` (porte build) | A floor of a house: walls on the bays' boundaries, so no wall cuts a window. |
| `houseFloor.stairs` | já existia | `native/src/sim/buildings/interior/interior_homes.cpp` → `FK_STAIRWELL` (porte build) | A flight to the floor above (its foot at the front), and the opening it arrives through. |
| `interiorAt` | já existia | `native/src/sim/buildings/interior/interior.h` → `const FloorInterior &at(int level)` (porte build) | One floor of a building, inside: furnished once for its blocks, the rest it reads and its type (the records sharing them share it; a buildin |
| `isFurnitureKind` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. |  |
| `itemCell` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `cell_key(` (porte build) | A cell's key, a small integer (cells within 32 km of the block's frame): DO NOT key past 2^30, every key a heap number made for each lookup. |
| `kitchen` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void kitchen` (porte build) | A kitchen behind `wall`: cooking along the back, a prep table. |
| `letGoInterior` | não se aplica | `native/src/sim/buildings/interior/interior.h` → `class Interior` (porte build) — cache do 4.0 sobre listas de blocos; no jogo o cache é o objeto Interior por corpo | A blocks list's furnished floors let go now (`buildingMesh.ts` `letGoEmitted`: a job's building emitted, its furniture not read again by the |
| `levelFlights` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | The flights between the blocks that have a storey on `level` at different heights and stand side by side |
| `lobby` | já existia | `native/src/sim/buildings/interior/interior_homes.cpp` → `void lobby` (porte build) |  |
| `officeRoom` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void office_room` (porte build) | An office room: rows of desks from the windows back, chairs behind them, a shelf on the side wall |
| `placed` | outro porte | sim/buildings/interior (a portar lá) — Sem prova por símbolo até agora. Alimenta o cômodo atrás do vidro. | A placed piece at its kind's size. |
| `roomLights` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `void room_lights` (porte build) | A ceiling light over each group of furniture: a room's worth. |
| `roomsFloor` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `bool rooms_floor` (porte build) | A floor as rooms off a corridor, like the flats (`spaces.flatPlan`) |
| `roomsFloor.P` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `RoomCell::put` (porte build) |  |
| `roomsFloor.wall` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `r.wall(0, D` (porte build) | A wall from (u0, s0) to (u1, s1). |
| `shopsAround` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void shops_around` (porte build) |  |
| `wardRoom` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void ward_room` (porte build) | A ward: beds along the side walls, heads to the wall, a chair by each. |
| `wards` | já existia | `native/src/sim/buildings/interior/interior_rooms.cpp` → `void wards` (porte build) |  |

### `src/world/buildings/items.ts`

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingItems.tableRef` | outro porte | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal (a portar lá) — Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas. | src/world/buildings/items.ts:62-67 method BuildingItems.tableRef: the feature's own |

### `src/world/buildings/lotLibrary.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LOT_NAMES` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The names of the lots, in a stable order. |
| `loadLot` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Reads one lot, or null when there is none of that name. |
| `loaded` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The lots read so far, by name (`loadedLot`). |
| `loadedLot` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A lot already read (`loadLot`), or null: for what must answer at once (a picture of it). |
| `loaders` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The lots made in the lot lab (`lots/*.json`, 2026-10-05): each a building of the tower kit with its whole lot |
| `lotKey` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | The key a lot goes by in the builder's models. |
| `nameOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |

### `src/world/buildings/lotTemplate.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `LotTemplate` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | A lot kept in the lot lab's library (`?lab=lots`, `editor/lotLab.ts`, `lot-library-plugin.ts`) |

### `src/world/buildings/materials.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `MaterialTarget` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/materials.ts:118-122 type MaterialTarget: the feature's own |
| `applyMaterial` | já existia | `native/src/sim/commands/body_finish.cpp` → `std::string op_paint(` (porte build) | src/world/buildings/materials.ts:152-223 function applyMaterial: the feature's own |
| `applyMaterial.tidyVolume` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. |  |
| `applyStyle` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/materials.ts:284-296 function applyStyle: the feature's own |
| `applyStyle.state` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | The storeys' own paint too (`wallMaterial` reads it first) |
| `materialAt` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/materials.ts:125-149 function materialAt: the feature's own |

### `src/world/buildings/pick.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingHit` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `OPEN_PLATE` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How thick an open block's plate is picked: its kerb and its edge, under its surface. |
| `Ray3` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A ray, in WORLD axes: x and y on the map, z up. |
| `heightOf` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | How high a building reaches over its ground floor: its tallest volume with its roof, and the parts standing on it |
| `pickBuilding` | já existia | `native/src/sim/commands/body_pick.cpp` → `BodyHit pick_body(` (porte build) | The nearest building face under a ray. |
| `rayNearBox` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. |  |
| `slab` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | Ray against an axis-aligned box: entry distance and the axis entered through. |

### `src/world/buildings/reference.ts`

**Para o jogador:** O editor de prédios do Construir: modelos, massa, fachada, peças, telhado, interior, cores.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Point3` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A point of the model, metres: x and y on the plan, z up. |
| `ReferenceSampler` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | A colour the model shows at a point of its surface, seen along the normal, or null (nothing there). |
| `Triangle` | outro porte | sim/commands/building_edit, project/game/tools/building_tool.gd, project/ui (a portar lá) — Sem prova por símbolo até agora. Abas hoje 'em construção'; exige corpo editável por prédio. | One triangle of the model's surface. |

### `src/world/buildings/spaces.ts`

**Para o jogador:** Plantas e móveis de cada andar pelo uso do prédio; é o que se vê pela janela e no corte.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `CORRIDOR` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `CORRIDOR = 1.6f` (porte build) | Corridor width, and the depth from which a floor has homes on both sides. |
| `DOUBLE_LOADED` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `DOUBLE_LOADED = 11.f` (porte build) |  |
| `FlatCell` | já existia | `native/src/sim/buildings/interior/plan.h` → `struct FlatCell` (porte build) | One home on a residential floor, and how it lies. |
| `FlatPlan` | já existia | `native/src/sim/buildings/interior/plan.h` → `struct FlatPlan` (porte build) |  |
| `KIND` | não se aplica | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` (porte build) — cache e tipo de espaço do 4.0; o jogo guarda só o retângulo do espaço |  |
| `StoreySpaces` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `floor->spaces` (porte build) | Extension point for interiors and occupants. |
| `deriveSpaces` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` (porte build) | A building's spaces, worked out once for its blocks list, use and rhythm: DO NOT key them by the record (every emission's body is a new reco |
| `flatPlan` | já existia | `native/src/sim/buildings/interior/plan.cpp` → `FlatPlan flat_plan(` (porte build) | A residential floor of one block as homes: a corridor down its long axis |
| `spacesOf` | não se aplica | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` (porte build) — cache e tipo de espaço do 4.0; o jogo guarda só o retângulo do espaço | Each blocks list's spaces, one per use and rhythm it was asked with (`workOutSpaces` reads the blocks, the use, the module and the storey he |
| `workOutSpaces` | já existia | `native/src/sim/buildings/interior/interior.cpp` → `void spaces_of(` (porte build) |  |

### `src/world/buildings/splitLevel.ts`

**Para o jogador:** Prédio em encosta: platô, degraus, meio-nível, garagem no subsolo; portas e portões.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `liftStep` | outro porte | sim/buildings/foundation, render/buildings/facade_emit (a portar lá) — Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam. | src/world/buildings/splitLevel.ts:243-243 function liftStep: the feature's own |
| `opSetLift` | outro porte | sim/buildings/foundation, render/buildings/facade_emit (a portar lá) — Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam. | src/world/buildings/splitLevel.ts:253-280 function opSetLift: the feature's own |
| `snapLift` | outro porte | sim/buildings/foundation, render/buildings/facade_emit (a portar lá) — Sem prova por símbolo até agora. Parte existe (plinto e degraus); platô e meio-nível faltam. | src/world/buildings/splitLevel.ts:246-250 function snapLift: the feature's own |

### `src/world/buildings/store.ts`

**Para o jogador:** Onde os prédios ficam guardados, salvar, desfazer.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `BuildingStore.tableRef` | outro porte | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal (a portar lá) — Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas. | src/world/buildings/store.ts:104-107 method BuildingStore.tableRef: the feature's own |
| `BuildingStore.wearRevision` | outro porte | sim/buildings/building_store, sim/persistence, sim/commands/edit_journal (a portar lá) — Sem prova por símbolo até agora. O registro plano + semente do jogo cumpre o papel da tabela em colunas. |  |

### `src/world/buildings/types.ts`

**Para o jogador:** A gramática dos prédios: estilos, massing, fachadas, paletas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `cloneBuilding` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/types.ts:368-370 function cloneBuilding: the feature's own |
| `jsonCopy` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/types.ts:373-395 function jsonCopy: the feature's own |
| `volumeById` | outro porte | sim/buildings/b40/* (a portar lá) — Sem prova por símbolo até agora. Gerador b40 lê as mesmas tabelas. | src/world/buildings/types.ts:449-451 function volumeById: the feature's own |

### `src/world/roads/catalog.ts`

**Para o jogador:** Catálogo de vias, teclas e painéis do desenho de estradas.

| Declaração | Estado | Prova ou dono | O que faz (comentário do 4.0) |
|---|---|---|---|
| `Lane` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:18-18 type Lane: the feature's own |
| `Shape` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:19-38 interface Shape: the feature's own |
| `catalogRoads` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:69-109 function catalogRoads: the feature's own |
| `catalogRoads.cls` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | Streets |
| `road` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:65-66 function road: the feature's own |
| `shapeProfile` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:41-63 function shapeProfile: the feature's own |
| `shapeProfile.kerb` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `shapeProfile.lane` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. |  |
| `type` | outro porte | Vias (estradas) e Interface — Entra no rastreamento só por vizinhança do HUD; nada da vista de dentro. | src/world/roads/catalog.ts:16-16 function type: the feature's own |

## Pedaços de arquivos grandes (`slices`) da vista de dentro

| Pedaço | Estado | Prova |
|---|---|---|
| `src/render/buildings/buildingMesh.ts#FurnitureSink` (linhas 478-478) | não se aplica | `native/src/render/buildings/furniture_emit.cpp` → `void emit_furniture(` — o 4.0 junta as poses por tipo para instanciar; o jogo escreve as peças do móvel no casco e como caixas instanciadas |
| `src/render/buildings/buildingMesh.ts#emitInterior` (linhas 1371-1455) | portado | `native/src/render/buildings/interior_emit.cpp` → `void emit_interior(Shell &shell, Shell &parts, const Placed &pl)` — piso, núcleos (poço do elevador), divisórias e móveis do andar aberto |
| `src/render/buildings/buildingMesh.ts#furnitureRows` (linhas 482-482) | não se aplica | `native/src/render/buildings/furniture_emit.cpp` → `void emit_furniture(` — o 4.0 junta as poses por tipo para instanciar; o jogo escreve as peças do móvel no casco e como caixas instanciadas |
| `src/render/buildings/buildingMesh.ts#interiorFurniture` (linhas 4175-4196) | portado | `native/src/render/buildings/furniture_emit.cpp` → `void emit_furniture(` |
| `src/render/buildings/buildingMesh.ts#placeFurniture` (linhas 1458-1463) | portado | `native/src/render/buildings/furniture_emit.cpp` → `void emit_furniture(` |
| `src/render/buildings/cellArrays.ts#furniturePoses` (linhas 485-495) | não se aplica | `native/src/render/buildings/furniture_emit.cpp` → `void emit_furniture(` — as poses por célula do 4.0; o jogo não as guarda |
| `src/render/buildings/furnitureKit.ts#build` (linhas 107-336) | portado | `native/src/render/buildings/furniture_look.h` → `struct FurniturePart` — cada móvel é uma lista de peças (caixa, cilindro, bola) vinda de data/render/furniture.json, não código |
| `src/render/buildings/furnitureKit.ts#createFurnitureGeometries` (linhas 361-384) | portado | `native/src/render/buildings/furniture_look.h` → `struct InteriorLook` — cada móvel é uma lista de peças (caixa, cilindro, bola) vinda de data/render/furniture.json, não código |
| `src/render/buildings/furnitureKit.ts#furnitureTones` (linhas 442-462) | portado | `project/data/render/furniture.json` → `"palette"` |
