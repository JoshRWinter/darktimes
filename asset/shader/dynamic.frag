#version 330 core

in vec2 ftexcoords;

out vec4 color;
uniform sampler2D tex;

void main()
{
    color = /*vec4(1.0, 0.0, 0.0, 1.0);//*/texture(tex, ftexcoords);
}
