## A road being drawn by hand: the points a free stroke keeps, how long it has run, and the words and paint of its preview.
class_name RoadDraft
extends RefCounted

const DENSE_SAMPLES := 256
const MOST_SAMPLES := 4096
const SPARSE_SAMPLE_M := 1.0
const ROUND_TO_M := 10
const SHORTEST_STRETCH_M := 5.0 ## a stretch built some way is named from this length on


static func sample(draft: Dictionary, p: Vector3) -> void:
	var samples: PackedVector3Array = draft.samples
	if samples.size() >= MOST_SAMPLES:
		return
	if samples.size() >= DENSE_SAMPLES and p.distance_to(samples[-1]) < SPARSE_SAMPLE_M:
		return
	samples.push_back(p)
	draft.samples = samples


static func traveled(d: Dictionary) -> float:
	var total := 0.0
	var samples: PackedVector3Array = d.samples
	for i in range(1, samples.size()):
		total += samples[i].distance_to(samples[i - 1])
	return total + samples[-1].distance_to(d.end.position)


## The paint the preview draws: none on a road without lines.
static func paint(profile: int) -> String:
	var p: Dictionary = CityStore.cached(&"road_profiles")[profile]
	if not p.painted:
		return ""
	return "yellow" if p.yellow else "white"


## "120 m · R$ 150.000 · bridge 80 m · embankment 40 m": the length in tens of metres, its cost, then each way it is built
## that is not plain ground, the longest first (the metres the price counts: the plan's `built`), and whether its end
## stopped short of the height asked.
static func measure(plan: Dictionary) -> String:
	var tens := roundi(plan.length / ROUND_TO_M) * ROUND_TO_M
	var parts := [Formatters.format_length(tens), Formatters.format_cost(plan.price)]
	var built: Dictionary = plan.get("built", {})
	var kinds := built.keys().filter(func(k: String) -> bool: return built[k] >= SHORTEST_STRETCH_M)
	kinds.sort_custom(func(a: String, b: String) -> bool: return built[a] > built[b])
	for kind: String in kinds:
		var metres := maxi(ROUND_TO_M, roundi(built[kind] / ROUND_TO_M) * ROUND_TO_M)
		parts.append(TranslationServer.translate("road.built." + kind).format({"length": Formatters.format_length(metres)}))
	if plan.get("limited", false):
		parts.append(TranslationServer.translate("road.built.limited"))
	return " · ".join(parts)
