// Runs the source project for real and counts every function call per frame: its own dev server, a headless
// Chromium on the real GPU, precise call counts in the page and every worker, each variant measured over a window
// after a warm-up, and every counted function attributed to a graph unit through the source maps.
import { normalizePath } from '../common/paths.ts';
import type { GraphIR } from '../graph/ir.ts';
import { loadSourceMap, loadTraceMapping, packageOf, projectFile, ScriptMapper, UnitIndex } from './attribute.ts';
import { CdpClient } from './cdp.ts';
import { startChromium } from './chromium.ts';
import { CoverageRecorder, type ContextCoverage } from './coverage.ts';
import { PROFILE_VERSION, type Profile, type VariantProfile } from './profile.ts';
import { startDevServer } from './viteServer.ts';

export interface MeasureOptions {
  readonly sourceRoot: string;
  readonly ir: GraphIR;
  readonly chromium: string;
  readonly chromiumArgs: readonly string[];
  readonly scenario: string;
  readonly ready: string;
  readonly frameUnit: string;
  readonly variants: readonly { readonly name: string; readonly setup: string }[];
  readonly warmupSeconds: number;
  readonly windowSeconds: number;
  readonly readyTimeoutSeconds: number;
  readonly width: number;
  readonly height: number;
  readonly allowSoftwareGl: boolean;
}

const RENDERER_PROBE = `(() => { const gl = document.createElement('canvas').getContext('webgl2') ?? document.createElement('canvas').getContext('webgl');
  if (!gl) return 'no WebGL'; const info = gl.getExtension('WEBGL_debug_renderer_info');
  return String(info ? gl.getParameter(info.UNMASKED_RENDERER_WEBGL) : gl.getParameter(gl.RENDERER)); })()`;

const sleep = (seconds: number): Promise<void> => new Promise((resolve) => setTimeout(resolve, seconds * 1000));

async function evaluate(cdp: CdpClient, sessionId: string, expression: string): Promise<unknown> {
  const result = await cdp.send('Runtime.evaluate', { expression, returnByValue: true, awaitPromise: true }, sessionId);
  const exception = result['exceptionDetails'] as { exception?: { description?: string }; text?: string } | undefined;
  if (exception) throw new Error(exception.exception?.description ?? exception.text ?? 'evaluation failed');
  return (result['result'] as { value?: unknown }).value;
}

async function waitReady(cdp: CdpClient, sessionId: string, expression: string, timeoutSeconds: number): Promise<void> {
  const deadline = Date.now() + timeoutSeconds * 1000;
  let last = 'false';
  while (Date.now() < deadline) {
    try {
      if ((await evaluate(cdp, sessionId, expression)) === true) return;
      last = 'false';
    } catch (error) {
      last = (error as Error).message;
    }
    await sleep(0.5);
  }
  throw new Error(`not ready after ${timeoutSeconds} s: \`${expression}\` was ${last}`);
}

export async function measure(options: MeasureOptions): Promise<Profile> {
  const root = normalizePath(options.sourceRoot);
  const index = new UnitIndex(options.ir);
  if (!options.ir.units.some((u) => u.id === options.frameUnit)) throw new Error(`--frame-unit ${options.frameUnit} is not a unit of the graph`);
  const trace = loadTraceMapping(root);
  const dev = await startDevServer(root);
  const chrome = await startChromium(options.chromium, options.chromiumArgs, options.width, options.height).catch(async (error: unknown) => {
    await dev.close();
    throw error;
  });
  let cdp: CdpClient | null = null;
  try {
    cdp = await CdpClient.connect(chrome.endpoint);
    const recorder = new CoverageRecorder(cdp);
    const created = await cdp.send('Target.createTarget', { url: 'about:blank', width: options.width, height: options.height });
    const attached = await cdp.send('Target.attachToTarget', { targetId: created['targetId'], flatten: true });
    const page = attached['sessionId'] as string;
    await recorder.settled();
    await cdp.send('Page.enable', {}, page);
    const renderer = String(await evaluate(cdp, page, RENDERER_PROBE));
    if (/SwiftShader|llvmpipe|Software|no WebGL/i.test(renderer) && !options.allowSoftwareGl) {
      throw new Error(`the browser renders in software (${renderer}); pass the platform's ANGLE backend with --chromium-arg ` +
        '(--use-angle=d3d11 on Windows, --use-angle=metal on macOS, --use-angle=vulkan on Linux) or --allow-software-gl yes');
    }
    await cdp.send('Page.navigate', { url: dev.origin + options.scenario }, page);
    await waitReady(cdp, page, options.ready, options.readyTimeoutSeconds);
    const variants: VariantProfile[] = [];
    for (const variant of options.variants.length > 0 ? options.variants : [{ name: 'default', setup: '' }]) {
      if (variant.setup) await evaluate(cdp, page, variant.setup);
      await sleep(options.warmupSeconds);
      await recorder.take(); // resets the counters: the window starts now
      await sleep(options.windowSeconds);
      variants.push(await summarise(variant, await recorder.take(), recorder, { root, origin: dev.origin, index, trace, frameUnit: options.frameUnit, seconds: options.windowSeconds }));
    }
    return {
      version: PROFILE_VERSION, sourceRoot: root, scenario: options.scenario, frameUnit: options.frameUnit, renderer,
      chromiumArgs: [...options.chromiumArgs], warmupSeconds: options.warmupSeconds, windowSeconds: options.windowSeconds,
      graphUnits: options.ir.units.length, variants,
    };
  } finally {
    cdp?.close();
    chrome.stop();
    await dev.close();
  }
}

