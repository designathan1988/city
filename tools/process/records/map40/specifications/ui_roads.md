# ui:roads

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/roads/catalogPanel.ts`, `src/ui/roads/connectorPanel.ts`, `src/ui/roads/crossSection.ts`, `src/ui/roads/cutWalls.ts`, `src/ui/roads/drawProfile.ts`, `src/ui/roads/furnitureChoice.ts`, `src/ui/roads/junctionPanel.ts`, `src/ui/roads/keys.ts`, `src/ui/roads/keysPanel.ts`, `src/ui/roads/money.ts`, `src/ui/roads/profileEditor.ts`, `src/ui/roads/profilePanel.ts`, `src/ui/roads/profileSwatch.ts`, `src/ui/roads/templates.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 2 arquivo(s), 13 linhas
- Alcance total: 449 unidades; usa 300 compartilhadas de outras; é dona principal de 145 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `catalog.cat.…`, `catalog.customise`, `catalog.meta`, `catalog.tip`, `compass.…`, `connectors.auto`, `connectors.custom`, `connectors.lastExit`, `connectors.none`, `connectors.pickIn`, `connectors.pickOut`, `connectors.reset`, `connectors.title`, `economy.balance`, `economy.money`, `economy.refund`, `inspector.heightNode`, `junction.auto.…`, `junction.autoChoice`, `junction.busPriority`, `junction.control`, `junction.control.…`, `junction.earlier`, `junction.from`, `junction.greenWave`, `junction.greenWaveNone`, `junction.later`, `junction.legs`, `junction.locked`, `junction.longer`, `junction.lower`, `junction.mainRoad`, `junction.mode.…`, `junction.movements`, `junction.offset`, `junction.perHour`, `junction.raise`, `junction.rule.…`, `junction.rulesReset`, `junction.shorter`, `junction.signal`, `junction.signalMode`, `junction.stage`, `junction.unlock`, `profile.copied`, `profile.diagram`, `profile.drawWith`, `profile.drawWithTip`, `profile.element.lane`, `profile.element.…`, `profile.material.carriageway`, `profile.materialName.…`, `profile.notRemoved`, `profile.notSaved`, `profile.notStored`, `profile.problem.…`, `profile.templateName`, `profile.title`, `profile.toStreet`, `profile.toStreetTip` … (+56)
- Eventos de entrada: `ouve:click`, `ouve:focusin`, `ouve:focusout`, `ouve:keydown`, `ouve:pointercancel`, `ouve:pointerdown`, `ouve:pointermove`, `ouve:pointerup`, `ouve:resize`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#gameKeys@2479` (1), `src/main.ts#ExpressionStatement@159` (1), `src/main.ts#drawOverlayScreen@2942` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `recebe:cycle`, `recebe:footway`, `recebe:lane`, `recebe:median`, `recebe:parking`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:buildings` (1), `ui:i18n` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadNode.approachRules`, `RoadNode.blockedMovements`, `RoadNode.control`, `RoadNode.heightOffset`, `RoadNode.incident`, `RoadNode.signal`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.type`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/connectorPanel.spec.ts`, `tests/ui/money.spec.ts`, `tests/ui/roadCatalogPanel.spec.ts`, `tests/ui/roadKeys.spec.ts`, `tests/ui/roadKeysCamera.spec.ts`, `tests/ui/roadProfileEditor.spec.ts`, `tests/ui/roadTemplates.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `economy`: já existia 1, portado 2
- porte `interface`: outra sessão 153
- porte `interior`: outro porte 21, não se aplica 1
- porte `roads`: em parte 11, não portado 196
- porte `street-furniture`: portado 6
- porte `traffic`: portado 1, de outra funcionalidade 1

## Código próprio (linhas — declaração — comentário)

### src/ui/roads/keys.ts
- 86-89 `roadKeyAction` (function): The road action a key press asks for, or null.
- 105-108 `roadKeyParams` (function): The keys as placeholders for the interface's sentences (`{heightUp}`, `{heightDown}`, `{alignmentKey}`).
- 109-110 `paramsOf` (variable)

### src/ui/roads/money.ts
- 11-13 `formatCost` (function): What a road in hand costs, for the preview: its price, or what a demolition-like edit gives back.
