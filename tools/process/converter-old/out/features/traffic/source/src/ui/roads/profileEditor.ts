// The code of src/ui/roads/profileEditor.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/profileEditor.ts:69-79 slice of src/ui/roads/profileEditor.ts#newElement (shared): the feature's statements; "…" marks code left out
function newElement(kind: PaletteKind, profile: RoadProfileSpec): ProfileElement {
  // … 1 lines
  switch (kind) {
    // … 4 lines
    case 'parking': return { kind: 'parking', width: PARKING_DEPTH.parallel };
    case 'cycle': return { kind: 'cycle', width: PARKING_DEPTH.cycle };
  }
}
