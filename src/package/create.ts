// Builds a conversion package without touching the game: a throw-away isolated clone of the game at its base commit
// receives the generated files topic by topic, each topic one commit, and the commits leave as a patch series
// (`git format-patch`) next to the manifest. The clone is deleted afterwards; the game is only read.
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

export function createPackage(input: PackageInput): PackageManifest {
  const gameRoot = normalizePath(input.gameRoot);
  const base = revParse(gameRoot, input.base ?? 'HEAD');
  const sourceCommit = revParse(input.sourceRoot, 'HEAD');
  const converter = converterCommit(input.converterRoot ?? CONVERTER_ROOT);
  const sourceFiles = [...input.sourceFiles].sort().map((relative) => {
    const file = path.join(input.sourceRoot, relative);
    if (!fs.existsSync(file)) throw new Error(`source file ${relative} does not exist under ${input.sourceRoot}`);
    return { path: relative.replace(/\\/g, '/'), sha256: sha256(file) };
  });
  const outDir = path.resolve(input.outDir);
  if (normalizePath(outDir).startsWith(gameRoot + '/')) throw new Error(`the package must live outside the game (${outDir})`);
  if (fs.existsSync(outDir) && fs.readdirSync(outDir).length > 0) throw new Error(`${outDir} is not empty`);
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
    fs.mkdirSync(path.join(outDir, PATCHES), { recursive: true });
    // format-patch prints the name of each file it writes (--quiet would suppress exactly that list).
    const written = git(work, ['format-patch', '--no-signature', '-o', path.join(outDir, PATCHES), `${base}..HEAD`])
      .split(/\r?\n/).filter(Boolean).map((f) => path.basename(f));
    const manifest: PackageManifest = {
      version: PACKAGE_VERSION,
      feature: input.feature,
      source: { root: normalizePath(input.sourceRoot), commit: sourceCommit, files: sourceFiles },
      game: { root: gameRoot, base },
      converter: { commit: converter },
      author: { name: input.author.name, email: input.author.email },
      patches: written,
    };
    fs.writeFileSync(path.join(outDir, MANIFEST), JSON.stringify(manifest, null, 2) + '\n', 'utf8');
    return manifest;
  } finally {
    removeTree(work);
  }
}
