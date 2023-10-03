#version 330 core

in vec3 ftexcoord;
out vec4 color;

uniform sampler2DArray tex;

void main()
{
    color = texture(tex, ftexcoord);
}
