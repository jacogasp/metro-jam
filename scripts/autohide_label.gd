extends Node2D

func _ready():
	$Label.hide()


func body_entered(_body):
	$Label.show()


func body_exited(_body):
	$Label.hide()
