// The code of src/core/health.ts that is the feature's (the rest of the file is shared).

// src/core/health.ts:51-53 getter HealthLog.changes: the feature's own
  get changes(): number {
    return this.touched;
  }

// src/core/health.ts:94-103 method HealthLog.status: the feature's own
  status(windowMs = 30_000): 'ok' | 'slow' | 'broken' {
    const since = performance.now() - windowMs;
    let slow = false;
    for (const entry of this.latest(KEPT)) {
      if (entry.earlier || entry.lastAt < since) continue;
      if (entry.severity === 'broken') return 'broken';
      if (entry.severity !== 'warning') slow = true;
    }
    return slow ? 'slow' : 'ok';
  }

// src/core/health.ts:106-108 method HealthLog.toJSON: the feature's own
  toJSON(): HealthEntry[] {
    return this.latest(50).reverse();
  }

// src/core/health.ts:111-119 method HealthLog.restore: the feature's own
  restore(entries: readonly HealthEntry[]): void {
    for (const entry of entries) {
      if (!entry || typeof entry.message !== 'string' || typeof entry.kind !== 'string') continue;
      const serial = ++this.serial;
      this.ring[serial % KEPT] = { ...entry, serial, earlier: true, at: -Infinity, lastAt: -Infinity };
      if (this.count < KEPT) this.count++;
    }
    this.touched++;
  }

// src/core/health.ts:129-129 function systemKey: the feature's own
export const systemKey = (system: string): string => `health.system.${system}`;

// src/core/health.ts:171-173 method FrameTimer.between: the feature's own
  between(from: number, to: number): FrameRecord[] {
    return this.frames.filter((f) => f.end >= from && f.start <= to);
  }

// src/core/health.ts:212-216 function systemsOf: the feature's own
export function systemsOf(frames: readonly FrameRecord[]): [string, number][] {
  const sum = new Map<string, number>();
  for (const f of frames) for (const [name, ms] of f.systems) sum.set(name, (sum.get(name) ?? 0) + ms);
  return [...sum].filter(([, ms]) => ms >= 0.5).sort((a, b) => b[1] - a[1]).map(([name, ms]) => [name, Math.round(ms * 10) / 10]);
}
