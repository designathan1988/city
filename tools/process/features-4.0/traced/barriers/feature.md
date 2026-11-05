# Feature: src/editor/barriers.ts, src/render/barriers.ts, src/world/barriers.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 56 declarations
- Slices of shared code: 40
- Code: 741 lines in 11 files (source/)
- Resources: 152 (22 entry, 130 style, 0 import; 22 only the feature uses)
- Uses: 54 shared declarations (interface/: 90 declarations in 22 files, the types they name included); hooks: 25; external APIs: 42

## Code

- src/editor/barriers.ts — whole, 175 lines
- src/editor/bulldozer.ts — part, 7 lines
- src/editor/chunkedSave.ts — part, 6 lines
- src/main.ts — part, 68 lines
- src/render/barriers.ts — whole, 258 lines
- src/render/flatAtlas.ts — part, 3 lines
- src/render/renderer.ts — part, 52 lines
- src/ui/v2/shell.ts — part, 11 lines
- src/world/barriers.ts — whole, 28 lines
- src/world/doc.ts — part, 113 lines
- src/world/solids.ts — part, 20 lines

## Slices of shared code

- `src/editor/bulldozer.ts#Bulldozer.boxed` — 7 of 25 lines
- `src/editor/chunkedSave.ts#KEYED` — 6 of 35 lines
- `src/main.ts#<IfStatement>@4036` — 9 of 196 lines
- `src/main.ts#barrierTool` — 14 of 14 lines
- `src/main.ts#cancelGestures` — 3 of 13 lines
- `src/main.ts#currentGesture` — 4 of 18 lines
- `src/main.ts#drawBarrierPlan` — 17 of 20 lines
- `src/main.ts#gameKeys` — 3 of 77 lines
- `src/main.ts#gameState` — 3 of 38 lines
- `src/main.ts#mountInterface` — 5 of 88 lines
- `src/main.ts#pointerDown` — 7 of 180 lines
- `src/main.ts#pointerMove` — 3 of 72 lines
- `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent` — 3 of 19 lines
- `src/render/renderer.ts#createSceneRenderer` — 36 of 3774 lines
- `src/render/renderer.ts#createSceneRenderer.switchedOff` — 16 of 16 lines
- `src/ui/v2/shell.ts#mountShell.renderLandscape` — 11 of 145 lines
- `src/world/doc.ts#RoadDoc.addBarrier` — 6 of 8 lines
- `src/world/doc.ts#RoadDoc.barrierCells` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barrierMap` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barrierNear` — 14 of 15 lines
- `src/world/doc.ts#RoadDoc.barriers` — 1 of 1 lines
- `src/world/doc.ts#RoadDoc.barriersIn` — 3 of 3 lines
- `src/world/doc.ts#RoadDoc.clearBarrierRecords` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.deleteBarrierRecord` — 3 of 5 lines
- `src/world/doc.ts#RoadDoc.fromJSON` — 5 of 187 lines
- `src/world/doc.ts#RoadDoc.layOver` — 3 of 47 lines
- `src/world/doc.ts#RoadDoc.ownIndex` — 7 of 14 lines
- `src/world/doc.ts#RoadDoc.putBarrierRecord` — 4 of 5 lines
- `src/world/doc.ts#RoadDoc.removeBarrier` — 5 of 7 lines
- `src/world/doc.ts#RoadDoc.replaceContents` — 6 of 149 lines
- `src/world/doc.ts#RoadDoc.restore` — 9 of 105 lines
- `src/world/doc.ts#RoadDoc.restore.itemRects` — 5 of 11 lines
- `src/world/doc.ts#RoadDoc.serialized` — 5 of 35 lines
- `src/world/doc.ts#RoadDoc.takeFork` — 3 of 49 lines
- `src/world/doc.ts#barrierRecord` — 3 of 6 lines
- `src/world/doc.ts#itemDifferences` — 11 of 33 lines
- `src/world/doc.ts#sameBarriers` — 10 of 11 lines
- `src/world/doc.ts#savedBarrier` — 1 of 1 lines
- `src/world/solids.ts#createSolids` — 7 of 47 lines
- `src/world/solids.ts#solidsOfItem` — 13 of 38 lines

## Resources

