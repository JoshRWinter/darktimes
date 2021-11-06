#version 330 core

out vec4 color;
in vec3 ftexcoord;

uniform sampler2DArray tex;

void main()
{
	color = texture(tex, ftexcoord);
}
