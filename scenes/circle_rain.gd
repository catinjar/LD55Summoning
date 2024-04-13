extends Node2D

@export var circle_scene : PackedScene

func _on_timer_timeout():
	spawn_circle()

func spawn_circle():
	var circle = circle_scene.instantiate()
	circle.global_position = Vector2(randf_range(0, 1920), -100)
	add_child(circle)
