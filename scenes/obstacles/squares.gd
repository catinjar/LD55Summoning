extends Node

@export var square_scene : PackedScene

func _on_timer_timeout():
	spawn_square()

func spawn_square():
	var square = square_scene.instantiate()
	
	var side_index = randi_range(0, 3)
	
	if side_index == 0:
		square.global_position = Vector2(-300, randi_range(0, 1080))
		square.velocity = Vector2(1, 0)
	elif side_index == 1:
		square.global_position = Vector2(2220, randi_range(0, 1080))
		square.velocity = Vector2(-1, 0)
	elif side_index == 2:
		square.global_position = Vector2(randi_range(0, 1920), -300)
		square.velocity = Vector2(0, 1)
	elif side_index == 3:
		square.global_position = Vector2(randi_range(0, 1920), 1380)
		square.velocity = Vector2(0, -1)
	
	add_child(square)
