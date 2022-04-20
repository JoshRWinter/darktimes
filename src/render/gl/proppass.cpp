#include <glm/gtc/type_ptr.hpp>
#include <win/gl.hpp>

#include "gl.hpp"
#include "proppass.hpp"

static std::vector<float> get_prop_verts(const std::vector<LevelProp> &props)
{
	std::vector<float> verts;

	for (const auto &prop : props)
	{
		std::array<float, 12> qverts;
		get_quad_verts(prop.x, prop.y, prop.width, prop.height, qverts);

		// excluder (debugging)
		std::array<float, 12> qverts2;
		get_quad_verts(prop.x - prop.excluder_padding_x, prop.y - prop.excluder_padding_y, prop.width + (prop.excluder_padding_x * 2.0f), prop.height + (prop.excluder_padding_y * 2.0f), qverts2);

		for (auto f : qverts)
			verts.push_back(f);

		for (auto f : qverts2)
			verts.push_back(f);
	}

	return verts;
}

PropPass::PropPass(win::AssetRoll &roll)
{
	propvert_count = 0;

	shader = win::load_gl_shaders(roll["shader/prop.vert"], roll["shader/prop.frag"]);
	glUseProgram(shader);

	uniform_projection = get_uniform(shader, "projection");
	uniform_view = get_uniform(shader, "view");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
}

PropPass::~PropPass()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(shader);
}

void PropPass::draw()
{
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, propvert_count / 2);
}

void PropPass::set_props(const std::vector<LevelProp> &props)
{
	const auto &prop_verts = get_prop_verts(props);
	propvert_count = prop_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * propvert_count, prop_verts.data(), GL_STATIC_DRAW);
}

void PropPass::set_projection(const glm::mat4 &projection)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_projection, 1, false, glm::value_ptr(projection));
}

void PropPass::set_view(const glm::mat4 &view)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_view, 1, false, glm::value_ptr(view));
}
