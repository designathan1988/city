# Cobertura do rastreamento: interface

Fonte: `converter/out/features/interface/boundary.json`, 1259 declarações próprias. Cada linha foi julgada pelo nome da declaração (`evidence/cobertura_decl.py`), sem expressão regular.
Totais: portado 586, outra sessão 336, não se aplica 262, já existia 75.

- **portado:** arquivo e função do jogo.
- **já existia:** arquivo e função que o jogo já tinha.
- **não se aplica:** o motivo.
- **outra sessão:** a prova do dono.

## src/core/commands.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CommandRegistry.bind` | portado | `game/flow/game_keys.gd::_ready`, `game/flow/game_keys.gd::_unhandled_key_input` |
| `CommandRegistry.register` | portado | `game/flow/game_keys.gd::_ready`, `game/flow/game_keys.gd::_unhandled_key_input` |

## src/core/dynamicResolution.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `AIM` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DEADBAND` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.constructor` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.costs` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.count` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.reset` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.sample` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.scale` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.skip` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.sorted` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `DynamicResolution.steps` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `RISE_MARGIN` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `SETTLE` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `STEPS` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |
| `WINDOW` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `dynamic_resolution` |

## src/core/health.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `FrameTimer.between` | portado | `game/health/health_log.gd::_process` |
| `HealthLog.changes` | portado | `game/health/health_log.gd::changes` |
| `HealthLog.restore` | portado | `game/health/health_log.gd::_restore` |
| `HealthLog.status` | portado | `game/health/health_log.gd::status` |
| `HealthLog.toJSON` | portado | `game/health/health_log.gd::_save` |
| `systemKey` | portado | `game/health/health_log.gd::_slow_frame` |
| `systemsOf` | portado | `game/health/health_log.gd::_slow_frame` |

## src/core/perfBudget.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `stepBudget` | portado | `game/health/health_log.gd::_slow_frame`, sessões de `render_cost` |

## src/desktopSaves.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Answer` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `CONNECT_MS` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `DesktopStores` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `LOST_MESSAGE` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `MIGRATED` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `MIGRATE_CHUNK` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `PORT_MESSAGE` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `Request` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `SavesBridge` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.call` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.connect` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.connectWorker` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.nextPort` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.nextPort.take` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.onLost` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.open` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.ready` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `desktopSaves.written` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `migrate` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |
| `read` | não se aplica | ponte entre a janela do Electron e o disco; o Godot grava direto (`game/store/save_slots.gd::path`) |

## src/desktopShown.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DesktopShown` | portado | `game/settings/settings.gd::_notification`, `game/store/city_saves.gd::save_on_hide` |
| `watchDesktopShown` | portado | `game/settings/settings.gd::_notification`, `game/store/city_saves.gd::save_on_hide` |

## src/editor/chunkedSave.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DENSE_SQUARE` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `GLOBAL_KINDS` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `GUNZIP_BATCH` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `JSON_CODEC` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KEYED` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KEYED_NAMES` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `Keyed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedName` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedSpec` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.dirty` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.dirtyOver` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.dirtyOver.visit` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.members` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.names` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.place` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.stored` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.wear` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `KeyedState.where` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `LAYER_NAMES` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `LISTS` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `LIST_NAMES` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `LayerName` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListName` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListSpec` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.changed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.counts` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.dirty` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.ords` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.rects` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.square` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `ListState.stored` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `NOWHERE` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `OldManifest` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `Place` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `RegionManifest` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SAVE_TILE` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SavedLayout` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SquareBlob` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SquareCodec` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SquareKeys` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledManifest` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.adopt` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.blobKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.clear` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.constructor` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.cursor` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.doc` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.everything` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.forget` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.globalKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.globals` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.keyed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.keysOf` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.lists` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.manifestKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.note` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.oldKeys` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.oldKeysOf` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.oldManifestKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.queue` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.queued` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.read` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.readOld` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.readSquares` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.readSquares.items` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.readSquares.records` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.reconcile` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.recordsIn` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.regionKeys` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.save` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.savedKeys` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.setAside` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.storedGlobals` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.take` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.tileKeys` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.tilesManifestKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.touch` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.walk` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.walk.visit` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.wearKey` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.worn` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.write` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSaves.writer` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `TiledSession` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `freshKeyed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `freshLists` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `idOf` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `isObject` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `isOldManifest` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `isSquareBlob` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `isTiledManifest` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `itself` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `keyed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `listed` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `meets` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `nodeOf` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `placeInOrder` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `readWears` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `squareAt` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `squareRect` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `withWear` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |

## src/editor/cookedCities.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `COOKED_CITIES_DIR` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `COOKED_CITY_SEEDS` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `CookedCitiesManifest` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `CookedCity` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `CookedKind` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `gunzipped` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `manifest` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |
| `readCookedCity` | não se aplica | cidade cozida na exportação do navegador para abrir sem gerar; o núcleo gera o mapa 0 na hora (`sim/citygen/city_gen.cpp`), e a abertura é medida no relatório |

## src/editor/gameKeys.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `GameKeyDeps` | portado | `game/flow/game_keys.gd::_ready`, regras em `data/ui/keys.json` |
| `TOOL_LETTERS` | portado | `game/flow/game_keys.gd::_ready`, regras em `data/ui/keys.json` |
| `createGameKeys` | portado | `game/flow/game_keys.gd::_ready`, regras em `data/ui/keys.json` |
| `createGameKeys.command` | portado | `game/flow/game_keys.gd::_run` |
| `createGameKeys.digits` | portado | `game/flow/game_keys.gd::_ready`, regras em `data/ui/keys.json` |
| `createGameKeys.tool` | portado | `game/flow/game_keys.gd::_run` |
| `done` | portado | `game/flow/game_keys.gd::_run` |
| `held` | portado | `game/flow/game_keys.gd::_run` |
| `used` | portado | `game/flow/game_keys.gd::_run` |

