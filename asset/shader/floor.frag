#version 330 core

out vec4 color;
in vec3 ftexcoord;

uniform sampler2DArray tex;

void main()
{
	vec4 pix = texture(tex, ftexcoord);
	color = vec4(pix.rgb, pix.a);
}
