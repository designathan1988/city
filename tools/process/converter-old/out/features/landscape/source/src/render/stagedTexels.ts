// The code of src/render/stagedTexels.ts that is the feature's (the rest of the file is shared).

// src/render/stagedTexels.ts:109-109 method stageTexels.hold: the feature's own
    hold(array) { held = array; letGo = false; },

// src/render/stagedTexels.ts:110-114 method stageTexels.letGoWhenSent: the feature's own
    letGoWhenSent() {
      if (!held) return;
      if (sentVersion === texture.version) held = null;
      else letGo = true;
    },
