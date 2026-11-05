// The code of src/peopleLab/thumbs.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/thumbs.ts:34-34 slice of src/peopleLab/thumbs.ts#Thumbs.layers (shared): the feature's statements; "…" marks code left out
  private readonly layers: PersonLayers = createPersonLayers();

// src/peopleLab/thumbs.ts:107-144 slice of src/peopleLab/thumbs.ts#Thumbs.shoot (shared): the feature's statements; "…" marks code left out
  private async shoot(person: LabPerson, framing: Framing): Promise<string> {
    // … 1 lines
    const skin = await dressPerson(built, person.spec, this.layers, this.painted);
    // … 32 lines
    void skin;
  // … 1 lines
  }
