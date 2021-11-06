#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "../darktimes.hpp"

class Renderer
{
public:
	NOCOPYMOVE(Renderer);

	Renderer(int, int, float, float, float, float, win::AssetRoll&);
	~Renderer();

	void set_wall_verts(const std::vector<float>&);
	void computeframe();

private:
	int wallvert_count;

	struct
	{
		struct
		{
			unsigned shader;
			unsigned vao, vbo;
			int uniform_projection, uniform_rot;
		} wall;

		struct
		{
			unsigned shader;
			unsigned vao, vbo;
			int uniform_projection;
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
