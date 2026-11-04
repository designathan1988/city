// Precise call counting in every JavaScript context of a page: the page itself and each worker it starts, attached
// before their first line runs (Target.setAutoAttach with waitForDebuggerOnStart), with
// Profiler.startPreciseCoverage({ callCount: true }) and Debugger.enable for their scripts and source maps.
// Profiler.takePreciseCoverage returns the counts since the last take and resets them.
import type { CdpClient } from './cdp.ts';

export interface ScriptInfo {
  readonly url: string;
  readonly sourceMapURL: string;
}

export interface FunctionCount {
  readonly functionName: string;
  readonly startOffset: number;
  readonly count: number;
}

export interface ContextCoverage {
  readonly sessionId: string;
  readonly type: string;
  readonly url: string;
  readonly scripts: Map<string, FunctionCount[]>;
}

interface Context {
  readonly sessionId: string;
  readonly type: string;
  readonly url: string;
  readonly scripts: Map<string, ScriptInfo>;
  alive: boolean;
}

export class CoverageRecorder {
  readonly #cdp: CdpClient;
  readonly #contexts = new Map<string, Context>();
  readonly #ready: Promise<void>[] = [];

  constructor(cdp: CdpClient) {
    this.#cdp = cdp;
    cdp.on('Target.attachedToTarget', (params) => {
      const info = params['targetInfo'] as { type: string; url: string };
      const sessionId = params['sessionId'] as string;
      if (info.type !== 'page' && info.type !== 'worker' && info.type !== 'shared_worker') return;
      this.#ready.push(this.#instrument(sessionId, info.type, info.url));
    });
    cdp.on('Target.detachedFromTarget', (params) => {
      const context = this.#contexts.get(params['sessionId'] as string);
      if (context) context.alive = false;
    });
    cdp.on('Debugger.scriptParsed', (params, sessionId) => {
      const context = sessionId ? this.#contexts.get(sessionId) : undefined;
      if (!context) return;
      context.scripts.set(params['scriptId'] as string, { url: params['url'] as string, sourceMapURL: (params['sourceMapURL'] as string) ?? '' });
    });
  }

  /** Every context attached so far has its counters on. */
  async settled(): Promise<void> {
    for (let done = 0; done < this.#ready.length; done = this.#ready.length) await Promise.all(this.#ready.slice(done));
  }

  /** The counts of every live context since the last take (and resets them). */
  async take(): Promise<ContextCoverage[]> {
    await this.settled();
    const out: ContextCoverage[] = [];
    for (const context of this.#contexts.values()) {
      if (!context.alive) continue;
      const taken = await this.#cdp.send('Profiler.takePreciseCoverage', {}, context.sessionId).catch((error: Error) => {
        if (/No target|closed|detached/i.test(error.message)) return null; // a worker that finished meanwhile
        throw error;
      });
      if (!taken) continue;
      const scripts = new Map<string, FunctionCount[]>();
      for (const script of taken['result'] as { scriptId: string; functions: { functionName: string; ranges: { startOffset: number; count: number }[] }[] }[]) {
        const counted = script.functions
          .filter((f) => f.ranges[0] !== undefined && f.ranges[0].count > 0)
          .map((f) => ({ functionName: f.functionName, startOffset: f.ranges[0]!.startOffset, count: f.ranges[0]!.count }));
        if (counted.length > 0) scripts.set(script.scriptId, counted);
      }
      out.push({ sessionId: context.sessionId, type: context.type, url: context.url, scripts });
    }
    return out;
  }

  script(sessionId: string, scriptId: string): ScriptInfo | undefined {
    return this.#contexts.get(sessionId)?.scripts.get(scriptId);
  }

  async source(sessionId: string, scriptId: string): Promise<string> {
    const result = await this.#cdp.send('Debugger.getScriptSource', { scriptId }, sessionId);
    return result['scriptSource'] as string;
  }

  async #instrument(sessionId: string, type: string, url: string): Promise<void> {
    this.#contexts.set(sessionId, { sessionId, type, url, scripts: new Map(), alive: true });
    await this.#cdp.send('Profiler.enable', {}, sessionId);
    await this.#cdp.send('Profiler.startPreciseCoverage', { callCount: true, detailed: false }, sessionId);
    await this.#cdp.send('Debugger.enable', {}, sessionId);
    // Workers started by this context (and theirs) are attached paused, like this one.
    await this.#cdp.send('Target.setAutoAttach', { autoAttach: true, waitForDebuggerOnStart: true, flatten: true }, sessionId);
    await this.#cdp.send('Runtime.runIfWaitingForDebugger', {}, sessionId);
  }
}
