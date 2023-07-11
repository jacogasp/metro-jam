@tool
extends Node2D

enum Direction {
	left, right
}

var shadow_visible = true
const physics_layer = 0
const block_layer = 5
var bit_mask = 0

var tile_map
var area_position : Vector2
var enter_side = Direction.right

func _get_configuration_warnings():
	var warnings = []
	if not $Area2D:
		warnings.push_back("A child of type Area2D is required")
	if not $TileMap:
		warnings.push_back("A child of type TileMap is required")
	return warnings

func _ready():
	if not $Area2D:
		return
	$Area2D.connect("body_entered", body_entered)
	$Area2D.connect("body_exited", body_exited)
	area_position = $Area2D/CollisionShape2D.global_position
	print(area_position)


func body_entered(body):
	if (body.is_in_group("Player")):
		enter_side = get_body_side(body)


func body_exited(body):
	if (body.is_in_group("Player")):
		var exit_side = get_body_side(body)
		if (exit_side == enter_side):
			return
		if shadow_visible:
			tile_map = $TileMap.duplicate()
			$TileMap.queue_free()
		else:
			add_child(tile_map)
		shadow_visible = not shadow_visible


func get_body_side(body):
	var collision_shape = body.get_node("CollisionShape2D")
	var x = body.position.x + collision_shape.position.x
	if x < area_position.x:
		return Direction.left
	else:
		return Direction.right
