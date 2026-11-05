// The code of src/sim/world.ts that is the feature's (the rest of the file is shared).

// src/sim/world.ts:231-231 field SimWorld.nextPersonId: the feature's own
  nextPersonId = 1;

// src/sim/world.ts:461-475 method SimWorld.someVehicleNear: the feature's own
  someVehicleNear(x: number, y: number, reach: number, test: (v: Vehicle) => boolean): boolean {
    const lanes = this.nearLanes;
    lanesNearInto(this, x, y, reach + BODY_OFF_LANE, lanes);
    try {
      for (const lane of lanes) {
        for (const id of this.rt(lane.id).order) {
          const v = this.vehicles.get(id);
          if (v && test(v)) return true;
        }
      }
      return false;
    } finally {
      lanes.length = 0;
    }
  }

// src/sim/world.ts:595-599 method SimWorld.signalAtCrossing: the feature's own
  signalAtCrossing(x: CrossingId): void {
    const node = crossingNode(x);
    const c = this.asleep.get(node);
    if (c && c.plan.stages.some((stage) => stage.pedWalk.includes(x))) this.wakeSignal(node);
  }

// src/sim/world.ts:1311-1311 function crossingNode: the feature's own
const crossingNode = (id: CrossingId): NodeId => Number(id.slice(0, id.indexOf(':'))) as NodeId;
