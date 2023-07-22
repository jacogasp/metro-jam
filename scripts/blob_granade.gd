extends RigidBody2D


var exploding = false;

func _ready():
	$AnimatedSprite2D.stop()

func _process(_delta):
	if exploding and not $AnimatedSprite2D.is_playing():
		queue_free()


func detonate(_node):
	$AnimatedSprite2D.play()
	exploding = true
