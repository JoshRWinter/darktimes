#include "../darktimes.h"

game::quadpass::quadpass(win::roll &roll, const win::area &screen)
{
	program = win::load_shaders(roll["../src/render/quadpass.vert"], roll["../src/render/quadpass.frag"]);
	glUseProgram(program);

	float matrix[16];
	const float zooma_zoom_zoom = 1.0f;
	win::init_ortho(matrix, screen.left * zooma_zoom_zoom, screen.right * zooma_zoom_zoom, screen.bottom * zooma_zoom_zoom, screen.top * zooma_zoom_zoom);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, matrix);

	glBindVertexArray(vao);

	const float verts[] =
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f
	};

	const unsigned int elements[] =
	{
		0, 1, 2, 2, 1, 3
	};

	// element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// triangle verts
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
	glEnableVertexAttribArray(0);

	// position and rotation
	glBindBuffer(GL_ARRAY_BUFFER, position_size_rotation);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 5, NULL);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(float) * 5, (void*)(sizeof(float) * 2));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(float) * 5, (void*)(sizeof(float) * 4));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
}

void game::quadpass::add(const ent::entity &entity)
{
	buffer_position_size_rotation.push_back(entity.x);
	buffer_position_size_rotation.push_back(entity.y);
	buffer_position_size_rotation.push_back(entity.w);
	buffer_position_size_rotation.push_back(entity.h);
	buffer_position_size_rotation.push_back(entity.rot);
}

void game::quadpass::send()
{
	glUseProgram(program);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, position_size_rotation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer_position_size_rotation.size(), buffer_position_size_rotation.data(), GL_DYNAMIC_DRAW);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, buffer_position_size_rotation.size() / 5);

	buffer_position_size_rotation.clear();
}
