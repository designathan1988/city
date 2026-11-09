// What the source's HTML pages load: the scripts the program starts from (Vite's `<script type="module" src>`) and
// the style sheets they link. Paths are relative to the source root, which is what a leading `/` means to Vite.
import fs from 'node:fs';
import path from 'node:path';

function pages(sourceRoot: string): string[] {
  return fs.readdirSync(sourceRoot).filter((f) => f.endsWith('.html')).sort().map((f) => fs.readFileSync(path.join(sourceRoot, f), 'utf8'));
}

const rooted = (href: string): string => href.replace(/\\/g, '/').replace(/^\//, '');

/** The scripts the program starts from. */
export function htmlRoots(sourceRoot: string): string[] {
  const roots = pages(sourceRoot).flatMap((html) => [...html.matchAll(/<script\b[^>]*\btype="module"[^>]*\bsrc="([^"]+)"/g)].map((m) => rooted(m[1]!)));
  if (roots.length === 0) throw new Error(`no <script type="module" src> in the HTML pages of ${sourceRoot}: the program has no root`);
  return roots;
}

/** The style sheets the pages link. */
export function htmlStyleSheets(sourceRoot: string): string[] {
  return pages(sourceRoot).flatMap((html) => [...html.matchAll(/<link\b[^>]*\brel="stylesheet"[^>]*>/g)]
    .map((m) => /\bhref="([^"]+)"/.exec(m[0])?.[1]).filter((h): h is string => h !== undefined && !/^[a-z]+:/i.test(h)).map(rooted));
}
