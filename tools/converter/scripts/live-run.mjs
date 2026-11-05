// A queue of items run by one process, watched live: no fixed time limit, a heartbeat instead.
// The process tells what it does with marker lines (on its stdout, or in a log file it writes): an item begins, an
// item ends; errors between the two are that item's. The watcher prints one event per line as it happens
// (begin/ok/fail/stall/exit, JSON) to stdout and to a status file, so a monitor sees each item the moment it settles.
// Items may run at once (a pool of workers): each in flight is watched on its own; an error line names its item
// (`ITEM FAIL <id> <message>`), or belongs to the only item in flight.
// A stall is judged by the items already done, not by a guess: when an item in flight has run longer than
// max(--min-idle, --factor × the slowest item so far), it is declared hung, the process (its own PID, from its
// marker; never anything else) is killed, and the queue resumes after it — the items that remain do not wait.
// Resuming: the watcher writes every settled item to --skip-file; the process skips the items listed there.
//
// node scripts/live-run.mjs --name <n> --status <file.jsonl> [--log <file>] --begin <re> --end <re> [--fail <re>]
//   [--pid <re>] [--skip-file <file>] [--min-idle <s>] [--factor <k>] [--restarts <n>] -- <command> [args...]
import fs from 'node:fs';
import { spawn, spawnSync } from 'node:child_process';

const argv = process.argv.slice(2);
const sep = argv.indexOf('--');
const opts = argv.slice(0, sep < 0 ? argv.length : sep);
const command = sep < 0 ? [] : argv.slice(sep + 1);
const opt = (n, d) => { const i = opts.indexOf(n); return i >= 0 ? opts[i + 1] : d; };
const name = opt('--name', 'run');
const statusFile = opt('--status', `${name}.jsonl`);
const logFile = opt('--log', null);
const begin = new RegExp(opt('--begin', '^ITEM BEGIN (\\S+)'));
const end = new RegExp(opt('--end', '^ITEM END (\\S+)'));
const failRe = new RegExp(opt('--fail', 'ERROR|Error:'));
const pidRe = new RegExp(opt('--pid', '^LIVE PID (\\d+)'));
const skipFile = opt('--skip-file', null);
const minIdle = Number(opt('--min-idle', '20')) * 1000;
const factor = Number(opt('--factor', '8'));
const maxRestarts = Number(opt('--restarts', '20'));
if (command.length === 0) { console.error('give the command after --'); process.exit(2); }

fs.writeFileSync(statusFile, '');
// the skip file may hold items the command already knows are settled (a cache): kept, appended to
if (skipFile && !fs.existsSync(skipFile)) fs.writeFileSync(skipFile, '');
const t0 = Date.now();
const emit = (e) => { const line = JSON.stringify({ run: name, s: +((Date.now() - t0) / 1000).toFixed(1), ...e }); fs.appendFileSync(statusFile, line + '\n'); console.log(line); };
const settled = new Map(); // id -> ok | fail | hung
const inflight = new Map(); // id -> { since, errors: [] }
const failItem = /^ITEM FAIL (\S+) (.*)$/;
let slowest = 0;
let pid = null;
let child = null;
let restarts = 0;
let ended = false;

const settle = (id, state, extra = {}) => {
  if (settled.has(id)) return;
  settled.set(id, state);
  if (skipFile) fs.appendFileSync(skipFile, id + '\n');
  emit({ event: state, id, ...extra });
};
let lastLine = Date.now();
const looseErrors = [];
const onLine = (line) => {
  lastLine = Date.now();
  if (inflight.size === 0 && failRe.test(line)) looseErrors.push(line.trim().slice(0, 200));
  const p = pidRe.exec(line); if (p) { pid = Number(p[1]); return; }
  const b = begin.exec(line);
  if (b) { inflight.set(b[1], { since: Date.now(), errors: [] }); return; }
  const fi = failItem.exec(line);
  if (fi) { inflight.get(fi[1])?.errors.push(fi[2].trim().slice(0, 200)); return; }
  const e = end.exec(line);
  const cur = e ? inflight.get(e[1]) : null;
  if (e && cur) {
    const ms = Date.now() - cur.since;
    slowest = Math.max(slowest, ms);
    settle(e[1], cur.errors.length ? 'fail' : 'ok', { ms, ...(cur.errors.length ? { errors: [...new Set(cur.errors)].slice(0, 3) } : {}) });
    inflight.delete(e[1]);
    return;
  }
  if (inflight.size === 1 && failRe.test(line)) [...inflight.values()][0].errors.push(line.trim().slice(0, 200));
};

