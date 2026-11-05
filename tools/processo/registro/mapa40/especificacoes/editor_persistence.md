# editor:persistence

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/persistence.ts`
- Unidades próprias (só esta funcionalidade alcança): 68 em 1 arquivo(s), 810 linhas
- Alcance total: 954 unidades; usa 884 compartilhadas de outras; é dona principal de 2 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:focus`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#restoreBootSave@248` (7), `src/main.ts#openImported@2263` (6), `src/main.ts#IfStatement@4036` (4), `src/main.ts#SLIDERS@1715` (4), `src/main.ts#openSession@2273` (3), `src/main.ts#persistence@205` (2), `src/main.ts#saveOnClose@3885` (2), `src/main.ts#comesBackFromLostContext@674` (2), `src/main.ts#ExpressionStatement@3908` (1), `src/main.ts#ExpressionStatement@874` (1), `src/main.ts#ExpressionStatement@3904` (1), `src/main.ts#bootSave@241` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 4
- Usa: `world:doc` (14), `editor:chunkedSave` (14), `editor:saveStore` (9), `editor:saveBinary` (7), `world:road` (3), `world:terrain` (3), `editor:blueprintLibrary` (2), `world:rescale` (1), `editor:buildingPlans` (1), `world:structures` (1), `world:ecology` (1), `world:buildings` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/binarySave.spec.ts`, `tests/editor/chunkedSave.spec.ts`, `tests/editor/economy.spec.ts`, `tests/editor/exportMap.spec.ts`, `tests/editor/persistence.spec.ts`, `tests/people/spec.spec.ts`, `tests/world/buildings.spec.ts`, `tests/world/rescale.spec.ts`, `tests/world/surfaces.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `economy`: portado 2
- porte `interface`: portado 106, não se aplica 38
- porte `landscape`: portado 4

## Código próprio (linhas — declaração — comentário)

