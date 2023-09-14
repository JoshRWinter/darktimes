#include <thread>
#include <chrono>

#include "Simulation.hpp"

#include "World.hpp"
#include "levelgen/LevelGenerator.hpp"

#include "system/PlayerSystem.hpp"

static void map_renderables(const win::Pool<RenderableComponent> &renderable_component_input, std::vector<Renderable> &renderable)
{
	for (const auto &input : renderable_component_input)
	{
		auto physical = input.entity.get<PhysicalComponent>();

#ifndef NDEBUG
		if (physical == NULL)
			win::bug("null physical");
#endif

		renderable.emplace_back(input.texture, physical->x, physical->y, physical->w, physical->h, physical->rot);
	}
}

static void generate_and_set_level_data(SyncObjectManager<LevelRenderState> &level_renderables_som, World &world)
{
	LevelRenderState *data;
	do
	{
		data = level_renderables_som.writer_acquire();
	} while (data == NULL);

	data->reset();

	const int seed = time(NULL);
	level_generate(seed, world.entities, world.physicals, world.atlas_renderables, world.tile_renderables);

	map_renderables(world.tile_renderables, data->renderables);
	map_renderables(world.atlas_renderables, data->renderables);
	data->seed = seed;

	level_renderables_som.writer_release(data);
}

void simulation(
	std::atomic<bool> &stop,
	SyncObjectManager<LevelRenderState> &level_render_state_som,
	SyncObjectManager<RenderState> &render_state_som,
	SyncObjectManager<Input> &input_som
)
{
	World world;

	generate_and_set_level_data(level_render_state_som, world);

	Input input;

	while(!stop)
	{
		// gather input
		Input *i;
		if ((i = input_som.reader_acquire()) != NULL)
		{
			input = *i;
			input_som.reader_release(i);
		}

		player_system(
			world.entities,
			world.physicals,
			world.atlas_renderables,
			world.players,
			input,
			world.centerx,
			world.centery
		);

		// send render state
		RenderState *rs;
		do
		{
			rs = render_state_som.writer_acquire();
		} while (rs == NULL);

		map_renderables(world.atlas_renderables, rs->renderables);
		rs->centerx = world.centerx;
		rs->centery = world.centery;

		render_state_som.writer_release(rs);

		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(16.666));
	}
}
