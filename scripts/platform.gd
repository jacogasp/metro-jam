
extends StaticBody2D

@export var amplitude = 1.0
@export var frequency = 1.0
var time = 0.0
var center = Vector2.ZERO
var active = true

# Called when the node enters the scene tree for the first time.
func _ready():
	center = position


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not active:
		return
	
	time += delta
	var y = amplitude * sin(frequency * time)
	position.y -= y * delta

func enable():
	self.active = true


func disable():
	time = 0
	self.active = false
