#ifndef GL_FLOORPASS_HPP
#define GL_FLOORPASS_HPP

#include <GL/gl.h>

#include <win/assetroll.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../darktimes.hpp"
#include "../../sim/levelgen/levelobjects.hpp"

class FloorPass
{
	NOCOPYMOVE(FloorPass);

public:
	FloorPass(win::AssetRoll&);
	~FloorPass();

	void draw();
    void set_floors(const std::vector<LevelFloor>&);
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

#endif
