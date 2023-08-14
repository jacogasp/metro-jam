extends Node2D

@export var closed = true
@export var one_shot = false

var collision_layer = 0
var shutdown = false
var path = "user://savings/"
var label_text = ""

func _ready():
	$Valve/Label.hide()
	label_text = $Valve/Label.text
	collision_layer = $Door/StaticBody2D.collision_layer
	path += get_path().get_concatenated_names().replace("/", "-")
	var main_scene = get_node("/root/Main")
	main_scene.connect("using_joypad_changed", using_joypad_changed)
	main_scene.connect("save", save_state)
	using_joypad_changed(false)
	if not load_state():
		if closed:
			close()
		else:
			open()


func get_event_button(using_joypad: bool):
	var events = InputMap.action_get_events("interact")
	for event in events:
		var event_name = event.as_text()
		if not using_joypad:
			return event_name.get_slice("(", 0);
		if event_name.begins_with("Joypad"):
			var start = event_name.find("Xbox ")
			return event_name[start + 5]
		

func show_label(_body):
	$Valve/Label.show()

func hide_label(_body):
	$Valve/Label.hide()

func interact():
	if shutdown:
		return
	if closed:
		open()
	elif not closed:
		close()
	save_state()

func open():
	$Valve/AnimationPlayer.play("Open")
	$Door/Area2D.monitoring = false
	$Door/SteamParticles.emitting = false
	$Door/StaticBody2D.set_physics_process(false)
	$Door/StaticBody2D.collision_layer = 0
	$Door.hide()
	closed = false
	if one_shot:
		$Valve/Area2D.monitoring = false
		hide_label(null)
		shutdown = true

func close():
	$Valve/AnimationPlayer.play("Close")
	$Door/Area2D.monitoring = true
	$Door/SteamParticles.emitting = true
	$Door/StaticBody2D.set_physics_process(true)
	$Door/StaticBody2D.collision_layer = collision_layer
	$Door.show()
	closed = true

func on_body_entered(body):
	if (body.is_in_group("Player")):
		body.call("hit"); # Replace with function body.

func save_state():
	var state = {
		"closed": closed,
		"one_shot": one_shot,
		"shutdown": shutdown
	}
	var json = JSON.stringify(state)
	var file = FileAccess.open(path, FileAccess.WRITE)
	file.store_line(json)
	print("SteamDoor state saved to ", path)

func load_state():
	if not FileAccess.file_exists(path):
		print("SteamDoor save file not found")
		return false
	
	var file = FileAccess.open(path, FileAccess.READ)
	var buffer = file.get_as_text()
	var json = JSON.parse_string(buffer)
	closed = json["closed"]
	one_shot = json["one_shot"]
	shutdown = json["shutdown"]
	if closed:
		close()
	else:
		open()
	print("SteamDoor state loaded from ", path)
	return true

func using_joypad_changed(using_joypad):
	var button_name = get_event_button(using_joypad)
	var text = label_text
	text = text.replace("$BUTTON", button_name)
	$Valve/Label.text = text
