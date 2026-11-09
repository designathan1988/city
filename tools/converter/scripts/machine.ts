// The machine file resolved, as JSON on stdout, for tools that are not TypeScript (scripts/machine_config.py): one reader of
// configs/roadcraft-machine.json (src/common/machine.ts) for every language. `node scripts/machine.ts`.
import { machine } from '../src/common/machine.ts';

process.stdout.write(`${JSON.stringify(machine().all())}\n`);
