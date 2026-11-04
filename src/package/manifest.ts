// A conversion package: one feature converted, stored outside the game as a series of commits (one patch per
// topic), with what it was made from, so the same inputs always give the same package.
import fs from 'node:fs';
import path from 'node:path';

export const PACKAGE_VERSION = 1;

export interface PackageManifest {
  readonly version: number;
  readonly feature: string;
  readonly source: { readonly root: string; readonly commit: string };
  readonly game: { readonly root: string; readonly base: string };
  readonly converter: { readonly commit: string };
  /** Who the package's commits are by; applying them commits under the same identity. */
  readonly author: { readonly name: string; readonly email: string };
  /** File names under patches/, in the order they apply. */
  readonly patches: readonly string[];
}

export const MANIFEST = 'manifest.json';
export const PATCHES = 'patches';
export const EVIDENCE = 'evidence';

export function readManifest(packageDir: string): PackageManifest {
  const file = path.join(packageDir, MANIFEST);
  if (!fs.existsSync(file)) throw new Error(`no ${MANIFEST} in ${packageDir}: not a conversion package`);
  const manifest = JSON.parse(fs.readFileSync(file, 'utf8')) as PackageManifest;
  if (manifest.version !== PACKAGE_VERSION) throw new Error(`${file} is package version ${manifest.version}; this converter reads ${PACKAGE_VERSION}`);
  for (const patch of manifest.patches) {
    if (!fs.existsSync(path.join(packageDir, PATCHES, patch))) throw new Error(`${file} lists ${patch}, which is missing from ${PATCHES}/`);
  }
  return manifest;
}
