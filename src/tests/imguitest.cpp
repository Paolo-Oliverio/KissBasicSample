#include "pch.h"
#include <Kiss/app.h>
#include <imgui.h>

using namespace kiss;

namespace
{
	bool demo_window_open = true;
}

void app::on_imgui(float dt)
{
	ImGui::ShowDemoWindow(&demo_window_open);
}