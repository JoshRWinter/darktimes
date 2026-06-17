#version 460 core

layout (location = 0) out vec4 frag;

uniform sampler2D tex;

in vec2 ftexcoord;

float dither(float f)
{
	float gamma = f <= 0.0031308 ? 12.92 * f : ((1.055 * pow(f, 1.0 / 2.4)) - 0.055);
	float igamma = gamma * 255;

	int lesser = int(floor(igamma));
	int greater = int(ceil(igamma));
	int bias = int(round((igamma - lesser) * 10));

	int chance = int(gl_FragCoord.x * gl_FragCoord.y);

	bool pick_greater_color;
	bool flip = false;

	if (bias > 5)
	{
		flip = true;
		bias = 10 - bias;
	}

	if (bias == 0)
	pick_greater_color = false;
	else if (bias == 1)
	pick_greater_color = chance % 10 == 0; // 1 in 10 chance
	else if (bias == 2)
	pick_greater_color = chance % 5 == 0; // 1 in 5 chance
	else if (bias == 3)
	pick_greater_color = chance % 10 == 0 || chance % 10 == 3 || chance % 10 == 7; // 3 in 10 chance
	else if (bias == 4)
	pick_greater_color = chance % 5 == 0 || chance % 5 == 2; // 2 in 5 chance
	else if (bias == 5)
	pick_greater_color = chance % 2 == 0; // 1 in 2 chance

	if (flip)
	pick_greater_color = !pick_greater_color;

	int color = pick_greater_color ? greater : lesser;

	return color / 255.0;
}

vec4 dither(vec4 color)
{
	return vec4(dither(color.r), dither(color.g), dither(color.b), color.a);
}

void main()
{
	frag = dither(texture(tex, ftexcoord));
}
