extends RigidBody2D

@export var rotate_velocity = 0

var velocity : Vector2

func _ready():
	var scale_factor = randf_range(0.25, 0.75)
	$CollisionShape2D.shape.size = $CollisionShape2D.shape.size * scale_factor
	$Sprite2D.scale *= scale_factor

func _process(delta):
	global_rotation += rotate_velocity * delta
	global_position += velocity * 200 * delta

func _on_visible_on_screen_notifier_2d_screen_exited():
	queue_free()
