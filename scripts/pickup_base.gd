class_name PickupBase

extends Area2D

enum ObstacleType
{
	CIRCLE_RAIN = 0, # Balls dropping from above
	SQUARES = 1, # Just moving squares
	BOUNCING_TRIANGLES = 2, # Keeps bounding inside screen
	BOUNCING_BALL = 3, # Ball falls with gravity and bounces
	ROTATING_SQUARES = 4,
	SCALING_BALLS = 5, # Balls that are scaling and moving in random directions
}

@export var obstacle_type : ObstacleType
@export var price : int = 100

func _on_area_entered(area):
	if area.is_in_group("player"):
		GameCore.add_obstacle(self)
		queue_free()
