#pragma once

#include <win/gl/GL.hpp>

class GLConstants
{
public:
    constexpr static GLenum floor_texture_unit = GL_TEXTURE0;
    constexpr static GLenum atlas_texture_unit = GL_TEXTURE0;
    constexpr static GLenum font_texture_unit = GL_TEXTURE1;
    constexpr static GLenum intermediate_framebuffer_texture_unit = GL_TEXTURE2;
    constexpr static GLenum light_framebuffer_texture_unit = GL_TEXTURE3;
    constexpr static GLenum blur_framebuffer_texture_unit = GL_TEXTURE4;

    constexpr static GLuint occluders_ssbo_index = 0;
    constexpr static GLuint lights_ssbo_index = 1;
    constexpr static GLuint shadowmap_ssbo_index = 2;
};
