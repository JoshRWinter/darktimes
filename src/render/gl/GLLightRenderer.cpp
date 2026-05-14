#include <glm/gtc/type_ptr.hpp>

#include "GLConstants.hpp"
#include "GLLightRenderer.hpp"

using namespace win::gl;

GLLightRenderer::GLLightRenderer(win::AssetRoll &roll)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.shadowmap.get());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::shadowmap_ssbo_index, shadower.shadowmap.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * shadowmap_size, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.occluders.get());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::occluders_ssbo_index, shadower.occluders.get());

    {
        shadower.program = win::GLProgram(win::gl_load_compute_shader(roll["shader/shadower.comp"]));
        glUseProgram(shadower.program.get());

        const auto uniform_shadow_map_size = get_uniform(shadower.program, "shadow_map_size");
        shadower.uniform_light = get_uniform(shadower.program, "light");
        shadower.uniform_occluder_count = get_uniform(shadower.program, "occluder_count");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);

        {
            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "Occluders");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo occluders");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::occluders_ssbo_index);
        }

        {
            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "Shadowmap");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo shadowmap");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::shadowmap_ssbo_index);
        }
    }

    {
        lighter.program = win::GLProgram(win::gl_load_shaders(roll["shader/light.vert"], roll["shader/light.frag"]));
        glUseProgram(lighter.program.get());

        const auto uniform_shadow_map_size = get_uniform(lighter.program, "shadow_map_size");
        lighter.uniform_light = get_uniform(lighter.program, "light");
        lighter.uniform_transform = get_uniform(lighter.program, "transform");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);

        const auto location = glGetProgramResourceIndex(lighter.program.get(), GL_SHADER_STORAGE_BLOCK, "Shadowmap");
        if (location == GL_INVALID_INDEX)
            win::bug("No ssbo shadowmap");

        glShaderStorageBlockBinding(lighter.program.get(), location, GLConstants::shadowmap_ssbo_index);
    }

    check_error();
}

void GLLightRenderer::set_view_projection(const glm::mat4 &view_projection)
{
    this->view_projection = view_projection;
    glUseProgram(lighter.program.get());

    const glm::mat4 ident = glm::identity<glm::mat4>();
    const auto scale = glm::scale(ident, glm::vec3(2.0f / res.width, 2.0 / res.height, 1.0f));
    const auto translate = glm::translate(ident, glm::vec3(-1.0f, -1.0f, 0.0f));

    const auto screen_to_clip = translate * scale;
    const auto clip_to_world = glm::inverse(view_projection);

    glUniformMatrix4fv(lighter.uniform_transform, 1, GL_FALSE, glm::value_ptr(clip_to_world * screen_to_clip));
}

void GLLightRenderer::resize(const win::Dimensions<int> &res)
{
    this->res = res;
}

void GLLightRenderer::render(const std::vector<LightOccluder> &occluders, float x, float y)
{
    glUseProgram(shadower.program.get());
    glUniform2f(shadower.uniform_light, x, y);
    glUniform1i(shadower.uniform_occluder_count, occluders.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.occluders.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * occluders.size(), occluders.data(), GL_DYNAMIC_DRAW);

    glDispatchCompute(shadowmap_size, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindVertexArray(lighter.vao.get());

    glUseProgram(lighter.program.get());
    glUniform2f(lighter.uniform_light, x, y);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    check_error();
}
