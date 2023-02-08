#pragma once

#include <array>

#include <GL/gl.h>

#include "../../Darktimes.hpp"

int get_uniform(GLuint, const char*);
void get_quad_verts(float, float, float, float, std::array<float, 12>&);
