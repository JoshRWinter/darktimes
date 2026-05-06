#pragma once

#include <win/AssetRoll.hpp>

#include "GLRendererBase.hpp"

class GLPostRenderer : GLRendererBase
{
public:
    explicit GLPostRenderer(win::AssetRoll &roll, GLenum fbtextarget);

    void render();

private:
    win::GLProgram program;
    win::GLVertexArray vao;
};
