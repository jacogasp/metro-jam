extends Node2D

var shader : ShaderMaterial = null
var label_text = ""
var main_scene = null

func _ready():
	shader = $TextureRect.material
	label_text = $Label.text
	main_scene = get_node("/root/Main")
	main_scene.connect("using_joypad_changed", using_joypad_changed)
	using_joypad_changed(false)

func get_event_button(using_joypad: bool):
	var events = InputMap.action_get_events("grenade")
	for event in events:
		var event_name = event.as_text()
		if not using_joypad:
			return event_name.get_slice("(", 0);
		if event_name.begins_with("Joypad"):
			var start = event_name.find("Xbox ") + 5
			var s = event_name[start]
			if s in "LR":
				s += event_name[start + 1]
			return s
			

func using_joypad_changed(using_joypad):
	var button_name = get_event_button(using_joypad)
	var text = label_text
	text = text.replace("$BUTTON", button_name)
	$Label.text = text


func _on_gun_cooling_down(amount: float):
	shader.set_shader_parameter("amount", amount)


func pick_me(body):
	if body.is_in_group("Player"):
		main_scene.disconnect("using_joypad_changed", using_joypad_changed)
		$Label.queue_free()
		$Area2D.queue_free()
		body.call_deferred("pick", self)
