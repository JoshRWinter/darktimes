#pragma once

#include <vector>
#include <GL/gl.h>

#include <win/Utility.hpp>
#include <win/AssetRoll.hpp>

#include "../../Darktimes.hpp"
#include "../../Renderable.hpp"

#include "FloorPass.hpp"
#include "PropPass.hpp"
#include "DynamicPass.hpp"

class GLRenderer
{
public:
	NO_COPY_MOVE(GLRenderer);

	GLRenderer(const win::Area<float>&, win::AssetRoll&, bool debug);

	void set_level_data(const std::vector<Renderable>&, const std::vector<Renderable>&);
	void set_dynamics(const std::vector<Renderable>&);

	void set_center(float, float);

	void draw();

private:
	FloorPass floor_pass;
	PropPass prop_pass;
	DynamicPass dynamic_pass;
};
