#include <glm/gtc/type_ptr.hpp>
#include <win/gl.hpp>

#include "gl.hpp"
#include "debugquadpass.hpp"

static std::vector<float> get_wall_verts(const std::vector<DebugQuad> &walls)
{
	std::vector<float> verts;

	for (const auto &wall : walls)
	{
		std::array<float, 12> qverts;
		get_quad_verts(wall.x, wall.y, wall.w, wall.h, qverts);

		for (auto f : qverts)
			verts.push_back(f);
	}

	return verts;
}

DebugQuadPass::DebugQuadPass(win::AssetRoll &roll)
{
	quadvert_count = 0;
	shader = win::load_gl_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glUseProgram(shader);

	uniform_projection = get_uniform(shader, "projection");
	uniform_view = get_uniform(shader, "view");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
}

DebugQuadPass::~DebugQuadPass()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(shader);
}

void DebugQuadPass::draw()
{
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, quadvert_count / 2);
}

void DebugQuadPass::set_quads(const std::vector<DebugQuad> &quads)
{
	const auto &quad_verts = get_wall_verts(quads);
	quadvert_count = quad_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quadvert_count, quad_verts.data(), GL_STATIC_DRAW);
}

void DebugQuadPass::set_projection(const glm::mat4 &projection)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_projection, 1, false, glm::value_ptr(projection));
}

void DebugQuadPass::set_view(const glm::mat4 &view)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_view, 1, false, glm::value_ptr(view));
}
