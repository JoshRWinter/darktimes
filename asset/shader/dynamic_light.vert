#version 330 core

layout (location = 0) in vec2 pos;

uniform mat4 mvp;

const float verts[] = float[]
(
    -0.5, 0.5,
    -0.5, -0.5,
    0.5, 0.5,
    0.5, -0.5
);

void main()
{
    vec2 vert = vec2(verts[gl_VertexID * 2], verts[(gl_VertexID * 2) + 1]);
    gl_Position = mvp * vec4(vert.xy, 0.0, 1.0);
}
