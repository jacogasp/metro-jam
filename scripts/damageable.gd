extends Node

@export var health: float = 20

func hit(damage: int):
	health -= damage
	
	if health <= 0:
		get_parent().queue_free()
