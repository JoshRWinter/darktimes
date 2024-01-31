#include <win/Targa.hpp>

#include "GLFloorTextureCollection.hpp"

using namespace win::gl;

GLFloorTextureCollection::GLFloorTextureCollection(win::AssetRoll &roll, const TextureAssetMap &texture_map)
{
	int width = -1, height = -1;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex.get());
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// -1 initialize a bunch of entries in the map
	map.resize((int)Texture::max_texture, -1);

	int floor_texture_count = 0;
	for (int i = 0; i < (int)Texture::max_texture; ++i)
		if (texture_map[(Texture)i].layer_index != -1)
			++floor_texture_count;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (int i = 0; i < (int)Texture::max_texture; ++i)
	{
		const auto &item = texture_map[(Texture)i];

		if (item.layer_index == -1)
			continue;

		win::Targa tga(roll[item.asset_path]);
		map[i] = item.layer_index;

		if (tga.bpp() != 24)
			win::bug("Floor textures must have 24 bit color depth");

		if (width == -1)
		{
			width = tga.width();
			height = tga.height();
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, floor_texture_count, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		}

		if (width != tga.width() || height != tga.height())
			win::bug("All floor textures must be same dimensions!");

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, item.layer_index, width, height, 1, GL_BGR, GL_UNSIGNED_BYTE, tga.data());
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

const int &GLFloorTextureCollection::get_layer(Texture texture) const
{
	return map[(int)texture];
}
