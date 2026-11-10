// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:216-216 slice of src/world/doc.ts#RoadDoc.clouds (shared): the feature's statements; "…" marks code left out
  readonly clouds: PlacedCloud[] = [];

// src/world/doc.ts:1263-1270 method RoadDoc.addCloud: the feature's own
  addCloud(value: Omit<PlacedCloud, 'id'>): PlacedCloud | null {
    if (this.clouds.length >= MAX_PLACED_CLOUDS) return null;
    this.ownList('clouds');
    const id = this.clouds.reduce((m, c) => Math.max(m, c.id), 0) + 1;
    const cloud = placedCloud(value, id);
    this.clouds.push(cloud);    this.changes.record('clouds', [rectAround([cloud], cloud.size)!], { ids: [id] });
    return cloud;
  }

// src/world/doc.ts:1273-1286 slice of src/world/doc.ts#RoadDoc.addClouds (shared): the feature's statements; "…" marks code left out
  addClouds(values: readonly Omit<PlacedCloud, 'id'>[]): number {
    // … 1 lines
    let id = this.clouds.reduce((m, c) => Math.max(m, c.id), 0);
    // … 1 lines
    for (const value of values) {
      if (this.clouds.length >= MAX_PLACED_CLOUDS) break;
      this.clouds.push(placedCloud(value, ++id));
      added++;
    }
    if (added > 0) {      const placed = this.clouds.slice(-added);
      this.changes.record('clouds', [rectAround(placed, Math.max(...placed.map((c) => c.size)))!]);
    }
  // … 1 lines
  }

// src/world/doc.ts:1288-1295 method RoadDoc.updateCloud: the feature's own
  updateCloud(id: number, change: Partial<Omit<PlacedCloud, 'id'>>): void {
    const i = this.clouds.findIndex((c) => c.id === id);
    if (i < 0) return;
    this.ownList('clouds');
    const was = this.clouds[i]!;
    const now = { ...was, ...change, id };
    this.clouds[i] = now;    this.changes.record('clouds', [rectAround([was, now], Math.max(was.size, now.size))!], { ids: [id] });
  }

// src/world/doc.ts:1297-1302 slice of src/world/doc.ts#RoadDoc.removeCloud (shared): the feature's statements; "…" marks code left out
  removeCloud(id: number): void {
    const i = this.clouds.findIndex((c) => c.id === id);
    if (i < 0) return;
    // … 1 lines
    const [was] = this.clouds.splice(i, 1);    this.changes.record('clouds', [rectAround([was!], was!.size)!], { ids: [id] });
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 73 lines
    for (const raw of data.clouds ?? []) {
      const cloud = readCloud(raw);
      if (!cloud || doc.clouds.length >= MAX_PLACED_CLOUDS || cloudIds.has(cloud.id)) continue;
      cloudIds.add(cloud.id);
      doc.clouds.push(cloud);
    }
  // … 106 lines
  }

// src/world/doc.ts:3314-3315 slice of src/world/doc.ts#placedCloud (shared): the feature's statements; "…" marks code left out
const placedCloud = (v: Omit<PlacedCloud, 'id'>, id: number): PlacedCloud =>
  ({ id, x: v.x, y: v.y, height: v.height, size: v.size, density: v.density, yaw: v.yaw });

// src/world/doc.ts:3345-3359 slice of src/world/doc.ts#listBox (shared): the feature's statements; "…" marks code left out
function listBox(part: ListPart, items: readonly unknown[]): ChangeRect | null | undefined {
  // … 1 lines
  switch (part) {
    // … 4 lines
    case 'clouds': {
      // … 1 lines
      for (const c of items as readonly PlacedCloud[]) pad = Math.max(pad, c.size);
      return rectAround(items as readonly PlacedCloud[], pad);
    }
  // … 1 lines
  }
}
