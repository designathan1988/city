// The code of src/world/buildings/types.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/types.ts:368-370 function cloneBuilding: the feature's own
export function cloneBuilding<T extends Building>(b: T): T {
  return jsonCopy(b) as T;
}

// src/world/buildings/types.ts:373-395 function jsonCopy: the feature's own
export function jsonCopy(value: unknown): unknown {
  switch (typeof value) {
    case 'number': return Number.isFinite(value) ? value + 0 : null;
    case 'string': case 'boolean': return value;
    case 'object': break;
    default: return undefined;
  }
  if (value === null) return null;
  if (Array.isArray(value)) {
    const out = new Array<unknown>(value.length);
    for (let i = 0; i < value.length; i++) { const v = jsonCopy(value[i]); out[i] = v === undefined ? null : v; }
    return out;
  }
  const proto = Object.getPrototypeOf(value) as unknown;
  if ((proto !== Object.prototype && proto !== null) || typeof (value as { toJSON?: unknown }).toJSON === 'function') return JSON.parse(JSON.stringify(value)) as unknown;
  const out: Record<string, unknown> = {};
  for (const key of Object.keys(value)) {
    const v = jsonCopy((value as Record<string, unknown>)[key]);
    // A member named `__proto__` is a member as JSON.parse makes it, not the copy's prototype.
    if (v !== undefined) { if (key === '__proto__') Object.defineProperty(out, key, { value: v, enumerable: true, writable: true, configurable: true }); else out[key] = v; }
  }
  return out;
}

// src/world/buildings/types.ts:449-451 function volumeById: the feature's own
export function volumeById(b: Building, id: number): Volume | undefined {
  return b.volumes.find((v) => v.id === id);
}
