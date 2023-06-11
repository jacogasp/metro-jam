shader_type canvas_item;
uniform float pixel_density = 32;

void fragment() {
	vec2 new_uv = floor(UV * pixel_density) / pixel_density;
	vec4 color = texture(TEXTURE, new_uv);
	COLOR = color;
	
}
