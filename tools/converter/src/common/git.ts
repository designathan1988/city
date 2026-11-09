// Git operations the package and sandbox stages need. Every command names its repository explicitly (`-C dir`),
// so no operation can land in a repository by accident; the game's repository is only ever read.
import { spawnSync } from 'node:child_process';
import { MAX_CHILD_OUTPUT_BYTES } from './files.ts';

export class GitError extends Error {}

/** A git command of the stages (a query, a checkout, a commit of a package): past this its process is stopped. */
const GIT_TIMEOUT_MS = 600_000;
/** A clone copies every object of the game (`--no-hardlinks`): the longest git command the stages run. */
const CLONE_TIMEOUT_MS = 3_600_000;

function failed(what: string, dir: string, result: ReturnType<typeof spawnSync>): GitError | null {
  if (result.error) return new GitError(`${what} in ${dir}: ${result.error.message}`);
  if (result.status !== 0) return new GitError(`${what} in ${dir} failed (${result.status}):\n${String(result.stderr)}`);
  return null;
}

export function git(dir: string, args: readonly string[], input?: string): string {
  const result = spawnSync('git', ['-C', dir, ...args], { encoding: 'utf8', input, maxBuffer: MAX_CHILD_OUTPUT_BYTES, timeout: GIT_TIMEOUT_MS, windowsHide: true });
  const error = failed(`git ${args.join(' ')}`, dir, result);
  if (error) throw error;
  return result.stdout;
}

export function revParse(dir: string, ref: string): string {
  return git(dir, ['rev-parse', '--verify', `${ref}^{commit}`]).trim();
}

/**
 * A clone that shares nothing with its source: objects copied, not hard-linked (`--no-hardlinks`), line endings as
 * stored in the repository (`core.autocrlf=false`, set before checkout), detached at `commit`.
 */
export function cloneIsolated(source: string, target: string, commit: string): void {
  const result = spawnSync('git', ['clone', '--no-hardlinks', '--no-checkout', '-c', 'core.autocrlf=false', source, target],
    { encoding: 'utf8', timeout: CLONE_TIMEOUT_MS, windowsHide: true });
  const error = failed(`git clone ${source} -> ${target}`, source, result);
  if (error) throw error;
  git(target, ['checkout', '--quiet', '--detach', commit]);
}

/** Paths with changes not committed (staged, unstaged or untracked, ignored files excepted). */
export function uncommitted(dir: string): string[] {
  return git(dir, ['status', '--porcelain']).split(/\r?\n/).filter(Boolean).map((line) => line.slice(3));
}

/** Everything that identifies a repository's state: where HEAD is, its branches, and its working tree. */
export function fingerprint(dir: string): string {
  return [git(dir, ['rev-parse', 'HEAD']), git(dir, ['for-each-ref', '--format=%(refname) %(objectname)']), git(dir, ['status', '--porcelain'])].join('\n');
}
