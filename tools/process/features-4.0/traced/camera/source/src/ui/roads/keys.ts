// The code of src/ui/roads/keys.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/keys.ts:25-27 slice of src/ui/roads/keys.ts#cameraKey (shared): the feature's statements; "…" marks code left out
function cameraKey(key: string): boolean {
  return !lentByTheCamera(key) && codesOfKey(key).some((code) => CameraMotion.owns(code));
}
