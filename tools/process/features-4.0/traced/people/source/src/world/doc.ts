// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:346-346 slice of src/world/doc.ts#RoadDoc.people (shared): the feature's statements; "…" marks code left out
  readonly people: PersonSpec[] = [];

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 53 lines
    if (moved('people') && (whole || !sameText(this.people, source.people))) {
      // … 2 lines
      pushAll(this.people, source.people.map((p) => JSON.parse(JSON.stringify(p)) as PersonSpec));
    // … 1 lines
    }
  // … 86 lines
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 16 lines
      case 'people': return this.people.length > 0 ? { people: this.people.map((p) => JSON.parse(JSON.stringify(p)) as PersonSpec) } : {};
    // … 5 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 175 lines
    for (const raw of data.people ?? []) {
      const person = normalizePerson(raw);
      if (!person || seen.has(person.id)) continue;
      seen.add(person.id);
      doc.people.push(person);
    }
  // … 4 lines
  }