## src/editor/history.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `History` | já existia | `sim/commands/edit_history.cpp::begin` |
| `History.begin` | já existia | `sim/commands/edit_history.cpp::begin` |
| `History.bytes` | já existia | `sim/commands/edit_history.cpp::trim` |
| `History.canRedo` | já existia | `sim/commands/edit_history.cpp::redo` |
| `History.canUndo` | já existia | `sim/commands/edit_history.cpp::undo` |
| `History.cancel` | já existia | `sim/commands/edit_history.cpp::end` |
| `History.clear` | já existia | `sim/commands/edit_history.cpp::clear` |
| `History.close` | já existia | `sim/commands/edit_history.cpp::end` |
| `History.constructor` | já existia | `sim/commands/edit_history.cpp::begin` |
| `History.dropGroupsOf` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.dropGroupsOf.kept` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.end` | já existia | `sim/commands/edit_history.cpp::end` |
| `History.group` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.groupAt` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.groupOf` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.grouped` | já existia | `sim/commands/edit_history.cpp::begin_group` |
| `History.open` | já existia | `sim/commands/edit_history.cpp::begin` |
| `History.redo` | já existia | `sim/commands/edit_history.cpp::redo` |
| `History.redoStack` | já existia | `sim/commands/edit_history.cpp::redo` |
| `History.settle` | já existia | `sim/commands/edit_history.cpp::end` |
| `History.trim` | já existia | `sim/commands/edit_history.cpp::trim` |
| `History.undo` | já existia | `sim/commands/edit_history.cpp::undo` |
| `History.undoStack` | já existia | `sim/commands/edit_history.cpp::undo` |
| `followNetwork` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário reverte cada operação e acerta nós e lotes |
| `openedGenerated` | não se aplica | marca de "cidade gerada ainda não editada" do autosave do navegador; aqui é `game/store/city_saves.gd::_autosave_due` |
| `restoreInto` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário reverte cada operação e acerta nós e lotes |
| `restoreSnapshot` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário reverte cada operação e acerta nós e lotes |
| `serialize` | não se aplica | o diário guarda operações, não um retrato em JSON: nada a serializar |

## src/editor/persistence.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DEBOUNCE_MS` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `DEMANDS` | portado | `game/store/session.gd::normalized` |
| `ImportResult` | portado | `game/store/city_saves.gd::open_file`, `ui/hud_actions.gd::open_file` |
| `KEY` | portado | `game/settings/settings.gd::_save` |
| `LOCAL_LIMIT_CHARS` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `LargeStore` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `MAX_INTENSITY` | portado | `game/store/session.gd::normalized` |
| `MAX_STAMP_RADIUS` | não se aplica | conversão de unidade de saves antigos do 4.0; o jogo não abre saves do 4.0 e guarda metros |
| `MAX_WAIT_MS` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.adoptLoaded` | portado | `game/store/city_saves.gd::load_last`, `game/store/city_saves.gd::load_game` |
| `Persistence.clear` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `Persistence.constructor` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.documentText` | portado | `game/store/city_saves.gd::save_game` |
| `Persistence.dropParts` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `Persistence.firstPending` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.flush` | portado | `game/store/city_saves.gd::save_on_close`, `game/store/city_saves.gd::save_on_hide` |
| `Persistence.flushSettings` | portado | `game/settings/settings.gd::_save` |
| `Persistence.flushWhenIdle` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.generation` | portado | `game/store/city_saves.gd::save_game` |
| `Persistence.idle` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.lastWrite` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.load` | portado | `game/store/city_saves.gd::load_last`, `game/store/city_saves.gd::load_game` |
| `Persistence.loadSession` | portado | `game/store/city_saves.gd::load_last`, `game/store/city_saves.gd::load_game` |
| `Persistence.loadSessionAsync` | portado | `game/store/city_saves.gd::load_last`, `game/store/city_saves.gd::load_game` |
| `Persistence.loadedParts` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `Persistence.onSaveFailed` | portado | `ui/hud_actions.gd::_on_saved`, no máximo um aviso por `data/game/saves.json` `failed_hint_s` |
| `Persistence.parted` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `Persistence.partsKept` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `Persistence.pending` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.pendingSettings` | portado | `game/settings/settings.gd::_save` |
| `Persistence.quarantineStored` | portado | `game/store/save_slots.gd::quarantine` |
| `Persistence.readSettings` | portado | `game/settings/settings.gd::_save` |
| `Persistence.rejected` | portado | `game/store/save_slots.gd::quarantine` |
| `Persistence.save` | portado | `game/store/city_saves.gd::save_game` |
| `Persistence.saveSession` | portado | `game/store/city_saves.gd::save_game` |
| `Persistence.saveSessionSoon` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.saveSettingsSoon` | portado | `game/settings/settings.gd::_save` |
| `Persistence.saveSoon` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.settingsTimer` | portado | `game/settings/settings.gd::_save` |
| `Persistence.timer` | portado | `game/store/city_saves.gd::tick`, `game/store/city_saves.gd::_autosave_due`, período medido em `data/game/saves.json` |
| `Persistence.writeSettings` | portado | `game/settings/settings.gd::_save` |
| `Persistence.written` | portado | `game/store/city_saves.gd::save_game` |
| `QUARANTINE_KEY` | portado | `game/store/save_slots.gd::quarantine` |
| `SETTINGS_KEY` | portado | `game/settings/settings.gd::_save` |
| `SPEEDS` | portado | `game/store/session.gd::normalized` |
| `SavedSession` | portado | `game/store/session.gd::write`, `game/store/session.gd::read` |
| `SavedSettings` | portado | `game/store/session.gd::write`, `game/store/session.gd::read` |
| `cameraInUnit` | portado | `game/store/session.gd::write`, `game/store/session.gd::read` |
| `defaultParts` | não se aplica | partes do save do navegador; aqui um arquivo só (`api/city_core_persist.cpp::save_city`) |
| `defaultSettings` | portado | `game/settings/settings.gd::_save` |
| `importFromFile` | portado | `game/store/city_saves.gd::open_file`, `ui/hud_actions.gd::open_file` |
| `importFromFile.finish` | portado | `game/store/city_saves.gd::open_file`, `ui/hud_actions.gd::open_file` |
| `indexedDbStore` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `isFiniteNumber` | portado | `game/store/session.gd::normalized` |
| `isId` | não se aplica | conferência do JSON do documento; o binário valida ao ler (`sim/persistence/save_read.cpp::decode_city`) |
| `isJunctionControl` | não se aplica | conferência do JSON do documento; o binário valida ao ler (`sim/persistence/save_read.cpp::decode_city`) |
| `isRecord` | portado | `game/store/session.gd::normalized` |
| `isSavedSession` | portado | `game/store/session.gd::normalized` |
| `isSavedSettings` | portado | `game/store/session.gd::normalized` |
| `isSegmentDirection` | não se aplica | conferência do JSON do documento; o binário valida ao ler (`sim/persistence/save_read.cpp::decode_city`) |
| `isSerializedDoc` | não se aplica | conferência do JSON do documento; o binário valida ao ler (`sim/persistence/save_read.cpp::decode_city`) |
| `normalizeSettings` | portado | `game/store/session.gd::normalized` |
| `normalizeSettings.intensity` | portado | `game/store/session.gd::normalized` |
| `quarantine` | portado | `game/store/save_slots.gd::quarantine` |
| `readImportedSession` | portado | `game/store/city_saves.gd::open_file`, `ui/hud_actions.gd::open_file` |
| `readMapFile` | portado | `game/store/city_saves.gd::open_file`, `ui/hud_actions.gd::open_file` |
| `storeDelete` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `storeGet` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `storePut` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `storeRequest` | não se aplica | limite de tamanho do localStorage e armazém do navegador; o jogo grava em arquivo |
| `withUnit` | não se aplica | conversão de unidade de saves antigos do 4.0; o jogo não abre saves do 4.0 e guarda metros |

## src/editor/regionFrames.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `K_JSON,K_TABLE,K_ORDERS,K_PAIRS` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `REGION_CODEC` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `SquareRecords` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `readSquareFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `readValueFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `readWearFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `squareFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `valueFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |
| `wearFrame` | não se aplica | o 4.0 divide o save em ladrilhos porque grava no IndexedDB do navegador; o jogo grava um arquivo ZSTD em thread própria (`api/city_core_persist.cpp::save_city`) |

## src/editor/repair.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Candidate` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário restaura o grafo inteiro |
| `nearestCandidate` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário restaura o grafo inteiro |
| `repairNearConnections` | já existia | `sim/commands/edit_history.cpp::apply_entry`: o diário restaura o grafo inteiro |

## src/editor/saveBinary.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `FILE_MAGIC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `FILE_SETTINGS,FILE_DOC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `FORMAT` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `FrameSection` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `GLOBAL_MAGIC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `HEAD,ENTRY` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `MANIFEST_MAGIC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `SQUARE_MAGIC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `WEAR_MAGIC` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `align8` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `decoder` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `encoder` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `f64Bytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `f64Of` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `framePieces` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `gunzipBytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `gzipBytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `isGzip` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `jsonBytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `jsonOf` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `magicOf` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `packFrame` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |
| `readFrame` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city`: o formato binário próprio do jogo |

## src/editor/saveStore.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `GZIP_BATCH` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `PartStore` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `SAVES_DB` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `SAVES_STORE` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `SaveBatch` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `SaveEntry` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `SaveWriter` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `batchBuffers` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `gunzip` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `gzip` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `indexedDbParts` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `indexedDbParts.get` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `indexedDbParts.marks` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `indexedDbParts.transact` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `indexedDbParts.write` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `openSaves` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `portWriter` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `portWriter.failAll` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `portWriter.write` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `savesDb` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `storeWriter` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |
| `writeBatch` | não se aplica | armazém IndexedDB do navegador; o jogo grava em arquivo com troca atômica (`api/city_core_persist.cpp::write_atomic`) |

## src/editor/workerParts.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `markBytes` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `markTexts` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `portParts.call` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `portParts.get` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `portParts.write` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `workerParts.get` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `workerParts.open` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |
| `workerParts.write` | não se aplica | porta de mensagem com Web Worker; as threads do núcleo dividem memória (`api/city_core_persist.cpp::save_city`) |

## src/main.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `mapSlider.end` | outra sessão | sessão landscape: controle deslizante do clima, `ui` da gaveta Céu no branch `convert/landscape`, commit `ea41ebd` |
| `mapSlider.set` | outra sessão | sessão landscape: controle deslizante do clima, `ui` da gaveta Céu no branch `convert/landscape`, commit `ea41ebd` |

## src/render/canyonLight.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CELL` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `CanyonLight` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `FRAGMENT` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `MASK` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `OFFSET` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `SIDE` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `SNAP` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `STEP` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `WHOLE_ROWS` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `WINDOW` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.clearCells` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.dispose` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.follow` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.layVolume` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.markRow` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.setOcclusion` | portado | `game/settings/render_switches.gd::apply`: a chave `canyonLight`, parâmetro global `canyon_light` |
| `createCanyonLight.setSun` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |
| `createCanyonLight.upload` | outra sessão | sessão landscape: o passe é dela (`render/canyonLight.ts#*` em `boundary.json` de landscape); a interface só entrega a chave |

## src/render/effectSwitches.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `ALL_EFFECTS_ON` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |
| `BUILDING_SWITCHES` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |
| `BuildingChoices` | portado | `ui/effects_panel.gd::_switches` |
| `BuildingSwitch` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `DEFAULT_BUILDING_CHOICES` | portado | `ui/effects_panel.gd::_switches` |
| `EffectSwitch` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `EffectSwitches` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `FrameHider` | portado | `game/settings/render_switches.gd::cull_mask`: a máscara da câmera faz o papel de esconder por um quadro |
| `LAYER_SWITCHES` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |
| `LIGHT_SWITCHES` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |
| `LayerSwitch` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `LightSwitch` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `POST_SWITCHES` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |
| `PostSwitch` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `PostSwitches` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `createFrameHider` | portado | `game/settings/render_switches.gd::cull_mask`: a máscara da câmera faz o papel de esconder por um quadro |
| `createFrameHider.hide` | portado | `game/settings/render_switches.gd::cull_mask`: a máscara da câmera faz o papel de esconder por um quadro |
| `createFrameHider.restore` | portado | `game/settings/render_switches.gd::cull_mask`: a máscara da câmera faz o papel de esconder por um quadro |
| `isEffectSwitch` | portado | `game/settings/render_switches.gd::apply`, chaves em `data/game/render_switches.json` |

