#version 330 core

uniform mat4 projection;

uniform float rotation;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;

out vec2 ftexcoord;

void main()
{
	mat4 rotate = mat4(cos(rotation), sin(rotation), 0.0, 0.0, -sin(rotation), cos(rotation), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	ftexcoord = texcoord;
	gl_Position = projection * rotate * vec4(pos.xy, 0.0, 1.0);
}
