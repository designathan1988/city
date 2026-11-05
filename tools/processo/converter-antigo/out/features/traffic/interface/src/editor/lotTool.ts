// What the feature uses of src/editor/lotTool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/lotTool.ts:27-27 type ZoneMode
/** THE ZONING TOOL: the lots the player draws, edits and zones (`world/lots.ts`), and the gestures doing it */

/** Brush paints the cells under the pointer; the other modes edit the lots. */
export type ZoneMode = 'brush' | 'fill' | 'edit' | 'front' | 'split' | 'join' | 'add' | 'polygon' | 'curve' | 'delete';

// src/editor/lotTool.ts:29-29 type LotSplitKind
/** How the split tool cuts (`LotCut`): across the front, parallel to it, or along a drawn line. */
export type LotSplitKind = 'vertical' | 'horizontal' | 'line';
