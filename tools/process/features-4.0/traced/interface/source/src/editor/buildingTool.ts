// The code of src/editor/buildingTool.ts that is the feature's (the rest of the file is shared).

// src/editor/buildingTool.ts:916-932 slice of src/editor/buildingTool.ts#BuildingTool.interiorPreview (shared): the feature's statements; "…" marks code left out
  interiorPreview(): BuildingPreview | null {
    // … 3 lines
    const dir = Math.round(Math.atan2(this.cutView.y, this.cutView.x) / (Math.PI / 4));
    // … 1 lines
    if (b !== this.cutRecord || level !== this.cutAt || dir !== this.cutDir) {
      this.cutRecord = b;
      this.cutAt = level;
      this.cutDir = dir;
      const a = dir * (Math.PI / 4);
      const cut = cutOpen(cloneBuilding(b), level, { x: Math.cos(a), y: Math.sin(a) });
      this.serial++;
      this.cutPreview = { building: cut, valid: true, problem: null, hides: b.id, serial: this.serial, solid: true };
    }
  // … 1 lines
  }
