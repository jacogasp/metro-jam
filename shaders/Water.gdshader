shader_type canvas_item;
uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_linear_mipmap;
uniform sampler2D noise1 : repeat_enable;
uniform sampler2D noise2 : repeat_enable;
uniform vec2 scroll_direction1 = vec2(0.05, 0.05);
uniform vec2 scroll_direction2 = vec2(0.12, -0.03);
uniform float amplitude : hint_range(-1.0, 1.0) = 0.2;
uniform vec4 tone_color : source_color;
uniform vec4 reflection_color : source_color;
uniform float light_start : hint_range(0.0, 1.0) = 0.125;
uniform float light_stop : hint_range(0.0, 1.0) = 0.4;

void fragment() {
	float wave = texture(noise1, UV + scroll_direction1 * TIME).r +
					texture(noise2, UV + scroll_direction2 * TIME).r;
	vec4 screen_color = texture(SCREEN_TEXTURE, SCREEN_UV + amplitude * wave);
	vec4 top_light = smoothstep(light_start, light_stop, wave) * reflection_color;
	COLOR = screen_color * tone_color + top_light;
	COLOR.a = 1.0;
}
