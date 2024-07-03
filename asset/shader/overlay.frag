#version 330 core

uniform sampler2D tex;

in vec2 ftexcoord;

out vec4 frag;

void main()
{
    frag = texture2D(tex, ftexcoord);
}