## src/render/environment.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `createEnvironment.remakeShadowMaps` | outra sessão | sessão landscape: `boundary.json` de landscape; nenhuma API do Godot atualiza a sombra direcional a cada N quadros |
| `createEnvironment.setFarCascadeEvery` | outra sessão | sessão landscape: `boundary.json` de landscape; nenhuma API do Godot atualiza a sombra direcional a cada N quadros |

## src/render/isoViewport.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BACK` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `DEFAULT_AZIMUTH` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `DEFAULT_ELEVATION` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `DISTANCE` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `GROUND_CLEARANCE` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `IsoRig` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `KEEP_PASSES` | portado | `game/camera/city_camera.gd::keeping` |
| `MAX_ELEVATION` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `MAX_HALF_HEIGHT` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `MIN_ELEVATION` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `MIN_HALF_HEIGHT` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `PERSPECTIVE_FOV` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `PULL_HOLD` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `PULL_IN` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `PULL_OUT` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `PULL_SAMPLES` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `PULL_SOFT` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `PULL_WAIT` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `TAU` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `clampElevation` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `createIsoRig` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `createIsoRig.apply` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `createIsoRig.askTilt` | portado | `game/camera/city_camera.gd::_ask_tilt` |
| `createIsoRig.hitAt` | portado | `game/camera/city_camera.gd::hit_at` |
| `createIsoRig.holdHeight` | portado | `game/camera/city_camera.gd::hold_height` |
| `createIsoRig.keeping` | portado | `game/camera/city_camera.gd::keeping` |
| `createIsoRig.resize` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `createIsoRig.setGround` | portado | `game/camera/city_camera.gd::_ground_height` |
| `createIsoRig.setPerspective` | portado | `game/camera/camera_profile.gd::lens` |
| `createIsoRig.setSolids` | portado | `game/camera/camera_pull.gd::settle`, consulta `api/city_core_inspect.cpp::clear_reach` |
| `createIsoRig.settle` | portado | `game/camera/city_camera.gd::keeping` |
| `createIsoRig.tiltNow` | portado | `game/camera/city_camera.gd::_ask_tilt` |
| `createIsoRig.turn` | portado | `game/camera/city_camera.gd::turn` |
| `createIsoRig.worldAt` | portado | `game/camera/city_camera.gd::hit_at` |
| `isoZoomBounds` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |
| `wrapAzimuth` | portado | `game/camera/city_camera.gd::_apply`, valores em `data/game/camera.json` |

## src/render/occlusion.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `LEAST_TEXELS` | portado | `render/buildings/occluders.cpp::add`, altura mínima em `data/catalog/tuning.json` `render.occluder_min_storeys` |
| `MOST_TEXELS_READ` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OCCLUDER_REACH` | portado | `render/buildings/building_renderer.cpp::drop`: as paredes acompanham o nível perto |
| `OcclusionBuffer` | portado | `render/buildings/occluders.cpp::add` |
| `OcclusionBuffer.build` | portado | `render/buildings/occluders.cpp::add` |
| `OcclusionBuffer.clear` | portado | `game/settings/render_switches.gd::apply` |
| `OcclusionBuffer.corners` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.edgeA` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.edgeB` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.edgeC` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.eyeX` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.eyeY` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.eyeZ` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.height` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.heights` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.hidden` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.laid` | portado | `game/settings/render_switches.gd::apply` |
| `OcclusionBuffer.layBox` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.layQuad` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.levels` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.near` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.project` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.reduce` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.resize` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.stats` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.view` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.vp` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.width` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionBuffer.widths` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `OcclusionStats` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |
| `WIDTH` | não se aplica | o buffer é do motor: `render/buildings/occluders.cpp::set` entrega as paredes ao Godot, que as rasteriza com Embree e testa as caixas (docs: tutorials/3d/occlusion_culling) |

## src/render/optimizations.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `AFFECTS` | não se aplica | mapa de "o que esta opção afeta" para o inspetor de objetos do laboratório, ferramenta do three.js sem par: o jogo não tem árvore de cena por objeto |
| `BASE` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `BaseId` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `BaseSpec` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `DEFAULT_OPTIMIZATIONS` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `OPTIMIZATIONS` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `OptId` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `OptTarget` | não se aplica | mapa de "o que esta opção afeta" para o inspetor de objetos do laboratório, ferramenta do three.js sem par: o jogo não tem árvore de cena por objeto |
| `OptimizationSpec` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `Optimizations` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `QUALITY_OPTIONS` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `TIER` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `affectsTarget` | não se aplica | mapa de "o que esta opção afeta" para o inspetor de objetos do laboratório, ferramenta do three.js sem par: o jogo não tem árvore de cena por objeto |
| `globMatch` | não se aplica | mapa de "o que esta opção afeta" para o inspetor de objetos do laboratório, ferramenta do three.js sem par: o jogo não tem árvore de cena por objeto |
| `isDefaultOptimizations` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `optimizationChoice` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `optimizationSpec` | portado | `ui/effects_panel.gd::_optimizations`, opções em `data/game/optimizations.json` |
| `qualityOptionsDiffer` | portado | `game/settings/settings.gd::apply_now` |
| `withOptimizations` | portado | `game/settings/settings.gd::apply_now` |
| `withOptimizations.num` | portado | `game/settings/settings.gd::apply_now` |

## src/render/optimizationsApply.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Optimizer` | portado | `game/settings/settings.gd::apply_now` |
| `OptimizerDeps` | portado | `game/settings/settings.gd::apply_now` |
| `REWALK_FRAMES` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `SortItem` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `createOptimizer` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.afterRender` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `createOptimizer.beforeRender` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `createOptimizer.dispose` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.due` | portado | `render/render_world.h::set_upload_every` |
| `createOptimizer.giveBack` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `createOptimizer.num` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.postSwitches` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.reliefDue` | não se aplica | pular passes de sombra e relevo: nenhuma API do Godot para a sombra direcional, e o relevo não é assado por quadro |
| `createOptimizer.report` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.set` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.setAnisotropy` | portado | `game/settings/settings.gd::apply_now` |
| `createOptimizer.shadowDue` | não se aplica | pular passes de sombra e relevo: nenhuma API do Godot para a sombra direcional, e o relevo não é assado por quadro |
| `createOptimizer.texturesOf` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `createOptimizer.uploadsDue` | portado | `render/render_world.h::set_upload_every` |
| `createOptimizer.walk` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `depthFirst` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |
| `sceneOrder` | não se aplica | percorre a cena do three.js a cada quadro para ordenar e trocar materiais; o Godot ordena sozinho e o jogo não tem nós por objeto |

## src/render/optimizationsQuality.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `FIELDS` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `Field` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `FieldSpec` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `Kind` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `QUALITY_FIELD_OPTIONS` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `QualityOptId` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `QualityOptionSpec` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `qualityFieldValue` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `rangeOf` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |
| `withQualityFields` | portado | `ui/effects_panel.gd::_optimizations`, `game/settings/quality.gd::merged` |

## src/render/quality.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DOWN_WINDOWS` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `GEOMETRY_BUDGETS` | não se aplica | orçamento de geometria por nível: o streaming do 4.0 despeja por MB; o jogo limita por alcance (`render/buildings/building_renderer.cpp::set_detail_scale`), e a medida por nível vai no relatório |
| `GeometryBudget` | não se aplica | orçamento de geometria por nível: o streaming do 4.0 despeja por MB; o jogo limita por alcance (`render/buildings/building_renderer.cpp::set_detail_scale`), e a medida por nível vai no relatório |
| `HAIR_STRAND_PEOPLE` | não se aplica | mechas de cabelo: `render/people/hairTexture.ts#hairStrandTexture` é de people; o jogo não tem mechas |
| `HOLD` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `NETWORK_FAR_MB` | não se aplica | orçamento de geometria por nível: o streaming do 4.0 despeja por MB; o jogo limita por alcance (`render/buildings/building_renderer.cpp::set_detail_scale`), e a medida por nível vai no relatório |
| `QUALITY` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `QUALITY_LEVELS` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `QualityGovernor` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.ceiling` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.clock` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.constructor` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.costs` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.count` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.current` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.hold` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.holdUntil` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.lastRaise` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.missed` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.roomy` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.sample` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityGovernor.set` | portado | `game/settings/frame_governor.gd::set_level` |
| `QualityGovernor.skip` | portado | `game/settings/frame_governor.gd::set_level` |
| `QualityGovernor.sorted` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `QualityLevel` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `QualitySettings` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `SETTLE` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `STALL` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `UNDONE_WITHIN` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `UP_SHARE` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `UP_WINDOWS` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `VEHICLE_ERROR_PIXELS` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `VEHICLE_WHOLE_CAP` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `WINDOW` | portado | `game/settings/frame_governor.gd::sample`, números em `data/game/settings.json` `governor` |
| `isQualityLevel` | portado | `data/game/settings.json` `tiers`, `game/settings/quality.gd::apply` |
| `roadBudgetMB` | não se aplica | orçamento de geometria por nível: o streaming do 4.0 despeja por MB; o jogo limita por alcance (`render/buildings/building_renderer.cpp::set_detail_scale`), e a medida por nível vai no relatório |

## src/render/revealGate.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `MAX_GAP_MS` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `REVEAL_LIMIT_MS` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.begin` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.last` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.opened` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.step` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.warmed` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.work` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealGate.worked` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |
| `RevealStep` | portado | `ui/start_screen.gd::_process`, limite em `data/game/start.json` |

