#include <cmath>
#include <glm/gtc/type_ptr.hpp>

#include "GLConstants.hpp"
#include "GLLightRenderer.hpp"

using namespace win::gl;

GLLightRenderer::GLLightRenderer(win::AssetRoll &roll)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::shadowmap_ssbo_index, shadower.shadowmap.get());

    {
        shadower.program = win::GLProgram(win::gl_load_compute_shader(roll["shader/shadower.comp"]));
        glUseProgram(shadower.program.get());

        const auto uniform_shadow_map_size = get_uniform(shadower.program, "shadow_map_size");
        shadower.uniform_light_count = get_uniform(shadower.program, "light_count");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);

        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::occluders_ssbo_index, shadower.occluders.get());
            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "Occluders");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo Occluders");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::occluders_ssbo_index);
        }

        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::shadower_lights_ssbo_index, shadower.lights.get());
            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "LightSources");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo LightSources");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::shadower_lights_ssbo_index);
        }

        {
            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "Shadowmap");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo Shadowmap");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::shadowmap_ssbo_index);
        }
    }

    {
        glBindFramebuffer(GL_FRAMEBUFFER, lighter.fbo.get());
        glActiveTexture(GLConstants::light_framebuffer_texture_unit);
        glBindTexture(GL_TEXTURE_2D, lighter.tex.get());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lighter.tex.get(), 0);

        {
            const GLenum bufs[] { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, bufs);
        }

        lighter.program = win::GLProgram(win::gl_load_shaders(roll["shader/light.vert"], roll["shader/light.frag"]));
        glUseProgram(lighter.program.get());

        const auto uniform_shadow_map_size = get_uniform(lighter.program, "shadow_map_size");
        lighter.uniform_light_count = get_uniform(lighter.program, "light_count");
        lighter.uniform_transform = get_uniform(lighter.program, "transform");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);

        {
            const auto location = glGetProgramResourceIndex(lighter.program.get(), GL_SHADER_STORAGE_BLOCK, "Shadowmap");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo Shadowmap");

            glShaderStorageBlockBinding(lighter.program.get(), location, GLConstants::shadowmap_ssbo_index);
        }

        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::lighter_lights_ssbo_index, lighter.lights.get());
            const auto location = glGetProgramResourceIndex(lighter.program.get(), GL_SHADER_STORAGE_BLOCK, "LightSources");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo LightSources");

            glShaderStorageBlockBinding(lighter.program.get(), location, GLConstants::lighter_lights_ssbo_index);
        }
    }

    {
        glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
        glActiveTexture(GLConstants::blur_framebuffer_texture_unit);
        glBindTexture(GL_TEXTURE_2D, blur.tex.get());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur.tex.get(), 0);

        {
            const GLenum bufs[] { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, bufs);
        }

        blur.program = win::GLProgram(win::gl_load_shaders(roll["shader/blur.vert"], roll["shader/blur.frag"]));
        glUseProgram(blur.program.get());
        blur.uniform_tex = get_uniform(blur.program, "tex");
        blur.uniform_horizontal = get_uniform(blur.program, "horizontal");
    }

    check_error();
}

void GLLightRenderer::set_view_projection(const glm::mat4 &view_projection)
{
    this->view_projection = view_projection;
    glUseProgram(lighter.program.get());

    const auto ident = glm::identity<glm::mat4>();
    const auto scale = glm::scale(ident, glm::vec3(2.0f / res.width, 2.0 / res.height, 1.0f));
    const auto translate = glm::translate(ident, glm::vec3(-1.0f, -1.0f, 0.0f));

    const auto screen_to_clip = translate * scale;
    const auto clip_to_world = glm::inverse(view_projection);

    glUniformMatrix4fv(lighter.uniform_transform, 1, GL_FALSE, glm::value_ptr(clip_to_world * screen_to_clip));
}

