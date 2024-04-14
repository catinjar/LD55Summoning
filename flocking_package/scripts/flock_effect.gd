extends Node

@export var resolution : Vector2

var flocks : Array


func _ready():
	var parent = get_parent()
	for child in parent.get_children():
		if child is Sprite2D:
			create_flock(child)


func _exit_tree():
	for flock in flocks:
		CommonFlockingSystem.remove_flock(flock)


func create_flock(sprite):
	#sprite.hide()
	
	var flock = Flock.new()
	flock.set_owner(sprite)
	
	var image = sprite.texture.get_image()
	var size : Vector2 = image.get_size()
	flock.set_size(image.get_size())
	
	assert(resolution.x > 0)
	assert(resolution.y > 0)
	
	var width = image.get_width() / resolution.x
	var height = image.get_height() / resolution.y
	
	for y in height:
		for x in width:
			var pixel_x = x * resolution.x
			var pixel_y = y * resolution.y
			
			var color = image.get_pixel(pixel_x, pixel_y)
			color *= sprite.modulate * sprite.self_modulate
			
			if color.a == 0:
				continue
			
			var random_offset = Vector2(-0.5 + randf() * 30, -0.5 + randf() * 30)
			
			var target_position = Vector2(randf() * image.get_width(), randf() * image.get_height()) + random_offset
			target_position *= sprite.global_scale
			
			if sprite.flip_h:
				target_position.x = size.x * sprite.global_scale.x - target_position.x
			if sprite.flip_v:
				target_position.y = size.y * sprite.global_scale.y - target_position.y
			if sprite.centered:
				target_position -= size * sprite.global_scale / 2
			
			target_position += sprite.offset * sprite.global_scale            
			
			if sprite.global_rotation != 0:
				target_position = target_position.rotated(sprite.global_rotation)
			
			target_position += sprite.global_position
			
			var dot = FlockDot.new()
			dot.set_current_position(target_position)
			dot.set_target_position(Vector2(pixel_x, pixel_y) + Vector2(-0.5 + randf(), -0.5 + randf()) * 5)
			dot.set_color(color)
			
			flock.add_dot(dot)

	flocks.append(flock)
	CommonFlockingSystem.add_flock(flock)
