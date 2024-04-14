extends Node

@export var rectangle_scene : PackedScene

func _on_timer_timeout():
	spawn_rectangle()

func spawn_rectangle():
	var rectangle = rectangle_scene.instantiate()
	
	var side_index = randi_range(0, 3)
	
	if side_index == 0:
		rectangle.global_position = Vector2(-300, randi_range(0, 1080))
		rectangle.velocity = Vector2(1, 0)
	elif side_index == 1:
		rectangle.global_position = Vector2(2220, randi_range(0, 1080))
		rectangle.velocity = Vector2(-1, 0)
	elif side_index == 2:
		rectangle.global_position = Vector2(randi_range(0, 1920), -300)
		rectangle.velocity = Vector2(0, 1)
	elif side_index == 3:
		rectangle.global_position = Vector2(randi_range(0, 1920), 1380)
		rectangle.velocity = Vector2(0, -1)
		
	rectangle.rotate_velocity = randf_range(-1.5, 1.5)
	
	add_child(rectangle)
