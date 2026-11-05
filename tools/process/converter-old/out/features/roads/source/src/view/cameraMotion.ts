// The code of src/view/cameraMotion.ts that is the feature's (the rest of the file is shared).

// src/view/cameraMotion.ts:96-98 method CameraMotion.owns: the feature's own
  static owns(code: string): boolean {
    return code in KEYS;
  }