void GLLightRenderer::resize(const win::Dimensions<int> &res)
{
    this->res = res;

    glActiveTexture(GLConstants::light_framebuffer_texture_unit);
    glBindTexture(GL_TEXTURE_2D, lighter.tex.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, res.width, res.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    glActiveTexture(GLConstants::blur_framebuffer_texture_unit);
    glBindTexture(GL_TEXTURE_2D, blur.tex.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, res.width, res.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
}

void GLLightRenderer::load(const std::vector<LightOccluder> &occluders, const std::vector<LightRenderable> &static_lights)
{
    this->occluders = occluders;
    occindex.reset(1.0f,
                   this->occluders,
                   [](const LightOccluder &x)
                   {
                       return win::SpatialIndexLocation(x.x1, x.y1, x.x2 - x.x1, x.y2 - x.y1);
                   });

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.occluders.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * occluders.size() * 4, occluders.data(), GL_DYNAMIC_DRAW);

    {
        lights.clear();
        int index = 0;
        for (const auto &light : static_lights)
        {
            lights.emplace_back(index, light.x, light.y, light.power, light.color.red, light.color.green, light.color.blue);
            ++index;
        }
    }

    glUseProgram(shadower.program.get());
    glUniform1i(shadower.uniform_light_count, lights.size());

    std::vector<unsigned char> v(lights.size() * 4 * 5);
    {
        const int start = 0;
        const int count = occluders.size();

        int offset = 0;
        for (const auto &light : lights)
        {
            memcpy(v.data() + offset + 0, &light.index, 4);
            memcpy(v.data() + offset + 4, &light.x, 4);
            memcpy(v.data() + offset + 8, &light.y, 4);
            memcpy(v.data() + offset + 12, &start, 4);
            memcpy(v.data() + offset + 16, &count, 4);

            offset += 5 * 4;
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.lights.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, v.size(), v.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.shadowmap.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, shadowmap_size * sizeof(float) * (lights.size() + max_dynamic_lights), NULL, GL_STATIC_DRAW);

    glDispatchCompute(std::ceil((shadowmap_size * lights.size()) / 32.0f), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    check_error();
}

void GLLightRenderer::render(const std::vector<LightRenderable> &dynamic_lights, GLuint fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, lighter.fbo.get());

    // compute shadowmap data for dynamic lights
    {
        std::vector<unsigned char> lightdata((lights.size() + dynamic_lights.size()) * 4 * 5);
        std::vector<float> occdata(occluders.size() * 4);
        occdata.clear();
        {
            int occstart = 0;
            int occlen = 0;
            int offset = 0;
            int index = lights.size();
            for (const auto &light : dynamic_lights)
            {
                const float cover_radius = 10.0f;
                win::SpatialIndexLocation loc(light.x - cover_radius, light.y - cover_radius, cover_radius * 2.0f, cover_radius * 2.0f);
                for (const auto &occ : occindex.query(loc))
                {
                    occdata.push_back(occ.x1);
                    occdata.push_back(occ.y1);
                    occdata.push_back(occ.x2);
                    occdata.push_back(occ.y2);
                    ++occlen;
                }

                memcpy(lightdata.data() + offset + 0, &index, 4);
                memcpy(lightdata.data() + offset + 4, &light.x, 4);
                memcpy(lightdata.data() + offset + 8, &light.y, 4);
                memcpy(lightdata.data() + offset + 12, &occstart, 4);
                memcpy(lightdata.data() + offset + 16, &occlen, 4);

                occstart += occlen;
                occlen = 0;
                offset += 5 * 4;
                ++index;
            }
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.lights.get());
        glBufferData(GL_SHADER_STORAGE_BUFFER, lightdata.size(), lightdata.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.occluders.get());
        glBufferData(GL_SHADER_STORAGE_BUFFER, occdata.size() * sizeof(float), occdata.data(), GL_DYNAMIC_DRAW);

        glUseProgram(shadower.program.get());
        glUniform1i(shadower.uniform_light_count, dynamic_lights.size());

        glDispatchCompute(std::ceil((shadowmap_size * dynamic_lights.size()) / 32.0f), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // run the light rendering pass
    {
        std::vector<unsigned char> v((lights.size() + dynamic_lights.size()) * 4 * 7);
        {
            int offset = 0;

            for (const auto &light : lights)
            {
                memcpy(v.data() + offset + 0, &light.index, 4);
                memcpy(v.data() + offset + 4, &light.x, 4);
                memcpy(v.data() + offset + 8, &light.y, 4);
                memcpy(v.data() + offset + 12, &light.power, 4);
                memcpy(v.data() + offset + 16, &light.r, 4);
                memcpy(v.data() + offset + 20, &light.g, 4);
                memcpy(v.data() + offset + 24, &light.b, 4);

                offset += 7 * 4;
            }

            int index = lights.size();
            for (const auto &light : dynamic_lights)
            {
                memcpy(v.data() + offset + 0, &index, 4);
                memcpy(v.data() + offset + 4, &light.x, 4);
                memcpy(v.data() + offset + 8, &light.y, 4);
                memcpy(v.data() + offset + 12, &light.power, 4);
                memcpy(v.data() + offset + 16, &light.color.red, 4);
                memcpy(v.data() + offset + 20, &light.color.green, 4);
                memcpy(v.data() + offset + 24, &light.color.blue, 4);

                offset += 7 * 4;
                ++index;
            }
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lighter.lights.get());
        glBufferData(GL_SHADER_STORAGE_BUFFER, v.size(), v.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(lighter.vao.get());

        glUseProgram(lighter.program.get());
        glUniform1i(lighter.uniform_light_count, lights.size() + dynamic_lights.size());

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // blur the result slightly
    {
        glBindVertexArray(blur.vao.get());

        glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());

        glUseProgram(blur.program.get());

        glUniform1i(blur.uniform_tex, GLConstants::light_framebuffer_texture_unit - GL_TEXTURE0);
        glUniform1i(blur.uniform_horizontal, 1);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glUniform1i(blur.uniform_tex, GLConstants::blur_framebuffer_texture_unit - GL_TEXTURE0);
        glUniform1i(blur.uniform_horizontal, 0);

        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    check_error();
}
