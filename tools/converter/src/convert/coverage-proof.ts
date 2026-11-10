// What a port's coverage claims, checked against the game instead of believed. A row says the game has a unit
// ("portado", "já existia") and where (`sim/commands/pole_runs.cpp`, `render/props/street_props.cpp + demolish_at`).
// The claim stands only when the game shows it: every owner file the row names exists in the game, and the owners'
// text cites the unit, by its own name or the game's form of it (closestInto, closest_into), or, for a whole file,
// by the 4.0 file it came from (the game's own provenance comments: "Roadcraft 4.0 editor/poles.ts"). A claim the
// game does not show goes back to what the ledger would decide without it.
import fs from 'node:fs';
import path from 'node:path';
import { snake } from '../translate/names.ts';
import type { Delivered } from './inventory.ts';

/** The owner files a coverage target names, relative to the game root (`sim/x.cpp` lives in native/src). */
export function ownerFiles(target: string): string[] {
  return [...new Set(target.match(/[\w./-]+\.(?:cpp|h|gd|gdshader|gdshaderinc|json|tscn)\b/g) ?? [])];
}

/** The text of an owner file in the game, or null when the game has none there. */
function ownerText(gameRoot: string, file: string): string | null {
  for (const at of [path.join(gameRoot, file), path.join(gameRoot, 'native', 'src', file), path.join(gameRoot, 'project', file)]) {
    if (fs.existsSync(at) && fs.statSync(at).isFile()) return fs.readFileSync(at, 'utf8');
  }
  return null;
}

export interface ProofInput {
  /** The unit: `src/editor/poles.ts#planRun`, or a whole file `src/editor/poles.ts`. */
  readonly id: string;
  readonly delivered: Delivered;
}

/**
 * Why the game does not show what the coverage claims for a unit; null when it does. A row with no owner file (an
 * owner named in words: `porte demolition`) shows nothing to read: it is not proven.
 */
export function unproven(gameRoot: string, input: ProofInput, cache = new Map<string, string | null>()): string | null {
  const files = ownerFiles(input.delivered.target);
  if (files.length === 0) return `its owner "${input.delivered.target}" names no file of the game`;
  const texts: string[] = [];
  for (const f of files) {
    if (!cache.has(f)) cache.set(f, ownerText(gameRoot, f));
    const text = cache.get(f)!;
    if (text === null) return `the game has no ${f}`;
    texts.push(text);
  }
  const all = texts.join('\n');
  const hash = input.id.indexOf('#');
  const file = hash < 0 ? input.id : input.id.slice(0, hash);
  const name = hash < 0 ? null : input.id.slice(hash + 1);
  const words = new Set(all.match(/[A-Za-z_]\w*/g) ?? []);
  if (name) {
    if (words.has(name) || words.has(snake(name)) || words.has(snake(name).toUpperCase())) return null;
    return `${files.join(', ')} never names ${name} (nor ${snake(name)})`;
  }
  // a whole file: cited by its path under src (editor/poles.ts), as the game's provenance comments write it
  const cited = file.replace(/^src\//, '');
  if (all.includes(cited) || all.includes(cited.replace(/\.tsx?$/, ''))) return null;
  return `${files.join(', ')} never cites ${cited}`;
}
