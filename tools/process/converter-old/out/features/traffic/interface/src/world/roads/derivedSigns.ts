// What the feature uses of src/world/roads/derivedSigns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/derivedSigns.ts:10-19 interface DerivedSign
/** THE SIGNS THE RULES PUT UP */
export interface DerivedSign {
  readonly type: 'stop' | 'yield' | 'speed';
  readonly text: string;
  readonly segment: SegmentId;
  readonly node: NodeId;
  readonly x: number;
  readonly y: number;
  /** The way the plate's face looks: at the driver it is for. */
  readonly facing: Vec2;
}

// src/world/roads/derivedSigns.ts:27-61 function signsAtNode
/** The signs the rules put up at one junction */
export function signsAtNode(net: Network, nodeId: NodeId): DerivedSign[];

// src/world/roads/derivedSigns.ts:67-83 function signApproach
/** The junction approach a stop or give-way plate put at `at` would govern ("placa = regra") */
export function signApproach(net: Network, at: Vec2, segment: SegmentId): { node: NodeId; segment: SegmentId } | null;
