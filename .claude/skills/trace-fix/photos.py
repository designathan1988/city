"""Photos of the game as it is now, and their enlarged crops, for the trace-fix skill.

`photos <plan.json>` runs the project's look scene on a plan of views (project/tests/visual/look.gd: setView, hour, shot, ...)
through the orchestrator (tools/tasks: hidden, on a private desktop, under a deadline, stopped by PID), always anew: a photo
is of the code as it is, never one kept from before. `crops <photo or folder>` cuts each photo into overlapping tiles and
enlarges them: a vehicle, a person or a kerb is a few pixels of a whole frame and cannot be judged there. Both print the
files to look at with the Read tool.
"""
import json
import os
import subprocess
import sys
import time

import store
from store import fail

LOOK = "res://tests/visual/look.tscn"  # the scene that looks at the game by a plan (CLAUDE.md, the commands table)
CLI = os.path.join("tools", "tasks", "cli.py")  # the one orchestration owner (CLAUDE.md, tools/tasks)
RESOLUTION = "1920x1080"  # the resolution CLAUDE.md gives for looking at the game by a plan
# The crops: an image is shown to the model at about 1568 px on its long edge (Anthropic's vision guide: larger ones are
# scaled down), so a 1920x1080 frame is cut in 3x2 tiles, each widened by a tenth so a body on a cut is whole in one
# tile, and doubled: about 1400x1190, shown at nearly its own size.
COLUMNS, ROWS = 3, 2
OVERLAP = 0.1
ZOOM = 2
SERVE_SLACK_S = 60  # what the orchestrator itself may take beyond the task's own deadline before this command gives up


def crops_of(photo):
    """Cuts one photo into its enlarged tiles, beside it under crops/; returns their paths."""
    from PIL import Image  # here: only this command needs it (the asset tools' own dependency)
    folder = os.path.join(os.path.dirname(photo), "crops")
    os.makedirs(folder, exist_ok=True)
    stem = os.path.splitext(os.path.basename(photo))[0]
    out = []
    with Image.open(photo) as image:
        width, height = image.size
        tile_w, tile_h = width / COLUMNS, height / ROWS
        for row in range(ROWS):
            for column in range(COLUMNS):
                box = (max(0, int(column * tile_w - tile_w * OVERLAP)), max(0, int(row * tile_h - tile_h * OVERLAP)),
                       min(width, int((column + 1) * tile_w + tile_w * OVERLAP)), min(height, int((row + 1) * tile_h + tile_h * OVERLAP)))
                tile = image.crop(box)
                tile = tile.resize((tile.width * ZOOM, tile.height * ZOOM), Image.LANCZOS)
                path = os.path.join(folder, f"{stem}-r{row + 1}c{column + 1}.png")
                tile.save(path)
                out.append(path)
    return out


def photos_in(where):
    if os.path.isfile(where):
        return [where]
    return sorted(os.path.join(where, n) for n in os.listdir(where) if n.lower().endswith(".png"))


def show(photos):
    """Crops every photo and prints what to look at."""
    if not photos:
        fail("no photo there: a look plan takes one at each {\"shot\": name} step")
    for photo in photos:
        print(photo.replace("\\", "/"))
        for crop in crops_of(photo):
            print("  " + crop.replace("\\", "/"))
    print(f"{len(photos)} photo(s), {len(photos) * COLUMNS * ROWS} crop(s). Read every one with the Read tool (photos and crops, in "
          "parallel) and write, for each, what every rule of `rules` shows there; every defect seen is a `problem`.")


def cmd_crops(args):
    where = os.path.abspath(args.where)
    if not os.path.exists(where):
        fail(f"{args.where}: no such photo or folder")
    show(photos_in(where))


def cmd_photos(args):
    """Runs the look scene on a plan, hidden and under a deadline, and crops what it shot."""
    state = store.load()
    plan = os.path.abspath(args.plan)
    if not os.path.isfile(plan):
        fail(f"{args.plan}: no such plan (a JSON of look.gd's steps: project/tests/visual/look.gd says each one)")
    out = os.path.abspath(args.out or os.path.join(store.trace_folder(state["slug"]), "photos"))
    os.makedirs(out, exist_ok=True)
    began = time.time()
    godot = subprocess.run([sys.executable, CLI, "var", "godot"], cwd=store.ROOT, capture_output=True, text=True, timeout=60).stdout.strip()
    if not godot:
        fail("the orchestrator does not know this machine's Godot (`python tools/tasks/cli.py var godot`)")
    request = {"id": 1, "name": "look_" + state["slug"], "hidden": True, "timeout_s": args.timeout,
               "extra": [str(time.time_ns())],  # never an earlier run's answer: the photo is of the game as it is now
               "argv": [godot, "--path", "project", "--resolution", args.resolution, LOOK, "--", "--no-flow",
                        "--plan=" + plan.replace("\\", "/"), "--capture-dir=" + out.replace("\\", "/")]}
    try:
        done = subprocess.run([sys.executable, CLI, "serve"], cwd=store.ROOT, input=json.dumps(request) + "\n", capture_output=True,
                              text=True, encoding="utf-8", timeout=args.timeout + SERVE_SLACK_S)
    except subprocess.TimeoutExpired:
        fail(f"the orchestrator did not answer in {args.timeout + SERVE_SLACK_S:g} s: `python tools/tasks/cli.py status` shows the run")
    answers = [json.loads(line) for line in done.stdout.splitlines() if line.startswith("{")]
    answer = answers[-1] if answers else {"status": "BROKEN", "log": "", "error": done.stderr.strip()[-400:]}
    print(f"look: {answer['status']} in {answer.get('seconds', 0):.0f} s, log {answer.get('log', '')}")
    if answer["status"] != "PASS":
        fail(f"the look did not pass ({answer.get('message') or answer.get('error') or 'see the log'}): read the log, it is a failure "
             "of the game or of the plan, and a problem either way")
    show([p for p in photos_in(out) if os.path.getmtime(p) >= began])
