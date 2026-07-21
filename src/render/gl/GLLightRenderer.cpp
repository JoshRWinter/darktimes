#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../../RandomNumberGenerator.hpp"
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
        const auto uniform_light_buffer_size = get_uniform(shadower.program, "light_buffer_size");
        const auto uniform_occluder_buffer_size = get_uniform(shadower.program, "occluder_buffer_size");
        shadower.uniform_light_start = get_uniform(shadower.program, "light_start");
        shadower.uniform_light_count = get_uniform(shadower.program, "light_count");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);
        glUniform1i(uniform_light_buffer_size, shadower_light_buf_count);
        glUniform1i(uniform_occluder_buffer_size, occluder_buf_count);

        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.occluders.get());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::occluders_ssbo_index, shadower.occluders.get());
            glBufferStorage(GL_SHADER_STORAGE_BUFFER,
                            sizeof(LightOccluder) * occluder_buf_count,
                            NULL,
                            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            void *mem = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
                                         0,
                                         sizeof(LightOccluder) * occluder_buf_count,
                                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            if (mem == NULL)
                win::bug("Couldn't map occluder buffer for writing");
            shadower.occbuf = win::GLMappedRingBuffer<LightOccluder>(mem, occluder_buf_count);

            const auto location = glGetProgramResourceIndex(shadower.program.get(), GL_SHADER_STORAGE_BLOCK, "Occluders");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo Occluders");

            glShaderStorageBlockBinding(shadower.program.get(), location, GLConstants::occluders_ssbo_index);
        }

        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.lights.get());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::shadower_lights_ssbo_index, shadower.lights.get());
            glBufferStorage(GL_SHADER_STORAGE_BUFFER,
                            sizeof(ShadowerLight) * shadower_light_buf_count,
                            NULL,
                            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            void *mem = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
                                         0,
                                         sizeof(ShadowerLight) * shadower_light_buf_count,
                                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            if (mem == NULL)
                win::bug("Could'nt map shadower lights for writing");
            shadower.lightbuf = win::GLMappedRingBuffer<ShadowerLight>(mem, shadower_light_buf_count);

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
        lighter.uniform_primary_index = get_uniform(lighter.program, "primary_index");
        lighter.uniform_light_start = get_uniform(lighter.program, "light_start");
        lighter.uniform_light_count = get_uniform(lighter.program, "light_count");
        const auto uniform_light_buffer_size = get_uniform(lighter.program, "light_buffer_size");
        lighter.uniform_transform = get_uniform(lighter.program, "transform");

        glUniform1i(uniform_shadow_map_size, shadowmap_size);
        glUniform1i(uniform_light_buffer_size, lighter_light_buf_count);

        {
            const auto location = glGetProgramResourceIndex(lighter.program.get(), GL_SHADER_STORAGE_BLOCK, "Shadowmap");
            if (location == GL_INVALID_INDEX)
                win::bug("No ssbo Shadowmap");

            glShaderStorageBlockBinding(lighter.program.get(), location, GLConstants::shadowmap_ssbo_index);
        }

        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lighter.lights.get());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLConstants::lighter_lights_ssbo_index, lighter.lights.get());
            glBufferStorage(GL_SHADER_STORAGE_BUFFER,
                            sizeof(LighterLight) * lighter_light_buf_count,
                            NULL,
                            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            void *mem = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
                                         0,
                                         sizeof(LighterLight) * lighter_light_buf_count,
                                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            if (mem == NULL)
                win::bug("Couldn't map lighter light buffer for writing");
            lighter.lightbuf = win::GLMappedRingBuffer<LighterLight>(mem, lighter_light_buf_count);

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

    {
        RandomNumberGenerator rng(69'420);

        float flicker = 0.0f;
        for (int i = 0; i < flicker_pattern.size() / 2; ++i)
        {
            const float a = std::sin(flicker * 0.0007f) * 0.35f;
            const float b = std::sin(flicker * 0.02f) * 0.06f;

            const float x = 1.0f + a + b + rng.uniform_real(-0.02f, 0.02f);
            flicker_pattern.at(i) = x;
            flicker_pattern.at((flicker_pattern.size() - 1) - i) = x;

            ++flicker;
        }
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, res.width, res.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    glActiveTexture(GLConstants::blur_framebuffer_texture_unit);
    glBindTexture(GL_TEXTURE_2D, blur.tex.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, res.width, res.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
}

void GLLightRenderer::load(const std::vector<LightOccluder> &occluders, const std::vector<LightRenderable> &static_lights)
{
    shadower.occ_staging.reserve(occluders.size());
    shadower.occ_ranges.reserve((static_lights.size() + max_dynamic_lights) * 2);

    this->occluders = occluders;
    occindex.reset(1.0f,
                   this->occluders,
                   [](const LightOccluder &x)
                   {
                       return win::SpatialIndexLocation(x.x1, x.y1, x.x2 - x.x1, x.y2 - x.y1);
                   });

    auto occrange = shadower.occbuf.reserve(occluders.size());
    occrange.write(occluders.data(), occluders.size());

    auto lightrange = shadower.lightbuf.reserve(static_lights.size());

    {
        lights.clear();
        int index = 0;
        for (const auto &light : static_lights)
        {
            auto &item = lightrange[index];
            item.index = index;
            item.x = light.x;
            item.y = light.y;
            item.occluder_start = occrange.head();
            item.occluder_count = occrange.length();

            lights.emplace_back(index, light.x, light.y, light.power, light.color.red, light.color.green, light.color.blue, light.angle);
            ++index;
        }
    }

    glUseProgram(shadower.program.get());
    glUniform1i(shadower.uniform_light_start, lightrange.head());
    glUniform1i(shadower.uniform_light_count, lightrange.length());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadower.shadowmap.get());
    glBufferData(GL_SHADER_STORAGE_BUFFER, shadowmap_size * sizeof(float) * (lights.size() + max_dynamic_lights), NULL, GL_STATIC_DRAW);

    glDispatchCompute(std::ceil((shadowmap_size * lights.size()) / 32.0f), 1, 1);
    shadower.lightbuf.lock(lightrange);
    shadower.occbuf.lock(occrange);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    check_error();
}

void GLLightRenderer::render(const std::vector<int> &static_lights, const std::vector<LightRenderable> &dynamic_lights, GLuint fbo, float flicker)
{
    if (dynamic_lights.size() > max_dynamic_lights)
        win::bug("Too many dynamic lights");

    glBindFramebuffer(GL_FRAMEBUFFER, lighter.fbo.get());

    // compute shadowmap data for dynamic lights
    {
        shadower.occ_staging.clear();
        shadower.occ_ranges.clear();

        for (const auto &light : dynamic_lights)
        {
            const float cover_radius = 10.0f;
            win::SpatialIndexLocation loc(light.x - cover_radius, light.y - cover_radius, cover_radius * 2.0f, cover_radius * 2.0f);
            const int occ_range_start = shadower.occ_staging.size();

            for (const auto &occ : occindex.query(loc))
                shadower.occ_staging.push_back(occ);

            shadower.occ_ranges.push_back(occ_range_start);
            shadower.occ_ranges.push_back(shadower.occ_staging.size() - occ_range_start);
        }

        auto lightrange = shadower.lightbuf.reserve(dynamic_lights.size());
        auto occrange = shadower.occbuf.reserve(shadower.occ_staging.size());
        occrange.write(shadower.occ_staging.data(), shadower.occ_staging.size());

        int i = 0;
        for (const auto &light : dynamic_lights)
        {
            auto &item = lightrange[i];

            item.index = lights.size() + i;
            item.x = light.x;
            item.y = light.y;
            item.occluder_start = shadower.occ_ranges[i * 2 + 0] + occrange.head();
            item.occluder_count = shadower.occ_ranges[i * 2 + 1];

            ++i;
        }

        glUseProgram(shadower.program.get());
        glUniform1i(shadower.uniform_light_start, lightrange.head());
        glUniform1i(shadower.uniform_light_count, lightrange.length());

        glDispatchCompute(std::ceil((shadowmap_size * dynamic_lights.size()) / 32.0f), 1, 1);
        shadower.lightbuf.lock(lightrange);
        shadower.occbuf.lock(occrange);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // run the light rendering pass
    {
        auto range = lighter.lightbuf.reserve(static_lights.size() + dynamic_lights.size());

        int i = 0;

        const auto get_flicker = [this, &flicker](float phase)
        {
            const float f = flicker + phase;
            const auto a = flicker_pattern[(int)std::floor(f) % flicker_pattern.size()];
            const auto b = flicker_pattern[(int)std::ceil(f) % flicker_pattern.size()];

            const float t = std::fmod(f, 1.0f);
            const float lerp = a + ((b - a) * t);

            return lerp;
        };

        for (const auto &light_index : static_lights)
        {
            const auto &light = lights[light_index];
            auto &item = range[i];

            item.index = light.index;
            item.x = light.x;
            item.y = light.y;
            item.power = light.power * get_flicker((light.power + light.r + light.g + light.b) * 100.0f);
            item.r = light.r;
            item.g = light.g;
            item.b = light.b;
            item.angle = light.angle;
            ++i;
        }

        int primary_index = -1;
        int j = 0;
        for (const auto &light : dynamic_lights)
        {
            auto &item = range[i];
            item.index = lights.size() + j;
            item.x = light.x;
            item.y = light.y;
            item.power = light.power * get_flicker((light.power + light.color.red + light.color.green + light.color.blue) * 100.0f);
            item.r = light.color.red;
            item.g = light.color.green;
            item.b = light.color.blue;
            item.angle = light.angle;
            if (light.primary)
                primary_index = i;
            ++j;
            ++i;
        }

        glBindVertexArray(lighter.vao.get());

        glUseProgram(lighter.program.get());
        glUniform1i(lighter.uniform_primary_index, primary_index);
        glUniform1i(lighter.uniform_light_start, range.head());
        glUniform1i(lighter.uniform_light_count, i);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        lighter.lightbuf.lock(range);
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
