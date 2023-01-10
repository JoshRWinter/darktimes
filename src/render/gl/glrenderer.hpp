#pragma once

#include <vector>
#include <GL/gl.h>

#include <win/utility.hpp>
#include <win/assetroll.hpp>

#include "../../darktimes.hpp"
#include "../../sim/levelgen/levelobjects.hpp"
#include "floorpass.hpp"
#include "proppass.hpp"
#include "debugquadpass.hpp"

class GLRenderer
{
public:
	NO_COPY_MOVE(GLRenderer);

	GLRenderer(const win::Area<float> &, win::AssetRoll &);

	void
	set_level_data(const std::vector<LevelFloor> &, const std::vector<LevelWall> &, const std::vector<LevelProp> &);

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
	} mode;

	FloorPass floor_pass;
	PropPass prop_pass;
	DebugQuadPass debugquad_pass;
};
