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

		if (width == -1)
		{
			width = tga.width();
			height = tga.height();
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, floor_texture_count, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		}

		std::unique_ptr<unsigned char[]> data;

		if (width != tga.width() || height != tga.height())
			win::bug("All floor textures must be same dimensions!");

		if (tga.bpp() == 32)
			win::bug("Floor textures must not contain an alpha channel");
		else if (tga.bpp() == 8)
			data = convert_8_to_24(tga.data(), width, height);
		else if (tga.bpp() != 24)
			win::bug("Floor textures must be either 8 or 24 bit color depth");

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, item.layer_index, width, height, 1, GL_BGR, GL_UNSIGNED_BYTE, data ? data.get() : tga.data());
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

std::unique_ptr<unsigned char[]> GLFloorTextureCollection::convert_8_to_24(const unsigned char *data, int width, int height)
{
	std::unique_ptr<unsigned char[]> allocated(new unsigned char[width * height * 3]);

	for (int i = 0; i < width * height * 1; ++i)
	{
		allocated[(i * 3) + 0] = data[i];
		allocated[(i * 3) + 1] = data[i];
		allocated[(i * 3) + 2] = data[i];
	}

	return allocated;
}