## src/render/sunCascades.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `createSunCascades.remakeMaps` | outra sessão | sessão landscape: `boundary.json` de landscape |
| `createSunCascades.setFarEvery` | outra sessão | sessão landscape: `boundary.json` de landscape |

## src/sim/host/simClient.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `SimClient.ask` | outra sessão | sessão roads: `boundary.json` de roads; aqui a consulta é direta |
| `SimClient.junction` | portado | `api/city_core_inspect.cpp::node_traffic` |

## src/steamAchievements.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `ROAD_MILESTONES` | não se aplica | conquistas só com a casca Electron e o Steam; o jogo não liga a biblioteca do Steam e não há pacote Steam a provar |
| `UnlockBridge` | não se aplica | conquistas só com a casca Electron e o Steam; o jogo não liga a biblioteca do Steam e não há pacote Steam a provar |
| `createSteamAchievements` | não se aplica | conquistas só com a casca Electron e o Steam; o jogo não liga a biblioteca do Steam e não há pacote Steam a provar |
| `createSteamAchievements.roads` | não se aplica | conquistas só com a casca Electron e o Steam; o jogo não liga a biblioteca do Steam e não há pacote Steam a provar |

## src/ui/about.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `mountAbout` | portado | `ui/hud_dialog.gd::fill` |
| `mountAbout.paint` | portado | `ui/hud_dialog.gd::fill` |
| `openAbout` | portado | `ui/hud_dialog.gd::fill` |

## src/ui/buildStamp.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BAKED` | portado | `game/flow/build_stamp.gd::text` |
| `BuildStamp` | portado | `game/flow/build_stamp.gd::text` |
| `__BUILD_STAMP__` | portado | `game/flow/build_stamp.gd::text` |
| `formatBuildStamp` | portado | `game/flow/build_stamp.gd::text` |
| `listeners` | não se aplica | o carimbo do navegador muda com uma nova versão sem recarregar; o executável do jogo não muda em execução |
| `onBuildStamp` | não se aplica | o carimbo do navegador muda com uma nova versão sem recarregar; o executável do jogo não muda em execução |
| `refreshBuildStamp` | não se aplica | o carimbo do navegador muda com uma nova versão sem recarregar; o executável do jogo não muda em execução |
| `stamp` | portado | `game/flow/build_stamp.gd::text` |

## src/ui/builder/workspace.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `initBuilderWorkspace.inside` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |
| `initBuilderWorkspace.onInside` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |
| `initBuilderWorkspace.subscribe` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |
| `initBuilderWorkspace.thumbnail` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |

## src/ui/chrome.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `focusCameFromKeyboard` | já existia | o Godot só desenha o foco quando ele veio do teclado (`Control` e `Viewport.gui_get_focus_owner`), sem código nosso |
| `tabbing` | já existia | o Godot só desenha o foco quando ele veio do teclado (`Control` e `Viewport.gui_get_focus_owner`), sem código nosso |
| `trackFocusModality` | já existia | o Godot só desenha o foco quando ele veio do teclado (`Control` e `Viewport.gui_get_focus_owner`), sem código nosso |

## src/ui/effectsPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `EffectsPanel` | portado | `ui/effects_panel.gd::_render` |
| `EffectsPanelDeps` | portado | `ui/effects_panel.gd::_render` |
| `EffectsPanelGroup` | portado | `ui/effects_panel.gd::_render` |
| `EffectsPanelItem` | portado | `ui/effects_panel.gd::_render` |
| `RESOLUTIONS` | portado | `ui/effects_panel.gd::_read_stats`, `data/game/render_switches.json` |
| `STATS_MS` | portado | `ui/effects_panel.gd::_read_stats`, `data/game/render_switches.json` |
| `createEffectsPanel` | portado | `ui/effects_panel.gd::_render` |
| `createEffectsPanel.button` | portado | `ui/effects_panel.gd::_group` |
| `createEffectsPanel.render` | portado | `ui/effects_panel.gd::_render` |
| `createEffectsPanel.renderStats` | portado | `ui/effects_panel.gd::_read_stats`, `data/game/render_switches.json` |
| `createEffectsPanel.startStats` | portado | `ui/effects_panel.gd::_read_stats`, `data/game/render_switches.json` |
| `createEffectsPanel.stopStats` | portado | `ui/effects_panel.gd::_read_stats`, `data/game/render_switches.json` |

## src/ui/graphicsPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `FPS` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `GraphicsChoices` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `GraphicsPanelDeps` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `PRESETS` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `PresetName` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `SHADOWS` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `createGraphicsPanel` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `createGraphicsPanel.choiceRow` | portado | `ui/graphics_panel.gd::_row` |
| `createGraphicsPanel.onOff` | portado | `ui/graphics_panel.gd::_on_off` |
| `createGraphicsPanel.open` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |
| `createGraphicsPanel.render` | portado | `ui/graphics_panel.gd::fill`, listas em `data/game/settings.json` |

## src/ui/healthPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CLOCK` | portado | `ui/health_panel.gd::_render` |
| `HealthPanelSources` | portado | `ui/health_panel.gd::_render` |
| `ICON` | portado | `ui/health_panel.gd::_render` |
| `ROWS` | portado | `ui/health_panel.gd::_render` |
| `STYLE` | portado | `ui/health_panel.gd::_render` |
| `TABS` | portado | `ui/health_panel.gd::_render` |
| `Tab` | portado | `ui/health_panel.gd::_render` |
| `clock` | portado | `ui/health_panel.gd::_render` |
| `mountHealthPanel` | portado | `ui/health_panel.gd::_render` |
| `mountHealthPanel.close` | portado | `ui/health_panel.gd::close` |
| `mountHealthPanel.label` | portado | `ui/health_panel.gd::_render` |
| `mountHealthPanel.place` | portado | `ui/health_panel.gd::_render` |
| `mountHealthPanel.render` | portado | `ui/health_panel.gd::_render` |
| `mountHealthPanel.renderHealth` | portado | `ui/health_panel.gd::_render_health` |
| `mountHealthPanel.renderLines` | portado | `ui/health_panel.gd::_lines` |
| `mountHealthPanel.said` | portado | `ui/health_panel.gd::_render_health` |
| `mountHealthPanel.select` | portado | `ui/health_panel.gd::_select` |
| `mountHealthPanel.toggle` | portado | `ui/health_panel.gd::toggle` |

## src/ui/healthWatch.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `HealthWatch` | portado | `game/health/health_log.gd::_process` |
| `KEPT_KEY` | portado | `game/health/health_log.gd::_save` |
| `LongFrameEntry` | portado | `game/health/health_log.gd::_process` |
| `WatchedWorker.constructor` | não se aplica | relógio de Web Worker; as threads do núcleo entram por `api/city_core_probe.cpp::render_cost` |
| `longFrameWork` | portado | `game/health/health_log.gd::_slow_frame` |
| `safeJSON` | portado | `game/health/health_log.gd::_save` |
| `stackOf` | portado | `game/health/health_log.gd::_log_error` |
| `stepsWithin` | portado | `game/health/health_log.gd::_slow_frame` |
| `systemName` | portado | `game/health/health_log.gd::_slow_frame` |
| `textOf` | portado | `game/health/health_log.gd::_log_error` |
| `watchHealth` | portado | `game/health/health_log.gd::_process` |
| `watchHealth.frameEnded` | portado | `game/health/health_log.gd::_process` |
| `watchHealth.save` | portado | `game/health/health_log.gd::_save` |
| `watchHealth.slowFrame` | portado | `game/health/health_log.gd::_slow_frame` |
| `watchHealth.write` | portado | `game/health/health_log.gd::_save` |

## src/ui/i18n/en.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `EN` | portado | `data/i18n/strings*.csv`: coluna en das chaves com tela no jogo; a contagem das que faltam vai na cobertura |

