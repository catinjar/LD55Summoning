extends Node

@export var bouncing_ball_scene : PackedScene

func _ready():
	for i in 1:
		spawn_bouncing_ball()

func spawn_bouncing_ball():
	var bouncing_ball = bouncing_ball_scene.instantiate()
	bouncing_ball.global_position = Vector2(randf_range(0, 1920), -300)
	add_child(bouncing_ball)
