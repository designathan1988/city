// Git operations the package and sandbox stages need. Every command names its repository explicitly (`-C dir`),
// so no operation can land in a repository by accident; the game's repository is only ever read.
import { spawnSync } from 'node:child_process';

export class GitError extends Error {}

export function git(dir: string, args: readonly string[], input?: string): string {
  const result = spawnSync('git', ['-C', dir, ...args], { encoding: 'utf8', input, maxBuffer: 256 * 1024 * 1024 });
  if (result.error) throw new GitError(`git ${args.join(' ')}: ${result.error.message}`);
  if (result.status !== 0) throw new GitError(`git ${args.join(' ')} in ${dir} failed (${result.status}):\n${result.stderr}`);
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
    { encoding: 'utf8' });
  if (result.status !== 0) throw new GitError(`git clone ${source} -> ${target} failed:\n${result.stderr}`);
  git(target, ['checkout', '--quiet', '--detach', commit]);
}

/** Everything that identifies a repository's state: where HEAD is, its branches, and its working tree. */
export function fingerprint(dir: string): string {
  return [git(dir, ['rev-parse', 'HEAD']), git(dir, ['for-each-ref', '--format=%(refname) %(objectname)']), git(dir, ['status', '--porcelain'])].join('\n');
}
