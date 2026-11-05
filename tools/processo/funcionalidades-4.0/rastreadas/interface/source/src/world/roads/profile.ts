// The code of src/world/roads/profile.ts that is the feature's (the rest of the file is shared).

// src/world/roads/profile.ts:266-267 function profileWidth: the feature's own
export const profileWidth = (profile: RoadProfileSpec): number =>
  profile.elements.reduce((sum, x) => sum + x.width, 0);
