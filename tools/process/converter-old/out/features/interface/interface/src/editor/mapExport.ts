// What the feature uses of src/editor/mapExport.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/mapExport.ts:9-15 function mapFileBlob
/** A map file (Save map) as the pieces of one Blob: the frame's header, the settings' JSON (with the unit its camera is in, as the autosave stamps it), the map's binary sections (`RoadDoc.toBinary`) - never one text of the map, nor one buffer of the file. */
export function mapFileBlob(doc: RoadDoc, settings?: SavedSettings): Blob;

// src/editor/mapExport.ts:18-34 function exportToFile
/** DO NOT go back to one `JSON.stringify` of the whole map: the file is the map's binary sections. */
export function exportToFile(
  doc: RoadDoc,
  settings?: SavedSettings,
  filename = MAP_FILE_NAME,
  onFailed?: (error: unknown) => void,
): boolean;

// src/editor/mapExport.ts:37-46 function offerFile
/** A file offered to the player to save (a link to it clicked): whether it was. */
export function offerFile(blob: Blob, filename: string): boolean;
