#ifndef GL_RENDERER_HPP
#define GL_RENDERER_HPP

#include <vector>

#include <win/font.hpp>
#include <win/fontrenderer.hpp>
#include <win/utility.hpp>
#include <win/assetroll.hpp>

#include "../../darktimes.hpp"
#include "../../sim/levelgen/levelobjects.hpp"

class GLRenderer
{
public:
	NOCOPYMOVE(GLRenderer);

	GLRenderer(const win::IDimensions2D&, const win::FScreenArea&, const win::FScreenArea&, win::AssetRoll&);
	~GLRenderer();

	void set_level_data(const std::vector<LevelFloor>&, const std::vector<LevelWall>&, const std::vector<LevelProp>&, int);
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

	// world state
	std::string levelseed;

	// render state
	win::FScreenArea hud_area;
	win::FontRenderer font_renderer;
	win::Font font_debug, font_ui;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_calc_time;
	int accumulated_fps;
	char fpsindicator[10];
};
#endif
