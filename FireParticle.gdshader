shader_type canvas_item;
uniform sampler2D noise1 : repeat_enable;
uniform sampler2D noise2 : repeat_enable;
uniform sampler2D color_texture;
uniform vec2 direction1 = vec2(0.05, 0.05);
uniform vec2 direction2 = vec2(-0.01, 0.02);

void fragment() {
	// Place fragment code here.
	vec2 uv = UV;
	vec4 noise_color1 = texture(noise1, UV + direction1 * TIME);
	vec4 noise_color2 = texture(noise2, UV + direction2 * TIME);
	float energy = noise_color1.r * noise_color2.r;
	vec4 color = texture(color_texture, vec2(energy, energy));
	vec4 tex = texture(TEXTURE, vec2(energy, energy));
	COLOR = color;
}