// lines from the child's stdout, or from the log file it writes (read as it grows)
let logOffset = 0;
let partial = '';
const readLog = () => {
  if (!logFile || !fs.existsSync(logFile)) return;
  const size = fs.statSync(logFile).size;
  if (size < logOffset) logOffset = 0;
  if (size === logOffset) return;
  const fd = fs.openSync(logFile, 'r');
  const buf = Buffer.alloc(size - logOffset);
  fs.readSync(fd, buf, 0, buf.length, logOffset);
  fs.closeSync(fd);
  logOffset = size;
  const text = partial + buf.toString('utf8');
  const lines = text.split(/\r?\n/);
  partial = lines.pop() ?? '';
  for (const l of lines) onLine(l);
};

const start = () => {
  pid = null;
  lastLine = Date.now();
  inflight.clear();
  // the log is kept across restarts (the command appends): read from where it stands now
  logOffset = logFile && fs.existsSync(logFile) ? fs.statSync(logFile).size : 0; partial = '';
  child = spawn(command[0], command.slice(1), { stdio: ['ignore', 'pipe', 'pipe'], windowsHide: true });
  emit({ event: 'start', pid: child.pid, restarts });
  let out = '';
  const feed = (d) => { if (logFile) return; out += d.toString('utf8'); const lines = out.split(/\r?\n/); out = lines.pop() ?? ''; for (const l of lines) onLine(l); };
  child.stdout.on('data', feed);
  child.stderr.on('data', feed);
  child.on('exit', (code) => {
    readLog();
    for (const [id] of inflight) settle(id, 'fail', { errors: [`the process exited (${code}) during this item`] });
    inflight.clear();
    child = null;
    if (!ended && restartPending) { restartPending = false; restarts++; if (restarts <= maxRestarts) { start(); return; } }
    finish(code);
  });
};
let restartPending = false;
const kill = () => {
  const target = pid ?? child?.pid;
  if (target) spawnSync('taskkill', ['/PID', String(target), '/T', '/F'], { stdio: 'ignore', windowsHide: true });
  if (child?.pid && target !== child.pid) spawnSync('taskkill', ['/PID', String(child.pid), '/T', '/F'], { stdio: 'ignore', windowsHide: true });
};
const finish = (code) => {
  if (ended) return;
  ended = true;
  clearInterval(timer);
  const count = (s) => [...settled.values()].filter((x) => x === s).length;
  emit({ event: 'exit', code, ok: count('ok'), fail: count('fail'), hung: count('hung'), hungIds: [...settled].filter(([, s]) => s === 'hung').map(([id]) => id), slowestMs: slowest });
};
const timer = setInterval(() => {
  readLog();
  if (!child) return;
  const limit = Math.max(minIdle, factor * slowest);
  // no item in flight and nothing said for as long as an item may take: stalled outside any item (it never began
  // one, or stopped between two) — reported with what it printed, and stopped (no item to skip on a restart)
  if (inflight.size === 0) {
    if (Date.now() - lastLine > limit) { emit({ event: 'stalled', ms: Date.now() - lastLine, limitMs: limit, errors: [...new Set(looseErrors)].slice(-5) }); kill(); }
    return;
  }
  const [hungId, oldest] = [...inflight].sort((a, b) => a[1].since - b[1].since)[0];
  const ran = Date.now() - oldest.since;
  if (ran > limit) {
    settle(hungId, 'hung', { ms: ran, limitMs: limit });
    // the others in flight are not settled: the resumed process takes them again
    inflight.clear();
    restartPending = true;
    kill();
  }
}, 500);
start();
