extends CharacterBody2D

const GRAVITY = 500
const JUMP = 300
var speed = 200.0
var state

enum State {
	STANDING,
	WALKING,
	JUMPING,
	AIR_JUMPING
}

# Called when the node enters the scene tree for the first time.
func _ready():
	state = State.STANDING
	$AnimatedSprite2D.stop()
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta):
	velocity.y += GRAVITY * delta
	
	match state:
		State.STANDING:
			if Input.is_action_just_pressed("jump"):
				velocity.y -= JUMP
				state = State.JUMPING
		State.JUMPING:
			if Input.is_action_just_pressed("jump"):
				velocity.y -= JUMP
				state = State.AIR_JUMPING
	
	if velocity.y == 0:
		state = State.STANDING
	
	if Input.is_action_pressed("move_left"):
		velocity.x = -speed
		
	elif Input.is_action_pressed("move_right"):
		velocity.x = speed
	else:
		velocity.x = 0
		
	if absf(velocity.x) > 0:
		$AnimatedSprite2D.animation = "Run"
		$AnimatedSprite2D.play()
		$AnimatedSprite2D.flip_h = velocity.x < 0
	else:
		$AnimatedSprite2D.stop()
	move_and_slide()
	
	if velocity.y == 0:
		state = State.STANDING

