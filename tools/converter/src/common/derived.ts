// The graphs every stage reads, as artifacts derived from their inputs: the source program's graph (`converter graph`)
// and the target's C++ graph (`converter cppgraph`). Each is a task keyed by the content it reads and the converter code
// that makes it; it lives in the cache folder its key names, so a command asks for it and gets the one its inputs make,
// never a path a person or another session handed over.
import fs from 'node:fs';
import path from 'node:path';
import { CONVERTER_ROOT, machine } from './machine.ts';
import { filesKey, keyedDir, keyOf, logOf, mustRun, passed, runTasksSync, toolKey, treeKey } from './tasks.ts';

/** The whole 4.0's graph took 79 s on this machine; past this the build's process tree is stopped. */
const GRAPH_TIMEOUT_S = 1800;
/** clangd's background index of the game (its own limit, DEFAULT_INDEX_TIMEOUT_S of cppgraph) and the walk after it. */
const CPPGRAPH_TIMEOUT_S = 2400;
/** The graph of a whole program needs more than Node's default heap. */
const GRAPH_HEAP_MB = 12288;
const CLI = path.join(CONVERTER_ROOT, 'src', 'cli.ts');

/** The converter's own code that makes a graph: a change to it makes the graph again. */
function codeKey(dirs: readonly string[]): string {
  const files = dirs.flatMap((d) => fs.readdirSync(path.join(CONVERTER_ROOT, 'src', d)).filter((f) => f.endsWith('.ts')).sort().map((f) => path.join(CONVERTER_ROOT, 'src', d, f)));
  return filesKey(files);
}

export interface ProgramGraphInput {
  readonly sourceRoot: string;
  readonly tsconfig?: string | undefined;
  readonly typescriptDir?: string | undefined;
  readonly include?: readonly string[];
}

/** The whole program's graph of a TypeScript source (no entries): the file that holds it. */
export function programGraph(input: ProgramGraphInput): string {
  const sourceRoot = path.resolve(input.sourceRoot);
  const tsconfig = path.resolve(input.tsconfig ?? path.join(sourceRoot, 'tsconfig.json'));
  const typescript = path.resolve(input.typescriptDir ?? path.join(sourceRoot, 'node_modules', 'typescript'));
  const include = [...(input.include ?? [])];
  const key = keyOf('program-graph', treeKey(sourceRoot), filesKey([tsconfig]), toolKey(typescript), codeKey(['graph', 'common']), process.version, ...include);
  const out = path.join(keyedDir('graph', key), 'program.json');
  fs.mkdirSync(path.dirname(out), { recursive: true });
  const [run] = runTasksSync([{ name: 'converter-program-graph', cwd: CONVERTER_ROOT, extra: [key], outputs: [out], timeoutS: GRAPH_TIMEOUT_S,
    argv: [process.execPath, `--max-old-space-size=${GRAPH_HEAP_MB}`, CLI, 'graph', '--source-root', sourceRoot, '--tsconfig', tsconfig, '--typescript', typescript,
      ...include.flatMap((i) => ['--include', i]), '--out', out] }]);
  if (!passed(mustRun('converter graph', run!)) || !fs.existsSync(out)) throw new Error(`the program's graph was not made:\n${logOf(run!).slice(-4000)}`);
  return out;
}

export interface TargetGraphInput {
  readonly targetRoot: string;
  /** The target's compilation database (default: the machine's "compileCommands", made by the game's debug configure). */
  readonly compileCommands?: string;
  readonly include?: readonly string[];
}

/** The target's C++ graph (its native code through clangd): the file that holds it. */
export function targetGraph(input: TargetGraphInput): string {
  const m = machine();
  const targetRoot = path.resolve(input.targetRoot);
  const compileCommands = path.resolve(input.compileCommands ?? m.get('compileCommands'));
  if (!fs.existsSync(compileCommands)) {
    throw new Error(`no compilation database at ${compileCommands}: configure the game's debug build with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ` +
      '(the sandbox configuration\'s configure-debug step), or name another in configs/roadcraft-machine.json "compileCommands"');
  }
  const include = input.include && input.include.length > 0 ? [...input.include] : ['native/src'];
  const clangd = m.get('clangd');
  const cxx = m.get('cxx');
  const key = keyOf('target-graph', treeKey(targetRoot, include), filesKey([compileCommands]), toolKey(clangd), toolKey(cxx), codeKey(['cppgraph', 'graph', 'common']), process.version, ...include);
  // The graph's file in the converter's cache; clangd's index outside the target it indexes (the converter lives in the game's tree).
  const dir = keyedDir('cppgraph', key);
  const out = path.join(dir, 'cppgraph.json');
  fs.mkdirSync(dir, { recursive: true });
  const [run] = runTasksSync([{ name: 'converter-target-graph', cwd: CONVERTER_ROOT, extra: [key], outputs: [out], timeoutS: CPPGRAPH_TIMEOUT_S,
    argv: [process.execPath, `--max-old-space-size=${GRAPH_HEAP_MB}`, CLI, 'cppgraph', '--root', targetRoot, '--compile-commands', compileCommands, '--clangd', clangd,
      `--clangd-arg=--query-driver=${cxx}`, ...include.flatMap((i) => ['--include', i]), '--cache', path.join(m.get('clangdCache'), key), '--out', out] }]);
  if (!passed(mustRun('converter cppgraph', run!)) || !fs.existsSync(out)) throw new Error(`the target's graph was not made:\n${logOf(run!).slice(-4000)}`);
  return out;
}
