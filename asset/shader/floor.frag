#version 330 core

out vec4 color;
in vec2 ftexcoord;

uniform sampler2DArray tex;

void main()
{
	color = texture(tex, vec3(ftexcoord.st, 0.0));
}
