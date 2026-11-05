// The code of src/render/riggedCitizens.ts that is the feature's (the rest of the file is shared).

// src/render/riggedCitizens.ts:905-919 slice of src/render/riggedCitizens.ts#createRiggedCitizens.emit (shared): the feature's statements; "…" marks code left out
  function emit(batch: CitizenBatch, clips: readonly ClipFrames[], phases: readonly number[],
    // … 1 lines
    lean = 0, ground: Gradient | null = null, expression?: FaceWeights, lost: readonly Severable[] | null = null, level = lod): void {
    // … 6 lines
    if (ground) shearMatrix(transform.matrix, ground, x, -y);
  // … 4 lines
  }

// src/render/riggedCitizens.ts:967-1058 slice of src/render/riggedCitizens.ts#createRiggedCitizens.draw (shared): the feature's statements; "…" marks code left out
    draw(ped: PedView, x: number, y: number, heading: number, deck: number, alpha: number,
      ground: Gradient | null = null, lean = 0, priority = false) {
      // … 40 lines
      emit(batch, mixClips, mixPhases, mixWeights, x, deck, y, gaitHeading(gait), m(scale), lean, ground,
        level === 0 ? faceAt(ped.id, time, ped.panic ? (ped.gesture?.kind === 'crouch' ? 'cry' : 'panic') : ped.gesture?.kind, CROWD[index]?.person?.mood) : undefined,
        lost, level);
    // … 46 lines
    },
