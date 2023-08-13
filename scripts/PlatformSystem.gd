
extends Node2D

@export var enabled = false
@export var one_shot = false
var path = "user://savings/"

var shutdown = false
var label_text = ""

func _ready():
	update_platforms_state()
	$Handle/Label.hide()
	path += get_path().get_concatenated_names().replace("/", "-")
	var main_scene = get_node("/root/Main")
	main_scene.connect("using_joypad_changed", using_joypad_changed)
	main_scene.connect("save", save_state)
	label_text = $Handle/Label.text
	using_joypad_changed(false)
	load_state()
	
func get_event_button(using_joypad: bool):
	var events = InputMap.action_get_events("interact")
	for event in events:
		var event_name = event.as_text()
		if not using_joypad:
			return event_name.get_slice("(", 0);
		if event_name.begins_with("Joypad"):
			var start = event_name.find("Xbox ")
			return event_name[start]

func using_joypad_changed(using_joypad):
	var button_name = get_event_button(using_joypad)
	var text = label_text
	text = text.replace("$BUTTON", button_name)
	$Handle/Label.text = text

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
	get_interaction_key()

func get_interaction_key():
	var events = InputMap.action_get_events("interact")
	for event in events:
		print(event.as_text())

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
