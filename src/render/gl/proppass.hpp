#ifndef GL_PROPPASS_HPP
#define GL_PROPPASS_HPP

#include <GL/gl.h>

#include <glm/glm.hpp>
#include <win/assetroll.hpp>

#include "../../darktimes.hpp"
#include "../../sim/levelgen/levelobjects.hpp"

class PropPass
{
	NOCOPYMOVE(PropPass);
public:
	PropPass(win::AssetRoll&);
	~PropPass();

	void draw();
	void set_props(const std::vector<LevelProp>&);
	void set_projection(const glm::mat4&);
	void set_view(const glm::mat4&);

private:
    GLuint shader;
    GLuint vao, vbo;

    GLint uniform_projection;
    GLint uniform_view;

    int propvert_count;
};

#endif
