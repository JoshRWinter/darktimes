#include <thread>
#include <chrono>

#include "simulation.hpp"
#include "levelgen/levelmanager.hpp"

static void set_level_data_sync(SyncObjectManager<LevelData> &level_data_sync_object_manager, LevelManager &levelmanager)
{
	LevelData *ldso;
	do
	{
		ldso = level_data_sync_object_manager.writer_acquire();
	} while (ldso == NULL);

	ldso->walls = levelmanager.walls;
	ldso->floors = levelmanager.floors;
	ldso->props = levelmanager.props;
	ldso->seed = levelmanager.seed;

	level_data_sync_object_manager.writer_release(ldso);
}

void simulation(std::atomic<bool> &stop, SyncObjectManager<LevelData> &level_data_sync_object_manager, SyncObjectManager<Input> &input_sync_object_manager, SyncObjectManager<RenderState> &render_state_sync_object_manager)
{
	LevelManager level_manager(time(NULL));
	set_level_data_sync(level_data_sync_object_manager, level_manager);

	float xpos = 0.0f;
	float ypos = 0.0f;
	Input input;
	while(!stop)
	{
		Input *i;
		if ((i = input_sync_object_manager.reader_acquire()) != NULL)
		{
			input = *i;
			input_sync_object_manager.reader_release(i);
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
		do { rs = render_state_sync_object_manager.writer_acquire(); } while (rs == NULL);

		rs->centerx = xpos;
		rs->centery = ypos;

		render_state_sync_object_manager.writer_release(rs);

		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(16));

		/*
		level_manager.generate();
		set_level_data_sync(level_data_sync_object_manager, level_manager);
		 */
	}
}
