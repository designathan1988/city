# The sampler's report (sampler.py): per thread, how much it runs and how much it waits, the functions that cost most (self and
# inclusive), where it waits, the hot and waiting chains, and the code that triggered engine or driver time (the first frame with
# symbols); then the main thread's timeline in windows (the start-up is the first). System = the modules of the Windows folder,
# with no list of names.
import os
import re
from collections import Counter, defaultdict

ACTIVE = 0.25  # share of the wall time between two samples the thread spent on a CPU: below it, it was waiting
TOP = 8
DEPTH = 6
SYSTEM = {m.lower() for m in os.listdir(os.path.join(os.environ.get("WINDIR", "C:\\Windows"), "System32")) if m.lower().endswith(".dll")}


def function_of(name):
    """'mod!f(args) (file.cpp:12:3)' -> 'mod!f (file.cpp)': the function and its file, without arguments nor line."""
    m = re.match(r"^(.*?!)?(.*?)(\(.*\))?\s*\(([^()]*?):\d+(?::\d+)?\)$", name)
    if not m:
        return name
    return f"{m.group(1) or ''}{re.sub(r'\(.*\)$', '', m.group(2)).strip()} ({m.group(4)})"


def module_of(name):
    return name.split("!", 1)[0].lower()


def report(samples, names, main, window_s, interval_ms):
    per_thread = defaultdict(lambda: {"n": 0, "waiting": 0, "cpu": 0, "wall": 0, "self": Counter(), "inclusive": Counter(), "waits": Counter(), "chains": Counter(),
                                      "code": Counter(), "code_waiting": Counter(), "waiting_chains": Counter()})
    timeline = defaultdict(lambda: {"n": 0, "waiting": 0, "hot": Counter(), "others": Counter()})
    for t, tid, stack, cpu, wall in samples:
        frames = [function_of(names.get(a, hex(a))) for a in stack]  # by function: the lines of one function add up
        app = [f for f in frames if module_of(f) not in SYSTEM]
        waiting = cpu < ACTIVE * wall  # by the thread's own CPU time, not by the name on top
        r = per_thread[tid]
        r["n"] += 1
        r["cpu"] += cpu
        r["wall"] += wall
        top = app[0] if app else (frames[0] if frames else "?")
        owner = next((f for f in app if f.endswith(")")), "(no symbols on the stack)")  # the code that triggered the time above it
        chain = " <- ".join(app[:DEPTH])
        if waiting:
            r["waiting"] += 1
            r["waits"][top] += 1
            r["code_waiting"][owner] += 1
            r["waiting_chains"][chain] += 1
        else:
            r["self"][top] += 1
            r["chains"][chain] += 1
            r["code"][owner] += 1
        for f in set(app):
            r["inclusive"][f] += 1
        if tid == main:
            w = timeline[int(t // window_s)]
            w["n"] += 1
            w["waiting"] += waiting
            w["hot"][("waiting: " if waiting else "") + top] += 1
        elif not waiting:  # who works while the main thread is where it is: what a long wait there is waiting for
            timeline[int(t // window_s)]["others"][top] += 1
    data = {"main": main, "threads": {}, "timeline": []}
    print(f"Samples: {len(samples)} ({interval_ms:g} ms between sweeps); main thread: {main}")
    order = sorted(per_thread.items(), key=lambda kv: (kv[0] != main, -kv[1]["cpu"]))  # by the CPU they really took
    for tid, r in order[:TOP]:
        print(f"\n== thread {tid}{' (main)' if tid == main else ''}: {r['n']} samples, running {100 * (r['n'] - r['waiting']) / max(1, r['n']):.0f}%,"
              f" waiting {100 * r['waiting'] / max(1, r['n']):.0f}%; uses {100 * r['cpu'] / max(1, r['wall']):.0f}% of a core")
        for title, counts in (("self (where the CPU is)", r["self"]), ("inclusive (who pays)", r["inclusive"]),
                              ("waits (who waits)", r["waits"]), ("hot chains", r["chains"]),
                              ("code behind the running time (first frame with symbols)", r["code"]),
                              ("code behind the waiting (first frame with symbols)", r["code_waiting"]), ("waiting chains", r["waiting_chains"])):
            if counts:
                print(f"  {title}:")
                for f, n in counts.most_common(TOP):
                    print(f"    {100 * n / max(1, r['n']):5.1f}%  {f}")
        data["threads"][str(tid)] = {k: (v.most_common(40) if isinstance(v, Counter) else v) for k, v in r.items()}
    if timeline:
        print(f"\n== main thread timeline (windows of {window_s:g} s; the start-up is the first)")
        for k in sorted(timeline):
            v = timeline[k]
            hot = ", ".join(f"{f} {100 * n / max(1, v['n']):.0f}%" for f, n in v["hot"].most_common(3))
            busy = ", ".join(f"{f} x{n}" for f, n in v["others"].most_common(3))
            print(f"  {k * window_s:6.1f} s: waiting {100 * v['waiting'] / max(1, v['n']):3.0f}% | {hot}" + (f" || others: {busy}" if busy else ""))
            data["timeline"].append({"t": k * window_s, "waiting": v["waiting"] / max(1, v["n"]), "hot": v["hot"].most_common(5),
                                     "others": v["others"].most_common(5)})
    return data
