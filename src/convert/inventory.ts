// What is known before a conversion starts: a feature of the functional inventory (its seeds, found in the source's
// code) and what the ports already delivered, read from each port's coverage table.
import fs from 'node:fs';
import path from 'node:path';

export interface InventoryFeature {
  readonly id: string;
  readonly sementes: readonly string[];
}

/** The inventory's id as a feature folder name: `editor:poleRoutes` is `editor-pole-routes`. */
export const folderName = (id: string): string => id.replace(/([a-z0-9])([A-Z])/g, '$1-$2').replace(/[^a-zA-Z0-9]+/g, '-').toLowerCase();

export function readInventory(file: string): readonly InventoryFeature[] {
  const parsed = JSON.parse(fs.readFileSync(file, 'utf8')) as { funcionalidades?: InventoryFeature[] };
  if (!Array.isArray(parsed.funcionalidades)) throw new Error(`${file} has no "funcionalidades" list`);
  return parsed.funcionalidades;
}

export interface Delivered {
  readonly port: string;
  readonly target: string;
  readonly state: string;
}

/** States of a coverage row that mean the game has it. */
const IN_GAME = /^(portado|já existia|ja existia)\b/i;

/**
 * Source file or declaration → what a port says the game has. A coverage row is a table line whose first cell is a
 * backquoted source path (`src/x.ts` or `src/x.ts#name`), with the target and the state among its cells. Only a
 * whole file's row speaks for all its declarations; a partial row speaks for nothing.
 */
/**
 * A port's own file by its English name (`coverage.md`, `constants.json`), or by the Portuguese name the older port
 * folders still carry (`cobertura.md`, `constantes.json`); the English path when neither exists.
 */
export function portFile(dir: string, name: string, legacy: string): string {
  const file = path.join(dir, name);
  if (fs.existsSync(file)) return file;
  const old = path.join(dir, legacy);
  return fs.existsSync(old) ? old : file;
}

export function readCoverage(portsDir: string): Map<string, Delivered> {
  const out = new Map<string, Delivered>();
  if (!fs.existsSync(portsDir)) return out;
  for (const port of fs.readdirSync(portsDir).sort()) {
    const file = portFile(path.join(portsDir, port), 'coverage.md', 'cobertura.md');
    if (!fs.existsSync(file)) continue;
    for (const line of fs.readFileSync(file, 'utf8').split(/\r?\n/)) {
      const cells = line.split('|').map((c) => c.trim());
      const key = /^`(src\/[^`]+)`$/.exec(cells[1] ?? '')?.[1];
      if (!key) continue;
      const state = cells.slice(2).find((c) => IN_GAME.test(c));
      if (!state) continue;
      if (!key.includes('#') && !/^whole\b/.test(cells[2] ?? '')) continue;
      const target = cells.slice(2).find((c) => c.startsWith('`')) ?? '';
      out.set(key, { port, target: target.replace(/`/g, ''), state });
    }
  }
  return out;
}
