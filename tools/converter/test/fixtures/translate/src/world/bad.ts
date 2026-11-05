// Fixture: an edit that reads an index of the state it changes (refused: its pointers may be stale).
import { identityMemo } from '../core/memo';

export interface BinData {
  readonly items: readonly number[];
}

const sizes = identityMemo((t: BinData) => {
  const all = new Map<number, number>();
  for (const v of t.items) all.set(v, v);
  return all;
});

export function addItem(t: BinData, v: number): BinData {
  return { ...t, items: sizes(t).has(v) ? t.items : [...t.items, v] };
}
