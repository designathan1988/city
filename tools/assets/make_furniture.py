"""Writes project/data/render/furniture.json: what each piece of furniture is made of, part by part, for the cut-open
view of a building (native/src/render/buildings/interior_emit.cpp). Each model is built at the size of its row in
data/catalog/interiors.json "sizes" (across W, deep D, high H) and scaled to the piece placed. A part is
  ["box", [across, up, deep], [centre x, bottom y, centre z], colour, {options}]
  ["cyl", [radius, height, radius at top], [centre x, bottom y, centre z], colour, {options}]
  ["ball", [radius], [centre x, centre y, centre z], colour]
x across (right of the piece), y up, z towards the front (the way it faces). Options: n copies each moved by step and
grown by grow, spin copies turned about the up axis, colours cycled one per copy.
The shapes are the modelling of a piece as people use it (a bed: frame, legs, mattress, duvet, headboard, pillows), drawn
against the photos of the reference; no norm fixes them. Run: python3 tools/assets/make_furniture.py; --check writes nothing and fails
when the committed table is not what the sizes make (the quick suite's furniture_table task).
"""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "..", "..", "project", "data", "render")
CATALOG = os.path.join(HERE, "..", "..", "project", "data", "catalog")
GAME = os.path.join(HERE, "..", "..", "project", "data", "game")
PALETTE = {
	"oak": ["wood", "#9a6a43"], "walnut": ["wood", "#5e3d26"], "linen": ["plaster", "#f2efe8"], "duvet": ["plaster", "#7d93ad"],
	"fabric": ["plaster", "#6f7f8f"], "cushion": ["plaster", "#84929f"], "black": ["plaster", "#1e2125"], "steel": ["metal", "#b3b9bd"],
	"chrome": ["metal", "#d9dde0"], "white": ["plaster", "#f6f6f3"], "ceramic": ["ceramic", "#eef0ef"], "leaf": ["plaster", "#4c8a3a"],
	"leaf_dark": ["plaster", "#386d2b"], "pot": ["ceramic", "#b4673f"], "red": ["plaster", "#a22b2b"], "screen": ["plaster", "#0f1418"],
	"board": ["plaster", "#2c4a3a"], "altar": ["stone", "#e9e2cf"], "gold": ["metal", "#d7b65a"], "shade": ["plaster", "#fff0d0"],
	"bulb": ["plaster", "#fffbe8"], "pewter": ["wood", "#d8d4cc"], "bar_steel": ["metal", "#40444a"], "burner": ["metal", "#3b3d40"],
	"desk_pad": ["plaster", "#3a3d40"], "porcelain": ["ceramic", "#c7d8de"], "cabinet": ["wood", "#7a5a3a"], "tread": ["wood", "#a67650"],
	"tread_dark": ["wood", "#9a6a43"], "well": ["plaster", "#2a2622"], "locker": ["metal", "#5f7d93"], "console": ["metal", "#3a3a46"],
	"amp": ["plaster", "#2a2a2a"], "belt": ["plaster", "#2e3136"], "machine": ["metal", "#6f8a9a"], "machine_dark": ["metal", "#587080"],
	"atm": ["metal", "#2c5a8a"], "rack_post": ["metal", "#2f63b0"], "rack_beam": ["metal", "#e0782a"], "crate": ["wood", "#c8a271"],
	"pallet": ["wood", "#b08a5a"], "pallet_load": ["wood", "#c9a777"], "stove_front": ["plaster", "#f6f6f3"], "fridge_seam": ["metal", "#cfd3d5"],
	"counter_top": ["stone", "#e6e1d8"], "sink_top": ["stone", "#d8d3c9"],
	"floor_wood": ["wood", "#a27548"], "floor_stone": ["stone", "#b9b2a5"], "floor_porcelain": ["stone", "#d9d3c7"],
	"partition": ["plaster", "#ece6da"], "inner_wall": ["plaster", "#eee8dc"], "shaft": ["concrete", "#b9b6ae"], "cab": ["metal", "#9aa3a8"],
	"step": ["stone", "#c9c2b3"],
	"book0": ["plaster", "#8b2c2c"], "book1": ["plaster", "#2c4f8b"], "book2": ["plaster", "#2e6b3f"], "book3": ["plaster", "#c49a3c"],
	"book4": ["plaster", "#5a3b6b"], "book5": ["plaster", "#d8d2c0"],
	"goods0": ["plaster", "#d9483b"], "goods1": ["plaster", "#f2b33d"], "goods2": ["plaster", "#3d8ad9"], "goods3": ["plaster", "#5fb34a"],
	"goods4": ["plaster", "#e8e3d6"], "goods5": ["plaster", "#8b5a2b"],
}
BOOKS = ["book%d" % i for i in range(6)]
GOODS = ["goods%d" % i for i in range(6)]


