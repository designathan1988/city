// The code of src/render/groundChanges.ts that is the feature's (the rest of the file is shared).

// src/render/groundChanges.ts:38-54 method GroundChanges.touches: the feature's own
  touches(since: number, area: Area): boolean {
    if (area === null || since >= this.serial) return false;
    const list = isRect(area) ? [area] : area;
    if (list.length === 0) return false;
    if (since < this.forgotten) return true;
    let all: Rect | null = null;
    for (const r of list) all = unionRect(all, r);
    for (let i = this.log.length - 1; i >= 0; i--) {
      const entry = this.log[i]!;
      if (entry.serial <= since) break;
      const r = entry.rect;
      if (r === null) return true;
      if (!overlaps(r, all!)) continue;
      for (const a of list) if (overlaps(r, a)) return true;
    }
    return false;
  }

// src/render/groundChanges.ts:72-72 function isRect: the feature's own
const isRect = (area: Rect | readonly Rect[]): area is Rect => typeof area[0] === 'number';

// src/render/groundChanges.ts:74-74 function overlaps: the feature's own
const overlaps = (r: Rect, a: Rect): boolean => r[0] <= a[2] && r[2] >= a[0] && r[1] <= a[3] && r[3] >= a[1];
