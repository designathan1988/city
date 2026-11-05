// What the feature uses of src/ui/roads/drawProfile.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/drawProfile.ts:4-8 interface DrawProfile
/** The profile the road tool lays new roads with, chosen in the profile editor */
export interface DrawProfile {
  readonly name: string;
  readonly type: number;
  readonly profile: RoadProfileSpec;
}

// src/ui/roads/drawProfile.ts:12-14 function drawProfile
export function drawProfile(): DrawProfile | null;

// src/ui/roads/drawProfile.ts:16-18 function setDrawProfile
export function setDrawProfile(next: DrawProfile | null): void;
