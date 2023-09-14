#pragma once

#include <vector>

struct TextureDefinition
{
	TextureDefinition(const char *resource, int atlas, int atlas_index)
		: resource(resource)
		, atlas(atlas)
		, atlas_index(atlas_index)
	{}

	const char *resource;
	int atlas;
	int atlas_index;
};

#define ATLAS(name, path) private: const int hidden_atlas_##name = add_atlas(path); public:
#define TEXTURE(name, atlas, index) const int name = add_atlas_texture(TextureDefinition(atlases.at(hidden_atlas_##atlas), hidden_atlas_##atlas, index));
#define FLOOR_TEXTURE(name, resource) const int name = add_floor_texture(TextureDefinition(resource, -1, -1));

class TextureDefinitionsStorage
{
public:
	std::vector<const char*> atlases;
	std::vector<TextureDefinition> textures;
	std::vector<int> floor_textures;

#include "Textures.txt"

private:
	int add_atlas(const char *path) { atlases.push_back(path); return atlases.size() - 1; }
	int add_atlas_texture(const TextureDefinition &t) { textures.push_back(t); return textures.size() - 1; }
	int add_floor_texture(const TextureDefinition &t) { textures.push_back(t); floor_textures.push_back(textures.size() - 1); return textures.size() - 1; }
};

#ifndef TEXTURE_DEFINITIONS_CPP
extern const TextureDefinitionsStorage TextureDefinitions;
#endif

#undef ATLAS
#undef TEXTURE
#undef FLOOR_TEXTURE
