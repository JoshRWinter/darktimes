#ifndef GL_DEBUGQUADPASS_HPP
#define GL_DEBUGQUADPASS_HPP

#include <GL/gl.h>

#include <glm/glm.hpp>
#include <win/assetroll.hpp>
#include <win/utility.hpp>

#include "../../darktimes.hpp"

struct DebugQuad
{
	DebugQuad(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h), color(1.0f, 1.0f, 1.0f, 0.8f) {}
	DebugQuad(float x, float y, float w, float h, const win::Color &color)
		: x(x), y(y), w(w), h(h), color(color) {}
	DebugQuad()
		: x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}

	float x, y, w, h;
	win::Color color;
};

class DebugQuadPass
{
	NO_COPY_MOVE(DebugQuadPass);

public:
	DebugQuadPass(win::AssetRoll&);
	~DebugQuadPass();

	void draw();
	void set_quads(const std::vector<DebugQuad>&);
	void set_projection(const glm::mat4&);
	void set_view(const glm::mat4&);

private:
    GLuint shader;
    GLuint vao, vbo;

    GLint uniform_projection;
    GLint uniform_view;

    int quadvert_count;
};

#endif
