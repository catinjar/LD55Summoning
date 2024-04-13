extends Node2D

var obstacles = []

func add_obstacle(obstacle_type : PickupBase.ObstacleType):
	obstacles.push_back(obstacle_type)
	
	if obstacle_type == PickupBase.ObstacleType.CIRCLE_RAIN:
		pass
