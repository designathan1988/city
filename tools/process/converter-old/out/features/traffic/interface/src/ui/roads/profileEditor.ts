// What the feature uses of src/ui/roads/profileEditor.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/profileEditor.ts:33-33 type PaletteKind
type PaletteKind = 'footway' | 'laneBackward' | 'laneForward' | 'median' | 'parking' | 'cycle';

// src/ui/roads/profileEditor.ts:69-79 function newElement
/** A new element of each kind, as the palette adds it; a lane as wide as the road's others. */
function newElement(kind: PaletteKind, profile: RoadProfileSpec): ProfileElement;
