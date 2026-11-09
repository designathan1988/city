## The city's Steam milestones (game/steam_achievements.gd) on a stand-in for GodotSteam's singleton: each road count's achievement
## asked once as the count passes it and sent with one store, none asked again in the session, one Steam already holds not set
## again, nothing asked with no bridge, and the node in the tree reading the town's roads from the store's snapshot.
## Run: godot --path project res://tests/core/steam_achievements_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const MANY := 5000 # past every milestone


## What the game asks of GodotSteam's singleton, recorded.
class SteamStandIn:
	extends RefCounted
	var set_ids: Array[String] = []
	var stores := 0
	var achieved := {}

	func getAchievement(id: String) -> Dictionary:
		return {"ret": true, "achieved": achieved.has(id)}

	func setAchievement(id: String) -> bool:
		set_ids.append(id)
		achieved[id] = true
		return true

	func storeStats() -> bool:
		stores += 1
		return true


func _ready() -> void:
	if not boot():
		finish()
		return
	var steam := SteamStandIn.new()
	var session := SteamAchievements.new()
	session.bridge = steam
	session.roads(0)
	check(steam.set_ids.is_empty() and steam.stores == 0, "no road: nothing asked")
	session.roads(1)
	check(steam.set_ids == ["ROADS_1"] and steam.stores == 1, "the first road unlocks ROADS_1, stored (%s, %d)" % [steam.set_ids, steam.stores])
	session.roads(50)
	session.roads(1)
	check(steam.set_ids.size() == 1 and steam.stores == 1, "no milestone passed: nothing asked again (%s)" % [steam.set_ids])
	session.roads(MANY)
	check(steam.set_ids == ["ROADS_1", "ROADS_100", "ROADS_1000"] and steam.stores == 2, "the rest unlocked at once, one store (%s, %d)" % [steam.set_ids, steam.stores])
	var next := SteamAchievements.new()
	next.bridge = steam
	next.roads(MANY)
	check(steam.set_ids.size() == 3 and steam.stores == 2, "a new session: what Steam holds is not set again")
	var bare := SteamAchievements.new()
	bare.roads(MANY)
	check(bare.bridge == null and steam.set_ids.size() == 3, "no bridge: nothing asked")
	for node: Node in [session, next, bare]:
		node.free()
	var town := SteamStandIn.new()
	var live := SteamAchievements.new()
	live.bridge = town
	add_child(live)
	new_map(TOWN_MAP, SEED, true)
	CityStore.refresh()
	var shown: Dictionary = await CityStore.changed
	var roads := int(shown.get("segments", 0))
	check(roads >= int(SteamAchievements.ROAD_MILESTONES[1][0]) and town.set_ids.has("ROADS_1") and town.set_ids.has("ROADS_100"),
			"the town's %d roads, read from the snapshot, unlock %s" % [roads, town.set_ids])
	finish()
