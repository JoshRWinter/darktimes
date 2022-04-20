#include <win/gl.hpp>

#include "gl.hpp"

int get_uniform(GLuint program, const char *name)
{
	int location = glGetUniformLocation(program, name);

	if (location == -1)
		win::bug("No uniform " + std::string(name));

	return location;
}
