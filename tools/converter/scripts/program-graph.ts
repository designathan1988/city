// The whole program's graph of the machine file's source (src/common/derived.ts: made once per state of the source and of the
// converter code that makes it), its path on stdout: what the oracle copy's Vite config instruments by. `node scripts/program-graph.ts`.
import { programGraph } from '../src/common/derived.ts';
import { machine } from '../src/common/machine.ts';

process.stdout.write(`${programGraph({ sourceRoot: machine().get('source'), typescriptDir: machine().get('typescript') })}\n`);
