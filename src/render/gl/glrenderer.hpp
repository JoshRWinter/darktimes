#ifndef GL_RENDERER_HPP
#define GL_RENDERER_HPP

#include <vector>
#include <GL/gl.h>

#include <win/utility.hpp>
#include <win/assetroll.hpp>

#include "../../darktimes.hpp"
#include "../../sim/levelgen/levelobjects.hpp"

class GLRenderer
{
public:
	NOCOPYMOVE(GLRenderer);

	GLRenderer(const win::Area<float>&, win::AssetRoll&);
	~GLRenderer();

	void set_level_data(const std::vector<LevelFloor>&, const std::vector<LevelWall>&, const std::vector<LevelProp>&);
	void set_center(float, float);
	void send_frame();

private:
	struct
	{
		struct
		{
			GLuint shader;
			GLuint vao, vbo;

			GLint uniform_projection, uniform_rot;
			GLint uniform_view;

			int wallvert_count;
		} wall;

		struct
		{
			GLuint shader;
			GLuint vao, vbo;

			GLint uniform_projection;
			GLint uniform_view;

			GLuint floortextures;

			int floorvert_count;
		} floor;

		struct
		{
			GLuint shader;
			GLuint vao, vbo;

			GLint uniform_projection;
			GLint uniform_view;

			int propvert_count;
		} prop;
	} mode;
};
#endif
