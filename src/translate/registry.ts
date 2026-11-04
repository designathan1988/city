// The places a new subsystem is registered in the target, each changed as text in the file's own format and then
// checked by reading it back the way the target reads it: the layer rule (rules.json cpp_layers), the map of the
// target's instructions (CLAUDE.md between its markers: every file must be listed), and the state's member in the
// state manager. Nothing else in those files moves.

const isDeepStrictEqual = (a: unknown, b: unknown): boolean => JSON.stringify(a) === JSON.stringify(b);

/** The layer rule of a new folder, after the last rule of a sibling folder, indented like it. */
export function addLayer(rulesText: string, folder: string, includes: readonly string[]): string {
  const rules = JSON.parse(rulesText) as { cpp_layers?: Record<string, string[]> };
  const layers = rules.cpp_layers;
  if (!layers) throw new Error('rules.json has no cpp_layers');
  if (layers[folder]) throw new Error(`rules.json already has a layer for ${folder}`);
  const parent = folder.slice(0, folder.lastIndexOf('/') + 1);
  const sibling = Object.keys(layers).filter((k) => k.startsWith(parent)).pop() ?? Object.keys(layers).pop();
  if (!sibling) throw new Error('rules.json cpp_layers is empty');
  const at = rulesText.indexOf(`${JSON.stringify(sibling)}: [`);
  if (at < 0) throw new Error(`cannot find the layer ${sibling} as text in rules.json`);
  const close = rulesText.indexOf(']', at);
  const lineStart = rulesText.lastIndexOf('\n', at) + 1;
  const indent = rulesText.slice(lineStart, at);
  const inner = rulesText.slice(rulesText.indexOf('\n', at) + 1, close).match(/^\s*/)?.[0] ?? `${indent}  `;
  const block = `,\n${indent}${JSON.stringify(folder)}: [\n${includes.map((i) => `${inner}${JSON.stringify(i)}`).join(',\n')}\n${indent}]`;
  const out = rulesText.slice(0, close + 1) + block + rulesText.slice(close + 1);
  const check = JSON.parse(out) as { cpp_layers: Record<string, string[]> };
  const { [folder]: added, ...rest } = check.cpp_layers;
  if (!isDeepStrictEqual(added, [...includes]) || !isDeepStrictEqual(rest, layers)) throw new Error('the layer rule did not land as written');
  return out;
}

/**
 * The map line of a new folder (`dir/**`), after the last line that maps a sibling folder whose path sorts before it.
 * Checked with the gate's own reading: a backquoted `dir/**` between the markers lists every file under dir.
 */
export function addMapLine(text: string, markers: readonly [string, string], folder: string, line: string): string {
  const [start, end] = markers;
  const from = text.indexOf(start);
  const to = text.indexOf(end);
  if (from < 0 || to < from) throw new Error(`the map markers ${start} / ${end} are missing`);
  if (!line.includes(`\`${folder}/**\``)) throw new Error(`the map line must list \`${folder}/**\``);
  const parent = folder.slice(0, folder.lastIndexOf('/') + 1);
  const lines = text.slice(from, to).split('\n');
  let after = -1;
  lines.forEach((l, i) => {
    const first = /`([^`\s]+)`/.exec(l)?.[1];
    if (first && first.startsWith(parent) && first < `${folder}/`) after = i;
  });
  if (after < 0) throw new Error(`no map line of a folder under ${parent} to place ${folder} after`);
  lines.splice(after + 1, 0, line);
  const out = text.slice(0, from) + lines.join('\n') + text.slice(to);
  const section = out.split(start, 2)[1]!.split(end, 2)[0]!;
  if (!new Set([...section.matchAll(/`([^`\s]+)\/\*\*`/g)].map((m) => m[1])).has(folder)) throw new Error('the map line is not read as the gate reads it');
  return out;
}

/** The state's member in the state manager: its header included in order, the member after the anchor line. */
export function addStateMember(header: string, include: string, anchor: string, member: string): string {
  if (header.includes(`#include "${include}"`)) throw new Error(`${include} is already included`);
  const lines = header.split('\n');
  const includes = lines.map((l, i) => [l, i] as const).filter(([l]) => /^#include "/.test(l));
  if (includes.length === 0) throw new Error('the state manager includes no project header to place it among');
  const before = includes.find(([l]) => l > `#include "${include}"`);
  const at = before ? before[1] : includes[includes.length - 1]![1] + 1;
  lines.splice(at, 0, `#include "${include}"`);
  const anchorAt = lines.findIndex((l) => l.includes(anchor));
  if (anchorAt < 0) throw new Error(`the anchor "${anchor}" is not in the state manager`);
  if (lines.findIndex((l, i) => i !== anchorAt && l.includes(anchor)) >= 0) throw new Error(`the anchor "${anchor}" is not unique`);
  const indent = lines[anchorAt]!.match(/^\s*/)![0];
  lines.splice(anchorAt + 1, 0, `${indent}${member}`);
  return lines.join('\n');
}
