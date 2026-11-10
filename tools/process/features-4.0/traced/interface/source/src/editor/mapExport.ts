// The code of src/editor/mapExport.ts that is the feature's (the rest of the file is shared).

// src/editor/mapExport.ts:9-15 slice of src/editor/mapExport.ts#mapFileBlob (shared): the feature's statements; "…" marks code left out
export function mapFileBlob(doc: RoadDoc, settings?: SavedSettings): Blob {
  const sections = [
    ...(settings ? [{ kind: FILE_SETTINGS, bytes: jsonBytes(withUnit(settings)) }] : []),
    { kind: FILE_DOC, bytes: new Uint8Array(doc.toBinary()) },
  ];
  return new Blob(framePieces(FILE_MAGIC, sections) as Uint8Array<ArrayBuffer>[], { type: 'application/octet-stream' });
}

// src/editor/mapExport.ts:18-34 slice of src/editor/mapExport.ts#exportToFile (shared): the feature's statements; "…" marks code left out
export function exportToFile(
  // … 1 lines
  settings?: SavedSettings,
// … 2 lines
): boolean {
  // … 1 lines
  try {
    blob = mapFileBlob(doc, settings);
  } catch (error) {
  // … 4 lines
  }
  return offerFile(blob, filename);
}
