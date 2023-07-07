shader_type canvas_item;
uniform vec2 direction1 = vec2(0.005, 0.005);
uniform vec2 direction2 = vec2(0.005, 0.005);
uniform sampler2D noise1 : repeat_enable;
uniform sampler2D noise2 : repeat_enable;
uniform sampler2D colorTexture;
uniform float pixel_density = 64.0;
uniform float vignette_strenght : hint_range(1.0, 100.0) = 20.0;
uniform float cutoff : hint_range(0.0, 1.0) = 0.2;


float vignette(vec2 uv) {
	uv *= 1.0 - uv.xy;
	return uv.x  * cutoff * vignette_strenght;
} 

void fragment() {
	vec2 uv = 0.5 * floor(UV * pixel_density) / pixel_density;
	float energy1 = texture(noise1, uv + direction1 * TIME).r;
	float energy2 = texture(noise2, uv + direction2 * TIME).r;
	float energy = energy1 * energy2 - (1.0 - UV.y) + 0.2;
	vec4 color = texture(colorTexture, vec2(energy, energy));
	//color *= 2.0;
	color *= vignette(UV);
	COLOR = color;
}
