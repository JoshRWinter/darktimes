#pragma once

#include <win/gl/GL.hpp>

#include "../../Darktimes.hpp"

class GLSubRenderer
{
public:
	virtual void flush() = 0;

protected:
	~GLSubRenderer() = default;

	static void check_error()
	{
#ifndef NDEBUG
		win::gl_check_error();
#endif
	}

	static int get_uniform(const win::GLProgram &program, const char *name)
	{
		auto loc = win::gl::glGetUniformLocation(program.get(), name);
		if (loc == -1)
			win::bug("GLDynamicLightRenderer: no uniform " + std::string(name));

		return loc;
	}
};
