// Where each file of the source goes in the target: one module per source file, placed by the source's own
// architecture. The source keeps each area in a folder (core, world, sim, people, jobs, render, ui, editor); each area
// has one place in the target's layers, and the concept a file belongs to is its folder under the area, or its own
// name (world/transit.ts and sim/transit/transit.ts are both the concept transit, so both go to one owner). Two files
// of one owner with the same name are told apart by their area, in path order, so the result never depends on what
// was translated first.
import { snake } from './names.ts';

export interface ModuleId {
  /** The source file, relative to the source root (src/world/transit.ts). */
  readonly source: string;
  /** The target folder that owns it (native/src/sim/transit). */
  readonly owner: string;
  /** The base of its file names in the owner (transit → transit_types.h, transit.h, transit_edit.cpp...). */
  readonly stem: string;
}

/** The source areas and the target layer each one belongs to. */
const AREAS: Readonly<Record<string, { readonly layer: string; readonly concept: boolean }>> = {
  core: { layer: 'native/src/core', concept: false },
  world: { layer: 'native/src/sim', concept: true },
  sim: { layer: 'native/src/sim', concept: true },
  people: { layer: 'native/src/sim', concept: true },
  jobs: { layer: 'native/src/sim', concept: true },
  render: { layer: 'native/src/render', concept: true },
  // the player's tools: what they compute and change goes where the target changes state at the player's request
  // (CLAUDE.md: only sim/commands changes CityState for the player), one folder per tool; what they draw is render's and
  // what they show is project/ui's, by the ledger's roles (ledger.ts ownerOf reads its owner here)
  editor: { layer: 'native/src/sim/commands', concept: true },
  // the 4.0 lab (meters, doctor, code analysis, resources, gpu): its pure logic, exposed to the game's script by a
  // CityCore area like builder, landscape and probes; one folder of the game's existing api layer
  lab: { layer: 'native/src/api/lab', concept: false },
};

export class ModulePlacement {
  readonly #byPath = new Map<string, ModuleId>();
  /** Why a file has no place; an error only when a translation reaches it. */
  readonly #unplaced = new Map<string, string>();

  /**
   * Every file of the whole program placed at once, in path order: a file has the same owner and name whatever is
   * translated with it.
   */
  constructor(sources: readonly string[]) {
    const taken = new Map<string, Set<string>>();
    for (const source of [...sources].sort()) {
      const placed = this.#place(source);
      if (!placed) continue;
      const stems = taken.get(placed.owner) ?? new Set<string>();
      let stem = placed.stem;
      if (stems.has(stem)) stem = `${stem}_${placed.area}`;
      if (stems.has(stem)) {
        this.#unplaced.set(source, `two source files would be ${placed.owner}/${stem}`);
        continue;
      }
      stems.add(stem);
      taken.set(placed.owner, stems);
      this.#byPath.set(source, { source, owner: placed.owner, stem });
    }
  }

  /** The module of a source file, or why it has none. */
  of(source: string): ModuleId | { readonly error: string } {
    return this.#byPath.get(source) ?? { error: `${source}: ${this.#unplaced.get(source) ?? 'not a file of the source program'}` };
  }

  #place(source: string): { owner: string; stem: string; area: string } | null {
    const parts = source.split('/');
    if (parts[0] !== 'src' || parts.length < 3) {
      this.#unplaced.set(source, 'not under the src/ areas of the source');
      return null;
    }
    const area = parts[1]!;
    const rule = AREAS[area];
    if (!rule) {
      this.#unplaced.set(source, `the area src/${area} has no place yet in the target's simulation, core or render layers`);
      return null;
    }
    const stem = snake(parts[parts.length - 1]!.replace(/\.tsx?$/, ''));
    if (!rule.concept) return { owner: rule.layer, stem, area };
    // The concept: the folder under the area, or the file itself when it lies directly in the area.
    const concept = parts.length > 3 ? snake(parts[2]!) : stem;
    return { owner: `${rule.layer}/${concept}`, stem, area };
  }
}
