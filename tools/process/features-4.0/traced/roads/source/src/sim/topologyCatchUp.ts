// The code of src/sim/topologyCatchUp.ts that is the feature's (the rest of the file is shared).

// src/sim/topologyCatchUp.ts:15-22 slice of src/sim/topologyCatchUp.ts#TopologyCatchUp.stepWindow (shared): the feature's statements; "…" marks code left out
  stepWindow(sim: SimWorld, revision: number): void {
    if (sim.topologyRevision !== revision) return;
  // … 5 lines
  }

// src/sim/topologyCatchUp.ts:25-43 slice of src/sim/topologyCatchUp.ts#TopologyCatchUp.step (shared): the feature's statements; "…" marks code left out
  step(sim: SimWorld, revision: number, worldBusy: boolean): boolean {
    if (sim.topologyRevision === revision) return false;
    // … 1 lines
    if (sim.vehicleTopologyRevision !== revision) {
      if (!this.vehicles || this.vehicles.revision !== revision) {
        this.vehicles = { revision, steps: sim.prepareVehicleTopology(this.measurer) };
      }
      let prep = this.vehicles.steps.next();
      // WAITING: the workers have the pairs; nothing to do here this frame.
      while (!prep.done && prep.value !== WAITING) prep = this.vehicles.steps.next();
      if (!prep.done) return true;
      this.vehicles = null;
      sim.rebuildVehicleTopology();
      rebindVehicles(sim);
    }
  // … 3 lines
  }
