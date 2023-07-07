shader_type canvas_item;
uniform float time : hint_range(0.0, 1.0) = 0.0;

void fragment() {
	// Place fragment code here.
	vec4 color = texture(TEXTURE, UV);
	vec4 white = vec4(1.0, 1.0, 1.0, color.a);
	color = mix(color, white, time);
	COLOR = color;
}
