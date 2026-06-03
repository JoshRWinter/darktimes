#version 460 core

uniform sampler2D tex;
uniform bool horizontal;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

const float[] weights = float[](0.108802, 0.104830, 0.093763, 0.077853, 0.060009, 0.042939, 0.028522, 0.017588, 0.010068, 0.005350, 0.002639, 0.001209);

void main()
{
	vec2 size = textureSize(tex, 0);
	frag = texture(tex, ftexcoord) * weights[0];
	vec2 mask = horizontal ? vec2(1.0, 0.0) : vec2(0.0, 1.0);

	for (int i = 1; i < weights.length(); ++i)
	{
		vec2 shift = vec2(1.0 / size.x * i, 1.0 / size.y * i) * mask;
		frag += texture(tex, ftexcoord + shift) * weights[i];
		frag += texture(tex, ftexcoord - shift) * weights[i];
	}
}
