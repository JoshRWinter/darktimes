#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"
#include "../sim/levelmanager.hpp"

class Renderer
{
public:
	NOCOPYMOVE(Renderer);

	Renderer(int, int, float, float, float, float, AssetManager&);
	~Renderer();

	void set_level_data(const std::vector<LevelFloor>&, const std::vector<LevelWall>&, int);
	void computeframe();
	void set_center(float, float);

private:
	struct
	{
		struct
		{
			unsigned shader;
			unsigned vao, vbo;
			int uniform_projection, uniform_rot;
			int wallvert_count;
		} wall;

		struct
		{
			unsigned shader;
			unsigned vao, vbo;
			int uniform_projection;
			unsigned floortextures;
			int floorvert_count;
		} floor;
	} mode;

	std::string levelseed;

	win::FontRenderer font_renderer;
	win::Font font_debug, font_ui;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_calc_time;
	int accumulated_fps;
	char fpsindicator[10];

	const float left, right, bottom, top;
};
#endif
