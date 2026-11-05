// The code of src/ui/toolChoices.ts that is the feature's (the rest of the file is shared).

// src/ui/toolChoices.ts:19-25 function keep: the feature's own
function keep(key: string, value: string): void {
  try {
    if (typeof localStorage !== 'undefined') localStorage.setItem(key, value);
  } catch {
    // Not kept between sessions; it still applies now.
  }
}

// src/ui/toolChoices.ts:30-32 slice of src/ui/toolChoices.ts#streetscapeKind (shared): the feature's statements; "…" marks code left out
export function streetscapeKind(): LandscapeKind {
  return kind;
}

// src/ui/toolChoices.ts:34-37 function setStreetscapeKind: the feature's own
export function setStreetscapeKind(next: LandscapeKind): void {
  kind = next;
  keep(KIND_KEY, next);
}

// src/ui/toolChoices.ts:42-44 slice of src/ui/toolChoices.ts#poleLampMode (shared): the feature's statements; "…" marks code left out
export function poleLampMode(): PoleLampMode {
  return lamps;
}

// src/ui/toolChoices.ts:46-49 function setPoleLampMode: the feature's own
export function setPoleLampMode(next: PoleLampMode): void {
  lamps = next;
  keep(LAMP_KEY, next);
}

// src/ui/toolChoices.ts:61-63 function setPoleToolMode: the feature's own
export function setPoleToolMode(next: PoleToolMode): void {
  poleMode = next;
}

// src/ui/toolChoices.ts:70-72 function setFogErase: the feature's own
export function setFogErase(next: boolean): void {
  fogErasing = next;
}

// src/ui/toolChoices.ts:84-84 function setTreeMode: the feature's own
export function setTreeMode(next: TreeMode): void { treeModeNow = next; }

// src/ui/toolChoices.ts:86-86 function setTreeKind: the feature's own
export function setTreeKind(next: TreeKind): void { treeKindNow = next; }

// src/ui/toolChoices.ts:98-100 function setGullyErase: the feature's own
export function setGullyErase(next: boolean): void {
  gullyErasing = next;
}

// src/ui/toolChoices.ts:112-112 function setElementMode: the feature's own
export function setElementMode(next: ElementBrushMode): void { elementModeNow = next; }

// src/ui/toolChoices.ts:120-122 function setElementKind: the feature's own
export function setElementKind(next: ElementKind): void {
  elementKindNow = next;
}

// src/ui/toolChoices.ts:131-133 function setCloudMode: the feature's own
export function setCloudMode(next: CloudMode): void {
  cloudModeNow = next;
}

// src/ui/toolChoices.ts:186-188 slice of src/ui/toolChoices.ts#paintKind (shared): the feature's statements; "…" marks code left out
export function paintKind(): PaintKind {
  return paint;
}

// src/ui/toolChoices.ts:190-193 function setPaintKind: the feature's own
export function setPaintKind(next: PaintKind): void {
  paint = next;
  keep(PAINT_KEY, next);
}

// src/ui/toolChoices.ts:203-205 function setRoadWidth: the feature's own
export function setRoadWidth(next: number | null): void {
  width = next === null ? null : Math.max(ROAD_WIDTH_RANGE[0], Math.min(ROAD_WIDTH_RANGE[1], Math.round(next)));
}

// src/ui/toolChoices.ts:219-219 slice of src/ui/toolChoices.ts#zoneColoursShown (shared): the feature's statements; "…" marks code left out
export function zoneColoursShown(): boolean { return zoneColours; }

// src/ui/toolChoices.ts:220-220 function setZoneColoursShown: the feature's own
export function setZoneColoursShown(on: boolean): void { zoneColours = on; keep(ZONE_COLOUR_KEY, on ? 'on' : 'off'); }

// src/ui/toolChoices.ts:226-226 slice of src/ui/toolChoices.ts#roadGridShown (shared): the feature's statements; "…" marks code left out
export function roadGridShown(): boolean { return roadGrid; }

// src/ui/toolChoices.ts:227-231 function setRoadGridShown: the feature's own
export function setRoadGridShown(on: boolean): void {
  roadGrid = on;
  keep(ROAD_GRID_KEY, on ? 'on' : 'off');
  for (const listener of gridListeners) listener();
}
