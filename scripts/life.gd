extends Node2D



func body_entered(body: Node2D):
	if body.is_in_group("Player"):
		if body.call("is_life_full"):
			return
		body.call("add_life")
		queue_free()