### src/editor/persistence.ts
- 17-17 `KEY` (variable): Where the map is kept.
- 19-19 `QUARANTINE_KEY` (variable): Where storage the loader could not read is kept, rather than deleted.
- 20-20 `DEBOUNCE_MS` (variable)
- 22-22 `MAX_WAIT_MS` (variable): The longest an edit waits to be saved, however many follow it (lodash `debounce`'s `maxWait`)
- 24-24 `SETTINGS_KEY` (variable): The camera and simulation settings, on their own.
- 26-26 `LOCAL_LIMIT_CHARS` (variable): The largest session text written to `localStorage`, characters.
- 55-58 `SavedSession` (interface)
- 61-345 `Persistence` (class): Autosave and file export.
- 62-62 `Persistence.timer` (field)
- 63-63 `Persistence.pending` (field)
- 65-65 `Persistence.firstPending` (field): When the oldest unsaved edit asked to be saved (`MAX_WAIT_MS`).
- 68-68 `Persistence.onSaveFailed` (field): Called when a write fails (storage full or refused).
- 71-71 `Persistence.parted` (field): The map by tiles (`chunkedSave.ts`): what the diary says changed, written off the page; null without IndexedDB (the whole text).
- 73-73 `Persistence.loadedParts` (field): How the map the opening read was stored (`loadSessionAsync`), for `adoptLoaded`.
- 75-77 `Persistence.constructor` (method)
- 80-83 `Persistence.adoptLoaded` (method): The document just restored from `loadSessionAsync`: read from the parts, they are it, and the next save writes only what changes.
- 86-86 `Persistence.rejected` (field): Set once this instance has rejected the stored entry.
- 89-91 `Persistence.save` (method): Stores the document, keeping whatever settings are already saved.
- 94-100 `Persistence.saveSoon` (method): Coalesces rapid edits into one write.
- 103-103 `Persistence.documentText` (field): The document as text, when the editor already has it
- 106-106 `Persistence.generation` (field): DO NOT write the pointer unasked
- 108-108 `Persistence.lastWrite` (field): The last write started (`written`, settled either way): what a closing desktop window waits for.
- 109-109 `Persistence.written` (method)
- 110-153 `Persistence.saveSession` (method)
- 156-156 `Persistence.partsKept` (field): Whether parts of a large map may still be stored (`dropParts`).
- 159-163 `Persistence.dropParts` (method): The parts of a large map removed once the map is saved whole again (it shrank, or another map was opened)
- 166-201 `Persistence.loadSessionAsync` (method): Reads the autosave from wherever it is: the small key, or IndexedDB for a session too large for it
- 203-210 `Persistence.saveSessionSoon` (method)
- 213-213 `Persistence.idle` (field): The idle callback a debounced save waits in (`flushWhenIdle`).
- 216-222 `Persistence.flushWhenIdle` (method): The debounced save, in the browser's idle time - within two seconds - rather than at a fixed moment
- 224-224 `Persistence.settingsTimer` (field)
- 227-231 `Persistence.saveSettingsSoon` (method): Stores the camera and simulation settings alone, coalescing rapid changes.
- 233-233 `Persistence.pendingSettings` (field)
- 235-241 `Persistence.flushSettings` (method)
- 243-250 `Persistence.writeSettings` (method)
- 253-263 `Persistence.readSettings` (method): The separately stored settings, when present and readable (a streamed world's camera, its map being its tiles).
- 266-278 `Persistence.flush` (method): Writes a debounced save now.
- 280-282 `Persistence.load` (method)
- 285-316 `Persistence.loadSession` (method): Reads the autosave.
- 319-336 `Persistence.quarantineStored` (method): Sets the stored map aside after it passed validation but failed to load (the loader threw).
- 338-344 `Persistence.clear` (method)
- 348-372 `quarantine` (function): Sets unreadable storage aside — and never destroys it to do so.
- 375-375 `SPEEDS` (variable): The speeds the Simulation menu offers; anything else was never chosen there.
- 377-377 `DEMANDS` (variable): The demand levels the menu offers.
- 380-392 `normalizeSettings` (function): Settings as the controls can show them.
- 395-395 `MAX_INTENSITY` (variable): The panel's Traffic and People: shares of the city's own density (`SimWorld.followCity`), 1 = 100 %, at most this.
- 398-402 `cameraInUnit` (function): The saved camera in this build's world unit (`world/rescale.ts`): its zoom is pixels a unit.
- 404-406 `defaultSettings` (function)
- 408-411 `isSavedSession` (function)
- 413-426 `isSavedSettings` (function)
- 430-433 `ImportResult` (type): What a file pick came to: a map, nothing chosen, or a file that is not one.
- 436-468 `importFromFile` (function): Reads a map file chosen by the user.
- 471-489 `readMapFile` (function): A map file's session, known by its first bytes: the binary file (magic 'RCMF', `mapExport.ts` mapFileBlob), or a JSON file of the builds before it (a session, or a map alone); null when it is neither.
- 492-495 `readImportedSession` (function): Accepts legacy geometry-only files while preserving complete session files.
- 498-595 `isSerializedDoc` (function): Strict boundary validation for local storage and imported files.
- 597-599 `isJunctionControl` (function)
- 601-603 `isSegmentDirection` (function)
- 605-607 `isRecord` (function)
- 609-611 `isFiniteNumber` (function)
- 615-615 `MAX_STAMP_RADIUS` (variable): The widest stamp a map may hold: the map itself (4 800 units across).
- 617-620 `isId` (function)
- 625-629 `LargeStore` (interface): Where a session too large for Web Storage is kept (`LOCAL_LIMIT_CHARS`).
- 630-630 `indexedDbStore` (variable)
- 632-638 `defaultParts` (function): The map's tiles in IndexedDB where the browser has it and gzip streams (MDN, `CompressionStream`), written by the job system's `save` lane; none elsewhere.
- 639-653 `storeRequest` (function)
- 654-656 `storeGet` (function)
- 657-659 `storePut` (function)
- 660-662 `storeDelete` (function)