interface SummaryInput {
  readonly root: string;
  readonly origin: string;
  readonly index: UnitIndex;
  readonly trace: ReturnType<typeof loadTraceMapping>;
  readonly frameUnit: string;
  readonly seconds: number;
}

async function summarise(
  variant: { name: string; setup: string },
  coverage: ContextCoverage[],
  recorder: CoverageRecorder,
  input: SummaryInput,
): Promise<VariantProfile> {
  const mappers = new Map<string, ScriptMapper>();
  const units = new Map<string, { calls: number; closureCalls: number; contexts: Set<string> }>();
  const packages = new Map<string, number>();
  const contexts: { type: string; url: string; calls: number }[] = [];
  const unmapped = { functions: 0, calls: 0 };
  for (const context of coverage) {
    let contextCalls = 0;
    for (const [scriptId, functions] of context.scripts) {
      const info = recorder.script(context.sessionId, scriptId);
      if (!info) {
        unmapped.functions += functions.length;
        unmapped.calls += functions.reduce((n, f) => n + f.count, 0);
        continue;
      }
      let mapper = mappers.get(info.url);
      if (!mapper) {
        const source = await recorder.source(context.sessionId, scriptId);
        const map = await loadSourceMap(info.sourceMapURL, info.url);
        mapper = new ScriptMapper(input.trace, source, map?.map ?? null, map?.url ?? info.url);
        mappers.set(info.url, mapper);
      }
      for (const fn of functions) {
        contextCalls += fn.count;
        const original = mapper.original(fn.startOffset);
        const sourceUrl = original?.source ?? info.url;
        const file = original ? projectFile(input.root, input.origin, original.source) : null;
        const attribution = file && original ? input.index.locate(file, original.at) : null;
        if (attribution) {
          const entry = units.get(attribution.unit) ?? { calls: 0, closureCalls: 0, contexts: new Set<string>() };
          if (attribution.own) entry.calls += fn.count;
          else entry.closureCalls += fn.count;
          entry.contexts.add(context.type);
          units.set(attribution.unit, entry);
          continue;
        }
        const pkg = packageOf(sourceUrl);
        if (pkg) packages.set(pkg, (packages.get(pkg) ?? 0) + fn.count);
        else {
          unmapped.functions++;
          unmapped.calls += fn.count;
        }
      }
    }
    contexts.push({ type: context.type, url: context.url, calls: contextCalls });
  }
  const frames = units.get(input.frameUnit)?.calls ?? 0;
  if (frames === 0) throw new Error(`variant ${variant.name}: ${input.frameUnit} never ran in the window; is the scenario animating?`);
  const perFrame = (calls: number): number => Math.round((calls / frames) * 10_000) / 10_000;
  return {
    name: variant.name, setup: variant.setup, frames, seconds: input.seconds,
    units: [...units].map(([id, e]) => ({ id, calls: e.calls, perFrame: perFrame(e.calls), closureCalls: e.closureCalls, contexts: [...e.contexts].sort() }))
      .sort((a, b) => b.calls - a.calls || a.id.localeCompare(b.id)),
    packages: [...packages].map(([name, calls]) => ({ name, calls, perFrame: perFrame(calls) })).sort((a, b) => b.calls - a.calls),
    contexts,
    unmapped,
  };
}
