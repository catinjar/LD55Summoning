class_name PickupBase

extends Area2D

enum ObstacleType
{
	CIRCLE_RAIN = 0,
}

@export var obstacle_type : ObstacleType
@export var price : int = 100

func _on_area_entered(area):
	if area.is_in_group("player"):
		GameCore.add_obstacle(self)
		queue_free()
