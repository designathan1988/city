// What the feature uses of src/sim/host/simHost.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simHost.ts:404-409 method SimHost.lotPlan
/** THE ZONING TOOL'S PROPOSAL (`editor/lotTool.ts`): the land along the streets cut into lots on this copy of the region, which the page's diary keeps as the page has it (sent before the question, `SimClient.lotPlan`) - planned again only where the roads and the lots changed (`world/lots.ts` LotPlanner, one for this network), DO NOT plan it on the page. Null while a tile's network is being read (its junctions would be solved here): asked again. */
  private lotPlan(): LotPlan | null;
