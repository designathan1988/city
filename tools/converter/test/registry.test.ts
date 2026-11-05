// Tests of the registration of a new subsystem: each insertion lands in the file's own format, next to its siblings,
// read back the way the target reads it, and leaves everything else as it was.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import { addLayer, addMapLine, addStateMember } from '../src/translate/registry.ts';

const RULES = `{
  "cpp_layers": {
    "native/src/core": [
      "core/"
    ],
    "native/src/sim/lots": [
      "core/",
      "sim/lots/"
    ],
    "native/src/render": [
      "render/"
    ]
  },
  "other": [1, 2]
}
`;

const DOC = `# Doc
<!-- MAPA:INICIO -->
**Sim**
- \`native/src/sim/lots/lot_cache.h\` — lots
- \`native/src/sim/traffic/astar.h\` — paths
**Render**
- \`native/src/render/x.h\` — x
<!-- MAPA:FIM -->
`;

const STATE = `#pragma once

#include "core/job_pool.h"
#include "sim/state/pose_buffer.h"
#include "sim/traffic/traffic_state.h"

struct CityState {
	TrafficState traffic; // vehicles
	double money = 0;
};
`;

describe('registry', () => {
  it('adds a layer after its siblings, indented like them, and nothing else changes', () => {
    const out = addLayer(RULES, 'native/src/sim/transit', ['core/', 'sim/transit/']);
    assert.match(out, /"native\/src\/sim\/lots": \[\n {6}"core\/",\n {6}"sim\/lots\/"\n {4}\],\n {4}"native\/src\/sim\/transit": \[\n {6}"core\/",\n {6}"sim\/transit\/"\n {4}\],\n {4}"native\/src\/render"/);
    assert.equal(out.replace(/,\n {4}"native\/src\/sim\/transit": \[[^\]]*\]/, ''), RULES);
    assert.throws(() => addLayer(out, 'native/src/sim/transit', ['core/']), /already has a layer/);
  });

  it('maps the folder after the sibling that sorts before it, as the gate reads the map', () => {
    const line = '- `native/src/sim/transit/**` — transit';
    const out = addMapLine(DOC, ['<!-- MAPA:INICIO -->', '<!-- MAPA:FIM -->'], 'native/src/sim/transit', line);
    assert.match(out, /astar\.h` — paths\n- `native\/src\/sim\/transit\/\*\*` — transit\n\*\*Render\*\*/);
    assert.throws(() => addMapLine(DOC, ['<!-- MAPA:INICIO -->', '<!-- MAPA:FIM -->'], 'native/src/sim/transit', '- `x` — no'), /must list/);
  });

  it('includes the state header in order and puts the member after its anchor', () => {
    const out = addStateMember(STATE, 'sim/transit/transit_types.h', 'TrafficState traffic;', 'TransitState transit; // transit');
    assert.match(out, /#include "sim\/traffic\/traffic_state\.h"\n#include "sim\/transit\/transit_types\.h"\n/);
    assert.match(out, /\tTrafficState traffic; \/\/ vehicles\n\tTransitState transit; \/\/ transit\n\tdouble money/);
    assert.throws(() => addStateMember(STATE, 'x.h', 'missing anchor', 'X x;'), /not in the state manager/);
  });
});
