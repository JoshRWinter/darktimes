#version 330 core

uniform mat4 view_projection;

layout (location = 0) in vec2 vert;
layout (location = 1) in ivec2 texcoord;

out vec3 ftexcoord;

uniform vec2 position;
uniform vec2 size;
uniform float layer;
uniform vec2 tc_scale;

void main()
{
	ftexcoord = vec3((texcoord.s / 65535.0) * tc_scale.s , (texcoord.t / 65535.0) * tc_scale.t, layer);

    mat4 trans = mat4(size.x, 0.0, 0.0, 0.0, 0.0, size.y, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, position.x, position.y, 0.0, 1.0);
	gl_Position = view_projection * trans * vec4(vert.xy, 0.0, 1.0);
}
