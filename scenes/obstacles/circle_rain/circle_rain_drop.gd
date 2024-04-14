extends RigidBody2D

func _ready():
	var scale_factor = randf_range(0.4, 1)
	$CollisionShape2D.shape.radius = $CollisionShape2D.shape.radius * scale_factor
	$Sprite2D.scale *= scale_factor

func _process(delta):
	if global_position.y > 1300:
		queue_free()