## src/ui/i18n/index.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DICTIONARIES` | portado | `game/store/formatters.gd::tr_static` |
| `Dictionary` | portado | `game/store/formatters.gd::tr_static` |
| `LANGUAGES` | portado | `game/settings/settings.gd::set_language` |
| `LanguageCode` | portado | `game/settings/settings.gd::set_language` |
| `LanguageSpec` | portado | `game/settings/settings.gd::set_language` |
| `STORAGE_KEY` | portado | `game/settings/settings.gd::set_language` |
| `applyTranslations` | portado | `ui/roadcraft_hud.gd::_notification` |
| `current` | portado | `game/settings/settings.gd::set_language` |
| `decimalFormats` | portado | `game/store/formatters.gd::format_value` |
| `detect` | portado | `game/flow/game_flow.gd::_system_locale` |
| `formatDecimal` | portado | `game/store/formatters.gd::format_value` |
| `globalParams` | portado | `game/store/formatters.gd::tr_static` |
| `initLanguage` | portado | `game/settings/settings.gd::set_language` |
| `isLanguage` | portado | `game/settings/settings.gd::set_language` |
| `language` | portado | `game/settings/settings.gd::set_language` |
| `listeners` | portado | `ui/roadcraft_hud.gd::_notification` |
| `onLanguageChange` | portado | `ui/roadcraft_hud.gd::_notification` |
| `parseDecimal` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |
| `plural` | portado | `game/store/formatters.gd::count_label` |
| `setGlobalParams` | portado | `game/store/formatters.gd::tr_static` |
| `setLanguage` | portado | `game/settings/settings.gd::set_language` |
| `t` | portado | `game/store/formatters.gd::tr_static` |

## src/ui/i18n/pt-BR.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `PT_BR` | portado | `data/i18n/strings*.csv`: coluna pt_BR das chaves com tela no jogo |

## src/ui/inspector.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `InspectorActions` | portado | `ui/inspector_panel.gd::_road` |
| `InspectorContext` | portado | `ui/inspector_panel.gd::show_info` |
| `InspectorSelection` | portado | `ui/inspector_panel.gd::show_info` |
| `actionsForNode` | portado | `ui/inspector_panel.gd::_node` |
| `builtFor` | portado | `ui/inspector_panel.gd::show_info` |
| `card` | portado | `ui/inspector_panel.gd::_rows` |
| `closeInspector` | portado | `ui/inspector_panel.gd::close` |
| `current` | portado | `ui/inspector_panel.gd::show_info` |
| `curveText` | outra sessão | sessão roads: curva, ponta da via e faixas por seção, `ui/roads/*` em `boundary.json` de roads |
| `directionOptions` | portado | `ui/inspector_panel.gd::_road` |
| `endSelect` | outra sessão | sessão roads: curva, ponta da via e faixas por seção, `ui/roads/*` em `boundary.json` de roads |
| `grid` | portado | `ui/inspector_panel.gd::_rows` |
| `heightStepper` | outra sessão | sessão road-elevation: escopo "Pontes, túneis, viadutos e greide"; `doc.heightOffset` não existe no jogo |
| `junctionHost` | outra sessão | sessão roads: curva, ponta da via e faixas por seção, `ui/roads/*` em `boundary.json` de roads |
| `junctionHost.setControl` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `junctionHost.setHeight` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `junctionHost.setMovementBlocked` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `junctionHost.setRules` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `junctionHost.setSignal` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `junctionHost.setSignals` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `laneOptions` | outra sessão | sessão roads: curva, ponta da via e faixas por seção, `ui/roads/*` em `boundary.json` de roads |
| `meters` | portado | `ui/inspector_rows.gd::segment_rows`, `api/city_core_inspect.cpp::segment_traffic` |
| `nodeStats` | portado | `ui/inspector_rows.gd::node_rows`, `api/city_core_inspect.cpp::node_traffic` |
| `onInspectorChange` | portado | `ui/inspector_panel.gd::close` |
| `openInspector` | portado | `ui/inspector_panel.gd::show_info` |
| `openListeners` | portado | `ui/inspector_panel.gd::close` |
| `openNow` | portado | `ui/inspector_panel.gd::close` |
| `parkingSelect` | outra sessão | sessão traffic: escopo "Estacionar (rua e lote)" |
| `percent` | outra sessão | sessão roads: curva, ponta da via e faixas por seção, `ui/roads/*` em `boundary.json` de roads |
| `phaseLabel` | portado | `ui/inspector_rows.gd::node_rows`, `api/city_core_inspect.cpp::node_traffic` |
| `refreshInspector` | portado | `ui/inspector_panel.gd::refresh` |
| `renderCurrent` | portado | `ui/inspector_panel.gd::show_info` |
| `renderNode` | portado | `ui/inspector_panel.gd::_node` |
| `renderSegment` | portado | `ui/inspector_panel.gd::_road` |
| `renderSegment.setParking` | outra sessão | sessão traffic: escopo "Estacionar (rua e lote)" |
| `segmentStats` | portado | `ui/inspector_rows.gd::segment_rows`, `api/city_core_inspect.cpp::segment_traffic` |
| `setTitle` | portado | `ui/inspector_panel.gd::_road` |
| `signalLabel` | portado | `ui/inspector_rows.gd::node_rows`, `api/city_core_inspect.cpp::node_traffic` |
| `tellOpen` | portado | `ui/inspector_panel.gd::close` |

## src/ui/labels.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `nodeCountLabel` | portado | `game/store/formatters.gd::count_label` |
| `peopleCountLabel` | portado | `game/store/formatters.gd::count_label` |
| `roadCountLabel` | portado | `game/store/formatters.gd::count_label` |
| `roadTypeDescription` | portado | `ui/road_catalog.gd::_card` |
| `roadTypeName` | portado | `ui/road_catalog.gd::_card` |
| `vehicleCountLabel` | portado | `game/store/formatters.gd::count_label` |

## src/ui/materialSwatch.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BASE` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `CACHE` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `draw` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `draw.line` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `materialSwatch` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `shade` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |
| `shade.mix` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: amostra de material do Construir |

## src/ui/planSwatch.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CACHE` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: figura de planta do Construir |
| `OUTLINES` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: figura de planta do Construir |
| `planSwatch` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: figura de planta do Construir |

## src/ui/profilerHud.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `amber` | portado | `ui/profiler_hud.gd::_refresh` |
| `esc` | portado | `ui/profiler_hud.gd::_refresh` |
| `head` | portado | `ui/profiler_hud.gd::_refresh` |
| `merge` | portado | `ui/profiler_hud.gd::_refresh` |
| `mountProfilerHud` | portado | `ui/profiler_hud.gd::_refresh` |
| `mountProfilerHud.toggle` | portado | `ui/profiler_hud.gd::toggle` |
| `red` | portado | `ui/profiler_hud.gd::_refresh` |
| `render` | portado | `ui/profiler_hud.gd::_refresh` |
| `render.e` | portado | `ui/profiler_hud.gd::_refresh` |
| `render.n` | portado | `ui/profiler_hud.gd::_refresh` |
| `render.peak` | portado | `ui/profiler_hud.gd::_refresh` |

## src/ui/roadSectionEditor.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `mountRoadSectionEditor` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountRoadSectionEditor.draw` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountRoadSectionEditor.draw.band` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/catalogPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BuiltStrip` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `CatalogHost` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `allCatalogRoads` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `catalogStrip` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `customButton` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `markPicked` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `nameOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `pickRoad` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `pickedRoad` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `sameRoad` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `strip` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/connectorPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `ConnectorPanelHost` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `Picked` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `draw` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `draw.at` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `draw.keyOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `draw.press` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `draw.view` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountConnectorPanel` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `panel` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `refreshConnectorPanel` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `turnOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `turnOf.dir` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/crossSection.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `COLOUR` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `ICON_Y` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `SectionDrawing` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `SectionOptions` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `arrow` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `bicycleGlyph` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `busRear` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `carRear` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `carRear.v` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `carriageway` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `cyclist` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `decoration` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `decoration.car` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `drawSection` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `elementColour` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `esc` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `laneLine` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `laneLine.road` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `metresText` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `person` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `r1` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `raised` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `surface` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `tree` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/cutWalls.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `setCutWalls` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/furnitureChoice.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `setFurnitureChosen` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/junctionPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CONTROLS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `JunctionPanelHost` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `RULES` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `button` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `compassOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `div` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `flowOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `flowsShown` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `fmt` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `head` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountJunctionPanel` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountJunctionPanel.set` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `moveChip` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `refreshJunctionFlows` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `row` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `rowOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `screenOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `segmented` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `stepper` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `toggle` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `withoutFixed` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/keys.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `bindableKey` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `resetRoadKeys` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `roadKey` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `save` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `setRoadKey` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/keysPanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `closeRoadKeys` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `open` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openRoadKeys` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openRoadKeys.onKey` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openRoadKeys.render` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `roadKeysOpen` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/money.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `balanceTip` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/profileEditor.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `EdgeDrag` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `ICON` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `MoveDrag` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `PALETTE` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `PaletteKind` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `ProfileEditorOptions` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `WIDTH_LIMITS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `button` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `closeProfileEditor` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `div` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `icon` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `iconButton` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `insertionIndex` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `insertionIndex.clamp` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `insertionIndex.firstIndex` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `insertionIndex.kerbBand` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `insertionIndex.lastIndex` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `moved` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `newElement` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openEditor` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.drawStage` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.endDrag` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.issueLine` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.issues` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.moveSelected` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.place` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.removeSelected` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.renderAll` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.renderCards` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.renderCards.card` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.renderFoot` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.renderProps` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.replace` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.sameAs` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.save` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.stageSize` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `openProfileEditor.svgX` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `profileEditorOpen` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `row` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `segmented` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `stepper` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `toggle` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `withWidth` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/profilePanel.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `mountProfilePanel` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `mountProfilePanel.hold` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/profileSwatch.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `ASPHALT` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `BUS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `CARS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `CYCLE` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `FOOTWAY` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `GRASS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `KERB` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `MEDIAN_GRASS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `WHITE` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `YELLOW` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `profileSwatch` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/roads/templates.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `deleteTemplate` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `inThisUnit` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `kept` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `readTemplate` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `readable` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `saveTemplate` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `savedTemplates` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `withUnit` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/ui/thunder.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Audio` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |
| `LONGEST` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |
| `NOISE_SECONDS` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |
| `audio` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |
| `noiseOf` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |
| `playThunder` | portado | `render/thunder.gd::play`, dono da Porte 3, com o volume e o início sorteado do 4.0 |

