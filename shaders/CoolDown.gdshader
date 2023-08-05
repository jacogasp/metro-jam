shader_type canvas_item;
uniform vec4 mask_color : source_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float amount : hint_range(0.0, 1.0, 0.01) = 1.0f;

void fragment() {
	vec4 color = texture(TEXTURE, UV);
	float t = 1.0 - amount;
	float y = 1.0 - smoothstep(t - 0.05, t + 0.1, UV.y);
	vec4 mask = vec4(mask_color.rgb, color.a) * y;
	color = mix(color, mask, y * 0.5);
	COLOR = color;
}