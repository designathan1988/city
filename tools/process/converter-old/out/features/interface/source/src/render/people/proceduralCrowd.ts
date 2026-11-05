// The code of src/render/people/proceduralCrowd.ts that is the feature's (the rest of the file is shared).

// src/render/people/proceduralCrowd.ts:951-2531 slice of src/render/people/proceduralCrowd.ts#createProceduralCrowd (shared): the feature's statements; "…" marks code left out
export function createProceduralCrowd(options: { hair?: boolean; /** World units per metre (the game's are 2.5). */ unit?: number } = {}): ProceduralCrowd {
  // … 734 lines
  let strandPeople = HAIR_STRAND_PEOPLE;
// … 844 lines
}

// src/render/people/proceduralCrowd.ts:2308-2413 slice of src/render/people/proceduralCrowd.ts#createProceduralCrowd.update (shared): the feature's statements; "…" marks code left out
    update(eye, simTime, shadows = true, strands = HAIR_STRAND_PEOPLE) {
      strandPeople = strands;
    // … 103 lines
    },