## src/ui/toolChoices.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CLOUD_BRUSH_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `CLOUD_MODES` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `CloudBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `CloudMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `ELEMENT_BRUSH_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `ElementBrushMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `FOG_BRUSH_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `FogBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `KIND_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `LAMP_KEY` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `PAINT_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `POLE_TOOL_MODES` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `PoleToolMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `ROAD_GRID_KEY` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `ROAD_WIDTH_RANGE` | outra sessão | sessão roads: largura da via e grade de quarteirões da gaveta Vias |
| `TREE_BRUSH_KEY` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `TREE_MODES` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `TreeMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `ZONE_COLOUR_KEY` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |
| `blockGridChoice` | outra sessão | sessão roads: largura da via e grade de quarteirões da gaveta Vias |
| `cloudBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `cloudBrushNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `cloudMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `cloudModeNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementBrushes` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementKindNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `elementModeNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `fogBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `fogBrushNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `fogErase` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `fogErasing` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `gridListeners` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `gullyErase` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `gullyErasing` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `keep` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |
| `kind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `lamps` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `onRoadGridChange` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `paint` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `paintKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `poleLampMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `poleMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `poleToolMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `roadGrid` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `roadGridShown` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `roadWidth` | outra sessão | sessão roads: largura da via e grade de quarteirões da gaveta Vias |
| `setCloudBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setCloudMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setElementBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setElementKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setElementMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setFogBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setFogErase` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setGullyErase` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setPaintKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setPoleLampMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `setPoleToolMode` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `setRoadGridShown` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::set_layer` |
| `setRoadWidth` | outra sessão | sessão roads: largura da via e grade de quarteirões da gaveta Vias |
| `setStreetscapeKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setTreeBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setTreeKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setTreeMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `setZoneColoursShown` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |
| `signChoice` | outra sessão | sessão street-furniture: escopo "Postes, fios, barreiras, placas e semáforos da rua" |
| `stored` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |
| `streetscapeKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `strikeChoice` | outra sessão | sessão demolition: pistola e bomba, próximo porte desta sessão |
| `treeBrush` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `treeBrushNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `treeKind` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `treeKindNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `treeMode` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `treeModeNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `width` | outra sessão | sessão roads: largura da via e grade de quarteirões da gaveta Vias |
| `zoneColours` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |
| `zoneColoursShown` | portado | `ui/hud_popover.gd::_build_layers`, `game/settings/settings.gd::keep` |

## src/ui/v2/compass.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CLICK_SLOP` | portado | `ui/compass.gd::_gui_input` |
| `Compass` | portado | `ui/compass.gd::_gui_input` |
| `CompassHandlers` | portado | `ui/compass.gd::_gui_input` |
| `DRAG_TILT` | portado | `ui/compass.gd::_gui_input` |
| `DRAG_TURN` | portado | `ui/compass.gd::_gui_input` |
| `groundAngleToNorth` | portado | `ui/compass.gd::_process` |
| `mountCompass` | portado | `ui/compass.gd::_gui_input` |
| `mountCompass.end` | portado | `ui/compass.gd::_gui_input` |
| `mountCompass.label` | portado | `ui/compass.gd::_process` |
| `mountCompass.point` | portado | `ui/compass.gd::_gui_input` |
| `northOnScreen` | portado | `ui/compass.gd::_process` |
| `northTurn` | portado | `ui/compass.gd::_process` |
| `toNorth` | portado | `game/camera/camera_motion.gd::aim_north` |

## src/ui/v2/shell.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BIOME_SWATCHES` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `Category` | portado | `ui/roadcraft_hud.gd::_build_dock` |
| `ICON` | portado | `ui/hud_widgets.gd::icon` |
| `PAINT_SWATCH` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `SWATCHES` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `ShellDeps` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `ShellGame` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `ShellGameValues` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `ShellHandle` | portado | `ui/roadcraft_hud.gd::_ready` |
| `ShellReadouts` | portado | `ui/hud_popover.gd::_build_camera` |
| `ShellRoadClass` | não se aplica | tipo do TypeScript: não existe em tempo de execução; o contrato no jogo é o dicionário do dono |
| `ShellSlider` | portado | `ui/hud_widgets.gd::range_row` |
| `ShellStatus` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `ShellTransitTool` | outra sessão | sessão transit: escopo "com as ferramentas do jogador" |
| `ShellZonePatch` | portado | `ui/hud_drawer.gd::_zone` |
| `TransitToolKind` | outra sessão | sessão transit: escopo "com as ferramentas do jogador" |
| `button` | portado | `ui/hud_widgets.gd::button` |
| `el` | portado | `ui/hud_widgets.gd::label` |
| `hexOf` | não se aplica | conversão de cor para CSS; as cores vêm de `ui/theme/theme_builder.gd` |
| `mountShell` | portado | `ui/roadcraft_hud.gd::_ready` |
| `mountShell.BARRIER_ICON` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.addToBar` | outra sessão | sessão roads: a linha "Adicionar à barra" é de `ui/roads/catalogPanel.ts` |
| `mountShell.ask` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.card` | portado | `ui/hud_drawer.gd::_build_strip` |
| `mountShell.cardRows` | portado | `ui/hud_drawer.gd::_build_strip` |
| `mountShell.categoryOf` | portado | `ui/roadcraft_hud.gd::_build_dock` |
| `mountShell.choices` | portado | `ui/hud_widgets.gd::choices` |
| `mountShell.closeDropdown` | portado | `ui/hud_widgets.gd::dropdown` |
| `mountShell.closePop` | portado | `ui/hud_popover.gd::toggle` |
| `mountShell.cutShort` | portado | `ui/hud_tip.gd::_process` |
| `mountShell.dockName` | portado | `ui/roadcraft_hud.gd::_build_dock` |
| `mountShell.dropdown` | portado | `ui/hud_widgets.gd::dropdown` |
| `mountShell.elementRows` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.fadeAll` | portado | `ui/road_catalog.gd::_fade_edges` |
| `mountShell.fadeEdges` | portado | `ui/road_catalog.gd::_fade_edges` |
| `mountShell.fillPop` | portado | `ui/hud_popover.gd::toggle` |
| `mountShell.fitDrawer` | portado | `ui/hud_drawer.gd::_fit` |
| `mountShell.group` | portado | `ui/hud_widgets.gd::option_row` |
| `mountShell.isEffect` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.keyList` | portado | `ui/hud_widgets.gd::kbd` |
| `mountShell.landTabOf` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.later` | portado | `ui/hud_drawer.gd::_fit` |
| `mountShell.mapAction` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.nameShown` | portado | `ui/hud_tip.gd::_process` |
| `mountShell.namedRow` | portado | `ui/hud_widgets.gd::option_row` |
| `mountShell.note` | portado | `ui/hud_popover.gd::_build_menu` |
| `mountShell.numberText` | portado | `ui/hud_widgets.gd::range_row` |
| `mountShell.openLandTab` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.openTool` | portado | `ui/roadcraft_hud.gd::open_category` |
| `mountShell.orow` | portado | `ui/hud_widgets.gd::option_row` |
| `mountShell.picture` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.popBody` | portado | `ui/hud_popover.gd::toggle` |
| `mountShell.range` | portado | `ui/hud_widgets.gd::range_row` |
| `mountShell.range.show` | portado | `ui/hud_widgets.gd::range_row` |
| `mountShell.readoutValue` | portado | `ui/hud_popover.gd::_build_camera` |
| `mountShell.readyOf` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.render` | portado | `ui/hud_drawer.gd::open` |
| `mountShell.renderBuild` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.renderLandscape` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.renderLandscape.brushRows` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.renderLandscape.run` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.renderLandscape.terrainCard` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.renderModels` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.renderModels.fill` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.renderRoads` | outra sessão | sessão roads: gaveta Vias, `ui/road_options.gd` no branch `convert/roads` |
| `mountShell.renderSection` | portado | `ui/hud_drawer.gd::open` |
| `mountShell.renderSimple` | portado | `ui/hud_drawer.gd::open` |
| `mountShell.renderTransit` | outra sessão | sessão transit: escopo "com as ferramentas do jogador" |
| `mountShell.renderZones` | portado | `ui/hud_drawer.gd::_zone` |
| `mountShell.row` | portado | `ui/hud_widgets.gd::option_row` |
| `mountShell.section` | portado | `ui/hud_drawer.gd::open` |
| `mountShell.seeInside` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.seg` | portado | `ui/hud_widgets.gd::choices` |
| `mountShell.showStatus` | portado | `ui/roadcraft_hud.gd::show_hint` |
| `mountShell.sigOf` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.stepper` | portado | `ui/hud_widgets.gd::range_row` |
| `mountShell.stopReadouts` | portado | `ui/hud_popover.gd::_build_camera` |
| `mountShell.subhead` | portado | `ui/hud_popover.gd::_build_menu` |
| `mountShell.swatchArt` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.syncHistory` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `mountShell.syncImpossible` | portado | `ui/hud_top_bar.gd::_sync_impossible`, `api/city_core_inspect.cpp::impossible_nodes` |
| `mountShell.syncInside` | outra sessão | sessão build: `boundary.json` de build tem esta declaração: gaveta Construir; a interface deixa as abas "em construção" por `ui/hud_widgets.gd::soon` |
| `mountShell.syncMetrics` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `mountShell.syncMoney` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `mountShell.syncRanges` | portado | `ui/hud_widgets.gd::range_row` |
| `mountShell.syncReadouts` | portado | `ui/hud_popover.gd::_build_camera` |
| `mountShell.syncSide` | portado | `ui/roadcraft_hud.gd::_on_inspector` |
| `mountShell.syncSky` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `mountShell.syncSpeed` | portado | `ui/hud_top_bar.gd::_on_changed` |
| `mountShell.tab` | portado | `ui/hud_drawer.gd::_build_strip` |
| `mountShell.terrainModeNow` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.textInput` | não se aplica | campo de texto das paradas e linhas da aba Transporte: sem transporte no jogo |
| `mountShell.tipCheck` | portado | `ui/hud_tip.gd::_process` |
| `mountShell.toggle` | portado | `ui/hud_widgets.gd::dropdown` |
| `mountShell.tool` | portado | `ui/roadcraft_hud.gd::open_category` |
| `mountShell.toolButton` | portado | `ui/hud_widgets.gd::button` |
| `mountShell.toolButton.run` | portado | `ui/hud_widgets.gd::button` |
| `mountShell.toolKeyed` | portado | `ui/roadcraft_hud.gd::open_category` |
| `mountShell.toolKeys` | portado | `ui/hud_widgets.gd::kbd` |
| `mountShell.updateHint` | portado | `ui/roadcraft_hud.gd::show_hint` |
| `mountShell.useBarrier` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.useElement` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.useTerrain` | outra sessão | sessão landscape: gaveta já no branch `convert/landscape`, commit `ea41ebd` |
| `mountShell.useTool` | portado | `ui/roadcraft_hud.gd::open_category` |
| `mountShell.verb` | portado | `ui/hud_widgets.gd::button` |
| `svg` | portado | `ui/hud_widgets.gd::icon` |

