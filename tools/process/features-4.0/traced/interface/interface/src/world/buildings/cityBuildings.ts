// What the feature uses of src/world/buildings/cityBuildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/cityBuildings.ts:24-24 type CityCategory
/** The city's buildings - homes, public services, shops, places to eat and go out, places of work and of leisure */

type CityCategory = 'homes' | 'public' | 'commerce' | 'leisure' | 'work';

// src/world/buildings/cityBuildings.ts:26-30 interface CityBuilding
export interface CityBuilding {
  readonly fn: BuildingFunction;
  readonly category: CityCategory;
  readonly body: BlueprintBody;
}

// src/world/buildings/cityBuildings.ts:55-167 class Model
/** A model being put together, block by block. */
export class Model { … }

// src/world/buildings/cityBuildings.ts:169-169 function mat
export const mat = (finish: MaterialSpec['finish'], colour: number): MaterialSpec => …;

// src/world/buildings/cityBuildings.ts:175-175 variable PLASTER
const PLASTER = mat('plaster', 0xeae3d6);

// src/world/buildings/cityBuildings.ts:176-176 variable CREAM
const CREAM = mat('stucco', 0xe8dcc2);

// src/world/buildings/cityBuildings.ts:177-177 variable STONE
const STONE = mat('stone', 0xcfc7b6);

// src/world/buildings/cityBuildings.ts:178-178 variable BRICK
const BRICK = mat('brick', 0xa4563f);

// src/world/buildings/cityBuildings.ts:179-179 variable GLASS
const GLASS = mat('glass', 0x8fb0c0);

// src/world/buildings/cityBuildings.ts:180-180 variable CONCRETE
const CONCRETE = mat('concrete', 0xbcbab3);

// src/world/buildings/cityBuildings.ts:181-181 variable PANEL
const PANEL = mat('panel', 0x8f9ba5);

// src/world/buildings/cityBuildings.ts:182-182 variable TILE
const TILE = mat('tile', 0xb5603f);

// src/world/buildings/cityBuildings.ts:183-183 variable SLATE
const SLATE = mat('slate', 0x55595e);

// src/world/buildings/cityBuildings.ts:184-184 variable WHITE
const WHITE = mat('plaster', 0xf3f1ec);

// src/world/buildings/cityBuildings.ts:185-185 variable RED
const RED = mat('plaster', 0xb8382e);

// src/world/buildings/cityBuildings.ts:188-191 variable OCTAGON
/** The cross of a stepped tower, normalised. */
const OCTAGON = Array.from({ length: 8 }, (_, i) => {
  const a = (i * Math.PI * 2) / 8 - Math.PI / 2 + Math.PI / 8;
  return { x: 0.5 + 0.5 * Math.cos(a), y: 0.5 + 0.5 * Math.sin(a) };
});

// src/world/buildings/cityBuildings.ts:193-195 function make
function make(fn: BuildingFunction, category: CityCategory, model: Model): CityBuilding;

