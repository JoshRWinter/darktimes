#pragma once

#include <GL/gl.h>

#include <win/AssetRoll.hpp>
#include <glm/glm.hpp>

#include "../../Darktimes.hpp"
#include "../Renderable.hpp"

class FloorPass
{
	NO_COPY_MOVE(FloorPass);

public:
	FloorPass(win::AssetRoll&);
	~FloorPass();

	void draw();
    void set_floors(const std::vector<Renderable>&);
	void set_projection(const glm::mat4&);
	void set_view(const glm::mat4&);

private:
    GLuint shader;
    GLuint vao, vbo;

    GLint uniform_projection;
    GLint uniform_view;

    GLuint floortextures;

    int floorvert_count;
};
