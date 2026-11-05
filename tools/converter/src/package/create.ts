// Builds a conversion package without touching the game: a throw-away isolated clone of the game at its base commit
// receives the generated files topic by topic, each topic one commit, and the commits leave as a patch series
// (`git format-patch`) next to the manifest. The clone is deleted afterwards; the game is only read. A port written
// in a workspace (sandbox/workspace.ts) is packaged the same way, from the commits of its branch.
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { removeTree, sha256 } from '../common/files.ts';
import { cloneIsolated, git, revParse, uncommitted } from '../common/git.ts';
import { normalizePath } from '../common/paths.ts';
import { MANIFEST, PACKAGE_VERSION, PATCHES, type PackageManifest } from './manifest.ts';

/** One commit of the package: a message and the files it writes (paths relative to the game root). */
export interface Topic {
  readonly message: string;
  readonly files: ReadonlyMap<string, string>;
}

export interface PackageInput {
  readonly feature: string;
  readonly sourceRoot: string;
  /** The source files the conversion read, relative to the source root (recorded by content hash). */
  readonly sourceFiles: readonly string[];
  readonly gameRoot: string;
  /** The game commit the package is made against (default: the game's HEAD). */
  readonly base?: string | undefined;
  readonly outDir: string;
  readonly author: { readonly name: string; readonly email: string };
  /** The repository of the converter making the package (default: this converter). */
  readonly converterRoot?: string | undefined;
  /** Edits the clone sees: file contents are read from it, so a topic may extend an existing file. */
  readonly topics: (readClone: (relative: string) => string | null) => readonly Topic[];
}

/** The converter's own repository. */
export const CONVERTER_ROOT = path.resolve(import.meta.dirname, '..', '..');

/** The converter's commit; refused when its tree has changes, which that commit would not describe. */
function converterCommit(root: string): string {
  const changed = uncommitted(root);
  if (changed.length > 0) throw new Error(`the converter has uncommitted changes (${changed.slice(0, 5).join(', ')}): commit them so the package names the code that made it`);
  return revParse(root, 'HEAD');
}

/** The source as read: its commit and the content hash of each file the conversion read. */
function sourceRecord(sourceRoot: string, files: readonly string[]): PackageManifest['source'] {
  return {
    root: normalizePath(sourceRoot),
    commit: revParse(sourceRoot, 'HEAD'),
    files: [...files].sort().map((relative) => {
      const file = path.join(sourceRoot, relative);
      if (!fs.existsSync(file)) throw new Error(`source file ${relative} does not exist under ${sourceRoot}`);
      return { path: relative.replace(/\\/g, '/'), sha256: sha256(file) };
    }),
  };
}

/** The package folder: outside the game, and empty. */
function packageDir(outDir: string, gameRoot: string): string {
  const dir = path.resolve(outDir);
  if (normalizePath(dir).startsWith(gameRoot + '/')) throw new Error(`the package must live outside the game (${dir})`);
  if (fs.existsSync(dir) && fs.readdirSync(dir).length > 0) throw new Error(`${dir} is not empty`);
  return dir;
}

/** The commits of `range` in `repo` as a patch series in the package, and the manifest beside it. */
function writeSeries(repo: string, range: string, outDir: string, manifest: Omit<PackageManifest, 'patches'>): PackageManifest {
  fs.mkdirSync(path.join(outDir, PATCHES), { recursive: true });
  // format-patch prints the name of each file it writes (--quiet would suppress exactly that list).
  const patches = git(repo, ['format-patch', '--no-signature', '-o', path.join(outDir, PATCHES), range])
    .split(/\r?\n/).filter(Boolean).map((f) => path.basename(f));
  const written: PackageManifest = { ...manifest, patches };
  fs.writeFileSync(path.join(outDir, MANIFEST), JSON.stringify(written, null, 2) + '\n', 'utf8');
  return written;
}

export function createPackage(input: PackageInput): PackageManifest {
  const gameRoot = normalizePath(input.gameRoot);
  const base = revParse(gameRoot, input.base ?? 'HEAD');
  const source = sourceRecord(input.sourceRoot, input.sourceFiles);
  const converter = converterCommit(input.converterRoot ?? CONVERTER_ROOT);
  const outDir = packageDir(input.outDir, gameRoot);
  const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-package-'));
  try {
    cloneIsolated(gameRoot, work, base);
    const identity = ['-c', `user.name=${input.author.name}`, '-c', `user.email=${input.author.email}`];
    const read = (relative: string): string | null => {
      const file = path.join(work, relative);
      return fs.existsSync(file) ? fs.readFileSync(file, 'utf8') : null;
    };
    const topics = input.topics(read);
    if (topics.length === 0) throw new Error(`feature ${input.feature}: nothing to package`);
    for (const topic of topics) {
      for (const [relative, content] of topic.files) {
        const file = path.join(work, relative);
        fs.mkdirSync(path.dirname(file), { recursive: true });
        fs.writeFileSync(file, content, 'utf8');
      }
      git(work, ['add', '--', ...topic.files.keys()]);
      git(work, [...identity, 'commit', '--quiet', '-F', '-'], topic.message);
    }
    return writeSeries(work, `${base}..HEAD`, outDir, {
      version: PACKAGE_VERSION, feature: input.feature, source, game: { root: gameRoot, base }, converter: { commit: converter },
      author: { name: input.author.name, email: input.author.email },
    });
  } finally {
    removeTree(work);
  }
}

export interface BranchPackageInput {
  readonly feature: string;
  readonly sourceRoot: string;
  /** The source files the port read, relative to the source root (recorded by content hash). */
  readonly sourceFiles: readonly string[];
  readonly gameRoot: string;
  /** The repository holding the port (a workspace), the game commit it starts from, and its branch. */
  readonly repo: string;
  readonly base: string;
  readonly branch: string;
  readonly outDir: string;
  readonly converterRoot?: string | undefined;
}

/**
 * A port committed topic by topic on a branch of a workspace, as a package: the branch's commits since the base become
 * the patch series. Only commits travel, so a tree with changes not committed is refused (they would be lost).
 */
export function packageBranch(input: BranchPackageInput): PackageManifest {
  const gameRoot = normalizePath(input.gameRoot);
  const changed = uncommitted(input.repo);
  if (changed.length > 0) {
    throw new Error(`the workspace has changes not committed (${changed.slice(0, 8).join(', ')}): commit each with its topic, ` +
      'or, when the verification of the game made it (a Godot .uid the game lacks), in a topic of its own');
  }
  const range = `${input.base}..${input.branch}`;
  const count = Number(git(input.repo, ['rev-list', '--count', range]).trim());
  if (count === 0) throw new Error(`${input.branch} has no commits since ${input.base.slice(0, 12)}: nothing to package`);
  // The identity the commits carry, so that applying them in a sandbox commits under the same one.
  const [name = '', email = ''] = git(input.repo, ['log', '-1', '--format=%an%n%ae', input.branch]).trim().split('\n');
  const outDir = packageDir(input.outDir, gameRoot);
  return writeSeries(input.repo, range, outDir, {
    version: PACKAGE_VERSION, feature: input.feature, source: sourceRecord(input.sourceRoot, input.sourceFiles),
    game: { root: gameRoot, base: revParse(gameRoot, input.base) }, converter: { commit: converterCommit(input.converterRoot ?? CONVERTER_ROOT) },
    author: { name, email },
  });
}
