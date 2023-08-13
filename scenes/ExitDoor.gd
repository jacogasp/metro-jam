extends Node2D

var label_text = ""
var locked = true

func _ready():
	$Label.hide()
	label_text = $Label.text
	var main_scene = get_node("/root/Main")
	main_scene.connect("using_joypad_changed", using_joypad_changed)
	using_joypad_changed(false)

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
	$Label.text = text

func body_entered(body):
	if body.has_node("Superpowers/ExitKey"):
		$Label.show() # Replace with function body.
		locked = false

func body_exited(_body):
	$Label.hide() # Replace with function body.
	locked = true

func interact():
	if locked:
		return
	
	var main_scene = get_node("/root/Main")
	main_scene.call_deferred("complete_game")
