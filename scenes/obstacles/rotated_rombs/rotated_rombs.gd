extends Node

@export var romb_scene : PackedScene

func _on_timer_timeout():
	spawn_romb()

func spawn_romb():
	var romb = romb_scene.instantiate()
	
	var side_index = randi_range(0, 3)
	
	if side_index == 0:
		romb.global_position = Vector2(-300, randi_range(0, 1080))
		romb.velocity = Vector2(1, 0)
	elif side_index == 1:
		romb.global_position = Vector2(2220, randi_range(0, 1080))
		romb.velocity = Vector2(-1, 0)
	elif side_index == 2:
		romb.global_position = Vector2(randi_range(0, 1920), -300)
		romb.velocity = Vector2(0, 1)
	elif side_index == 3:
		romb.global_position = Vector2(randi_range(0, 1920), 1380)
		romb.velocity = Vector2(0, -1)
	
	romb.velocity = romb.velocity.rotated(randi_range(-30, 30))
	
	add_child(romb)
