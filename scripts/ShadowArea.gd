extends Node

var visible = true
const physics_layer = 0
const block_layer = 5
var bit_mask = 0

var tile_map


func _on_body_entered(body):
	if (body.is_in_group("Player")):
		if visible:
			tile_map = $TileMap.duplicate()
			$TileMap.queue_free()
		else:
			add_child(tile_map)
			
		visible = not visible
