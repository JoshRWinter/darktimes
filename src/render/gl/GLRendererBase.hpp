#pragma once

#include <win/gl/GL.hpp>

class GLRendererBase
{
protected:
    GLRendererBase() = default;

    static GLint get_uniform(win::GLProgram &program, const char *name)
    {
        const auto loc = win::gl::glGetUniformLocation(program.get(), name);
        if (loc == -1)
            win::bug("No uniform " + std::string(name));

        return loc;
    }

    static void check_error()
    {
#ifndef NDEBUG
        win::gl_check_error();
#endif
    }
};
