#version 330 core

out vec4 color;
in vec3 ftexcoord;

uniform sampler2DArray tex;

void main()
{
	vec4 pix = texture(tex, ftexcoord);
	float avg = (pix.r + pix.g + pix.b) / 14.0;
	color = vec4(avg, avg, avg, pix.a);
}
