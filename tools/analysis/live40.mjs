// The running 4.0 kept open: a question to the reference costs milliseconds instead of a browser, a game and a city each time.
// node tools/analysis/live40.mjs <4.0 root> [--url=http://127.0.0.1:5300/] [--port=5301] [--view=1600x900]
// Then, on http://127.0.0.1:<port>/: POST /eval (a JS expression in the body; its value comes back as JSON), GET /shot?path=<png>,
// /click?x=&y=, /move?x=&y=, /key?k=PageUp, /console (and clears it), /reload, /quit. The dev server of the 4.0 must be running.
import { createServer } from 'node:http';
import { createRequire } from 'node:module';
import { join, resolve } from 'node:path';

const args = process.argv.slice(2);
const flag = (name, fallback) => (args.find((a) => a.startsWith(`--${name}=`)) ?? `--${name}=${fallback}`).split('=').slice(1).join('=');
const root = resolve(args.find((a) => !a.startsWith('--')) ?? '.');
const url = flag('url', 'http://127.0.0.1:5300/');
const port = Number(flag('port', 5301));
const [width, height] = flag('view', '1600x900').split('x').map(Number);

const { chromium } = createRequire(join(root, 'package.json'))('@playwright/test');
const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist'] });
const page = await browser.newPage({ viewport: { width, height } });
const lines = [];
page.on('console', (m) => { if (m.type() === 'error' || m.type() === 'warning') lines.push(`${m.type()}: ${m.text()}`); });
page.on('pageerror', (e) => lines.push(`pageerror: ${e.message}\n${e.stack ?? ''}`));
await page.goto(url, { waitUntil: 'load' });

const body = (req) => new Promise((done) => { let text = ''; req.on('data', (c) => { text += c; }); req.on('end', () => done(text)); });
const routes = {
  '/eval': async (req) => page.evaluate(await body(req)),
  '/shot': async (_, q) => { await page.screenshot({ path: q.get('path') }); return q.get('path'); },
  '/click': async (_, q) => { await page.mouse.click(Number(q.get('x')), Number(q.get('y'))); return 'ok'; },
  '/move': async (_, q) => { await page.mouse.move(Number(q.get('x')), Number(q.get('y'))); return 'ok'; },
  '/key': async (_, q) => { await page.keyboard.press(q.get('k')); return 'ok'; },
  '/console': async () => lines.splice(0),
  '/reload': async () => { await page.reload({ waitUntil: 'load' }); return 'ok'; },
};

createServer(async (req, res) => {
  const at = new URL(req.url, 'http://x');
  if (at.pathname === '/quit') { res.end('bye'); await browser.close(); process.exit(0); }
  try {
    const route = routes[at.pathname];
    res.end(JSON.stringify(route ? await route(req, at.searchParams) : `no route ${at.pathname}`));
  } catch (err) {
    res.statusCode = 500;
    res.end(JSON.stringify(String(err.message)));
  }
}).listen(port, '127.0.0.1', () => console.log(`live 4.0 on http://127.0.0.1:${port}/`));
