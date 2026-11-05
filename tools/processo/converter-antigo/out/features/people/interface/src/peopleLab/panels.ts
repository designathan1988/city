// What the feature uses of src/peopleLab/panels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/panels.ts:19-56 interface PanelContext
/** THE CREATOR'S PANELS, as Create-a-Sim lays them out: pictures of every choice first, the fine sliders folded away under them */

export interface PanelContext {
  readonly person: LabPerson;
  readonly showAll: boolean;
  readonly activity: string | undefined;
  readonly pose: PoseName;
  readonly clip: LibraryClipName;
  readonly ui: ReturnType<typeof widgets>;
  readonly pictures: Thumbs;
  setSpec(patch: Partial<PersonSpec>): void;
  setLook(patch: { [K in keyof PersonLook]?: PersonLook[K] | undefined }): void;
  setBody(k: keyof MacroParams, v: number): void;
  setFeature(k: string, v: number): void;
  setStyle(s: CartoonStyle): void;
  setMood(v: number): void;
  setShowAll(v: boolean): void;
  act(activity: string | undefined, pose: PoseName, clip: LibraryClipName): void;
  render(): void;
  openSculptor(item?: string): void;
  /** Takes a piece's sculpt off. */
  dropPiece(item: string): void;
  dropSculpt(): void;
  /** The worn hairstyle's length, volume and waves (`hairShape.ts`). */
  hairShape(): HairShape;
  setHairShape(s: HairShape): void;
  /** The worn outfit's length and looseness. */
  outfitShape(): HairShape;
  setOutfitShape(s: HairShape): void;
  /** The outfit's pattern, accent and finish (`clothStyle.ts`), and setting it (with the main colour, when given): drawn at once, no build. */
  outfitStyle(): OutfitStyle | undefined;
  setOutfitStyle(s: OutfitStyle, tint?: number | null): void;
  fitFace(): void;
  extraOf(kinds: readonly WardrobeItem['kind'][]): string;
  setExtra(kinds: readonly WardrobeItem['kind'][], name: string): void;
  sex(): 'female' | 'male';
  setSex(v: 'female' | 'male'): void;
  /** Brushes on the body and face (`bodySculpt.ts`). */
  openCarver(): void;
}

// src/peopleLab/panels.ts:58-58 interface Pic
interface Pic { readonly id: string; readonly label: string; readonly variant: LabPerson }

// src/peopleLab/panels.ts:61-61 type Groups
/** A category's groups, in order: each an id, a label and what it shows. */
export type Groups = readonly (readonly [string, string, () => Node[]])[];

// src/peopleLab/panels.ts:64-74 variable BODY_SLIDERS
/** Regional body sliders by group, with their names (`modifiers.json` regional). */
const BODY_SLIDERS: Readonly<Record<string, readonly (readonly [string, string])[]>> = {
  tronco: [['measure-shoulder-dist-decr-incr', 'Ombros'], ['measure-bust-circ-decr-incr', 'Peito'], ['measure-underbust-circ-decr-incr', 'Sob o busto'],
    ['measure-waist-circ-decr-incr', 'Cintura'], ['stomach-pregnant-decr-incr', 'Barriga'], ['measure-hips-circ-decr-incr', 'Quadril'],
    ['buttocks-volume-decr-incr', 'Glúteos'], ['torso-vshape-decr-incr', 'Forma em V'], ['torso-scale-vert-decr-incr', 'Altura do tronco'],
    ['measure-neck-circ-decr-incr', 'Pescoço'], ['measure-neck-height-decr-incr', 'Altura do pescoço']],
  bracos: [['measure-upperarm-length-decr-incr', 'Comprimento'], ['measure-upperarm-circ-decr-incr', 'Braço'], ['upperarm-muscle-decr-incr', 'Bíceps'],
    ['upperarm-fat-decr-incr', 'Gordura'], ['lowerarm-muscle-decr-incr', 'Antebraço'], ['hand-scale-decr-incr', 'Mãos'], ['hand-fingers-length-decr-incr', 'Dedos']],
  pernas: [['upperlegs-height-decr-incr', 'Coxas (altura)'], ['lowerlegs-height-decr-incr', 'Canelas (altura)'], ['measure-thigh-circ-decr-incr', 'Coxas'],
    ['measure-calf-circ-decr-incr', 'Panturrilhas'], ['upperleg-muscle-decr-incr', 'Músculo'], ['measure-knee-circ-decr-incr', 'Joelhos'],
    ['foot-scale-decr-incr', 'Pés'], ['measure-ankle-circ-decr-incr', 'Tornozelos']],
};

// src/peopleLab/panels.ts:77-77 variable FRAMES
/** Each face part's pictures framed on the part itself. */
const FRAMES: Readonly<Record<string, Framing>> = { eyes: 'eyes', brows: 'brows', nose: 'nose', mouth: 'mouth', cheeks: 'cheeks', jaw: 'jaw', ears: 'ears' };

// src/peopleLab/panels.ts:79-321 function makePanels
export function makePanels(c: PanelContext): Record<string, Groups>;
