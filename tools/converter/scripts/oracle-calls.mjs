// Runs the instrumented copy of the 4.0 (scripts/oracle-instrument.mjs in its Vite config) and writes the complete
// dynamic call graph of the run: every edge caller -> callee of the page and of every worker, with the features that
// had a frame on the stack, the id table, and the run's integrity (each realm's shadow stack must be back to its root
// when the page is idle; page errors).
// `node scripts/oracle-calls.mjs <url> <out dir> <playwright package dir> [seconds of simulation] [--scenario <file.mjs>]`
// The features instrumented are the copy's Vite config list (ORACLE_FEATURES=<json path> when the dev server starts;
// default converter-slice/out/definitive-features.json): [{ name, seeds: [paths under src] }].
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';

const [url, out, pwDir, simArg] = process.argv.slice(2);
const simSeconds = Number(simArg ?? 20);
const { chromium } = createRequire(path.join(pwDir, 'package.json'))('playwright');
fs.mkdirSync(out, { recursive: true });
const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist'] });
const page = await (await browser.newContext({ viewport: { width: 1600, height: 900 } })).newPage();
const errors = [];
page.on('pageerror', (e) => errors.push(`${e.message}\n${(e.stack ?? '').split('\n').slice(0, 4).join('\n')}`));
const workers = [];
page.on('worker', (w) => workers.push(w));
const t0 = Date.now();
const say = (m) => console.log(`[${((Date.now() - t0) / 1000).toFixed(0)} s] ${m}`);
await page.goto(url, { waitUntil: 'load', timeout: 600_000 });
await page.waitForFunction(() => Boolean(window.__roadcraft?.memory), null, { timeout: 900_000, polling: 1000 });
say('page ready');
await page.evaluate(async () => { await window.__roadcraft.generateCity({ size: 'small', seed: 7 }); performance.clearMarks('opening:shown'); });
await page.waitForFunction(() => performance.getEntriesByName('opening:shown').length > 0 || (window.__roadcraft?.memory?.().drawCalls ?? 0) > 100, null, { timeout: 1_800_000, polling: 1000 });
say('city ready');
const frames = (n) => page.evaluate((n) => new Promise((r) => { let k = 0; const f = () => (++k >= n ? r() : requestAnimationFrame(f)); requestAnimationFrame(f); }), n);
// views near the poles (and so the streets, signs, furniture, buildings), from two sides each
const views = await page.evaluate(() => {
  const doc = window.__roadcraft.doc;
  const map = doc?.poleMap ?? doc?.nodes;
  const all = map ? [...map.values()] : [];
  const pts = [];
  for (let i = 0; i < all.length && pts.length < 6; i += Math.max(1, Math.floor(all.length / 6))) pts.push([all[i].x, all[i].y]);
  return pts;
});
for (const [x, y] of views) for (const [zoom, az, el] of [[60, 30, 25], [220, 210, 40]]) {
  await page.evaluate(([x, y, zoom, az, el]) => window.__roadcraft.setView({ x, y, zoom, azimuth: (az * Math.PI) / 180, elevation: (el * Math.PI) / 180 }), [x, y, zoom, az, el]);
  await frames(20);
}
say(`views done (${views.length * 2})`);
// a feature's own scenario (--scenario <file.mjs>): its default export, async ({ page, frames, say }), drives the
// game through what the default run does not reach (an elevated road, a tunnel...); every call it causes is recorded
const si = process.argv.indexOf('--scenario');
if (si > 0) {
  const scenario = (await import(new URL(`file:///${path.resolve(process.argv[si + 1]).replace(/\\/g, '/')}`).href)).default;
  await scenario({ page, frames, say });
  say(`scenario done (${path.basename(process.argv[si + 1])})`);
}
// time passing: the simulation runs (traffic, people)
await page.waitForTimeout(simSeconds * 1000);
await frames(10);
say('simulation done');
await page.screenshot({ path: path.join(out, 'last-view.png') });
const dump = () => ({ edges: Array.from(globalThis.__oracle?.edges ?? []), stack: Array.from(globalThis.__oracle?.st ?? []) });
const realms = [{ realm: 'page', ...(await page.evaluate(dump)) }];
for (const [i, w] of workers.entries()) {
  try { realms.push({ realm: `worker ${i} ${w.url().replace(/^https?:\/\/[^/]+/, '')}`, ...(await w.evaluate(dump)) }); }
  catch (e) { realms.push({ realm: `worker ${i} ${w.url()}`, gone: String(e.message).split('\n')[0] }); }
}
await page.waitForTimeout(2000); // the last pushes land
const ids = await (await fetch(new URL('/__oracle/ids', url))).json();
const pushed = await (await fetch(new URL('/__oracle/edges', url))).json();
fs.writeFileSync(path.join(out, 'calls.json'), JSON.stringify({ url, seconds: (Date.now() - t0) / 1000, errors, live: realms.map((r) => ({ realm: r.realm, stack: r.stack, edges: r.edges?.length, gone: r.gone })), realms: [...pushed, ...realms.filter((r) => r.edges).map((r) => ({ realm: `live ${r.realm}`, edges: r.edges }))] }));
fs.writeFileSync(path.join(out, 'ids.json'), JSON.stringify(ids));
say(`pushed realms: ${pushed.length} (${pushed.reduce((n, r) => n + r.edges.length, 0)} edges); live realms: ${realms.length} (${realms.filter((r) => r.edges).map((r) => `${r.realm.split(' ').slice(0, 2).join(' ')}: ${r.edges.length} edges, stack ${JSON.stringify(r.stack)}`).join('; ')}); ids: ${ids.table.length}; page errors: ${errors.length}`);
await browser.close();
