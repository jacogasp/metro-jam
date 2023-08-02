
extends Node2D

@export var enabled = false
@export var one_shot = false

var shutdown = false

func _ready():
	update_platforms_state()
	$Handle/Label.hide()

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

func on_body_entered(body):
	if (body.is_in_group("Player")):
		body.call("hit"); # Replace with function body.
