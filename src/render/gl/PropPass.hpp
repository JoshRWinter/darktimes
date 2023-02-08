#pragma once

#include <GL/gl.h>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>

#include "../../Darktimes.hpp"
#include "../Renderable.hpp"

class PropPass
{
	NO_COPY_MOVE(PropPass);
public:
	PropPass(win::AssetRoll&);
	~PropPass();

	void draw();
	void set_props(const std::vector<Renderable>&);
	void set_projection(const glm::mat4&);
	void set_view(const glm::mat4&);

private:
    GLuint shader;
    GLuint vao, vbo;

    GLint uniform_projection;
    GLint uniform_view;

    int propvert_count;
};
