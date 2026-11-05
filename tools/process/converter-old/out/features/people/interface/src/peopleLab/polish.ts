// What the feature uses of src/peopleLab/polish.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/polish.ts:11-28 function clearScalpPaint
/** The rig tints the scalp in the hair's colour (and the paint runs onto the forehead): every painted body vertex gets the skin back, the scalp
 *  included - the player wants no hair colour on the scalp. The eyeballs keep theirs. */
function clearScalpPaint(mesh: SkinnedMesh, look: PersonLook): void;

// src/peopleLab/polish.ts:65-65 variable HAIR_SLOT
/** `personRig.ts` card slots in `hairMask`: 1 is the hair, 3 the lashes. */
const HAIR_SLOT = 1;

// src/peopleLab/polish.ts:122-130 function solidHair
/** The hair in one even colour: the rig samples each card's strand texture into its vertices, and the overlapping cards read as stripes. */
function solidHair(mesh: SkinnedMesh, look: PersonLook): void;

// src/peopleLab/polish.ts:192-205 function polishPerson
export function polishPerson(meshes: readonly SkinnedMesh[], look: PersonLook, sculpted = false): void;

// src/peopleLab/polish.ts:208-235 function paintedSkin
/** Painted skin, as animated films and The Sims draw it: the skin texture's pores and wrinkles softened away, its tones kept. */
export function paintedSkin(source: Texture, softness: number): Texture | null;

// src/peopleLab/polish.ts:261-272 function sharpEyes
/** The eyes from their own full-size texture: the game draws them from a 256 px layer of its shared arrays (`skinAppearance.ts` SMALL_LAYER), a blur up close. */
export function sharpEyes(material: MeshStandardMaterial, eye: Texture): void;
