extends Node2D

@export var circle_rain_scene : PackedScene
@export var squares_scene : PackedScene
@export var bouncing_ball_scene : PackedScene
@export var pickup_scenes : Array[PackedScene]

var obstacles = []
var current_pickups = []

var score = 0
var high_score = 0

func add_obstacle(pickup):
	obstacles.push_back(pickup.obstacle_type)
	
	var pickup_scene
	if pickup.obstacle_type == PickupBase.ObstacleType.CIRCLE_RAIN:
		pickup_scene = circle_rain_scene
	elif pickup.obstacle_type == PickupBase.ObstacleType.SQUARES:
		pickup_scene = squares_scene
	elif pickup.obstacle_type == PickupBase.ObstacleType.BOUNCING_BALL:
		pickup_scene = bouncing_ball_scene
	
	var obstacle = pickup_scene.instantiate()
	score += pickup.price + (obstacles.size() - 1) * pickup.price / 2
	
	var container = get_node("/root/Main")
	container.add_child(obstacle)
	
	spawn_pickups()

func game_over():
	if score > high_score:
		high_score = score
	
	reset()
	get_tree().change_scene_to_file("res://scenes/main.tscn")
	
func reset():
	obstacles.clear()
	score = 0
	
func start_game():
	spawn_pickups()
	
func spawn_pickups():
	for pickup in current_pickups:
		if pickup != null:
			pickup.queue_free()
	current_pickups.clear()
	
	for i in 2:
		var pickup_scene = pickup_scenes[randi_range(0, pickup_scenes.size() - 1)]
		var pickup = pickup_scene.instantiate()
		pickup.global_position = Vector2(randf_range(0, 1920), randf_range(0, 1080))
		
		var container = get_node("/root/Main")
		container.add_child(pickup)
		
		current_pickups.push_back(pickup)
