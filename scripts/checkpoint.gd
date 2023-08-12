extends Node2D


func _ready():
	$Label.hide()

func interact():
	var main_scene = get_node("/root/Main")
	main_scene.call("save")


func _on_area_2d_body_entered(_body):
	$Label.show()


func _on_area_2d_body_exited(_body):
	$Label.hide()
