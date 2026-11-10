// What the feature uses of src/editor/mapExport.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/mapExport.ts:18-34 function exportToFile
/** DO NOT go back to one `JSON.stringify` of the whole map: the file is the map's binary sections. */
export function exportToFile(
  doc: RoadDoc,
  settings?: SavedSettings,
  filename = MAP_FILE_NAME,
  onFailed?: (error: unknown) => void,
): boolean;
