#include <win/GL.hpp>

#include "GL.hpp"

int get_uniform(GLuint program, const char *name)
{
	int location = glGetUniformLocation(program, name);

	if (location == -1)
		win::bug("No uniform " + std::string(name));

	return location;
}

void get_quad_verts(float x, float y, float w, float h, std::array<float, 12> &verts)
{
	verts[0] = x; verts[1] = y + h;
	verts[2] = x; verts[3] = y;
	verts[4] = x + w; verts[5] = y;

	verts[6] = x; verts[7] = y + h;
	verts[8] = x + w; verts[9] = y;
	verts[10] = x + w; verts[11] = y + h;
}
