extends Node2D


func body_entered(body: Node2D):
	if not body.is_in_group("Player"):
		return
		
	if body.call("is_life_full"):
		return
	pick_me(body)

func pick_me(body):
	body.call("add_life")
	$Audio/BottlePop.play()

func destroy():
	queue_free()
