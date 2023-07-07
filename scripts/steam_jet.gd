extends GPUParticles2D


func _on_start_timer_timeout():
	$Area2D/CollisionShape2D.disabled = false;
	$StopTimer.start()
	emitting = true
	

func _on_stop_timer_timeout():
	$Area2D/CollisionShape2D.disabled = true;
	$StartTimer.start()
	emitting = false


func _on_area_2d_body_entered(body):
	if (body.is_in_group("Player")):
		body.call("hit");
