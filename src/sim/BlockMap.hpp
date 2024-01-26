#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

#include "../Darktimes.hpp"

template<typename T> class BlockMap;

namespace impl
{

struct BlockKey
{
	BlockKey(std::uint16_t x, std::uint16_t y) : x(x), y(y) {}

	bool operator==(BlockKey rhs) const { return x == rhs.x && y == rhs.y; }

	std::uint16_t x;
	std::uint16_t y;
};

template<typename T> class BlockMapIterable;
template<typename T> class BlockMapIterator
{
	NO_COPY_MOVE(BlockMapIterator);

	friend class BlockMapIterable<T>;

public:
	BlockMapIterator(const BlockMap<T> &map, BlockKey key1, BlockKey key2, bool end = false)
		: x(key1.x)
		, y(key1.y)
		, key1(key1)
		, key2(key2)
		, map(map)
	{
		if (end)
		{
			map_index = map.index(key2);
			block_index = map.map[map_index].size();
		}
		else
		{
			map_index = map.index(key1);
			block_index = -1;

			next();
		}
	}

	T &operator*()
	{
#ifndef NDEBUG
		return *map.map.at(map_index).at(block_index);
#else
		return *map.map[map_index][block_index];
#endif
	}

	void operator++()
	{
		next();
	}

	//bool operator==(const BlockMapIterator<T> &rhs) const { return end == rhs.end; }//return block_index == rhs.block_index && map_index == rhs.map_index; }
	bool operator!=(const BlockMapIterator<T> &rhs) const { return block_index != rhs.block_index || map_index != rhs.map_index; }

private:
	void next()
	{
		++block_index;

#ifndef NDEBUG
		while (block_index >= map.map.at(map_index).size() && !(x == key2.x && y == key2.y))
#else
		while (block_index >= map.map[map_index].size() && x != key2.x && y != key2.y)
#endif
		{
			block_index = 0;
			++x;
			if (x > key2.x)
			{
				x = key1.x;
				++y;
			}

			map_index = map.index(BlockKey(x, y));
		}
	}

	int block_index;
	int map_index;
	int x, y;
	const BlockKey key1, key2;
	const BlockMap<T> &map;
};

template<typename T>
class BlockMapIterable
{
	NO_COPY_MOVE(BlockMapIterable);

public:
	BlockMapIterable(const BlockMap<T> &map, BlockKey key1, BlockKey key2)
		: key1(key1)
		, key2(key2)
		, map(map)
	{}

	BlockMapIterator<T> begin() const { return BlockMapIterator<T>(map, key1, key2); }
	BlockMapIterator<T> end() const { return BlockMapIterator<T>(map, key1, key2, true); }

private:
	const BlockKey key1, key2;
	const BlockMap<T> &map;
};

}

struct BlockMapLocation
{
	explicit BlockMapLocation(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) {}

	const float x, y, w, h;
};

template<typename T> class BlockMap
{
	NO_COPY_MOVE(BlockMap);
	friend class impl::BlockMapIterator<T>;

public:
	BlockMap()
		: block_size(0.0f)
		, map_left(0.0f)
		, map_right(0.0f)
		, map_bottom(0.0f)
		, map_top(0.0f)
		, map_width(0)
		, map_height(0)
	{}

	void reset(float block_size, float map_left, float map_right, float map_bottom, float map_top)
	{
		this->block_size = block_size;
		this->map_left = map_left;
		this->map_right = map_right;
		this->map_bottom = map_bottom;
		this->map_top = map_top;
		this->map_width = std::ceil(map_right - map_left) / block_size;
		this->map_height = std::ceil(map_top - map_bottom) / block_size;

		map.resize(map_width * map_height);
		for (auto &block : map)
			block.reserve(20);
	}

	void add(const BlockMapLocation &loc, T &id)
	{
		insert_or_delete<true>(sample(loc.x, loc.y), sample(loc.x + loc.w, loc.y + loc.h), id);
	}

	void move(const BlockMapLocation &old_loc, const BlockMapLocation &new_loc, T &id)
	{
		const auto old_key1 = sample(old_loc.x, old_loc.y);
		const auto old_key2 = sample(old_loc.x + old_loc.w, old_loc.y + old_loc.y);

		const auto new_key1 = sample(new_loc.x, new_loc.y);
		const auto new_key2 = sample(new_loc.x + new_loc.w, new_loc.y + new_loc.y);

		if (old_key1 == new_key1 && old_key2 == new_key2)
			return; // object is still in the same block. no update needed

		insert_or_delete<false>(old_key1, old_key2, id);
		insert_or_delete<true>(new_key1, new_key2, id);
	}

	void remove(const BlockMapLocation &loc, T &id)
	{
		insert_or_delete<false>(sample(loc.x, loc.y), sample(loc.x + loc.w, loc.y + loc.h), id);
	}

	impl::BlockMapIterable<T> iterate(const BlockMapLocation &loc)
	{

		const auto key1 = sample(loc.x, loc.y);
		const auto key2 = sample(loc.x + loc.w, loc.y + loc.h);

		return impl::BlockMapIterable<T>(*this, key1, key2);
	}

private:
	template<bool add> void insert_or_delete(impl::BlockKey a, impl::BlockKey b, T &id)
	{
		// add to all blocks that overlap
		for (auto x = a.x; x <= b.x; ++x)
		{
			for (auto y = a.y; y <= b.y; ++y)
			{
				const auto idx = index(impl::BlockKey(x, y));

				if constexpr (add)
				{
#ifndef NDEBUG
					map.at(idx).push_back(&id);
#else
					map[idx].push_back(&id);
#endif
				}
				else
				{
#ifndef NDEBUG
					auto &block = map.at(idx);
#else
					auto &block = map[idx];
#endif

					for (auto &item : block)
						//for (auto it = block.begin(), end = block.end(); it < end; ++it)
					{
						if (item == id)
						{
							item = NULL;
							return;
						}
					}
				}
			}
		}
	}

	impl::BlockKey sample(float x, float y) const
	{
#ifndef NDEBUG
		if (x < map_left || x > map_right)
			win::bug("BlockMap: x out of bounds (x=" + std::to_string(x) + ", map_left=" + std::to_string(map_left) + ", map_right=" + std::to_string(map_right) + ")");
		if (y < map_bottom || y > map_top)
			win::bug("BlockMap: y out of bounds (y=" + std::to_string(y) + ", map_bottom=" + std::to_string(map_bottom) + ", map_top=" + std::to_string(map_top) + ")");
#endif

		const std::uint16_t blockx = std::floor((x - map_left) / block_size);
		const std::uint16_t blocky = std::floor((y - map_bottom) / block_size);

		return impl::BlockKey(blockx, blocky);
	}

	int index(impl::BlockKey key) const
	{
		return (key.y * map_width) + key.x;
	}

	float block_size, map_left, map_right, map_bottom, map_top;
	int map_width, map_height;
	std::vector<std::vector<T*>> map;
};
