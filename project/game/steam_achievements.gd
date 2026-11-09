## The city's milestones told to Steam (the lab's steamAchievements.ts): each road count of ROAD_MILESTONES the city passes in a
## session asks its achievement once, through the Steam bridge when GodotSteam runs the game (its singleton "Steam": setAchievement
## keeps the unlock in Steam's memory, storeStats sends it to the server and shows the overlay's note, GodotSteam doc_classes
## Steam.xml); with no bridge nothing is asked, as in the lab without its desktop shell.
class_name SteamAchievements
extends Node

## Roads on the map and the achievement each count unlocks: the ids and counts set in Steamworks for the game (the lab's
## steamAchievements.ts ROAD_MILESTONES), an outside contract and not a tunable of the game.
const ROAD_MILESTONES: Array = [[1, "ROADS_1"], [100, "ROADS_100"], [1000, "ROADS_1000"]]
const STEAM_SINGLETON := &"Steam"

var bridge: Object = null # the Steam singleton, or a test's stand-in; null: nothing is asked
var _passed := 0 # the milestones passed this session: each asked once


func _ready() -> void:
	if bridge == null and Engine.has_singleton(STEAM_SINGLETON):
		bridge = Engine.get_singleton(STEAM_SINGLETON)
	CityStore.changed.connect(func(snapshot: Dictionary) -> void: roads(int(snapshot.get("segments", 0))))


## The roads on the map now: every milestone the count has reached and this session has not asked is unlocked (one Steam already
## holds is not set again), and the unlocks are sent in one store.
func roads(count: int) -> void:
	if bridge == null:
		return
	var unlocked := false
	while _passed < ROAD_MILESTONES.size() and count >= int(ROAD_MILESTONES[_passed][0]):
		var id: String = ROAD_MILESTONES[_passed][1]
		_passed += 1
		var held: Dictionary = bridge.getAchievement(id)
		if not held.get("achieved", false):
			unlocked = bool(bridge.setAchievement(id)) or unlocked
	if unlocked and not bool(bridge.storeStats()):
		push_warning("achievement not told to Steam")
