#pragma once

#include <fstream>

#include <win/Win.hpp>
#include <win/Stream.hpp>

namespace win
{

class AssetRollStreamRaw : public StreamBase
{
	WIN_NO_COPY_MOVE(AssetRollStreamRaw);

public:
	AssetRollStreamRaw(const std::string&, unsigned long long, unsigned long long);

	unsigned long long size() const override;
	void read(void*, unsigned long long) override;
	std::unique_ptr<unsigned char[]> read_all() override;
	std::string read_all_as_string() override;
	void seek(unsigned long long) override;
	unsigned long long tell() override;

private:
	void lazy_init();

	std::string asset_roll_name;
	std::ifstream stream;
	unsigned long long begin;
	unsigned long long length;
};

}
