// Attributes a runtime function to a graph unit. Its start offset in the served script goes through the script's
// source map to a position in the original TypeScript file; the innermost unit containing that position owns it. If
// the position lies in the unit's callable head (declaration start to body start), the function is the unit itself;
// otherwise it is a closure inside the unit.
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import { normalizePath, relativePath } from '../common/paths.ts';
import type { GraphIR, Position, Range, UnitRecord } from '../graph/ir.ts';

export interface Attribution {
  readonly unit: string;
  readonly own: boolean;
}

interface TraceMappingApi {
  TraceMap: new (map: unknown, mapUrl?: string) => unknown;
  originalPositionFor(map: unknown, needle: { line: number; column: number }): { source: string | null; line: number | null; column: number | null };
}

/** The source map library the source project's own Vite depends on (same handling of its maps). */
export function loadTraceMapping(sourceRoot: string): TraceMappingApi {
  return createRequire(path.join(sourceRoot, 'package.json'))('@jridgewell/trace-mapping') as TraceMappingApi;
}

function before(a: Position, b: Position): boolean {
  return a.line < b.line || (a.line === b.line && a.column <= b.column);
}

function contains(range: Range, at: Position): boolean {
  return before(range.start, at) && before(at, range.end);
}

function size(unit: UnitRecord): number {
  return (unit.end.line - unit.start.line) * 100_000 + (unit.end.column - unit.start.column);
}

export class UnitIndex {
  readonly #byFile = new Map<string, UnitRecord[]>();
  readonly #ids: Set<string>;

  constructor(ir: GraphIR) {
    for (const unit of ir.units) this.#byFile.set(unit.file, [...(this.#byFile.get(unit.file) ?? []), unit]);
    this.#ids = new Set(ir.units.map((u) => u.id));
  }

  /** `file` relative to the source root, `at` 1-based line and column. */
  locate(file: string, at: Position): Attribution | null {
    let best: UnitRecord | null = null;
    for (const unit of this.#byFile.get(file) ?? []) {
      if (!contains({ start: unit.start, end: unit.end }, at)) continue;
      if (best === null || size(unit) < size(best)) best = unit;
    }
    if (!best) return null;
    // A class's runtime constructor function spans the class itself: it is the class's constructor unit when there is one.
    if (best.kind === 'class') {
      const constructor = `${best.id}.constructor`;
      return this.#ids.has(constructor) ? { unit: constructor, own: true } : { unit: best.id, own: true };
    }
    return { unit: best.id, own: best.callableHead !== null && contains(best.callableHead, at) };
  }
}

/** Maps generated offsets of one served script to original positions. */
export class ScriptMapper {
  readonly #lineStarts: number[] = [0];
  readonly #map: unknown;
  readonly #api: TraceMappingApi;
  readonly #mapUrl: string;

  constructor(api: TraceMappingApi, source: string, map: unknown, mapUrl: string) {
    for (let i = 0; i < source.length; i++) if (source.charCodeAt(i) === 10) this.#lineStarts.push(i + 1);
    this.#api = api;
    this.#map = map === null ? null : new api.TraceMap(map, mapUrl);
    this.#mapUrl = mapUrl;
  }

  get hasMap(): boolean {
    return this.#map !== null;
  }

  /** The original file (URL as written in the map, resolved against the map's URL) and 1-based position. */
  original(offset: number): { source: string; at: Position } | null {
    if (this.#map === null) return null;
    let low = 0;
    let high = this.#lineStarts.length - 1;
    while (low < high) {
      const mid = (low + high + 1) >> 1;
      if (this.#lineStarts[mid]! <= offset) low = mid;
      else high = mid - 1;
    }
    const found = this.#api.originalPositionFor(this.#map, { line: low + 1, column: offset - this.#lineStarts[low]! });
    if (found.source === null || found.line === null || found.column === null) return null;
    return { source: new URL(found.source, this.#mapUrl).href, at: { line: found.line, column: found.column + 1 } };
  }
}

/** The source map of a served script: inline (data: URL) or fetched from the dev server. */
export async function loadSourceMap(sourceMapURL: string, scriptUrl: string): Promise<{ map: unknown; url: string } | null> {
  if (!sourceMapURL) return null;
  if (sourceMapURL.startsWith('data:')) {
    const comma = sourceMapURL.indexOf(',');
    const header = sourceMapURL.slice(0, comma);
    const body = sourceMapURL.slice(comma + 1);
    const text = header.endsWith(';base64') ? Buffer.from(body, 'base64').toString('utf8') : decodeURIComponent(body);
    return { map: JSON.parse(text), url: scriptUrl };
  }
  const url = new URL(sourceMapURL, scriptUrl).href;
  const response = await fetch(url);
  if (!response.ok) throw new Error(`source map ${url}: HTTP ${response.status}`);
  return { map: await response.json(), url };
}

/**
 * The project file a resolved source URL names, relative to the source root, or null for code outside it. The dev
 * server serves files under the root at their path (`/src/x.ts`) and others through `/@fs/<absolute path>`.
 */
export function projectFile(sourceRoot: string, origin: string, sourceUrl: string): string | null {
  if (sourceUrl.startsWith('file:')) return insideRoot(sourceRoot, new URL(sourceUrl).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
  if (!sourceUrl.startsWith(origin)) return null;
  const pathname = decodeURIComponent(new URL(sourceUrl).pathname);
  if (pathname.startsWith('/@fs/')) return insideRoot(sourceRoot, pathname.slice('/@fs/'.length));
  return insideRoot(sourceRoot, path.join(sourceRoot, pathname));
}

function insideRoot(sourceRoot: string, file: string): string | null {
  const normal = normalizePath(file);
  const root = normalizePath(sourceRoot);
  if (!normal.startsWith(root + '/') || normal.includes('/node_modules/')) return null;
  return fs.existsSync(normal) ? relativePath(root, normal) : null;
}

/** The npm package a source URL belongs to, or null. */
export function packageOf(sourceUrl: string): string | null {
  const marker = sourceUrl.lastIndexOf('/node_modules/');
  if (marker < 0) return null;
  const parts = decodeURIComponent(sourceUrl.slice(marker + '/node_modules/'.length)).split('/');
  if (parts[0] === '.vite') return null; // the prebundle itself; its map names the real package
  return parts[0]!.startsWith('@') ? `${parts[0]}/${parts[1]}` : parts[0]!;
}
