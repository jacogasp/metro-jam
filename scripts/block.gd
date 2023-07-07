
extends StaticBody2D

@export var amplitude = 0.1
var time = 0
var center = Vector2.ZERO

# Called when the node enters the scene tree for the first time.
func _ready():
	center = position


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	time += delta
	position.y += amplitude * sin(time)
