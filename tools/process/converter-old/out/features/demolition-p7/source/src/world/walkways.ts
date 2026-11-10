// The code of src/world/walkways.ts that is the feature's (the rest of the file is shared).

// src/world/walkways.ts:97-97 field WalkGraph.none: the feature's own
  private readonly none: readonly WalkChange[] = [];

// src/world/walkways.ts:103-107 method WalkGraph.changesSince: the feature's own
  changesSince(state: object): readonly WalkChange[] | null {
    if (state === this.token) return this.none;
    const i = this.log.findIndex((e) => e.from === state);
    return i < 0 ? null : this.log.slice(i).map((e) => e.change);
  }

// src/world/walkways.ts:115-117 method WalkGraph.holds: the feature's own
  holds(way: Walkway): boolean {
    return this.ways[way.id] === way;
  }
