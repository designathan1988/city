// The code of src/ui/toolChoices.ts that is the feature's (the rest of the file is shared).

// src/ui/toolChoices.ts:28-28 slice of src/ui/toolChoices.ts#kind (shared): the feature's statements; "…" marks code left out
let kind: LandscapeKind = stored(KIND_KEY, LANDSCAPE_KINDS, 'tree');

// src/ui/toolChoices.ts:30-32 slice of src/ui/toolChoices.ts#streetscapeKind (shared): the feature's statements; "…" marks code left out
export function streetscapeKind(): LandscapeKind {
// … 1 lines
}

// src/ui/toolChoices.ts:34-37 slice of src/ui/toolChoices.ts#setStreetscapeKind (shared): the feature's statements; "…" marks code left out
export function setStreetscapeKind(next: LandscapeKind): void {
  kind = next;
  keep(KIND_KEY, next);
}

// src/ui/toolChoices.ts:78-78 slice of src/ui/toolChoices.ts#treeKindNow (shared): the feature's statements; "…" marks code left out
let treeKindNow: TreeKind = 'mixed';

// src/ui/toolChoices.ts:80-82 slice of src/ui/toolChoices.ts#treeBrushNow (shared): the feature's statements; "…" marks code left out
let treeBrushNow: TreeBrush = (() => {
  try { return { ...DEFAULT_TREE_BRUSH, ...((JSON.parse(localStorage.getItem(TREE_BRUSH_KEY) ?? '{}') ?? {}) as Partial<TreeBrush>) }; } catch { return DEFAULT_TREE_BRUSH; }
})();

// src/ui/toolChoices.ts:85-85 slice of src/ui/toolChoices.ts#treeKind (shared): the feature's statements; "…" marks code left out
export function treeKind(): TreeKind { return treeKindNow; }

// src/ui/toolChoices.ts:86-86 slice of src/ui/toolChoices.ts#setTreeKind (shared): the feature's statements; "…" marks code left out
export function setTreeKind(next: TreeKind): void { treeKindNow = next; }

// src/ui/toolChoices.ts:87-87 slice of src/ui/toolChoices.ts#treeBrush (shared): the feature's statements; "…" marks code left out
export function treeBrush(): TreeBrush { return treeBrushNow; }

// src/ui/toolChoices.ts:88-91 slice of src/ui/toolChoices.ts#setTreeBrush (shared): the feature's statements; "…" marks code left out
export function setTreeBrush(change: Partial<TreeBrush>): void {
  treeBrushNow = { ...treeBrushNow, ...change };
  try { localStorage.setItem(TREE_BRUSH_KEY, JSON.stringify(treeBrushNow)); } catch { /* not kept */ }
}

// src/ui/toolChoices.ts:183-183 slice of src/ui/toolChoices.ts#paint (shared): the feature's statements; "…" marks code left out
let paint: PaintKind = stored(PAINT_KEY, PAINT_KINDS, 'sand');

// src/ui/toolChoices.ts:186-188 slice of src/ui/toolChoices.ts#paintKind (shared): the feature's statements; "…" marks code left out
export function paintKind(): PaintKind {
// … 1 lines
}

// src/ui/toolChoices.ts:190-193 slice of src/ui/toolChoices.ts#setPaintKind (shared): the feature's statements; "…" marks code left out
export function setPaintKind(next: PaintKind): void {
  paint = next;
  keep(PAINT_KEY, next);
}

// src/ui/toolChoices.ts:211-211 slice of src/ui/toolChoices.ts#signChoice (shared): the feature's statements; "…" marks code left out
export const signChoice: { type: SignType; text: string; streetName: string } = { type: 'stop', text: '', streetName: '' };