def r(v):
	return round(v, 4)


def box(w, h, d, x, y, z, c, **o):
	return ["box", [r(w), r(h), r(d)], [r(x), r(y), r(z)], c] + ([o] if o else [])


def cyl(rad, h, x, y, z, c, top=None, **o):
	return ["cyl", [r(rad), r(h), r(rad if top is None else top)], [r(x), r(y), r(z)], c] + ([o] if o else [])


def ball(rad, x, y, z, c):
	return ["ball", [r(rad)], [r(x), r(y), r(z)], c]


def legs(w, d, h, c, inset=0.06, rad=0.022):
	return [cyl(rad, h, sx * (w / 2 - inset), 0, sz * (d / 2 - inset), c, rad * 0.8) for sx in (-1, 1) for sz in (-1, 1)]


def bedlike(kind, W, D):
	back = -D / 2
	if kind == "wardBed":
		return [box(W, 0.08, D, 0, 0.55, 0, "steel"), *legs(W, D, 0.55, "steel", 0.05, 0.025), box(W - 0.06, 0.15, D - 0.1, 0, 0.63, 0, "linen"),
			box(0.6, 0.11, 0.35, 0, 0.77, back + 0.3, "linen"), box(W, 0.6, 0.04, 0, 0.55, back + 0.02, "steel"),
			box(0.04, 0.3, D * 0.6, -W / 2, 0.72, 0, "chrome"), box(0.04, 0.3, D * 0.6, W / 2, 0.72, 0, "chrome")]
	fh = 0.32
	out = [box(W, fh, D, 0, 0.05, 0, "walnut"), *legs(W, D, 0.06, "walnut", 0.06, 0.03), box(W - 0.06, 0.2, D - 0.12, 0, fh, 0.02, "linen"),
		box(W - 0.04, 0.06, D * 0.62, 0, fh + 0.19, D * 0.17, "duvet"), box(W + 0.04, 0.95, 0.07, 0, 0.05, back + 0.03, "walnut")]
	for px in ([-W / 4, W / 4] if kind == "bed" else [0]):
		out.append(box(min(0.6, W * 0.42), 0.12, 0.38, px, fh + 0.2, back + 0.32, "linen"))
	return out


