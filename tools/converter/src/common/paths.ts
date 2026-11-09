// One normal form for file identities (absolute, forward slashes, upper-case drive letter), so paths that come from
// the TypeScript compiler, the command line and the file system compare equal.
import path from 'node:path';

export function normalizePath(file: string): string {
  return path
    .resolve(file)
    .split(path.sep)
    .join('/')
    .replace(/^([a-z]):/, (_match, drive: string) => drive.toUpperCase() + ':');
}

export function relativePath(root: string, file: string): string {
  return path.relative(root, file).split(path.sep).join('/');
}

export function isInside(directory: string, file: string): boolean {
  const prefix = directory.endsWith('/') ? directory : directory + '/';
  return file.startsWith(prefix);
}
