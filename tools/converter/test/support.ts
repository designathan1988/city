// What every test shares, owned once: the tools the tests run (the machine file's, src/common/machine.ts: TypeScript,
// Jelly, PostCSS, clangd, the C++ compiler; an environment variable only overrides one) and the graph of the fixture project.
import path from 'node:path';
import { machine } from '../src/common/machine.ts';
import { buildGraph } from '../src/graph/build.ts';
import type { GraphIR } from '../src/graph/ir.ts';

export const FIXTURE = path.join(import.meta.dirname, 'fixtures', 'basic');

/** The TypeScript package the graph analyses with (5.8 or newer). */
export function typescriptDir(): string {
  return machine().get('typescript');
}

/** The Jelly package whose call graph says what each call runs. */
export function jellyDir(): string {
  return machine().get('jelly');
}

/** The PostCSS package style sheets are read with (8 or newer). */
export function postcssDir(): string {
  return machine().get('postcss');
}

/** The folder holding @shaderfrog/glsl-parser (its node_modules), the shader translation's parser. */
export function glslDir(): string {
  return machine().get('glsl');
}

/** The clangd the C++ graph is read through. */
export function clangdPath(): string {
  return machine().get('clangd');
}

/** The C++ compiler driver the translations, the runtime and the compile commands use. */
export function cxxPath(): string {
  return machine().get('cxx');
}

/** The fixture's graph from `entries`, or of its whole scope when there are none. */
export function fixtureGraph(entries: string[] = []): GraphIR {
  return buildGraph({ sourceRoot: FIXTURE, typescriptDir: typescriptDir(), entries, include: [], exclude: [] });
}
