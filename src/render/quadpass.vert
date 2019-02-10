#version 330 core

uniform mat4 projection;

layout (location = 0) in vec2 triangle;
layout (location = 1) in vec2 position;
layout (location = 2) in vec2 size;
layout (location = 3) in float rot;

void main()
{
	mat4 translate = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, position.x + (size.x / 2.0), position.y + (size.y / 2.0), 0.0, 1.0);
	mat4 rotate = mat4(cos(rot), sin(rot), 0.0, 0.0, -sin(rot), cos(rot), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	gl_Position = projection * translate * rotate * vec4(triangle.x * size.x, triangle.y * size.y, 0.0, 1.0);
}
