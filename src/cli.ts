// Entry point: `node src/cli.ts <command> --flag value ...`. Each stage of the conversion is one command.
import { AUDIT_FLAGS, runAuditCommand } from './audit/command.ts';
import { CLASSIFY_FLAGS, runClassifyCommand } from './classify/command.ts';
import { ArgsError, usage } from './common/args.ts';
import { CPPGRAPH_FLAGS, runCppGraph } from './cppgraph/command.ts';
import { FEATURE_FLAGS, runFeatureCommand } from './feature/command.ts';
import { GRAPH_FLAGS, runGraph } from './graph/command.ts';
import { PACK_FLAGS, runPackCommand } from './package/command.ts';
import {
  BASELINE_FLAGS, SANDBOX_FLAGS, VERIFY_FLAGS, WORKSPACE_FLAGS, runBaselineCommand, runSandboxCommand, runVerifyCommand, runWorkspaceCommand,
} from './sandbox/command.ts';
import { TRANSLATE_FLAGS, runTranslateCommand } from './translate/command.ts';
import { DIFFERENTIAL_FLAGS, runDifferentialCommand } from './translate/differential-command.ts';
import { PACKAGE_FLAGS, runPackageCommand } from './translate/package-command.ts';

const COMMANDS: Record<string, { run: (argv: readonly string[]) => void | Promise<void>; help: string }> = {
  graph: { run: runGraph, help: usage('graph', GRAPH_FLAGS) },
  cppgraph: { run: runCppGraph, help: usage('cppgraph', CPPGRAPH_FLAGS) },
  classify: { run: runClassifyCommand, help: usage('classify', CLASSIFY_FLAGS) },
  feature: { run: runFeatureCommand, help: usage('feature', FEATURE_FLAGS) },
  translate: { run: runTranslateCommand, help: usage('translate', TRANSLATE_FLAGS) },
  differential: { run: runDifferentialCommand, help: usage('differential', DIFFERENTIAL_FLAGS) },
  package: { run: runPackageCommand, help: usage('package', PACKAGE_FLAGS) },
  audit: { run: runAuditCommand, help: usage('audit', AUDIT_FLAGS) },
  workspace: { run: runWorkspaceCommand, help: usage('workspace', WORKSPACE_FLAGS) },
  verify: { run: runVerifyCommand, help: usage('verify', VERIFY_FLAGS) },
  pack: { run: runPackCommand, help: usage('pack', PACK_FLAGS) },
  sandbox: { run: runSandboxCommand, help: usage('sandbox', SANDBOX_FLAGS) },
  baseline: { run: runBaselineCommand, help: usage('baseline', BASELINE_FLAGS) },
};

async function main(argv: readonly string[]): Promise<number> {
  const [command, ...rest] = argv;
  const entry = command === undefined ? undefined : COMMANDS[command];
  if (!entry) {
    console.error(`commands: ${Object.keys(COMMANDS).join(', ')}\n\n${Object.values(COMMANDS).map((c) => c.help).join('\n\n')}`);
    return 2;
  }
  try {
    await entry.run(rest);
    // A command that finished with a failing verdict (a sandbox step failed) set its own exit code: keep it.
    return typeof process.exitCode === 'number' ? process.exitCode : 0;
  } catch (error) {
    if (error instanceof ArgsError) {
      console.error(`${error.message}\n\n${entry.help}`);
      return 2;
    }
    throw error;
  }
}

process.exitCode = await main(process.argv.slice(2));
