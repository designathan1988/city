// The code of src/world/section.ts that is the feature's (the rest of the file is shared).

// src/world/section.ts:80-80 function zonesOn: the feature's own
export const zonesOn = (section: CrossSection, side: RoadSide): SideZones => (side === 'left' ? section.left : section.right);
