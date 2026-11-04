// `converter sandbox`: tests a conversion package in an isolated clone of the game, before and after, and prints the
// manual integration commands. `converter baseline`: the game's own setup and verification on its base alone, which
// proves a sandbox configuration. `converter workspace` and `converter verify`: the clone a port is written and
// verified in (workspace.ts). None of them writes to the game.
import { parseFlags, type FlagSpec } from '../common/args.ts';
import { readSandboxConfig } from './config.ts';
import { integrationCommands, runBaseline, runSandbox, type StepResult } from './run.ts';
import { createWorkspace, verifyWorkspace } from './workspace.ts';

export const SANDBOX_FLAGS: readonly FlagSpec[] = [
  { name: 'package', required: true, help: 'conversion package folder (manifest.json, patches/)' },
  { name: 'config', required: true, help: 'sandbox configuration JSON: the game\'s setup and verification commands' },
  { name: 'dir', required: true, help: 'folder for the sandbox clone (must not exist; outside the game)' },
];

export const BASELINE_FLAGS: readonly FlagSpec[] = [
  { name: 'game', required: true, help: 'root of the game repository (read only)' },
  { name: 'base', help: 'game commit to verify (default: its HEAD)' },
  { name: 'config', required: true, help: 'sandbox configuration JSON' },
  { name: 'dir', required: true, help: 'folder for the sandbox clone (must not exist; outside the game)' },
  { name: 'evidence', required: true, help: 'folder for the logs and photos' },
];

export const WORKSPACE_FLAGS: readonly FlagSpec[] = [
  { name: 'game', required: true, help: 'root of the game repository (read only)' },
  { name: 'base', help: 'game commit to start from (default: its HEAD)' },
  { name: 'feature', required: true, help: 'feature name: lower case, digits, dashes (the branch is convert/<feature>)' },
  { name: 'port', required: true, help: 'port folder (outside the game): the clone goes to <port>/workspace, the evidence beside it' },
  { name: 'config', required: true, help: 'sandbox configuration JSON: its setup runs once in the workspace' },
];

export const VERIFY_FLAGS: readonly FlagSpec[] = [
  { name: 'port', required: true, help: 'port folder made by `converter workspace`' },
  { name: 'config', required: true, help: 'sandbox configuration JSON' },
  { name: 'step', repeatable: true, help: 'a verify step of the configuration to run (default: all, in order)' },
];

function print(steps: readonly StepResult[]): void {
  for (const step of steps) {
    console.log(`${step.ok ? 'ok  ' : 'FAIL'} ${step.phase}/${step.name} (${step.seconds} s)${step.problem ? `: ${step.problem} — see ${step.log}` : ''}`);
  }
}

export function runSandboxCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, SANDBOX_FLAGS);
  const result = runSandbox({ packageDir: flags.require('package'), config: readSandboxConfig(flags.require('config')), sandboxDir: flags.require('dir') });
  print(result.steps);
  if (result.gameChanged) console.log('\nnote: the game repository changed meanwhile (another process); the evidence is of its base as cloned');
  console.log(result.ok ? `\nsandbox OK: ${result.sandbox} (branch ${result.branch})` : '\nsandbox FAILED');
  if (result.ok) {
    console.log('\nManual integration, when you decide (nothing below is run by the converter):');
    for (const line of integrationCommands(result)) console.log(`  ${line}`);
  }
  process.exitCode = result.ok ? 0 : 1;
}

export function runWorkspaceCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, WORKSPACE_FLAGS);
  const result = createWorkspace({
    gameRoot: flags.require('game'), base: flags.one('base'), portDir: flags.require('port'), feature: flags.require('feature'),
    config: readSandboxConfig(flags.require('config')),
  });
  print(result.steps);
  if (result.gameChanged) console.log('\nnote: the game repository changed meanwhile (another process); the workspace is of its base as cloned');
  const { record } = result;
  console.log(result.ok ? `\nworkspace ${record.workspace} on ${record.branch}, base ${record.base.slice(0, 12)}` : '\nworkspace setup FAILED');
  process.exitCode = result.ok ? 0 : 1;
}

export function runVerifyCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, VERIFY_FLAGS);
  const result = verifyWorkspace(flags.require('port'), readSandboxConfig(flags.require('config')), flags.all('step'));
  print(result.steps);
  console.log(result.ok ? `\nverify OK -> ${result.evidence}` : `\nverify FAILED -> ${result.evidence}`);
  process.exitCode = result.ok ? 0 : 1;
}

export function runBaselineCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, BASELINE_FLAGS);
  const result = runBaseline({
    gameRoot: flags.require('game'), base: flags.one('base'), config: readSandboxConfig(flags.require('config')),
    sandboxDir: flags.require('dir'), evidenceDir: flags.require('evidence'),
  });
  print(result.steps);
  if (result.gameChanged) console.log('\nnote: the game repository changed meanwhile (another process); the evidence is of its base as cloned');
  console.log(result.ok ? `\nbaseline OK at ${result.base}` : `\nbaseline FAILED at ${result.base}`);
  process.exitCode = result.ok ? 0 : 1;
}
