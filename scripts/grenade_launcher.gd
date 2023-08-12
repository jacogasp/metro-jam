extends Node2D

var shader : ShaderMaterial = null

func _ready():
	shader = $TextureRect.material

func _on_gun_cooling_down(amount: float):
	shader.set_shader_parameter("amount", amount)


func pick_me(body):
	if body.is_in_group("Player"):
		$Label.queue_free()
		$Area2D.queue_free()
		body.call_deferred("pick", self)