def seating(kind, W, D, H):
	back = -D / 2
	if kind in ("sofa", "armchair"):
		arm, seats = 0.18, 3 if kind == "sofa" else 1
		inner = W - 2 * arm
		out = [box(W, 0.28, D, 0, 0.08, 0, "fabric"), *legs(W, D, 0.08, "walnut", 0.08, 0.025), box(arm, 0.32, D, -W / 2 + arm / 2, 0.36, 0, "fabric"),
			box(arm, 0.32, D, W / 2 - arm / 2, 0.36, 0, "fabric"), box(W - 0.04, 0.45, 0.2, 0, 0.36, back + 0.1, "fabric")]
		for i in range(seats):
			x = -inner / 2 + inner / seats * (i + 0.5)
			out += [box(inner / seats - 0.02, 0.14, D - 0.26, x, 0.36, 0.09, "cushion"), box(inner / seats - 0.04, 0.38, 0.16, x, 0.48, back + 0.24, "cushion")]
		return out
	if kind == "chair":
		return [box(W, 0.04, D, 0, 0.44, 0, "oak"), *legs(W, D, 0.44, "walnut", 0.04, 0.02), box(0.03, 0.46, 0.03, -W / 2 + 0.04, 0.48, back + 0.03, "walnut"),
			box(0.03, 0.46, 0.03, W / 2 - 0.04, 0.48, back + 0.03, "walnut"), box(W - 0.06, 0.08, 0.025, 0, 0.82, back + 0.03, "oak"),
			box(W - 0.06, 0.05, 0.025, 0, 0.66, back + 0.03, "oak")]
	if kind == "officeChair":
		return [box(W - 0.04, 0.08, D - 0.06, 0, 0.44, 0.02, "black"), box(W - 0.08, 0.5, 0.06, 0, 0.55, back + 0.06, "black"), cyl(0.03, 0.4, 0, 0.06, 0, "chrome"),
			box(0.04, 0.03, 0.28, 0, 0.04, -0.14, "black", spin=5), box(0.06, 0.06, 0.06, 0, 0.0, -0.27, "black", spin=5)]
	if kind == "seat":
		return [box(W - 0.1, 0.12, D - 0.12, 0, 0.4, 0.04, "red"), box(W - 0.1, 0.55, 0.12, 0, 0.42, back + 0.07, "red"),
			box(0.05, 0.62, D, -W / 2 + 0.025, 0, 0, "black"), box(0.05, 0.62, D, W / 2 - 0.025, 0, 0, "black")]
	return [box(W, 0.05, D * 0.75, 0, 0.44, 0.06, "oak"), box(W, 0.5, 0.05, 0, 0.45, back + 0.05, "oak"),
		box(0.06, H, D, -W / 2 + 0.03, 0, 0, "walnut"), box(0.06, H, D, W / 2 - 0.03, 0, 0, "walnut")]


def shelving(kind, W, D, H):
	back = -D / 2
	if kind == "rack":
		out = [box(0.06, H, 0.06, sx * (W / 2 - 0.03), 0, sz * (D / 2 - 0.03), "rack_post") for sx in (-1, 0, 1) for sz in (-1, 1)]
		for y in (0.15, 1.15, 2.15):
			out += [box(W, 0.08, 0.05, 0, y, D / 2 - 0.03, "rack_beam"), box(W, 0.08, 0.05, 0, y, -D / 2 + 0.03, "rack_beam")]
			out += [box(W / 3 - 0.15, 0.7, D - 0.2, bx, y + 0.08, 0, "crate") for bx in (-W / 3, 0, W / 3)]
		return out
	book = kind == "bookshelf"
	wood = "walnut" if book else "pewter"
	out = [box(W, H, 0.03, 0, 0, back + 0.015, wood), box(0.04, H, D, -W / 2 + 0.02, 0, 0, wood), box(0.04, H, D, W / 2 - 0.02, 0, 0, wood)]
	levels = 5 if book else 4
	for k in range(levels + 1):
		y = k / levels * (H - 0.04)
		out.append(box(W - 0.06, 0.03, D - 0.04, 0, y, 0, wood))
		if k == levels:
			break
		bw = 0.052 if book else 0.26
		bh = (H / levels - 0.06) * (0.86 if book else 0.7)
		out.append(box(bw - 0.006, bh, D * 0.7, -W / 2 + 0.06 + bw / 2, y + 0.03, 0.02, (BOOKS if book else GOODS)[0],
			n=int((W - 0.18) / bw), step=[bw, 0, 0], colors=BOOKS if book else GOODS))
	return out


