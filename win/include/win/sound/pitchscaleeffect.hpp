#pragma once

#include <win/sound/soundeffect.hpp>

namespace win
{

class PitchScaleEffect : public SoundEffect
{
	WIN_NO_COPY_MOVE(PitchScaleEffect);

public:
	PitchScaleEffect(int, float);

	int read_samples(float*, int) override;
	void set_scale(float);

private:
	float scale;
};

}
