// What every test shares, owned once: the TypeScript package the graph analyses with (given by
// CONVERTER_TYPESCRIPT, never a fixed path) and the graph of the fixture project.
import path from 'node:path';
import { buildGraph } from '../src/graph/build.ts';
import type { GraphIR } from '../src/graph/ir.ts';

export const FIXTURE = path.join(import.meta.dirname, 'fixtures', 'basic');

export function typescriptDir(): string {
  const dir = process.env['CONVERTER_TYPESCRIPT'];
  if (!dir) throw new Error('set CONVERTER_TYPESCRIPT to a typescript package directory (5.8 or newer) to run these tests');
  return dir;
}

/** The Jelly package whose call graph says what each call runs (CONVERTER_JELLY, never a fixed path). */
export function jellyDir(): string {
  const dir = process.env['CONVERTER_JELLY'];
  if (!dir) throw new Error('set CONVERTER_JELLY to the @cs-au-dk/jelly package directory to run these tests');
  return dir;
}

/** The PostCSS package style sheets are read with (CONVERTER_POSTCSS, never a fixed path). */
export function postcssDir(): string {
  const dir = process.env['CONVERTER_POSTCSS'];
  if (!dir) throw new Error('set CONVERTER_POSTCSS to a postcss package directory (8 or newer) to run these tests');
  return dir;
}

/** The fixture's graph from `entries`, or of its whole scope when there are none. */
export function fixtureGraph(entries: string[] = []): GraphIR {
  return buildGraph({ sourceRoot: FIXTURE, typescriptDir: typescriptDir(), entries, include: [], exclude: [] });
}
