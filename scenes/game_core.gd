extends Node2D

@export var circle_rain_scene : PackedScene
@export var pickup_scenes : Array[PackedScene]

var obstacles = []
var current_pickups = []

func add_obstacle(obstacle_type : PickupBase.ObstacleType):
	obstacles.push_back(obstacle_type)
	
	if obstacle_type == PickupBase.ObstacleType.CIRCLE_RAIN:
		var circle_rain = circle_rain_scene.instantiate()
		var container = get_node("/root/Main")
		container.add_child(circle_rain)
	
	spawn_pickups()

func game_over():
	reset()
	get_tree().change_scene_to_file("res://scenes/main.tscn")
	
func reset():
	obstacles.clear()
	
func start_game():
	spawn_pickups()
	
func spawn_pickups():
	current_pickups.clear()
	
	for i in 2:
		var pickup_scene = pickup_scenes[randi_range(0, pickup_scenes.size() - 1)]
		var pickup = pickup_scene.instantiate()
		pickup.global_position = Vector2(randf_range(0, 1920), randf_range(0, 1080))
		
		var container = get_node("/root/Main")
		container.add_child(pickup)
		
		current_pickups.push_back(pickup)
