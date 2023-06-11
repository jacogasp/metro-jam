extends GPUParticles2D


func _on_start_timer_timeout():
	$StopTimer.start()
	emitting = true


func _on_stop_timer_timeout():
	$StartTimer.start()
	emitting = false
