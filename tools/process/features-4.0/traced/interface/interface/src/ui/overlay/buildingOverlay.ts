// What the feature uses of src/ui/overlay/buildingOverlay.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/overlay/buildingOverlay.ts:23-42 interface BuildingOverlayInput
/** The building tool's hairlines on the overlay canvas */
export interface BuildingOverlayInput {
  readonly project: (x: number, y: number, z: number) => Vec2;
  readonly stage?: 'sketch' | 'shape' | 'facade' | 'roof';
  readonly plan?: { readonly points: readonly Vec2[]; readonly cursor: Vec2 | null; readonly groundAt: (x: number, y: number) => number } | null;
  readonly hover: { readonly building: Building; readonly floor: number } | null;
  readonly selected: {
    readonly building: Building;
    readonly volume: number;
    readonly floor: number;
    readonly bay: { readonly storey: number; readonly side: FaceId; readonly index: number } | null;
    readonly vertex?: number | null | undefined;
    /** The picked region of that face (bays and storeys, inclusive); defaults to the one bay. */
    readonly region?: { readonly side: FaceId; readonly bay0: number; readonly bay1: number; readonly storey0: number; readonly storey1: number } | null;
  } | null;
  readonly handles: readonly Handle[];
  readonly activeHandle?: Handle | null;
  readonly label: { readonly text: string; readonly valid: boolean; readonly building: Building; readonly floor: number } | null;
  /** The live measure of the gesture in progress, beside the pointer. */
  readonly measure?: { readonly text: string; readonly x: number; readonly y: number; readonly z: number } | null;
}

// src/ui/overlay/buildingOverlay.ts:44-225 function drawBuildingOverlay
export function drawBuildingOverlay(ctx: CanvasRenderingContext2D, input: BuildingOverlayInput): void;
