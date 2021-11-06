#include "assetmanager.hpp"

static std::vector<const char*> floor_texture_names =
{
	"texture/floor1.tga",
	"texture/floor2.tga"
};

AssetManager::AssetManager(win::AssetRoll &roll)
	: roll(roll) {}

win::AssetRollStream AssetManager::operator[](const char *name)
{
	return roll[name];
}

int AssetManager::floor_texture_count() const
{
	return floor_texture_names.size();
}

std::vector<win::Targa> AssetManager::get_floor_textures()
{
	std::vector<win::Targa> targas;

	int width, height;
	bool width_height_initialized = false;

	for (auto name : floor_texture_names)
	{
		auto item = win::Targa(roll[name]);

		fprintf(stderr, "x=%d, y=%d\n", item.width(), item.height());

		if (item.bpp() != 32)
			bug(std::string(name) + " is not 32 bits");

		if (!width_height_initialized)
		{
			width_height_initialized = true;
			width = item.width();
			height = item.height();
		}
		else if (item.width() != width || item.height() != height)
			bug(std::string(name) + " dimensions not " + std::to_string(width) + "x" + std::to_string(height));

		targas.push_back(std::move(item));
	}

	return targas;
}
