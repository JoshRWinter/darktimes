#include "GLPostRenderer.hpp"

using namespace win::gl;

GLPostRenderer::GLPostRenderer(win::AssetRoll &roll, GLenum fbtextarget)
    : program(win::gl_load_shaders(roll["shader/post.vert"], roll["shader/post.frag"]))
{
    glUseProgram(program.get());
    const auto loc = get_uniform(program, "tex");
    glUniform1i(loc, (int)fbtextarget - GL_TEXTURE0);

    check_error();
}

void GLPostRenderer::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(program.get());
    glBindVertexArray(vao.get());

    glDrawArrays(GL_TRIANGLES, 0, 3);

    check_error();
}
