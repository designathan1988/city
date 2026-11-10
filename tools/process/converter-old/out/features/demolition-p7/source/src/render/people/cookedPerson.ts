// The code of src/render/people/cookedPerson.ts that is the feature's (the rest of the file is shared).

// src/render/people/cookedPerson.ts:200-202 function uncookPerson: the feature's own
export function uncookPerson(buffer: ArrayBuffer): Group {
  return assemblePerson(decodeCookedPerson(buffer).person);
}
