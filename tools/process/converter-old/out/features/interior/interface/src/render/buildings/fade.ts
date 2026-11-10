// What the feature uses of src/render/buildings/fade.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/fade.ts:10-15 function partLook
/** How a part kind is drawn faded ("Ocultar outros") or not: its material, its order and whether it is drawn at all (a lantern's light is not). */
export function partLook(kit: BuildingKit, kind: PartKind, faded: boolean): { material: Material; renderOrder: number; visible: boolean };

// src/render/buildings/fade.ts:17-43 function fadeMeshes
export function fadeMeshes(root: Object3D, kit: BuildingKit, faded: boolean): void;
