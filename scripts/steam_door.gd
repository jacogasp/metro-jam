extends Node2D

@export var closed = true
var collision_layer = 0

func _ready():
	$Lever/Label.hide()
	collision_layer = $Door/StaticBody2D.collision_layer

func show_label(_body):
	$Lever/Label.show()

func hide_label(_body):
	$Lever/Label.hide()

func interact():
	if closed:
		close()
	else:
		open()

func open():
	$Lever/AnimationPlayer.play("Close")
	$Door/Area2D.monitoring = true
	$Door/SteamParticles.emitting = true
	$Door/StaticBody2D.set_physics_process(true)
	$Door/StaticBody2D.collision_layer = collision_layer
	$Door.show()
	closed = true

func close():
	$Lever/AnimationPlayer.play("Open")
	$Door/Area2D.monitoring = false
	$Door/SteamParticles.emitting = false
	$Door/StaticBody2D.set_physics_process(false)
	$Door/StaticBody2D.collision_layer = 0
	$Door.hide()
	closed = false

func on_body_entered(body):
	if (body.is_in_group("Player")):
		body.call("hit"); # Replace with function body.
