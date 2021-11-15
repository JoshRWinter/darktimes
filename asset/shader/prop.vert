#version 330 core

uniform mat4 projection;
layout (location = 0) in vec2 pos;

void main()
{
	gl_Position = projection * vec4(pos.x, pos.y, 0.0, 1.0);
}