def counters(kind, W, D, H):
	back = -D / 2
	body = "walnut" if kind == "barCounter" else "steel" if kind == "checkout" else "oak"
	out = [box(W, 0.08, D - 0.08, 0, 0, 0.02, "black"), box(W, H - 0.12, D - 0.04, 0, 0.08, 0, body),
		box(W + 0.04, 0.04, D + 0.04, 0, H - 0.04, 0, "black" if kind == "barCounter" else "counter_top")]
	if kind == "barCounter":
		out.append(cyl(0.035, 0.28, -W / 2 + 0.4, H, back + 0.12, GOODS[0], 0.015, n=6, step=[0.6, 0, 0], colors=GOODS))
	if kind == "checkout":
		out += [box(W * 0.6, 0.02, D * 0.5, -W * 0.15, H, 0.05, "black"), box(0.3, 0.25, 0.3, W / 2 - 0.25, H, 0, "desk_pad")]
	return out


def build(kind, W, D, H):
	back = -D / 2
	if kind in ("bed", "singleBed", "wardBed"):
		return bedlike(kind, W, D)
	if kind in ("sofa", "armchair", "chair", "officeChair", "seat", "pew"):
		return seating(kind, W, D, H)
	if kind in ("shelf", "bookshelf", "rack"):
		return shelving(kind, W, D, H)
	if kind in ("counter", "barCounter", "checkout"):
		return counters(kind, W, D, H)
	simple = {
		"table": lambda: [box(W, 0.04, D, 0, H - 0.04, 0, "oak"), *legs(W, D, H - 0.04, "walnut", 0.07, 0.025)],
		"altar": lambda: [box(W, H, D, 0, 0, 0, "altar"), box(W + 0.06, 0.04, D + 0.06, 0, H, 0, "linen"), box(W * 0.5, 0.02, D + 0.08, 0, H - 0.25, 0, "red"),
			cyl(0.03, 0.3, -W * 0.3, H, 0, "gold"), cyl(0.03, 0.3, W * 0.3, H, 0, "gold")],
		"desk": lambda: [box(W, 0.035, D, 0, H - 0.035, 0, "oak"), box(0.04, H - 0.035, D - 0.04, -W / 2 + 0.03, 0, 0, "walnut"),
			box(0.4, H - 0.12, D - 0.06, W / 2 - 0.22, 0.05, 0, "walnut"), box(0.5, 0.32, 0.03, 0, H + 0.12, back + 0.15, "black"),
			box(0.06, 0.12, 0.06, 0, H, back + 0.15, "black"), box(0.42, 0.015, 0.14, 0, H, 0.08, "desk_pad")],
		"fridge": lambda: [box(W, H, D, 0, 0, 0, "white"), box(W - 0.02, 0.01, 0.01, 0, H * 0.62, D / 2, "fridge_seam"),
			box(0.025, 0.35, 0.03, W / 2 - 0.08, H * 0.7, D / 2 + 0.02, "chrome"), box(0.025, 0.5, 0.03, W / 2 - 0.08, H * 0.18, D / 2 + 0.02, "chrome")],
		"stove": lambda: [box(W, H - 0.03, D, 0, 0, 0, "stove_front"), box(W, 0.03, D, 0, H - 0.03, 0, "black"), box(W - 0.12, 0.38, 0.01, 0, 0.18, D / 2, "black"),
			box(W - 0.2, 0.02, 0.03, 0, 0.62, D / 2 + 0.02, "chrome")] + [cyl(0.08, 0.012, sx * W * 0.22, H, sz * D * 0.22, "burner") for sx in (-1, 1) for sz in (-1, 1)],
		"sink": lambda: [box(W, H - 0.04, D, 0, 0, 0, "oak"), box(W, 0.04, D, 0, H - 0.04, 0, "sink_top"), box(W * 0.5, 0.02, D * 0.6, 0, H, 0.02, "chrome"),
			cyl(0.015, 0.25, 0, H, back + 0.08, "chrome")],
		"toilet": lambda: [cyl(0.17, 0.4, 0, 0, 0.08, "ceramic", 0.2), box(0.38, 0.05, 0.46, 0, 0.4, 0.06, "white"), box(W, 0.38, 0.18, 0, 0.4, back + 0.09, "ceramic")],
		"bath": lambda: [box(W, H, D, 0, 0, 0, "ceramic"), box(W - 0.12, 0.02, D - 0.14, 0, H - 0.015, 0, "porcelain"), cyl(0.015, 0.2, 0, H, back + 0.05, "chrome")],
		"wardrobe": lambda: [box(W, H, D, 0, 0, 0, "walnut"), box(0.01, H - 0.1, 0.01, 0, 0.05, D / 2, "black"), box(0.02, 0.18, 0.03, -0.06, H * 0.5, D / 2 + 0.01, "chrome"),
			box(0.02, 0.18, 0.03, 0.06, H * 0.5, D / 2 + 0.01, "chrome")],
		"locker": lambda: [box(W, H, D, 0, 0, 0, "locker"), box(0.01, H - 0.1, 0.01, 0, 0.05, D / 2, "black"), box(0.02, 0.18, 0.03, -0.06, H * 0.5, D / 2 + 0.01, "chrome"),
			box(0.02, 0.18, 0.03, 0.06, H * 0.5, D / 2 + 0.01, "chrome")],
		"tv": lambda: [box(W, 0.45, D, 0, 0, 0, "walnut"), box(W * 0.9, 0.55, 0.05, 0, 0.5, 0, "black"), box(W * 0.86, 0.5, 0.01, 0, 0.525, 0.03, "screen")],
		"plant": lambda: [cyl(0.17, 0.32, 0, 0, 0, "pot", 0.21), ball(0.24, 0, 0.6, 0, "leaf"), ball(0.18, 0.12, 0.82, 0.05, "leaf_dark"), ball(0.16, -0.1, 0.78, -0.06, "leaf")],
		"bars": lambda: [box(W, 0.06, D, 0, H - 0.06, 0, "black"), box(W, 0.06, D, 0, 1.0, 0, "black"),
			cyl(0.015, H, -W / 2 + 0.05, 0, 0, "bar_steel", 0.015, n=int((W - 0.09) / 0.13) + 1, step=[0.13, 0, 0])],
		"screen": lambda: [box(W, H, D, 0, 1.5, 0, "black"), box(W - 0.2, H - 0.2, 0.02, 0, 1.6, D / 2, "white")],
		"blackboard": lambda: [box(W, H, D, 0, 0.9, 0, "cabinet"), box(W - 0.1, H - 0.1, 0.02, 0, 0.95, D / 2, "board"), box(W - 0.2, 0.03, 0.08, 0, 0.88, D / 2 + 0.03, "cabinet")],
		"machine": lambda: [box(W, H * 0.7, D, 0, 0, 0, "machine"), box(W * 0.5, H * 0.3, D * 0.4, -W * 0.15, H * 0.7, 0, "machine_dark"),
			box(0.3, 0.25, 0.03, W * 0.25, H * 0.5, D / 2, "screen"), box(0.36, 0.36, D * 0.9, W * 0.2, H * 0.72, 0, "steel")],
		"atm": lambda: [box(W, H, D, 0, 0, 0, "atm"), box(W * 0.6, 0.25, 0.02, 0, H * 0.62, D / 2, "screen"), box(W * 0.4, 0.04, 0.12, 0, H * 0.45, D / 2, "black")],
		"stage": lambda: [box(W, H, D, 0, 0, 0, "black"), box(0.6, 1.2, 0.5, -W / 2 + 0.4, H, back + 0.4, "amp"), box(0.6, 1.2, 0.5, W / 2 - 0.4, H, back + 0.4, "amp"),
			box(1.4, 0.8, 0.6, 0, H, 0, "console")],
		"treadmill": lambda: [box(W, 0.18, D, 0, 0, 0, "black"), box(W - 0.14, 0.02, D - 0.3, 0, 0.18, 0.05, "belt"), box(0.05, 1.0, 0.05, -W / 2 + 0.05, 0.18, back + 0.15, "steel"),
			box(0.05, 1.0, 0.05, W / 2 - 0.05, 0.18, back + 0.15, "steel"), box(W, 0.12, 0.25, 0, 1.15, back + 0.15, "black")],
		"pallet": lambda: [box(W, 0.14, D, 0, 0, 0, "pallet"), box(W - 0.1, H - 0.18, D - 0.1, 0, 0.14, 0, "pallet_load")],
		"stairs": lambda: [box(W, H / 14, D / 14 + 0.004, 0, 0, D / 2 - D / 28, "tread", n=14, step=[0, 0, -D / 14], grow=[0, H / 14, 0], colors=["tread", "tread_dark"]),
			*[box(0.04, 0.9, 0.04, W / 2 - 0.03, H * t, D / 2 - D * t, "walnut") for t in (0.1, 0.5, 0.9)], box(0.06, 0.05, D * 0.95, W / 2 - 0.03, H * 0.5 + 0.9, 0, "walnut")],
		"stairwell": lambda: [box(W, 0.012, D, 0, 0, 0, "well"), box(0.05, H, D, -W / 2, 0, 0, "walnut"), box(0.05, H, D, W / 2, 0, 0, "walnut"), box(W, H, 0.05, 0, 0, D / 2, "walnut")],
		"ceilingLamp": lambda: [cyl(0.05, 0.12, 0, H - 0.12, 0, "steel"), cyl(W / 2, 0.2, 0, 0.02, 0, "shade", W / 2 * 0.55), ball(0.08, 0, 0.05, 0, "bulb")],
		"floorLamp": lambda: [cyl(0.16, 0.03, 0, 0, 0, "black"), cyl(0.018, H - 0.35, 0, 0.03, 0, "black"), cyl(W / 2 * 0.6, 0.32, 0, H - 0.34, 0, "shade", W / 2)],
		"tableLamp": lambda: [cyl(0.09, 0.03, 0, 0, 0, "oak", 0.1), cyl(0.02, H - 0.25, 0, 0.03, 0, "oak"), cyl(W / 2 * 0.65, 0.22, 0, H - 0.24, 0, "shade", W / 2)],
	}
	if kind not in simple:
		raise SystemExit("no model for " + kind)
	return simple[kind]()


