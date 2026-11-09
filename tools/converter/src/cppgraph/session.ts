// A clangd session over a copy of the project's compilation database: the copy lives in a folder of the converter, so
// clangd's background index (written next to compile_commands.json) never lands in the project. Answers the questions
// the graph asks: a file's symbols, its semantic tokens, where a name is defined, what overrides a virtual method.
import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL, fileURLToPath } from 'node:url';
import { normalizePath } from '../common/paths.ts';
import { LspClient } from './lsp.ts';
import type { DocumentSymbol, LspPosition, LspRange } from './symbols.ts';

export interface SemanticToken {
  readonly line: number;
  readonly column: number;
  readonly length: number;
  readonly type: string;
  readonly modifiers: readonly string[];
}

export interface Location {
  readonly file: string;
  readonly range: LspRange;
}

interface CompileCommand {
  readonly directory: string;
  readonly file: string;
}

const INDEX_START_GRACE_MS = 15_000;
const MS_PER_SECOND = 1000;

export function uriOf(file: string): string {
  return pathToFileURL(file).href;
}

export function fileOf(uri: string): string {
  return normalizePath(fileURLToPath(uri));
}

export class ClangdSession {
  readonly #lsp: LspClient;
  readonly #opened = new Set<string>();
  #legend: { tokenTypes: string[]; tokenModifiers: string[] } = { tokenTypes: [], tokenModifiers: [] };
  version = '';

  private constructor(lsp: LspClient) {
    this.#lsp = lsp;
  }

  /**
   * Copies the compile commands of the files inside `keep` into `cacheDir` and starts clangd on that copy. With `crossFile`, it
   * waits for its background index (cross-file definitions come from it; an index not finished in `indexTimeoutSeconds` fails
   * the start); without, no index is built at all: a file's own symbols need only that file's parse.
   */
  static async start(clangd: string, compileCommands: string, cacheDir: string, keep: (file: string) => boolean,
    rootDir: string, extraArgs: readonly string[], indexTimeoutSeconds: number, crossFile = true): Promise<ClangdSession> {
    const all = JSON.parse(fs.readFileSync(compileCommands, 'utf8')) as CompileCommand[];
    const kept = all.filter((c) => keep(normalizePath(path.resolve(c.directory, c.file))));
    if (kept.length === 0) throw new Error(`${compileCommands} has no translation unit inside the analysed scope`);
    fs.mkdirSync(cacheDir, { recursive: true });
    fs.writeFileSync(path.join(cacheDir, 'compile_commands.json'), JSON.stringify(kept));
    const lsp = new LspClient(clangd, [`--compile-commands-dir=${cacheDir}`, crossFile ? '--background-index' : '--background-index=false', '--log=error',
      '--header-insertion=never', ...extraArgs]);
    const session = new ClangdSession(lsp);
    // An index that does not finish in time leaves cross-file definitions out: the graph would be partial, so the run fails.
    const indexed = !crossFile ? Promise.resolve() : new Promise<void>((resolve, reject) => {
      let started = false;
      const timeout = setTimeout(() => reject(new Error(`clangd's background index did not finish in ${indexTimeoutSeconds} s: ` +
        'a graph read from a partial index misses cross-file definitions (give --index-timeout the time the project needs)')), indexTimeoutSeconds * MS_PER_SECOND);
      lsp.on('$/progress', (params) => {
        if (!/backgroundIndex/i.test(String(params['token']))) return;
        const kind = (params['value'] as { kind?: string }).kind;
        if (kind === 'begin') started = true;
        if (kind === 'end') { clearTimeout(timeout); resolve(); }
      });
      setTimeout(() => { if (!started) { clearTimeout(timeout); resolve(); } }, INDEX_START_GRACE_MS); // already indexed in the cache
    });
    // awaited below, after initialize: a rejection that comes first is held until then, not reported as unhandled
    indexed.catch(() => undefined);
    const init = await lsp.request<{ capabilities: { semanticTokensProvider?: { legend: { tokenTypes: string[]; tokenModifiers: string[] } } }; serverInfo?: { version?: string } }>('initialize', {
      processId: process.pid, rootUri: uriOf(rootDir),
      capabilities: {
        window: { workDoneProgress: true },
        textDocument: {
          documentSymbol: { hierarchicalDocumentSymbolSupport: true },
          semanticTokens: { requests: { full: true }, tokenTypes: [], tokenModifiers: [], formats: ['relative'] },
          definition: { linkSupport: true }, implementation: { linkSupport: true },
        },
      },
    });
    const legend = init.capabilities.semanticTokensProvider?.legend;
    if (!legend) throw new Error('clangd offers no semantic tokens');
    session.#legend = legend;
    session.version = init.serverInfo?.version ?? 'unknown';
    lsp.notify('initialized', {});
    try {
      await indexed;
    } catch (error) {
      await lsp.shutdown();
      throw error;
    }
    return session;
  }

  #open(file: string, text: string): void {
    if (this.#opened.has(file)) return;
    this.#opened.add(file);
    this.#lsp.notify('textDocument/didOpen', { textDocument: { uri: uriOf(file), languageId: 'cpp', version: 1, text } });
  }

  async symbols(file: string, text: string): Promise<DocumentSymbol[]> {
    this.#open(file, text);
    return (await this.#lsp.request<DocumentSymbol[] | null>('textDocument/documentSymbol', { textDocument: { uri: uriOf(file) } })) ?? [];
  }

  async tokens(file: string, text: string): Promise<SemanticToken[]> {
    this.#open(file, text);
    const result = await this.#lsp.request<{ data?: number[] } | null>('textDocument/semanticTokens/full', { textDocument: { uri: uriOf(file) } });
    const data = result?.data ?? [];
    const out: SemanticToken[] = [];
    let line = 0, column = 0;
    for (let i = 0; i + 4 < data.length; i += 5) {
      line += data[i]!;
      column = data[i]! !== 0 ? data[i + 1]! : column + data[i + 1]!;
      const bits = data[i + 4]!;
      out.push({
        line, column, length: data[i + 2]!, type: this.#legend.tokenTypes[data[i + 3]!] ?? 'unknown',
        modifiers: this.#legend.tokenModifiers.filter((_m, b) => (bits & (1 << b)) !== 0),
      });
    }
    return out;
  }

  definition(file: string, at: LspPosition): Promise<Location[]> {
    return this.#locations('textDocument/definition', file, at);
  }

  implementations(file: string, at: LspPosition): Promise<Location[]> {
    return this.#locations('textDocument/implementation', file, at);
  }

  async #locations(method: string, file: string, at: LspPosition): Promise<Location[]> {
    const result = await this.#lsp.request<unknown>(method, { textDocument: { uri: uriOf(file) }, position: at });
    const list = (Array.isArray(result) ? result : result ? [result] : []) as { uri?: string; targetUri?: string; range?: LspRange; targetSelectionRange?: LspRange }[];
    const out: Location[] = [];
    for (const loc of list) {
      const uri = loc.uri ?? loc.targetUri;
      const range = loc.range ?? loc.targetSelectionRange;
      if (uri && range) out.push({ file: fileOf(uri), range });
    }
    return out;
  }

  shutdown(): Promise<void> {
    return this.#lsp.shutdown();
  }
}