- feature entry `barrier.kind.fence` — src/ui/i18n/en.ts:161: `'barrier.kind.fence': 'Fence'`
- feature entry `barrier.kind.wall` — src/ui/i18n/en.ts:162: `'barrier.kind.wall': 'Wall'`
- feature entry `barrier.kind.hedge` — src/ui/i18n/en.ts:163: `'barrier.kind.hedge': 'Hedge'`
- feature entry `barrier.kind.guardrail` — src/ui/i18n/en.ts:164: `'barrier.kind.guardrail': 'Guardrail'`
- feature entry `barrier.kind.railing` — src/ui/i18n/en.ts:165: `'barrier.kind.railing': 'Railing'`
- feature entry `hint.barrier.built` — src/ui/i18n/en.ts:914: `'hint.barrier.built': 'Built'`
- feature entry `hint.barrier.removed` — src/ui/i18n/en.ts:915: `'hint.barrier.removed': 'Removed'`
- feature entry `hint.barrier.road` — src/ui/i18n/en.ts:916: `'hint.barrier.road': 'A wall or fence cannot stand on a road or its pavement'`
- feature entry `hint.barrier.short` — src/ui/i18n/en.ts:917: `'hint.barrier.short': 'Too short to build'`
- feature entry `hint.barrier.nothing` — src/ui/i18n/en.ts:918: `'hint.barrier.nothing': 'No wall, fence or hedge here to remove'`
- feature entry `barrier.kind.fence` — src/ui/i18n/pt-BR.ts:160: `'barrier.kind.fence': 'Cerca'`
- feature entry `barrier.kind.wall` — src/ui/i18n/pt-BR.ts:161: `'barrier.kind.wall': 'Muro'`
- feature entry `barrier.kind.hedge` — src/ui/i18n/pt-BR.ts:162: `'barrier.kind.hedge': 'Sebe'`
- feature entry `barrier.kind.guardrail` — src/ui/i18n/pt-BR.ts:163: `'barrier.kind.guardrail': 'Defensa (guard-rail)'`
- feature entry `barrier.kind.railing` — src/ui/i18n/pt-BR.ts:164: `'barrier.kind.railing': 'Gradil urbano'`
- feature entry `hint.barrier.built` — src/ui/i18n/pt-BR.ts:907: `'hint.barrier.built': 'Construído'`
- feature entry `hint.barrier.removed` — src/ui/i18n/pt-BR.ts:908: `'hint.barrier.removed': 'Removido'`
- feature entry `hint.barrier.road` — src/ui/i18n/pt-BR.ts:909: `'hint.barrier.road': 'Muro e cerca não podem ficar sobre a via nem sobre a calçada'`
- feature entry `hint.barrier.short` — src/ui/i18n/pt-BR.ts:910: `'hint.barrier.short': 'Curto demais para construir'`
- feature entry `hint.barrier.nothing` — src/ui/i18n/pt-BR.ts:911: `'hint.barrier.nothing': 'Não há muro, cerca nem sebe aqui para remover'`
- feature entry `guardrail` — src/ui/v2/shell.ts:187: `guardrail: '<path d="M5 18v-8M12 18v-8M19 18v-8"/><path d="M3 9h18v4H3Z"/><path d="M3 11h18"/>'`
- feature entry `railing` — src/ui/v2/shell.ts:188: `railing: '<path d="M4 20V6M20 20V6"/><path d="M4 7h16M4 18h16"/><path d="M8 7v11M12 7v11M16 7v11"/>'`
- shared style `rail` — src/peopleLab/peopleLab.css:29: `.rail { display: flex; flex-direction: column; align-items: center; gap: 2px; padding: 10px 0; background: #121417; border-right: 1px solid var(--line); }`
- shared style `rail` — src/peopleLab/peopleLab.css:30: `.rail button { position: relative; width: 40px; height: 40px; border: 0; border-radius: 8px; background: transparent; color: var(--ink-3); cursor: pointer; disp`
- shared style `rail` — src/peopleLab/peopleLab.css:31: `.rail button:hover { color: var(--ink); background: var(--panel-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:45: `.row { display: grid; grid-template-columns: 96px 1fr 38px; align-items: center; gap: 8px; min-height: 28px; color: var(--ink-2); }`
- shared style `row` — src/peopleLab/peopleLab.css:46: `.row output { text-align: right; color: var(--ink-3); font-variant-numeric: tabular-nums; font-size: 11px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:3: `.rp-panel { position: fixed; left: 16px; right: 16px; bottom: calc(var(--v-dock-h, 54px) + 14px); z-index: 40; max-width: 1000px; margin: 0 auto; max-height: ca`
- shared style `rp-panel` — src/ui/roads/roads.css:18: `.rp-panel *, .rp-summary * { box-sizing: border-box; }`
- shared style `rp-panel` — src/ui/roads/roads.css:19: `.rp-panel button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-panel` — src/ui/roads/roads.css:20: `.rp-panel button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-panel` — src/ui/roads/roads.css:21: `.rp-panel :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-head` — src/ui/roads/roads.css:23: `.rp-head { position: relative; display: flex; flex-wrap: wrap; align-items: center; gap: 8px 10px; padding-right: 38px; }`
- shared style `rp-head` — src/ui/roads/roads.css:24: `.rp-head > .rp-icon { position: absolute; top: 0; right: 0; }`
- shared style `rp-head` — src/ui/roads/roads.css:25: `.rp-head .rp-foot { margin-left: auto; }`
- shared style `rp-title` — src/ui/roads/roads.css:26: `.rp-title { font-size: 15px; font-weight: 650; letter-spacing: 0.01em; }`
- shared style `rp-sub` — src/ui/roads/roads.css:27: `.rp-sub { color: var(--v-muted, #a3b4b0); font-size: 12.5px; }`
- shared style `rp-chip` — src/ui/roads/roads.css:28: `.rp-chip { padding: 4px 10px; border-radius: 999px; background: var(--v-raise, rgba(255,255,255,.05)); border: 1px solid var(--v-line); font-variant-numeric: ta`
- shared style `rp-panel` — src/ui/roads/roads.css:29: `.rp-panel .rp-icon { display: grid; place-items: center; width: 30px; height: 30px; border: 0; border-radius: var(--v-r-s, 8px); background: none; color: var(--`
- shared style `rp-icon` — src/ui/roads/roads.css:30: `.rp-icon:hover { background: var(--v-raise-2, rgba(255,255,255,.09)); color: var(--v-text); }`
- shared style `rp-icon` — src/ui/roads/roads.css:31: `.rp-icon svg, .rp-tool svg, .rp-btn svg, .rp-issue svg, .rp-ok svg { width: 18px; height: 18px; fill: none; stroke: currentColor; stroke-width: 1.8; stroke-line`
- shared style `rp-label` — src/ui/roads/roads.css:33: `.rp-label { color: var(--v-muted); font-size: 11.5px; font-weight: 600; text-transform: none; letter-spacing: 0.02em; }`
- shared style `rp-cards` — src/ui/roads/roads.css:36: `.rp-cards { display: flex; gap: 8px; overflow-x: auto; padding-bottom: 4px; scrollbar-width: thin; scrollbar-color: rgba(255,255,255,.18) transparent; }`
- shared style `rp-cards` — src/ui/roads/roads.css:37: `.rp-cards::-webkit-scrollbar { height: 6px; }`
- shared style `rp-cards` — src/ui/roads/roads.css:38: `.rp-cards::-webkit-scrollbar-thumb { background: rgba(255,255,255,.18); border-radius: 3px; }`
- shared style `rp-cards` — src/ui/roads/roads.css:39: `.rp-cards::-webkit-scrollbar-track { background: transparent; }`
- shared style `rp-card` — src/ui/roads/roads.css:40: `.rp-card { position: relative; flex: none; width: 96px; border: 1px solid var(--v-line); border-radius: 10px; background: var(--v-raise); }`
- shared style `rp-panel` — src/ui/roads/roads.css:41: `.rp-panel .rp-card-main { display: grid; gap: 5px; width: 100%; padding: 6px 6px 7px; border: 0; background: none; text-align: left; border-radius: 10px; }`
- shared style `rp-card` — src/ui/roads/roads.css:42: `.rp-card:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); background: var(--v-raise-2); }`
- shared style `rp-card-art` — src/ui/roads/roads.css:44: `.rp-card-art { display: block; height: 34px; border-radius: 6px; overflow: hidden; background: linear-gradient(#26383a, #1a2729); }`
- shared style `rp-card-art` — src/ui/roads/roads.css:45: `.rp-card-art svg { display: block; width: 100%; height: 100%; }`
- shared style `rp-card-name` — src/ui/roads/roads.css:46: `.rp-card-name { font-size: 11.5px; font-weight: 600; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }`
- shared style `rp-panel` — src/ui/roads/roads.css:47: `.rp-panel .rp-card-x { position: absolute; top: 3px; right: 3px; width: 20px; height: 20px; display: grid; place-items: center; border: 0; border-radius: 6px; b`
- shared style `rp-card-x` — src/ui/roads/roads.css:48: `.rp-card-x svg { width: 12px; height: 12px; fill: none; stroke: currentColor; stroke-width: 2.4; stroke-linecap: round; }`
- shared style `rp-panel` — src/ui/roads/roads.css:49: `.rp-panel .rp-card-x:hover { color: var(--v-danger-ink, #ffb3aa); }`
- shared style `rp-card-blank` — src/ui/roads/roads.css:50: `.rp-card-blank { display: grid; place-items: center; height: 100%; color: var(--v-accent); background: var(--v-accent-soft, rgba(79,224,191,.14)); }`
- shared style `rp-card-blank` — src/ui/roads/roads.css:51: `.rp-card-blank svg { width: 20px; height: 20px; fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; }`
- shared style `rp-stage` — src/ui/roads/roads.css:54: `.rp-stage { position: relative; border-radius: 10px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); border: 1px solid var(--v-l`
- shared style `rp-end` — src/ui/roads/roads.css:64: `.rp-end { position: absolute; top: 6px; padding: 2px 7px; border-radius: 999px; background: rgba(0,0,0,.32); color: #c9d6d3; font-size: 11px; pointer-events: no`
- shared style `rp-end-l` — src/ui/roads/roads.css:65: `.rp-end-l { left: 8px; }`
- shared style `rp-end-r` — src/ui/roads/roads.css:65: `.rp-end-r { right: 8px; }`
- shared style `rp-picture` — src/ui/roads/roads.css:66: `.rp-picture { min-height: 120px; }`
- shared style `rp-empty` — src/ui/roads/roads.css:67: `.rp-empty { display: grid; place-items: center; align-content: center; gap: 8px; min-height: 160px; padding: 20px; color: var(--v-muted); text-align: center; fo`
- shared style `rp-empty` — src/ui/roads/roads.css:68: `.rp-empty svg { width: 34px; height: 34px; fill: none; stroke: currentColor; stroke-width: 1.5; }`
- shared style `rp-palette` — src/ui/roads/roads.css:72: `.rp-palette { display: flex; flex-wrap: wrap; gap: 6px; align-items: center; }`
- shared style `rp-palette` — src/ui/roads/roads.css:73: `.rp-palette > .rp-label { margin-right: 4px; }`
- shared style `rp-tool` — src/ui/roads/roads.css:74: `.rp-tool { display: inline-flex; align-items: center; gap: 7px; min-height: 34px; padding: 0 12px 0 9px; border: 1px solid var(--v-line); border-radius: var(--v`
- shared style `rp-tool` — src/ui/roads/roads.css:75: `.rp-tool:hover { border-color: var(--v-accent-line); color: var(--v-text); background: var(--v-raise-2); }`
- shared style `rp-tool` — src/ui/roads/roads.css:76: `.rp-tool svg { width: 22px; height: 22px; color: #cfe3de; }`
- shared style `rp-props` — src/ui/roads/roads.css:79: `.rp-props { display: grid; grid-template-columns: minmax(0, 1fr) minmax(0, 1.12fr); gap: 8px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:80: `@media (max-width: 760px) { .rp-panel { left: 8px; right: 8px; } }`
- shared style `rp-panel` — src/ui/roads/roads.css:82: `.rp-panel { container-type: inline-size; }`
- shared style `rp-props` — src/ui/roads/roads.css:83: `@container (max-width: 640px) { .rp-props { grid-template-columns: 1fr; } }`
- shared style `rp-block` — src/ui/roads/roads.css:84: `.rp-block { display: grid; gap: 8px; align-content: start; padding: 10px 12px; border: 1px solid var(--v-line); border-radius: 10px; background: var(--v-raise);`
- shared style `rp-block-head` — src/ui/roads/roads.css:85: `.rp-block-head { display: flex; align-items: center; gap: 8px; font-weight: 650; }`
- shared style `rp-block-head` — src/ui/roads/roads.css:86: `.rp-block-head .rp-swatch { width: 14px; height: 14px; border-radius: 4px; flex: none; }`
- shared style `rp-block-head` — src/ui/roads/roads.css:87: `.rp-block-head .rp-actions { margin-left: auto; display: flex; gap: 2px; }`
- shared style `rp-row` — src/ui/roads/roads.css:88: `.rp-row { display: grid; grid-template-columns: 76px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- shared style `rp-note` — src/ui/roads/roads.css:89: `.rp-note { color: var(--v-muted); font-size: 11.5px; }`
- shared style `rp-ctrls` — src/ui/roads/roads.css:90: `.rp-ctrls { display: flex; flex-wrap: wrap; align-items: center; gap: 6px; min-width: 0; }`
- shared style `rp-seg` — src/ui/roads/roads.css:92: `.rp-seg { display: inline-flex; flex-wrap: wrap; gap: 2px; padding: 3px; border-radius: 10px; background: rgba(0,0,0,.18); border: 1px solid var(--v-line); }`
- shared style `rp-seg` — src/ui/roads/roads.css:94: `.rp-seg button { min-height: 26px; padding: 0 11px; border: 0; border-radius: 7px; background: none; color: var(--v-muted); font-size: 12px; font-weight: 600; w`
- shared style `rp-seg` — src/ui/roads/roads.css:95: `.rp-seg button:hover { color: var(--v-text); }`
- shared style `rp-step` — src/ui/roads/roads.css:101: `.rp-step { display: inline-grid; grid-template-columns: 28px minmax(52px, auto) 28px; align-items: center; gap: 2px; padding: 2px; border-radius: 999px; backgro`
- shared style `rp-step` — src/ui/roads/roads.css:102: `.rp-step button { width: 28px; height: 26px; border: 0; border-radius: 999px; background: none; color: var(--v-text); font-size: 16px; font-weight: 700; line-he`
- shared style `rp-step` — src/ui/roads/roads.css:103: `.rp-step button:hover:not(:disabled) { background: var(--v-raise-2); }`
- shared style `rp-step` — src/ui/roads/roads.css:104: `.rp-step output { text-align: center; font-weight: 650; font-variant-numeric: tabular-nums; }`
- shared style `rp-switch` — src/ui/roads/roads.css:106: `.rp-switch { display: inline-flex; align-items: center; gap: 8px; border: 0; background: none; padding: 2px 0; color: var(--v-text); font-size: 12.5px; }`
- shared style `rp-panel` — src/ui/roads/roads.css:112: `.rp-panel .rp-issue { width: 100%; text-align: left; font: inherit; display: flex; gap: 8px; align-items: flex-start; padding: 8px 10px; border-radius: 8px; bac`
- shared style `rp-issue` — src/ui/roads/roads.css:113: `button.rp-issue:hover { background: rgba(255, 111, 97, 0.2); }`
- shared style `rp-issue` — src/ui/roads/roads.css:114: `.rp-issue svg { flex: none; margin-top: 1px; color: var(--v-danger, #ff6f61); }`
- shared style `rp-ok` — src/ui/roads/roads.css:115: `.rp-ok { display: flex; gap: 8px; align-items: center; color: #9be3c9; font-size: 12.5px; }`
- shared style `rp-foot` — src/ui/roads/roads.css:118: `.rp-foot { display: flex; flex-wrap: wrap; align-items: center; gap: 8px; }`
- shared style `rp-btn` — src/ui/roads/roads.css:119: `.rp-btn { display: inline-flex; align-items: center; gap: 6px; min-height: 34px; padding: 0 14px; border-radius: 999px; border: 1px solid var(--v-line); backgro`
- shared style `rp-btn` — src/ui/roads/roads.css:120: `.rp-btn:hover:not(:disabled) { border-color: var(--v-accent-line); background: var(--v-raise-2); }`
- shared style `rp-field` — src/ui/roads/roads.css:124: `.rp-field { min-height: 32px; width: 180px; padding: 0 12px; border-radius: 999px; border: 1px solid var(--v-line); background: rgba(0,0,0,.25); color: var(--v-`
- shared style `rp-field` — src/ui/roads/roads.css:125: `.rp-field::placeholder { color: var(--v-muted); }`
- shared style `rp-field` — src/ui/roads/roads.css:126: `.rp-field:focus { border-color: var(--v-accent-line); }`
- shared style `rp-summary` — src/ui/roads/roads.css:129: `.rp-summary { display: grid; gap: 8px; margin: 10px 0; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255`
- shared style `rp-summary-head` — src/ui/roads/roads.css:130: `.rp-summary-head { display: flex; align-items: baseline; gap: 8px; font-weight: 650; }`
- shared style `rp-summary-head` — src/ui/roads/roads.css:131: `.rp-summary-head .rp-sub { margin-left: auto; font-weight: 500; }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:132: `.rp-summary-art { border-radius: 8px; overflow: hidden; background: linear-gradient(180deg, #2b4448 0%, #1d2f32 70%); }`
- shared style `rp-summary-art` — src/ui/roads/roads.css:133: `.rp-summary-art svg { display: block; width: 100%; height: auto; }`
- shared style `rp-summary` — src/ui/roads/roads.css:134: `.rp-summary .rp-btn { justify-self: start; }`
- shared style `rp-keys` — src/ui/roads/roads.css:142: `.rp-keys { position: fixed; left: 330px; bottom: 80px; z-index: 41; width: 264px; display: grid; gap: 8px; padding: 12px; background: rgba(14, 22, 24, 0.97); co`
- shared style `rp-keys` — src/ui/roads/roads.css:148: `.rp-keys * { box-sizing: border-box; }`
- shared style `rp-keys` — src/ui/roads/roads.css:149: `.rp-keys button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-keys-head` — src/ui/roads/roads.css:150: `.rp-keys-head { display: flex; align-items: center; justify-content: space-between; }`
- shared style `rp-keys` — src/ui/roads/roads.css:151: `.rp-keys .rp-icon { display: grid; place-items: center; width: 28px; height: 28px; border: 0; border-radius: 8px; background: none; color: var(--v-muted); }`
- shared style `rp-keys` — src/ui/roads/roads.css:152: `.rp-keys .rp-icon svg { width: 16px; height: 16px; fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; }`
- shared style `rp-keys-row` — src/ui/roads/roads.css:153: `.rp-keys-row { display: flex; align-items: center; justify-content: space-between; gap: 8px; min-height: 32px; }`
- shared style `rp-key` — src/ui/roads/roads.css:154: `.rp-key { min-width: 86px; min-height: 30px; padding: 0 10px; border-radius: 8px; border: 1px solid var(--v-line); border-bottom-width: 3px; background: var(--v`
- shared style `rp-key` — src/ui/roads/roads.css:155: `.rp-key:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); }`
- shared style `rp-keys` — src/ui/roads/roads.css:157: `.rp-keys .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px solid va`
- shared style `rp-keys` — src/ui/roads/roads.css:158: `.rp-keys .rp-note { color: var(--v-danger-ink, #ffb3aa); font-size: 12px; }`
- shared style `rp-keys` — src/ui/roads/roads.css:159: `.rp-keys :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-connectors` — src/ui/roads/roads.css:177: `.rp-connectors .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px so`
- shared style `rp-junction` — src/ui/roads/roads.css:180: `.rp-junction { display: grid; gap: 8px; margin: 10px 0; color: var(--v-text, #eef4f2); font: 13px/1.3 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; }`
- shared style `rp-junction` — src/ui/roads/roads.css:181: `.rp-junction * { box-sizing: border-box; }`
- shared style `rp-junction` — src/ui/roads/roads.css:182: `.rp-junction button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rp-junction` — src/ui/roads/roads.css:183: `.rp-junction button:disabled { opacity: 0.35; cursor: default; }`
- shared style `rp-junction` — src/ui/roads/roads.css:184: `.rp-junction :focus-visible { outline: 2px solid var(--v-accent, #4fe0bf); outline-offset: 1px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:185: `.rp-junction .rp-block { display: grid; gap: 8px; padding: 10px; border: 1px solid rgba(255,255,255,.12); border-radius: 10px; background: rgba(255,255,255,.04)`
- shared style `rp-junction` — src/ui/roads/roads.css:186: `.rp-junction .rp-block-head { font-weight: 650; }`
- shared style `rp-junction` — src/ui/roads/roads.css:187: `.rp-junction .rp-row { display: grid; grid-template-columns: 84px minmax(0, 1fr); align-items: center; gap: 8px; min-height: 30px; }`
- shared style `rp-junction` — src/ui/roads/roads.css:189: `.rp-junction .rp-btn { justify-self: start; display: inline-flex; align-items: center; min-height: 30px; padding: 0 12px; border-radius: 999px; border: 1px soli`
- shared style `rp-junction` — src/ui/roads/roads.css:190: `.rp-junction .rp-btn:hover { border-color: var(--v-accent-line, rgba(79,224,191,.5)); }`
- shared style `rp-lock` — src/ui/roads/roads.css:191: `.rp-lock { display: flex; align-items: center; justify-content: space-between; gap: 8px; color: var(--v-muted, #a3b4b0); font-size: 12px; }`
- shared style `rp-flows` — src/ui/roads/roads.css:192: `.rp-flows { display: grid; gap: 2px; }`
- shared style `rp-flow` — src/ui/roads/roads.css:193: `.rp-flow { display: flex; justify-content: space-between; font-size: 12px; color: var(--v-muted, #a3b4b0); font-variant-numeric: tabular-nums; }`
- shared style `rp-flow` — src/ui/roads/roads.css:194: `.rp-flow strong { color: var(--v-text, #eef4f2); font-weight: 650; }`
- shared style `rp-moves` — src/ui/roads/roads.css:195: `.rp-moves { display: grid; gap: 4px; }`
- shared style `rp-moves` — src/ui/roads/roads.css:196: `.rp-moves .rp-ctrls { display: flex; flex-wrap: wrap; gap: 4px; }`
- shared style `rp-move` — src/ui/roads/roads.css:197: `.rp-move { min-height: 28px; padding: 0 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,.12); background: rgba(0,0,0,.18); color: var(--v-muted, #a`
- shared style `rc-catalog` — src/ui/roads/roads.css:210: `.rc-catalog { display: grid; gap: 6px; min-width: 0; font: 12px/1.25 Inter, ui-sans-serif, "Segoe UI", Roboto, sans-serif; color: var(--v-text, #eef4f2); }`
- shared style `rc-catalog` — src/ui/roads/roads.css:211: `.rc-catalog * { box-sizing: border-box; }`
- shared style `rc-catalog` — src/ui/roads/roads.css:212: `.rc-catalog button { font: inherit; color: inherit; cursor: pointer; }`
- shared style `rc-tabs` — src/ui/roads/roads.css:213: `.rc-tabs { display: flex; gap: 2px; padding: 2px; border-radius: 9px; background: rgba(0,0,0,.22); justify-self: start; }`
- shared style `rc-tab` — src/ui/roads/roads.css:214: `.rc-tab { display: inline-flex; align-items: center; gap: 5px; min-height: 26px; padding: 0 10px; border-radius: 7px; border: 0; background: none; font-weight: `
- shared style `rc-tab` — src/ui/roads/roads.css:215: `.rc-tab em { font-style: normal; font-size: 10.5px; opacity: .7; }`
- shared style `rc-tab` — src/ui/roads/roads.css:216: `.rc-tab:hover { color: var(--v-text, #eef4f2); background: rgba(255,255,255,.06); }`
- shared style `rc-cards` — src/ui/roads/roads.css:218: `.rc-cards { display: flex; gap: 6px; overflow-x: auto; padding: 2px 2px 6px; scrollbar-width: thin; scrollbar-color: rgba(255,255,255,.18) transparent; }`
- shared style `rc-card` — src/ui/roads/roads.css:219: `.rc-card { position: relative; flex: 0 0 124px; border-radius: 9px; }`
- shared style `rc-card-main` — src/ui/roads/roads.css:220: `.rc-card-main { display: grid; gap: 3px; width: 124px; padding: 0; border: 0; background: none; text-align: left; }`
- shared style `rc-art` — src/ui/roads/roads.css:221: `.rc-art { display: block; width: 124px; height: 64px; border-radius: 8px; border: 2px solid transparent; box-shadow: 0 1px 0 rgba(0,0,0,.35); transition: border`
- shared style `rc-card-main` — src/ui/roads/roads.css:222: `.rc-card-main:hover .rc-art { border-color: rgba(255,255,255,.45); transform: translateY(-1px); }`
- shared style `rc-name` — src/ui/roads/roads.css:224: `.rc-name { padding: 0 2px; font-weight: 600; font-size: 11px; line-height: 1.2; min-height: 3.6em; display: -webkit-box; -webkit-line-clamp: 3; -webkit-box-orie`
- shared style `rc-meta` — src/ui/roads/roads.css:226: `.rc-meta { padding: 0 2px; font-size: 10.5px; color: var(--v-muted, #a3b4b0); font-variant-numeric: tabular-nums; white-space: nowrap; }`
- shared style `rc-custom` — src/ui/roads/roads.css:227: `.rc-custom { position: absolute; top: 4px; right: 4px; display: grid; place-items: center; width: 24px; height: 24px; padding: 0; border-radius: 7px; border: 0;`
- shared style `rc-custom` — src/ui/roads/roads.css:228: `.rc-custom:hover { background: var(--v-accent, #4fe0bf); color: var(--v-accent-ink, #062520); }`
- shared style `rc-empty` — src/ui/roads/roads.css:229: `.rc-empty { padding: 10px 4px; color: var(--v-muted, #a3b4b0); max-width: 360px; }`
- shared style `rp-summary-actions` — src/ui/roads/roads.css:238: `.rp-summary-actions { display: flex; flex-wrap: wrap; gap: 6px; }`

## Where the program plugs the feature in

### src/editor/barriers.ts

- `src/editor/barriers.ts#BarrierTool` (class, line 76) ← construct from `src/main.ts#barrierTool`
- `src/editor/barriers.ts#BarrierTool.cancel` (method, line 91) ← call from `src/main.ts#cancelGestures`
- `src/editor/barriers.ts#BarrierTool.down` (method, line 96) ← call from `src/main.ts#pointerDown`
- `src/editor/barriers.ts#BarrierTool.gesture` (method, line 87) ← call from `src/main.ts#currentGesture`
- `src/editor/barriers.ts#BarrierTool.key` (method, line 123) ← call from `src/main.ts#gameKeys`
- `src/editor/barriers.ts#BarrierTool.move` (method, line 117) ← call from `src/main.ts#pointerMove`
- `src/editor/barriers.ts#BarrierTool.plan` (method, line 155) ← call from `src/main.ts#drawBarrierPlan`
- `src/editor/barriers.ts#BarrierTool.plan` (method, line 155) ← read from `src/main.ts#drawBarrierPlan`

### src/render/barriers.ts

- `src/render/barriers.ts#BarrierProduct.allKeys` (method, line 51) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.changed` (method, line 83) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.compute` (method, line 68) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.dirty` (method, line 58) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#BarrierProduct.equals` (method, line 79) ← implementation from `src/world/derived.ts#Derived.refresh`
- `src/render/barriers.ts#Barriers` (interface, line 12) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#Barriers` (interface, line 12) ← read from `src/render/renderer.ts#createSceneRenderer`, `src/render/renderer.ts#createSceneRenderer.switchedOff`
- `src/render/barriers.ts#createBarriers` (function, line 104) ← call from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.cull` (method, line 128) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.dispose` (method, line 132) ← implementation from `src/render/renderer.ts#createSceneRenderer`
- `src/render/barriers.ts#createBarriers.sync` (method, line 116) ← implementation from `src/render/renderer.ts#createSceneRenderer`

### src/world/barriers.ts

- `src/world/barriers.ts#Barrier` (interface, line 9) ← read from `src/editor/bulldozer.ts#Bulldozer.boxed`, `src/editor/chunkedSave.ts#KEYED`, `src/main.ts#barrierTool`, `src/render/flatAtlas.ts#createFlatAtlas.readWholeContent`, …
- `src/world/barriers.ts#Barrier` (interface, line 9) ← typeRef from `src/editor/chunkedSave.ts#KEYED`, `src/world/doc.ts#barrierRecord`, `src/world/doc.ts#RoadDoc.addBarrier`, `src/world/doc.ts#RoadDoc.barrierCells`, …
- `src/world/barriers.ts#BARRIER_KINDS` (variable, line 4) ← read from `src/main.ts#mountInterface`, `src/ui/v2/shell.ts#mountShell.renderLandscape`
- `src/world/barriers.ts#BARRIER_SIZE` (variable, line 17) ← read from `src/world/solids.ts#solidsOfItem`
- `src/world/barriers.ts#BarrierKind` (type, line 2) ← typeRef from `src/main.ts#gameState`, `src/main.ts#mountInterface`, `src/world/doc.ts#RoadDoc.addBarrier`
- `src/world/barriers.ts#isBarrierKind` (function, line 25) ← call from `src/world/doc.ts#barrierRecord`

## Shared declarations the feature uses

### src/core/polyline.ts

- `src/core/polyline.ts#Frame` (interface, line 7) — read by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — call by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`
- `src/core/polyline.ts#Polyline.closestPoint` (method, line 330) — read by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`
- `src/core/polyline.ts#Polyline.sampleAt` (method, line 152) — call by 1: `src/editor/barriers.ts#snapBarrierPoint`

### src/core/units.ts

- `src/core/units.ts#m` (function, line 6) — call by 12: `src/editor/barriers.ts#barrierProblem`, `src/editor/barriers.ts#FOOTWAY_REACH`, `src/editor/barriers.ts#MIN_BARRIER_RUN`, `src/editor/barriers.ts#onCarriageway`, …

### src/core/vec2.ts

- `src/core/vec2.ts#Vec2` (interface, line 4) — read by 4: `src/editor/barriers.ts#barrierProblem`, `src/editor/barriers.ts#BarrierTool.finish`, `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`
- `src/core/vec2.ts#Vec2` (interface, line 4) — typeRef by 9: `src/editor/barriers.ts#barrierProblem`, `src/editor/barriers.ts#BarrierTool.cursor`, `src/editor/barriers.ts#BarrierTool.down`, `src/editor/barriers.ts#BarrierTool.move`, …

### src/editor/snap.ts

- `src/editor/snap.ts#ribbonsNear` (function, line 74) — call by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`

### src/main.ts

- `src/main.ts#barrierTool` (variable, line 515) — implementation by 5: `src/editor/barriers.ts#BarrierTool.down`, `src/editor/barriers.ts#BarrierTool.finish`, `src/editor/barriers.ts#BarrierTool.key`, `src/editor/barriers.ts#BarrierTool.move`, …

### src/render/instanceRuns.ts

- `src/render/instanceRuns.ts#createInstanceRuns` (function, line 201) — call by 1: `src/render/barriers.ts#createBarriers`
- `src/render/instanceRuns.ts#createInstanceRuns.cull` (method, line 214) — implementation by 1: `src/render/barriers.ts#createBarriers.cull`
- `src/render/instanceRuns.ts#createInstanceRuns.dispose` (method, line 215) — implementation by 1: `src/render/barriers.ts#createBarriers.dispose`
- `src/render/instanceRuns.ts#createInstanceRuns.update` (method, line 209) — implementation by 1: `src/render/barriers.ts#createBarriers.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — call by 3: `src/render/barriers.ts#createBarriers.cull`, `src/render/barriers.ts#createBarriers.dispose`, `src/render/barriers.ts#createBarriers.sync`
- `src/render/instanceRuns.ts#InstanceRuns` (interface, line 170) — read by 1: `src/render/barriers.ts#createBarriers`
- `src/render/instanceRuns.ts#Run` (type, line 100) — typeRef by 2: `src/render/barriers.ts#BarrierPlace`, `src/render/barriers.ts#createBarriers.sync`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — construct by 1: `src/render/barriers.ts#BarrierProduct.compute`
- `src/render/instanceRuns.ts#RunBuilder` (class, line 111) — typeRef by 1: `src/render/barriers.ts#barrierInto`
- `src/render/instanceRuns.ts#RunBuilder.build` (method, line 137) — call by 1: `src/render/barriers.ts#BarrierProduct.compute`
- `src/render/instanceRuns.ts#RunBuilder.turned` (method, line 132) — call by 1: `src/render/barriers.ts#barrierInto`
- `src/render/instanceRuns.ts#RunStore` (interface, line 69) — typeRef by 2: `src/render/barriers.ts#BarrierProduct.constructor`, `src/render/barriers.ts#createBarriers`
- `src/render/instanceRuns.ts#sameRun` (function, line 152) — call by 1: `src/render/barriers.ts#BarrierProduct.equals`

### src/world/buildings/validate.ts

- `src/world/buildings/validate.ts#ROAD_CLEARANCE` (variable, line 117) — read by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/buildings/validate.ts#touchesRoad` (function, line 256) — call by 1: `src/editor/barriers.ts#barrierProblem`

### src/world/cells.ts

- `src/world/cells.ts#CellFiling` (class, line 60) — construct by 1: `src/render/barriers.ts#BarrierProduct.filing`
- `src/world/cells.ts#CellFiling.file` (method, line 76) — call by 1: `src/render/barriers.ts#BarrierProduct.changed`
- `src/world/cells.ts#CellFiling.ids` (method, line 126) — call by 1: `src/render/barriers.ts#BarrierProduct.allKeys`
- `src/world/cells.ts#CellFiling.idsMeetingAny` (method, line 169) — call by 2: `src/render/barriers.ts#BarrierProduct.dirty`, `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/cells.ts#CellFiling.remove` (method, line 107) — call by 1: `src/render/barriers.ts#BarrierProduct.changed`

### src/world/changes.ts

- `src/world/changes.ts#Change` (interface, line 35) — read by 1: `src/render/barriers.ts#BarrierProduct.dirty`
- `src/world/changes.ts#Change` (interface, line 35) — typeRef by 1: `src/render/barriers.ts#BarrierProduct.dirty`
- `src/world/changes.ts#ChangeJournal` (class, line 156) — typeRef by 1: `src/render/barriers.ts#BarrierSource`
- `src/world/changes.ts#ChangeKind` (type, line 27) — typeRef by 1: `src/render/barriers.ts#BarrierProduct.kinds`
- `src/world/changes.ts#ChangeRect` (type, line 4) — typeRef by 3: `src/render/barriers.ts#BarrierPlace`, `src/render/barriers.ts#BarrierProduct.compute`, `src/world/doc.ts#RoadDoc.barriersInAny`

### src/world/derived.ts

- `src/world/derived.ts#Derived` (class, line 71) — construct by 1: `src/render/barriers.ts#createBarriers`
- `src/world/derived.ts#Derived.get` (method, line 80) — call by 1: `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#Derived.refresh` (method, line 93) — call by 1: `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#Derived.work` (field, line 74) — read by 1: `src/render/barriers.ts#createBarriers.sync`
- `src/world/derived.ts#DerivedSpec` (interface, line 40) — implements by 1: `src/render/barriers.ts#BarrierProduct`
- `src/world/derived.ts#DerivedWork` (interface, line 60) — read by 1: `src/render/barriers.ts#createBarriers.sync`

### src/world/doc.ts

- `src/world/doc.ts#RoadDoc` (class, line 153) — typeRef by 2: `src/render/barriers.ts#Barriers`, `src/render/barriers.ts#BarrierSource`
- `src/world/doc.ts#RoadDoc.barrierCells` (field, line 172) — read by 1: `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/doc.ts#RoadDoc.barrierMap` (field, line 169) — read by 1: `src/world/doc.ts#RoadDoc.barriersInAny`
- `src/world/doc.ts#RoadDoc.barriers` (getter, line 170) — call by 2: `src/render/barriers.ts#BarrierProduct.allKeys`, `src/render/barriers.ts#BarrierProduct.compute`
- `src/world/doc.ts#RoadDoc.changes` (field, line 262) — read by 1: `src/render/barriers.ts#createBarriers.sync`
- `src/world/doc.ts#RoadDoc.segment` (method, line 420) — call by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/doc.ts#RoadSegment` (interface, line 124) — read by 1: `src/editor/barriers.ts#snapBarrierPoint`

### src/world/network.ts

- `src/world/network.ts#Network` (class, line 46) — typeRef by 4: `src/editor/barriers.ts#barrierProblem`, `src/editor/barriers.ts#BarrierToolHost`, `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/network.ts#Network.constructor` (constructor, line 100) — read by 1: `src/editor/barriers.ts#snapBarrierPoint`

### src/world/ribbons.ts

- `src/world/ribbons.ts#SegmentRibbon` (interface, line 25) — read by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`

### src/world/roadTypes.ts

- `src/world/roadTypes.ts#CURB_BAND` (variable, line 90) — read by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/roadTypes.ts#halfWidth` (function, line 339) — call by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/roadTypes.ts#Level` (enum, line 10) — reference by 1: `src/editor/barriers.ts#snapBarrierPoint`
- `src/world/roadTypes.ts#RoadType` (interface, line 29) — read by 2: `src/editor/barriers.ts#onCarriageway`, `src/editor/barriers.ts#snapBarrierPoint`

## External APIs

- `es:Math` × 33
- `es:Array.push` × 16
- `es:Math.max` × 14
- `es:ReadonlyArray.length` × 8
- `es:Math.hypot` × 6
- `es:Math.min` × 6
- `es:Array.length` × 5
- `es:Infinity` × 5
- `es:Array.map` × 3
- `es:Record` × 3
- `es:Set.add` × 3
- `es:Map.get` × 2
- `es:Math.atan2` × 2
- `es:Math.ceil` × 2
- `es:Math.round` × 2
- `es:ReadonlyArray.map` × 2
- `es:ReadonlySet.has` × 2
- `es:Set` × 2
- `three:BoxGeometry` × 2
- `three:MeshStandardMaterial` × 2
- `dom:performance` × 1
- `dom:Performance.now` × 1
- `es:Array.filter` × 1
- `es:Array.pop` × 1
- `es:Iterable` × 1
- `es:Map` × 1
- `es:Map.clear` × 1
- `es:Map.delete` × 1
- `es:Map.set` × 1
- `es:Math.abs` × 1
- `es:Object` × 1
- `es:ObjectConstructor.fromEntries` × 1
- `es:Readonly` × 1
- `es:ReadonlyArray.every` × 1
- `es:ReadonlyArray.forEach` × 1
- `es:ReadonlyMap.get` × 1
- `es:ReadonlyMap.keys` × 1
- `es:ReadonlySet` × 1
- `three:Frustum` × 1
- `three:Group` × 1
- `three:Material` × 1
- `three:Matrix4` × 1