// src/world/buildings/cityBuildings.ts:197-337 variable CITY_BUILDINGS
export const CITY_BUILDINGS: readonly CityBuilding[] = [
  // ------------------------------------------------------------ homes
  make('house', 'homes', new Model('house', 'residential', 0).look(PLASTER, TILE)
    // The house alone (player, 2026-10-03): the front garden and its fence it came with kept it six metres off the pavement.
    .block({ x: 0, y: 0, w: 9, d: 9, storeys: 2, roof: 'gable', door: 1 })),
  make('townhouse', 'homes', new Model('townhouse', 'residential', 1).look(BRICK, SLATE)
    .block({ x: 0, y: 0, w: 6, d: 12, storeys: 3, roof: 'gable', door: 0 })),
  make('apartments', 'homes', new Model('apartments', 'residential', 4).look(CREAM)
    .block({ x: 0, y: 0, w: 18, d: 12, storeys: 6, fill: 'balcony', ground: 'window', door: 'middle', doorKind: 'doubleDoor' })
    .core('stairLift', 7.5, 6, 5)),
  make('residentialTower', 'homes', new Model('residentialTower', 'residential', 2).look(WHITE)
    .block({ x: 0, y: 0, w: 24, d: 20, storeys: 2, ground: 'shopfront', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 3, y: 3, w: 18, d: 14, base: 2, storeys: 16, fill: 'balcony' })
    .core('stairLift', 10.5, 8.5, 17)),

  // ------------------------------------------------------------ public services
  make('cityHall', 'public', new Model('cityHall', 'commercial', 6).heights(5, 4.2).look(STONE, SLATE)
    .block({ x: 0, y: 0, w: 36, d: 20, storeys: 3, pattern: 'artDeco', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 13, y: 6, w: 10, d: 10, base: 3, storeys: 3, pattern: 'artDecoCrown' })
    .lot(-6, -14, 48, 14, 'paving').row('tree', -3, -10, 39, -10, 7).el('clock', 18, 5.7, 0, { z: 18 })
    .core('stairLift', 16, 12, 5)),
  make('council', 'public', new Model('council', 'commercial', 6).heights(5, 4.5).look(WHITE, mat('metal', 0x6d8a8a))
    .block({ x: 0, y: 0, w: 30, d: 22, storeys: 2, ground: 'frenchWindow', fill: 'window', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 10, y: 6, w: 10, d: 10, base: 2, storeys: 1, roof: 'hip', pitch: 40, shape: OCTAGON, fill: 'wideWindow' })
    .lot(-4, -10, 38, 10, 'paving').row('planter', 0, -6, 30, -6, 6)),
  make('courthouse', 'public', new Model('courthouse', 'commercial', 6).heights(5.5, 4.5).look(STONE)
    .block({ x: 0, y: 0, w: 30, d: 24, storeys: 3, ground: 'frenchWindow', pattern: 'gallery', door: 'middle', doorKind: 'doubleDoor' })),
  make('postOffice', 'public', new Model('postOffice', 'commercial', 3).look(mat('plaster', 0xf2d36b))
    .block({ x: 0, y: 0, w: 15, d: 12, storeys: 2, ground: 'shopfront', door: 'middle', doorKind: 'doubleDoor' })
    .el('awning', 7.5, -0.6, 0, { w: 9, z: 3 })),
  make('police', 'public', new Model('police', 'commercial', 6).look(mat('plaster', 0xdfe6ee))
    .block({ x: 0, y: 0, w: 20, d: 15, storeys: 3, door: 'middle', doorKind: 'doubleDoor' })
    .lot(20, 0, 12, 15, 'paving').el('parking', 26, 7.5, 3, { w: 12, d: 6 })),
  make('fireStation', 'public', new Model('fireStation', 'commercial', 3).heights(5, 3.4).look(RED)
    .block({ x: 0, y: 0, w: 18, d: 18, storeys: 2, ground: 'garageDoor' })
    .block({ x: 18, y: 6, w: 4, d: 4, storeys: 6, fill: 'wall' })),
  make('hospital', 'public', new Model('hospital', 'commercial', 6).look(WHITE)
    .block({ x: 0, y: 0, w: 36, d: 24, storeys: 2, ground: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 6, y: 4, w: 24, d: 16, base: 2, storeys: 6, pattern: 'office' })
    .el('canopy', 18, -1.5, 0, { w: 10, d: 3, z: 3.2 }).core('stairLift', 15, 10, 7).core('lift', 21, 10, 7)),
  make('clinic', 'public', new Model('clinic', 'commercial', 6).look(WHITE)
    .block({ x: 0, y: 0, w: 15, d: 12, storeys: 2, door: 'middle', doorKind: 'doubleDoor' })),
  make('school', 'public', new Model('school', 'commercial', 3).look(mat('brick', 0xb86a4a))
    .block({ x: 0, y: 0, w: 36, d: 12, storeys: 2, door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 0, y: 12, w: 12, d: 18, storeys: 2 })
    .block({ x: 24, y: 12, w: 12, d: 18, storeys: 2 })
    .lot(12, 12, 12, 18, 'paving').el('tree', 18, 20).ring('fence', -2, -8, 40, 40, 6)),
  make('university', 'public', new Model('university', 'commercial', 6).heights(4.5, 4).look(STONE, SLATE)
    .block({ x: 0, y: 0, w: 48, d: 16, storeys: 4, pattern: 'gallery', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 0, y: 16, w: 14, d: 30, storeys: 3 }).block({ x: 34, y: 16, w: 14, d: 30, storeys: 3 })
    .lot(14, 16, 20, 30, 'grass').row('tree', 16, 22, 32, 22, 4).row('bench', 17, 30, 31, 30, 3)
    .core('stairLift', 22, 8, 3)),
  make('library', 'public', new Model('library', 'commercial', 6).heights(6, 4.5).look(STONE)
    .block({ x: 0, y: 0, w: 24, d: 18, storeys: 2, ground: 'frenchWindow', fill: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })
    .lot(-2, -8, 28, 8, 'paving').row('bench', 2, -4, 22, -4, 3)),
  make('museum', 'public', new Model('museum', 'commercial', 6).heights(7, 5).look(WHITE)
    .block({ x: 0, y: 0, w: 32, d: 22, storeys: 2, fill: 'wall', ground: 'frenchWindow', door: 'middle', doorKind: 'doubleDoor' })),
  make('prison', 'public', new Model('prison', 'industrial', 7).look(CONCRETE)
    .block({ x: 0, y: 0, w: 30, d: 14, storeys: 3, fill: 'ribbon', door: 'middle' })
    .block({ x: 0, y: 14, w: 10, d: 20, storeys: 3, fill: 'ribbon' })
    .lot(10, 14, 20, 20, 'gravel').ring('wall', -4, -4, 38, 42, 0)),
  make('church', 'public', new Model('church', 'commercial', 0).heights(9, 4).look(WHITE, SLATE)
    .block({ x: 0, y: 0, w: 14, d: 26, storeys: 1, roof: 'gable', pitch: 45, fill: 'frenchWindow', door: 0, doorKind: 'doubleDoor' })
    .block({ x: 4, y: -6, w: 6, d: 6, storeys: 5, roof: 'hip', pitch: 60, fill: 'window', door: 0, doorKind: 'doubleDoor' })
    .lot(-4, -12, 22, 6, 'paving')),
  make('cemetery', 'public', new Model('cemetery', 'commercial', 7).look(CONCRETE)
    .lot(0, 0, 40, 30, 'grass').ring('wall', 0, 0, 40, 30, 6)
    .row('slab', 6, 8, 34, 8, 6).row('slab', 6, 15, 34, 15, 6).row('slab', 6, 22, 34, 22, 6)
    .row('tree', 3, 4, 37, 4, 5)
    .block({ x: 16, y: 24.5, w: 8, d: 4, storeys: 1, roof: 'gable', door: 'middle' })),
  make('busStation', 'public', new Model('busStation', 'commercial', 6).heights(6, 4).look(CONCRETE)
    .block({ x: 0, y: 0, w: 30, d: 12, storeys: 1, ground: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })
    .lot(0, -14, 30, 14, 'paving').el('canopy', 15, -7, 0, { w: 26, d: 8, z: 4 })),

  // ------------------------------------------------------------ commerce and services
  make('shop', 'commerce', new Model('shop', 'commercial', 3).look(PLASTER)
    .block({ x: 0, y: 0, w: 9, d: 12, storeys: 2, ground: 'shopfront', door: 1 }).el('awning', 4.5, -0.6, 0, { w: 8, z: 3 })),
  make('supermarket', 'commerce', new Model('supermarket', 'commercial', 3).heights(6, 4).look(mat('panel', 0xdfe3e6))
    .block({ x: 0, y: 0, w: 40, d: 30, storeys: 1, ground: 'shopfront', door: 'middle', doorKind: 'doubleDoor' })
    .lot(0, -18, 40, 18, 'paving').el('parking', 10, -9, 0, { w: 18, d: 14 }).el('parking', 30, -9, 0, { w: 18, d: 14 })),
  make('mall', 'commerce', new Model('mall', 'commercial', 6).heights(6, 5).look(mat('panel', 0xe4e2dc), undefined, GLASS)
    .block({ x: 0, y: 0, w: 60, d: 40, storeys: 3, fill: 'wall', ground: 'shopfront', door: 'middle', doorKind: 'doubleDoor' })
    .block({ x: 24, y: -4, w: 12, d: 6, storeys: 3, fill: 'wideWindow' })
    .core('stairLift', 28, 18, 2).core('lift', 32, 18, 2)),
  make('bank', 'commerce', new Model('bank', 'commercial', 6).heights(5.5, 4).look(STONE)
    .block({ x: 0, y: 0, w: 18, d: 16, storeys: 3, ground: 'frenchWindow', pattern: 'artDeco', door: 'middle', doorKind: 'doubleDoor' })),
  make('pharmacy', 'commerce', new Model('pharmacy', 'commercial', 3).look(WHITE)
    .block({ x: 0, y: 0, w: 10, d: 12, storeys: 1, ground: 'shopfront', door: 'middle' }).el('awning', 5, -0.6, 0, { w: 9, z: 3 })),
  make('bakery', 'commerce', new Model('bakery', 'commercial', 0).look(CREAM, TILE)
    .block({ x: 0, y: 0, w: 9, d: 10, storeys: 2, roof: 'gable', ground: 'shopfront', door: 1 }).el('awning', 4.5, -0.6, 0, { w: 8, z: 3 })),
  make('restaurant', 'commerce', new Model('restaurant', 'commercial', 0).look(mat('brick', 0x8c4a35))
    .block({ x: 0, y: 0, w: 14, d: 14, storeys: 1, ground: 'frenchWindow', door: 'middle', doorKind: 'doubleDoor' })
    .lot(0, -6, 14, 6, 'paving').el('awning', 7, -0.6, 0, { w: 13, z: 3 }).row('planter', 1, -5, 13, -5, 4)),
  make('snackBar', 'commerce', new Model('snackBar', 'commercial', 3).look(mat('plaster', 0xf2a03d))
    .block({ x: 0, y: 0, w: 8, d: 8, storeys: 1, ground: 'shopfront', door: 0 }).el('awning', 4, -0.6, 0, { w: 7, z: 3 })),
  make('bar', 'commerce', new Model('bar', 'commercial', 1).look(mat('wood', 0x6d4a33))
    .block({ x: 0, y: 0, w: 9, d: 12, storeys: 2, ground: 'frenchWindow', door: 1 }).el('awning', 4.5, -0.6, 0, { w: 8, z: 3 })),
  make('nightclub', 'commerce', new Model('nightclub', 'commercial', 7).heights(7, 4).look(mat('panel', 0x2b2d3a))
    .block({ x: 0, y: 0, w: 20, d: 24, storeys: 1, fill: 'wall', door: 'middle', doorKind: 'doubleDoor' }).el('canopy', 10, -1, 0, { w: 6, d: 2, z: 3 })),
  make('cinema', 'commerce', new Model('cinema', 'commercial', 7).heights(10, 4).look(mat('panel', 0x7d3b3b))
    .block({ x: 0, y: 0, w: 24, d: 30, storeys: 1, fill: 'wall', door: 'middle', doorKind: 'doubleDoor' }).el('canopy', 12, -1.5, 0, { w: 16, d: 3, z: 4 })),
  make('hotel', 'commerce', new Model('hotel', 'commercial', 2).look(CREAM)
    .block({ x: 0, y: 0, w: 24, d: 16, storeys: 10, fill: 'balcony', ground: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })
    .el('canopy', 12, -1.5, 0, { w: 8, d: 3, z: 3.4 }).core('stairLift', 10.5, 8, 9)),
  make('gym', 'commerce', new Model('gym', 'commercial', 7).heights(6, 4).look(mat('panel', 0x5d6b75))
    .block({ x: 0, y: 0, w: 20, d: 24, storeys: 1, ground: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })),
  make('club', 'leisure', new Model('club', 'commercial', 0).look(WHITE, TILE)
    .block({ x: 0, y: 0, w: 20, d: 12, storeys: 2, roof: 'hip', door: 'middle', doorKind: 'doubleDoor' })
    .lot(0, 12, 20, 16, 'water').lot(20, 0, 18, 28, 'paving').ring('fence', 20.5, 0.5, 17, 27, 0)),
  make('gasStation', 'commerce', new Model('gasStation', 'commercial', 3).look(WHITE)
    .block({ x: 0, y: 14, w: 12, d: 8, storeys: 1, ground: 'shopfront', door: 'middle' })
    .lot(-4, 0, 24, 14, 'paving').el('canopy', 6, 6, 0, { w: 16, d: 10, z: 5 })
    .el('pillar', 0, 3, 0, { h: 5 }).el('pillar', 12, 3, 0, { h: 5 }).el('pillar', 0, 9, 0, { h: 5 }).el('pillar', 12, 9, 0, { h: 5 })),
  make('office', 'work', new Model('office', 'commercial', 6).look(GLASS)
    .block({ x: 0, y: 0, w: 24, d: 18, storeys: 12, pattern: 'office', ground: 'wideWindow', door: 'middle', doorKind: 'doubleDoor' })
    .core('stairLift', 10.5, 7.5, 11).core('lift', 13.5, 7.5, 11)),

  // ------------------------------------------------------------ work
  make('factory', 'work', new Model('factory', 'industrial', 7).heights(7, 4).look(PANEL)
    .block({ x: 0, y: 0, w: 36, d: 24, storeys: 1, roof: 'sawtooth', fill: 'ribbon', ground: 'loadingDoor' })
    .block({ x: -10, y: 0, w: 10, d: 10, storeys: 2, door: 'middle' })
    .block({ x: 30, y: 18, w: 3, d: 3, storeys: 8, fill: 'wall' })),
  make('warehouse', 'work', new Model('warehouse', 'industrial', 7).heights(8, 4).look(PANEL)
    .block({ x: 0, y: 0, w: 30, d: 24, storeys: 1, roof: 'shed', fill: 'wall', ground: 'loadingDoor' })
    .lot(0, -12, 30, 12, 'paving')),

  // ------------------------------------------------------------ leisure
  make('park', 'leisure', new Model('park', 'commercial', 0).look(PLASTER)
    .lot(0, 0, 40, 30, 'grass')
    .el('pavement', 20, 15, 0, { w: 40, d: 3 }).el('pavement', 20, 15, 1, { w: 30, d: 3 })
    .row('tree', 4, 4, 36, 4, 5).row('tree', 4, 26, 36, 26, 5).row('bench', 8, 12, 32, 12, 4).row('bench', 8, 18, 32, 18, 4, 2)
    .row('flowers', 14, 8, 26, 8, 3)),
  make('square', 'leisure', new Model('square', 'commercial', 0).look(PLASTER)
    .lot(0, 0, 30, 30, 'paving').row('tree', 3, 3, 27, 3, 4).row('tree', 3, 27, 27, 27, 4)
    .row('bench', 6, 10, 24, 10, 3).row('bench', 6, 20, 24, 20, 3, 2).el('planter', 15, 15, 0, { w: 4, d: 4, h: 0.8 })),
  make('playground', 'leisure', new Model('playground', 'commercial', 0).look(PLASTER)
    .lot(0, 0, 20, 16, 'sand').ring('fence', 0, 0, 20, 16, 3).row('bench', 4, 2, 16, 2, 3)
    .el('slab', 6, 9, 0, { w: 3, d: 3, h: 1.2 }).el('railing', 14, 9, 0, { w: 4 })),
  make('sportsCourt', 'leisure', new Model('sportsCourt', 'commercial', 0).look(PLASTER)
    .lot(0, 0, 32, 20, 'paving').ring('fence', 0, 0, 32, 20, 0).el('railing', 16, 10, 0, { w: 0.6, h: 3 })),
];
