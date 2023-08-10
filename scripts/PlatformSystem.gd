
extends Node2D

@export var enabled = false
@export var one_shot = false
var path = "user://savings/"

var shutdown = false

func _ready():
	update_platforms_state()
	$Handle/Label.hide()
	path += get_path().get_concatenated_names().replace("/", "-")
	load_state()

func enable():
	enabled = true
	$Handle/AnimationPlayer.play("Enable")
	update_platforms_state()
	if one_shot:
		$Handle/Area2D.monitoring = false
		shutdown = true
		hide_label(null)

func disable():
	enabled = false
	$Handle/AnimationPlayer.play("Disable")
	update_platforms_state()

func update_platforms_state():
	for child in $Platforms.get_children():
		child.set_process(enabled)

func show_label(_body):
	$Handle/Label.show()

func hide_label(_body):
	$Handle/Label.hide()

func interact():
	if shutdown:
		return
	if enabled:
		disable()
	else:
		enable()
	save_state()

func on_body_entered(body):
	if (body.is_in_group("Player")):
		body.call("hit"); # Replace with function body.

func save_state():
	var state = {
		"enabled": enabled,
		"one_shot": one_shot,
		"shutdown": shutdown
	}
	var json = JSON.stringify(state)
	var file = FileAccess.open(path, FileAccess.WRITE)
	file.store_line(json)
	print("PlatformSystem state saved to ", path)

func load_state():
	if not FileAccess.file_exists(path):
		print("PlatformSystem save file not found")
		return false
	
	var file = FileAccess.open(path, FileAccess.READ)
	var buffer = file.get_as_text()
	var json = JSON.parse_string(buffer)
	enabled = json["enabled"]
	one_shot = json["one_shot"]
	shutdown = json["shutdown"]
	if enabled:
		enable()
	else:
		disable()
	print("PlatformSystem state loaded from ", path)
	return true
