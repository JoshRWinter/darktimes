#pragma once

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Darktimes.hpp"
#include "../../Renderable.hpp"

struct GLStaticFloor
{
	int vertex_offset;
};

class GLStaticFloorRenderer
{
	NO_COPY_MOVE(GLStaticFloorRenderer);

public:
	explicit GLStaticFloorRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &view);
	std::vector<const void*> load_static_floors(const std::vector<Renderable> &statics, const std::vector<int> &floor_layer_map);
	void add(const void *v);
	void flush();

private:
	win::GLProgram program;
	int uniform_view_projection;

	win::GLVertexArray vao;
	win::GLBuffer position_texcoord, layer, index;

	std::vector<GLStaticFloor> loaded_static_floors;
	std::vector<GLStaticFloor*> scene;
};
