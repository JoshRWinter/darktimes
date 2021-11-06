#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"

class Renderer
{
public:
	NOCOPYMOVE(Renderer);

	Renderer(int, int, float, float, float, float, AssetManager&);
	~Renderer();

	void set_level_data(const std::vector<float>&, const std::vector<float>&);
	void computeframe();

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

	win::FontRenderer font_renderer;
	win::Font font_debug, font_ui;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_calc_time;
	int accumulated_fps;
	char fpsindicator[10];

	const float left, right, bottom, top;
};
#endif
