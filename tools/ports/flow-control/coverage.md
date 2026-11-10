# Coverage: flow-control

Translated by `tools/converter/scripts/port-area.ts` (`configs/plans/flow-control.json`), the output copied unedited; the game side is `native/src/sim/traffic/flow_control.cpp`. Proof: `project/tests/core/flow_control_test.gd` against the 4.0 through `scripts/call40.mjs`.

| Unit | State | Owner |
|---|---|---|
| `src/sim/roads/controlAdvisor.ts#AutoControl` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.EVERY` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.changedAt` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.choice` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.choices` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.drop` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.forget` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.since` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#ControlAdvisor.tick` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#F` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#QUIET_FLOW` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#rank` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#warrantedControl` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/controlAdvisor.ts#warrantedControl.k` | portado | `native/src/sim/roads/control_advisor_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.carry` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.counted` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.counts` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.drop` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.held` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.hold` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.keepOnly` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.measured` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.rate` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.rates` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.record` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.resume` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.second` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.step` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#FlowStats.takeCounted` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#KEEP` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#Rate` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#TAU` | portado | `native/src/sim/roads/flow_stats_types.h` |
| `src/sim/roads/flowStats.ts#agedTo` | portado | `native/src/sim/roads/flow_stats_types.h` |
