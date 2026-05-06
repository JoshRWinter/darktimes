#pragma once

#include <win/gl/GL.hpp>

class GLConstants
{
public:
    constexpr static GLenum floor_texture_unit = GL_TEXTURE0;
    constexpr static GLenum atlas_texture_unit = GL_TEXTURE0;
    constexpr static GLenum font_texture_unit = GL_TEXTURE1;
    constexpr static GLenum intermediate_framebuffer_texture_unit = GL_TEXTURE2;
};
