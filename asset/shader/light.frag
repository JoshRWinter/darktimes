#version 460 core

layout (location = 0) out vec4 frag;

layout(std430) buffer Shadowmap
{
	float shadowmap[];
};

struct LightSource
{
	int index;
	float x;
	float y;
	float power;
	float r, g, b;
	float angle;
};

layout (std430) buffer LightSources
{
	LightSource lights[];
};

uniform int shadow_map_size;
uniform int light_start;
uniform int light_count;
uniform int light_buffer_size;
uniform mat4 transform;

const float pi = 3.1415926;
const float pi2 = pi * 2.0;

void main()
{
	vec2 coord = (transform * vec4(gl_FragCoord.xy, 0.0, 1.0)).xy;

	vec3 light = vec3(0.0, 0.0, 0.0);
	bool is_primary_visible = false;

	for (int i = light_count - 1; i >= 0; --i)
	{
		int light_index = (light_start + i) % light_buffer_size;
		bool is_primary = i == light_count - 1;

		float angle = atan(lights[light_index].y - coord.y, lights[light_index].x - coord.x) + pi;
		int shadow_index = int(round(angle / pi2 * shadow_map_size)) % shadow_map_size;

		float distance = distance(vec2(lights[light_index].x, lights[light_index].y), coord);
		bool lighted = distance <= shadowmap[shadow_index + (lights[light_index].index * shadow_map_size)];
		if (lighted)
		{
			if (is_primary || is_primary_visible)
			{
				float dproduct = dot(vec2(cos(lights[light_index].angle), sin(lights[light_index].angle)), vec2(cos(angle), sin(angle)));
				bool in_angle = lights[light_index].angle == -1.0 || dproduct > 0.88;

				light += vec3(lights[light_index].r, lights[light_index].g, lights[light_index].b) * ((lights[light_index].power * (in_angle ? 1.0 : 0.1)) / max(in_angle ? 0.000001 : 5, distance * distance));
				is_primary_visible = true;
			}
		}
	}

	frag = vec4(light, 1.0);
}
