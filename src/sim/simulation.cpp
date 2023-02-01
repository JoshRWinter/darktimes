#include <thread>
#include <chrono>

#include "simulation.hpp"
#include "world.hpp"
#include "levelgen/levelgenerator.hpp"

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

	win::Pool<RenderableComponent> tile_renderables;
	const int seed = time(NULL);
	level_generate(seed, world.entities, world.physicals, world.atlas_renderables, tile_renderables);

	map_renderables(world.atlas_renderables, data->atlas_renderables);
	map_renderables(tile_renderables, data->tile_renderables);
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

	float xpos = 0.0f;
	float ypos = 0.0f;
	Input input;
	while(!stop)
	{
		Input *i;
		if ((i = input_som.reader_acquire()) != NULL)
		{
			input = *i;
			input_som.reader_release(i);
		}

		const float scoot = 0.08f;
		if (input.left)
			xpos -= scoot;
		if (input.right)
			xpos += scoot;
		if (input.up)
			ypos += scoot;
		if (input.down)
			ypos -= scoot;

		RenderState *rs;
		do { rs = render_state_som.writer_acquire(); } while (rs == NULL);

		rs->centerx = xpos;
		rs->centery = ypos;

		render_state_som.writer_release(rs);

		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(16));

		/*
		level_manager.generate();
		set_level_data_sync(level_data_sync_object_manager, level_manager);
		 */
	}
}
