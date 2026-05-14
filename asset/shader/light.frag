#version 460 core

layout (location = 0) out vec4 frag;

layout(std430) buffer Shadowmap
{
	float shadowmap[];
};

uniform int shadow_map_size;
uniform vec2 light;
uniform mat4 transform;

const float pi = 3.1415926;
const float pi2 = pi * 2.0;

void main()
{
	vec2 coord = (transform * vec4(gl_FragCoord.xy, 0.0, 1.0)).xy;
	float angle = atan(light.y - coord.y, light.x - coord.x) + pi;

	int index = int(round(angle / pi2 * shadow_map_size)) % shadow_map_size;

	float distance = distance(light, coord);
	frag = distance > shadowmap[index] ? vec4(0.0, 0.0, 0.0, 1.0) : vec4(0.0, 0.0, 0.0, 0.0);
}
