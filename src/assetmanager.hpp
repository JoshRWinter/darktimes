#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <win.h>

#include "darktimes.hpp"

class AssetManager
{
	NOCOPYMOVE(AssetManager);
public:
	AssetManager(win::AssetRoll&);
	win::Stream operator[](const char*);

	int floor_texture_count() const;
	std::vector<win::Targa> get_floor_textures();
	static constexpr float floor_texture_tile_size = 0.5f;

private:
	win::AssetRoll &roll;
};

#endif
