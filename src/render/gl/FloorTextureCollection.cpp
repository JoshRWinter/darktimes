#include <win/Targa.hpp>

#include "../../TextureDefinitions.hpp"
#include "FloorTextureCollection.hpp"

FloorTextureCollection::FloorTextureCollection(win::AssetRoll &roll)
{
	int width = -1, height = -1;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex.get());
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// -1 initialize a bunch of entries in the map
	map.resize(TextureDefinitions.textures.size(), -1);

	int index = 0;
	for (const auto i : TextureDefinitions.floor_textures)
	{
		win::Targa tga(roll[TextureDefinitions.textures[i].resource]);
		map[i] = index;

		if (width == -1)
		{
			width = tga.width();
			height = tga.height();
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, TextureDefinitions.floor_textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		if (width != tga.width() || height != tga.height())
			win::bug("All floor textures must be same dimensions!");

		if (tga.bpp() != 24 && false)
			win::bug("Prefer 24 bit color for floor textures");

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tga.data());
		++index;
	}
}

const std::vector<int> &FloorTextureCollection::get_layer_map() const
{
	return map;
}
