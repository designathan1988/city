// What the feature uses of src/world/network.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/network.ts:46-380 class Network
export class Network { … }

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:143-145 method Network.changesSince
/** What changed since the network was in `state` (a token it handed out, `state`), or null: everything */
  changesSince(state: object | null): NetworkDelta | null;
