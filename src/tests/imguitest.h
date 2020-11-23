#include "pch.h"
#include <Kiss/app.h>
#include <imgui.h>

using namespace kiss;

namespace imguitest
{
	bool demo_window_open = true;

	void exec(float dt)
	{
		ImGui::ShowDemoWindow(&demo_window_open);
	}
}