## src/ui/v2/start.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `StartDeps` | portado | `ui/start_screen.gd::_fill` |
| `mountStart` | portado | `ui/start_screen.gd::_fill` |
| `mountStart.close` | portado | `ui/start_screen.gd::_action` |
| `mountStart.fill` | portado | `ui/start_screen.gd::_fill` |
| `mountStart.fill.action` | portado | `ui/start_screen.gd::_action` |
| `mountStart.keys` | portado | `ui/start_screen.gd::_unhandled_key_input` |

## src/ui/v2/transitLine.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `transitLineRow` | outra sessão | sessão transit: `boundary.json` de transit |

## src/view/camera.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Camera` | portado | `game/camera/city_camera.gd::px_per_m` |
| `Camera.fit` | portado | `game/camera/city_camera.gd::jump` |
| `Camera.matrix` | não se aplica | projeção do mapa 2D em canvas; o Godot projeta por `Camera3D.unproject_position` |
| `Camera.px` | portado | `game/camera/city_camera.gd::px_per_m` |
| `Camera.screenToWorld` | portado | `game/camera/city_camera.gd::ground_at` |
| `Camera.viewBounds` | não se aplica | projeção do mapa 2D em canvas; o Godot projeta por `Camera3D.unproject_position` |
| `Camera.worldToScreen` | não se aplica | projeção do mapa 2D em canvas; o Godot projeta por `Camera3D.unproject_position` |
| `Camera.x` | portado | `game/camera/city_camera.gd::jump` |
| `Camera.y` | portado | `game/camera/city_camera.gd::jump` |
| `Camera.zoom` | portado | `game/camera/city_camera.gd::px_per_m` |
| `Camera.zoomAt` | portado | `game/camera/city_camera.gd::zoom_at` |
| `MAX_ZOOM` | portado | `game/camera/city_camera.gd::px_per_m` |
| `MIN_ZOOM` | portado | `game/camera/city_camera.gd::px_per_m` |

## src/view/cameraGestures.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CLICK_SLOP` | portado | `game/camera/camera_gestures.gd::press` |
| `COAST_DECAY_S` | portado | `game/camera/camera_gestures.gd::advance` |
| `COAST_MAX_STEP_S` | portado | `game/camera/camera_gestures.gd::advance` |
| `COAST_MIN_ORBIT` | portado | `game/camera/camera_gestures.gd::_throw` |
| `COAST_MIN_PAN` | portado | `game/camera/camera_gestures.gd::_throw` |
| `CameraGestures` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.active` | portado | `game/camera/camera_gestures.gd::active` |
| `CameraGestures.advance` | portado | `game/camera/camera_gestures.gd::advance` |
| `CameraGestures.anchor` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.cancel` | portado | `game/camera/camera_gestures.gd::cancel` |
| `CameraGestures.coast` | portado | `game/camera/camera_gestures.gd::advance` |
| `CameraGestures.coasting` | portado | `game/camera/camera_gestures.gd::active` |
| `CameraGestures.constructor` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.gesture` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.handSpeed` | portado | `game/camera/camera_gestures.gd::_hand_speed` |
| `CameraGestures.has` | portado | `game/camera/camera_gestures.gd::move` |
| `CameraGestures.move` | portado | `game/camera/camera_gestures.gd::move` |
| `CameraGestures.orbit` | portado | `game/camera/camera_gestures.gd::move` |
| `CameraGestures.pan` | portado | `game/camera/camera_gestures.gd::move` |
| `CameraGestures.pinch` | portado | `game/camera/camera_gestures.gd::touch` |
| `CameraGestures.pinching` | portado | `game/camera/camera_gestures.gd::touch` |
| `CameraGestures.pointers` | portado | `game/camera/camera_gestures.gd::move` |
| `CameraGestures.press` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.pressedAt` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.release` | portado | `game/camera/camera_gestures.gd::release` |
| `CameraGestures.releaseAll` | portado | `game/camera/camera_gestures.gd::release` |
| `CameraGestures.samples` | portado | `game/camera/camera_gestures.gd::_track` |
| `CameraGestures.startOrbit` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.startPan` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.startPinch` | portado | `game/camera/camera_gestures.gd::touch` |
| `CameraGestures.startTrack` | portado | `game/camera/camera_gestures.gd::press` |
| `CameraGestures.throwCoast` | portado | `game/camera/camera_gestures.gd::_throw` |
| `CameraGestures.touches` | portado | `game/camera/camera_gestures.gd::touch` |
| `CameraGestures.track` | portado | `game/camera/camera_gestures.gd::_track` |
| `CameraGesturesHost` | portado | `game/camera/camera_gestures.gd::press` |
| `Coast` | portado | `game/camera/camera_gestures.gd::advance` |
| `MAX_FLING_PX_S` | portado | `game/camera/camera_gestures.gd::_throw` |
| `ORBIT_PER_PX` | portado | `game/camera/camera_gestures.gd::press` |
| `PointerSample` | portado | `game/camera/camera_gestures.gd::press` |
| `SPEED_HORIZON_MS` | portado | `game/camera/camera_gestures.gd::_hand_speed` |
| `SPEED_STOPPED_MS` | portado | `game/camera/camera_gestures.gd::_hand_speed` |
| `TWIST_MIN_SPREAD` | portado | `game/camera/camera_gestures.gd::touch` |
| `TWIST_SIGN` | portado | `game/camera/camera_gestures.gd::touch` |

