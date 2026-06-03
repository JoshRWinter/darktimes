#version 460 core

float[] verts = float[](-1.0, 3.0, -1.0, -1.0, 3.0, -1.0);
float[] tc = float[](0.0, 2.0, 0.0, 0.0, 2.0, 0.0);

out vec2 ftexcoord;

void main()
{
	ftexcoord = vec2(tc[gl_VertexID * 2 + 0], tc[gl_VertexID * 2 + 1]);
	gl_Position = vec4(verts[gl_VertexID * 2 + 0], verts[gl_VertexID * 2 + 1], 0.0, 1.0);
}
