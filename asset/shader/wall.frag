#version 330 core

out vec4 color;
in vec2 ftexcoord;

uniform sampler2D tex;

void main()
{
	color = texture(tex, ftexcoord);
}
