#pragma once

#include "pch.h"
#include "Kiss/sound/sound.h"
#include "soloud.h"
#include "soloud_sfxr.h"
#include <kinc/math/random.h>

using namespace kiss;
using namespace SoLoud;

namespace soundtest {
	auto elapsed = 0.f;
	Sfxr sfx;
	handle h;

	void play() {
		auto& s = sound::manager;
		sfx.loadPreset(3, kinc_random_get());
		h = s.play(sfx);
		s.setRelativePlaySpeed(h, kinc_random_get_in(75, 150) / 100.f);
	}

	void update(float dt) {
		elapsed += dt;
		
		if (elapsed > 1.5f) {
			play();
			elapsed = 0.f;
		}
	}
}