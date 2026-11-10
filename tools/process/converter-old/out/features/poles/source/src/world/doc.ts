// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:436-438 method RoadDoc.pole: the feature's own
  pole(id: PoleId): UtilityPole | undefined {
    return this.poles.get(id);
  }
