#version 460 core

layout (location = 0) out vec4 frag;

uniform sampler2D tex;

in vec2 ftexcoord;

void main()
{
	frag = texture(tex, ftexcoord);
}
