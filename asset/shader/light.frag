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
};

layout (std430) buffer LightSources
{
	LightSource lights[];
};

uniform int shadow_map_size;
uniform int light_count;
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
		bool is_primary = i == light_count - 1;

		float angle = atan(lights[i].y - coord.y, lights[i].x - coord.x) + pi;
		int index = int(round(angle / pi2 * shadow_map_size)) % shadow_map_size;

		float distance = distance(vec2(lights[i].x, lights[i].y), coord);
		bool lighted = distance <= shadowmap[index + (lights[i].index * shadow_map_size)];
		if (lighted)
		{
			if (is_primary || is_primary_visible)
			{
				light += vec3(lights[i].r, lights[i].g, lights[i].b) * (lights[i].power / max(0.00000f, distance * distance));
				is_primary_visible = true;
			}
		}
	}

	frag = vec4(light, 1.0);
}