def main():
	style = json.load(open(os.path.join(GAME, "start.json"), encoding="utf-8"))["default_style"]  # the style the game plays
	sizes = json.load(open(os.path.join(CATALOG, "interiors.json"), encoding="utf-8"))["styles"][style]["sizes"]
	models = {k: build(k, w, d, h) for k, w, d, h in sizes}
	doc = ("What each piece of furniture is made of, part by part, for the cut-open view of a building (tools/assets/make_furniture.py writes it "
		"from the sizes of interiors.json). A part is [shape, size, place, colour, options]; see the script for the shapes and options.")
	text = "{\n \"_doc\": %s,\n \"palette\": {\n" % json.dumps(doc)
	text += ",\n".join("  %s: %s" % (json.dumps(k), json.dumps(v)) for k, v in PALETTE.items()) + "\n },\n \"models\": {\n"
	text += ",\n".join("  %s: [\n%s\n  ]" % (json.dumps(k), ",\n".join("   " + json.dumps(p) for p in v)) for k, v in models.items()) + "\n }\n}\n"
	target = os.path.join(DATA, "furniture.json")
	if "--check" in sys.argv:
		with open(target, encoding="utf-8", newline="") as fh:
			if fh.read() != text:
				raise SystemExit("furniture.json is not what make_furniture.py makes of the sizes: run it, or change the script, not the table")
		print("furniture.json is the generator's")
		return
	open(target, "w", encoding="utf-8", newline="\n").write(text)
	print(sum(len(v) for v in models.values()), "parts in", len(models), "pieces")


if __name__ == "__main__":
	main()
