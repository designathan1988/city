// The code of src/ui/roads/keys.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/keys.ts:53-56 function save: the feature's own
function save(): void {
  try { globalThis.localStorage?.setItem(STORAGE_KEY, JSON.stringify(bindings)); } catch { /* kept for this session */ }
  for (const listener of listeners) listener();
}

// src/ui/roads/keys.ts:58-60 function roadKey: the feature's own
export function roadKey(action: RoadKeyAction): string {
  return bindings[action];
}

// src/ui/roads/keys.ts:63-65 function bindableKey: the feature's own
export function bindableKey(key: string): boolean {
  return !reserved(normaliseKey(key));
}

// src/ui/roads/keys.ts:68-78 function setRoadKey: the feature's own
export function setRoadKey(action: RoadKeyAction, key: string): boolean {
  const k = normaliseKey(key);
  if (!bindableKey(k)) return false;
  const other = ROAD_KEY_ACTIONS.find((a) => a !== action && bindings[a] === k);
  const next = { ...bindings };
  if (other) next[other] = bindings[action];
  next[action] = k;
  bindings = next;
  save();
  return true;
}

// src/ui/roads/keys.ts:80-83 function resetRoadKeys: the feature's own
export function resetRoadKeys(): void {
  bindings = { ...DEFAULTS };
  save();
}

// src/ui/roads/keys.ts:86-89 slice of src/ui/roads/keys.ts#roadKeyAction (shared): the feature's statements; "…" marks code left out
export function roadKeyAction(key: string): RoadKeyAction | null {
  // … 1 lines
  return ROAD_KEY_ACTIONS.find((a) => bindings[a] === k) ?? null;
}

// src/ui/roads/keys.ts:105-108 slice of src/ui/roads/keys.ts#roadKeyParams (shared): the feature's statements; "…" marks code left out
export function roadKeyParams(): Record<string, string> {
  // … 1 lines
  return paramsOf(bindings);
}
