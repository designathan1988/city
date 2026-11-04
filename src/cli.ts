// Entry point: `node src/cli.ts <command> --flag value ...`. Each stage of the conversion is one command.
import { AUDIT_FLAGS, runAuditCommand } from './audit/command.ts';
import { ArgsError, usage } from './common/args.ts';
import { GRAPH_FLAGS, runGraph } from './graph/command.ts';

const COMMANDS: Record<string, { run: (argv: readonly string[]) => void; help: string }> = {
  graph: { run: runGraph, help: usage('graph', GRAPH_FLAGS) },
  audit: { run: runAuditCommand, help: usage('audit', AUDIT_FLAGS) },
};

function main(argv: readonly string[]): number {
  const [command, ...rest] = argv;
  const entry = command === undefined ? undefined : COMMANDS[command];
  if (!entry) {
    console.error(`commands: ${Object.keys(COMMANDS).join(', ')}\n\n${Object.values(COMMANDS).map((c) => c.help).join('\n\n')}`);
    return 2;
  }
  try {
    entry.run(rest);
    return 0;
  } catch (error) {
    if (error instanceof ArgsError) {
      console.error(`${error.message}\n\n${entry.help}`);
      return 2;
    }
    throw error;
  }
}

process.exitCode = main(process.argv.slice(2));
