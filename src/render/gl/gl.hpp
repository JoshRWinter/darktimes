#ifndef GL_HPP
#define GL_HPP

#include <array>

#include <GL/gl.h>

#include "../../darktimes.hpp"

int get_uniform(GLuint, const char*);
void get_quad_verts(float, float, float, float, std::array<float, 12>&);

#endif
