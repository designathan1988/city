// The code of src/core/idSlots.ts that is the feature's (the rest of the file is shared).

// src/core/idSlots.ts:38-40 getter IdSlots.size: the feature's own
  get size(): number {
    return this.live;
  }

// src/core/idSlots.ts:43-45 getter IdSlots.capacity: the feature's own
  get capacity(): number {
    return this.capacityNow;
  }

// src/core/idSlots.ts:91-94 method IdSlots.forEach: the feature's own
  forEach(visit: (id: number, slot: number) => void): void {
    const keys = this.keys, states = this.states, values = this.values;
    for (let i = 0; i < keys.length; i++) if (states[i] === FULL) visit(keys[i]!, values[i]!);
  }
