extends Node2D


func body_entered(body):
	if body.is_in_group("Player"):
		$Area2D.queue_free()
		$Label.queue_free()
		body.call_deferred("pick", self)
