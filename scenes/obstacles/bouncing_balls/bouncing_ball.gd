extends RigidBody2D

func _ready():
	var scale_factor = randf_range(0.5, 1.5)
	$CollisionShape2D.shape.radius = $CollisionShape2D.shape.radius * scale_factor
	$Sprite2D.scale *= scale_factor
