## A road being drawn by hand: the points a free stroke keeps, how long it has run, and the words and paint of its preview.
class_name RoadDraft
extends RefCounted

## A long stroke keeps every sample up to DENSE_SAMPLES, then one each SPARSE_SAMPLE_M, up to MOST_SAMPLES.
const DENSE_SAMPLES := 256
const MOST_SAMPLES := 4096
const SPARSE_SAMPLE_M := 1.0
const ROUND_TO_M := 10


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
	var p: Dictionary = CityStore.query(&"road_profiles")[profile]
	if not p.painted:
		return ""
	return "yellow" if p.yellow else "white"


## "120 m · R$ 150.000": the length in tens of metres and its cost (Roadcraft main.ts drawRoadPreview).
static func measure(plan: Dictionary) -> String:
	var tens := roundi(plan.length / ROUND_TO_M) * ROUND_TO_M
	return "%s · %s" % [Formatters.format_length(tens), Formatters.format_cost(plan.price)]
