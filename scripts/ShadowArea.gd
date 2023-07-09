extends Node

enum Direction {
	left, right
}

var visible = true
const physics_layer = 0
const block_layer = 5
var bit_mask = 0

var tile_map
var area_position : Vector2
var enter_side = Direction.right

func _ready():
	var children = get_children()
	for child in children:
		if child is Area2D:
			child.connect("body_entered", body_entered)
			child.connect("body_exited", body_exited)
			area_position = child.get_node("CollisionShape2D").global_position


func body_entered(body):
	if (body.is_in_group("Player")):
		enter_side = get_body_side(body)


func body_exited(body):
	if (body.is_in_group("Player")):
		var exit_side = get_body_side(body)
		if (exit_side == enter_side):
			return
		if visible:
			tile_map = $TileMap.duplicate()
			$TileMap.queue_free()
		else:
			add_child(tile_map)
		visible = not visible


func get_body_side(body):
	var collision_shape = body.get_node("CollisionShape2D")
	var x = body.position.x + collision_shape.position.x
	if x < area_position.x:
		return Direction.left
	else:
		return Direction.right
