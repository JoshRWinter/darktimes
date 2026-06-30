#version 460 core

uniform sampler2D tex;
uniform bool horizontal;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

const float[] weights = float[](0.5, 0.1, 0.08, 0.04, 0.02, 0.01);

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
