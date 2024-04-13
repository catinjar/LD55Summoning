extends Node

func _process(delta):
	$Sprite2D.modulate.a = abs(sin($Timer.time_left)) * 0.25
	
	