## src/view/cameraMotion.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `AIM_CLOSE` | portado | `game/camera/camera_motion.gd::aim_north` |
| `AIM_MAX_S` | portado | `game/camera/camera_motion.gd::aim_north` |
| `AIM_SNAP` | portado | `game/camera/camera_motion.gd::aim_north` |
| `AXES` | portado | `game/camera/camera_motion.gd::step` |
| `Axis` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.aim` | portado | `game/camera/camera_motion.gd::aim_north` |
| `CameraMotion.aimedFor` | portado | `game/camera/camera_motion.gd::aim_north` |
| `CameraMotion.aiming` | portado | `game/camera/camera_motion.gd::aim_north` |
| `CameraMotion.at` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.constructor` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.held` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.moving` | portado | `game/camera/camera_motion.gd::moving` |
| `CameraMotion.owns` | portado | `game/camera/camera_motion.gd::_key` |
| `CameraMotion.pendingTurn` | portado | `game/camera/camera_motion.gd::_spend_turn` |
| `CameraMotion.pendingZoom` | portado | `game/camera/camera_motion.gd::_spend_zoom` |
| `CameraMotion.press` | portado | `game/camera/camera_motion.gd::_key` |
| `CameraMotion.release` | portado | `game/camera/camera_motion.gd::_key` |
| `CameraMotion.releaseAll` | portado | `game/camera/camera_motion.gd::_key` |
| `CameraMotion.shift` | portado | `game/camera/camera_motion.gd::_key` |
| `CameraMotion.step` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.stop` | portado | `game/camera/camera_motion.gd::stop` |
| `CameraMotion.turn` | portado | `game/camera/camera_motion.gd::turn` |
| `CameraMotion.velocity` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.wake` | portado | `game/camera/camera_motion.gd::step` |
| `CameraMotion.wheel` | portado | `game/camera/camera_motion.gd::wheel` |
| `CameraMotion.zoomAt` | portado | `game/camera/camera_motion.gd::_spend_zoom` |
| `EASE_S` | portado | `game/camera/camera_motion.gd::step` |
| `KEYS` | portado | `game/camera/camera_motion.gd::_key` |
| `MAX_STEP_S` | portado | `game/camera/camera_motion.gd::step` |
| `MotionHost` | portado | `game/camera/camera_motion.gd::step` |
| `PAN_PX_PER_S` | portado | `game/camera/camera_motion.gd::step` |
| `SHIFT` | portado | `game/camera/camera_motion.gd::_key` |
| `TILT_PER_S` | portado | `game/camera/camera_motion.gd::step` |
| `TURN_EASE_S` | portado | `game/camera/camera_motion.gd::_spend_turn` |
| `TURN_PER_S` | portado | `game/camera/camera_motion.gd::turn` |
| `ZOOM_EASE_S` | portado | `game/camera/camera_motion.gd::_spend_zoom` |
| `ZOOM_PER_S` | portado | `game/camera/camera_motion.gd::step` |

## src/view/cameraProfile.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `EYE_HEIGHT` | portado | `game/camera/camera_profile.gd::eye_lift` |
| `FAR_FOV` | portado | `game/camera/camera_profile.gd::fov_deg` |
| `FAR_MIN_TILT` | portado | `game/camera/camera_profile.gd::min_tilt` |
| `FAR_TILT` | portado | `game/camera/camera_profile.gd::profile_tilt` |
| `NEAR_FOV` | portado | `game/camera/camera_profile.gd::fov_deg` |
| `NEAR_HALF` | portado | `game/camera/camera_profile.gd::profile_tilt` |
| `NEAR_MIN_TILT` | portado | `game/camera/camera_profile.gd::min_tilt` |
| `NEAR_TILT` | portado | `game/camera/camera_profile.gd::profile_tilt` |
| `blend` | portado | `game/camera/camera_profile.gd::blend` |
| `eyeLift` | portado | `game/camera/camera_profile.gd::eye_lift` |
| `fieldOfView` | portado | `game/camera/camera_profile.gd::fov_deg` |
| `lerp` | portado | `game/camera/camera_profile.gd::blend` |
| `minTilt` | portado | `game/camera/camera_profile.gd::min_tilt` |
| `profileTilt` | portado | `game/camera/camera_profile.gd::profile_tilt` |
| `pullWeight` | portado | `game/camera/camera_pull.gd::eye` |
| `viewDistance` | portado | `game/camera/camera_profile.gd::view_distance` |

## src/view/viewport.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `Facing` | portado | `game/camera/city_camera.gd::look_at_ground` |
| `Viewport` | portado | `game/camera/city_camera.gd::look_at_ground` |
| `flatViewport` | portado | `game/camera/city_camera.gd::look_at_ground` |
| `flatViewport.holds` | portado | `game/camera/city_camera.gd::holds` |
| `flatViewport.moveTo` | portado | `game/camera/city_camera.gd::jump` |
| `flatViewport.orbit` | portado | `game/camera/city_camera.gd::orbit` |
| `flatViewport.panTo` | portado | `game/camera/city_camera.gd::pan_to` |
| `flatViewport.rotate` | portado | `game/camera/city_camera.gd::turn` |
| `flatViewport.setOrbit` | portado | `game/camera/city_camera.gd::orbit` |
| `flatViewport.slide` | portado | `game/camera/city_camera.gd::slide` |
| `flatViewport.toScreen` | portado | `game/camera/city_camera.gd::ground_at` |
| `flatViewport.toWorld` | portado | `game/camera/city_camera.gd::ground_at` |
| `flatViewport.toWorldAt` | portado | `game/camera/city_camera.gd::ground_at` |
| `flatViewport.zoomAt` | portado | `game/camera/city_camera.gd::zoom_at` |

## src/world/buildings/cameraSolids.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `SolidFiling.forEachIn` | portado | `api/city_core_inspect.cpp::clear_reach` |
| `SolidFiling.forEachInAny` | portado | `api/city_core_inspect.cpp::clear_reach` |
| `SolidFiling.forEachOccluder` | portado | `render/buildings/occluders.cpp::add` |
| `SolidFiling.visitCells` | portado | `api/city_core_inspect.cpp::clear_reach` |
| `SolidPages.epoch` | portado | `api/city_core_inspect.cpp::clear_reach` |
| `SolidPages.visitOnce` | portado | `api/city_core_inspect.cpp::clear_reach` |
| `SolidPages.walk` | portado | `api/city_core_inspect.cpp::clear_reach` |

## src/world/buildings/lotLibrary.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `lotKey` | outra sessão | sessão marcos: escopo "Os 38 projetos de `lots/*.json`" |

## src/world/buildings/sections.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `sectionsBytes` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |

## src/world/buildings/serialize.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `isSerializedBuildings` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |

## src/world/buildings/store.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `BuildingStore.toText` | outra sessão | sessão build: `boundary.json` de build tem esta declaração |

## src/world/doc.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `DocPatch` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `GlobalPart` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `MemberDiff.keyOrder` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `RoadDoc.beginEdit` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `RoadDoc.endEdit` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `RoadDoc.fromBinary` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `RoadDoc.recordOf` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `RoadDoc.savedGlobals` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `RoadDoc.toText` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `SavedGlobals` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `copyItem` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `memberDiffs` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `patchBytes` | já existia | `sim/commands/edit_history.cpp::begin` e `sim/commands/edit_history.cpp::end`: o diário registra cada mudança |
| `textBytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |
| `wholeBytes` | já existia | `sim/persistence/save_write.cpp::encode_city`, `sim/persistence/save_read.cpp::decode_city` |

## src/world/docBinary.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `imageBytes` | já existia | `sim/persistence/save_read.cpp::decode_city` |
| `isDocBinary` | já existia | `sim/persistence/save_read.cpp::decode_city` |
| `unpackImage` | já existia | `sim/persistence/save_read.cpp::decode_city` |

## src/world/docImage.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `PARTS_OF_SAVED` | já existia | `sim/persistence/save_write.cpp::write_roads` |
| `sideOf` | já existia | `sim/persistence/save_write.cpp::write_roads` |

## src/world/roads/catalog.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `CATALOG_CATEGORIES` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `CatalogCategory` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `CatalogRoad` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `Lane` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `Shape` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `catalogRoads` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `catalogRoads.cls` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `road` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `shapeProfile` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `shapeProfile.kerb` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `shapeProfile.lane` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `type` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/world/roads/connectors.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `toggleLaneLink` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `toggleLaneLink.arrives` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `toggleLaneLink.leaves` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/world/roads/profile.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `profileWidth` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/world/roads/streetNames.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `StreetNumbers` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá: `#NAMES` e `#nameFor` só em roads; sem eles não há nome |
| `streetNameOf` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá: `#NAMES` e `#nameFor` só em roads; sem eles não há nome |
| `streetNumbers` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá: `#NAMES` e `#nameFor` só em roads; sem eles não há nome |

## src/world/roads/templates.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `KINDS` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `RoadTemplate` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `classDirection` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `classTemplates` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `normalizeTemplate` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |
| `readElement` | outra sessão | sessão roads: `boundary.json` de roads tem esta declaração; `ui/road_options.gd` e o catálogo seguem lá |

## src/world/terrain.ts

| Declaração | Estado | Onde, ou por quê |
|---|---|---|
| `isTerrainMode` | outra sessão | sessão landscape: `boundary.json` de landscape |
| `isTerrainProfile` | outra sessão | sessão landscape: `boundary.json` de landscape |
