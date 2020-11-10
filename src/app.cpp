#include <pch.h>
#include <Kiss/app.h>
//#include "fsmSemaphore.h"
#include <entt/entt.hpp>
#include <Kore/Display.h>
#include <Kiss/components/sprite.h>
#include <Kiss/render/commands.h>
#include <Kiss/systems/spriteMove.h>
#include <Kiss/systems/SpriteSheet.h>
#include <kinc/graphics4/graphics.h>
#include <thread>

#ifdef KISS_IMGUI
	#include <imgui.h>
#endif

using namespace kiss;

namespace 
{
	entt::registry registry;

	#ifdef KISS_IMGUI //imgui state
		bool show_another_window = true;
	#endif

	namespace win 
	{
		u32 w = 1280;
		u32 h = 720;
		f32 scale = 1.f;
		f32 sw = (w / scale);
		f32 sh = (h / scale);
	}
	ecs::flipbookData Anims[1] = 
	{
		1.f,//u16 duration; //second = 1000
		id::spr::RectAnim1,
		3//u8	 numFrames;
	};

	constexpr int buffersize = 512;
	u8 bufferData[buffersize];
	gfx2d::command::buffer commandbuffer(bufferData, buffersize);
}

namespace entities 
{
	void init(entt::registry& world, int entities_n) 
	{
		using namespace ecs;
		auto group = world.group<pos2d, vel2d>();
		auto spr = sprite{ id::spr::RectAnim1, 2 ,0 };
		auto bounds = aabb(14, 32, win::sw - 10, win::sh);
		flipbook::properties options{ 1,0,0,4,0 };
		for (auto i = 0; i < entities_n; ++i) 
		{
			auto entity = world.create();
			auto pos = pos2d::rand(bounds);
			auto vel = vel2d::rand(-90, 90, (float)kinc_random_get_in(150, 500));
			auto s = vel.len()*0.01f;
			world.emplace<pos2d>(entity, pos);
			world.emplace<vel2d>(entity, vel);
			flipbook::play(world,entity, &Anims[0], options, kinc_random_get_in(0, 20) * 0.1f, s);
		}
	}

	void render(float dt, entt::registry& world, gfx2d::quad::colored* quads) 
	{
		using namespace ecs;
		const iColor c[3] = 
		{ 
			iColor::Orange, 
			iColor::Pink, 
			iColor::White 
		};
		auto view = world.group<pos2d, vel2d>(entt::get<flipbook>);
		for (auto entity : view) 
		{
			const auto& [pos,flip] = view.get<pos2d, flipbook>(entity);
			quads->sprite(flip.getFrame(), pos.x, pos.y, c[flip.position]);
			//quads.add_sprite(id::spr::RectAnim1, pos,iColor::White);
		}
	}
}

void setupCommandBuffer() 
{
	using namespace win;
	using namespace gfx2d::command;
	using namespace id;
	constexpr sprId spr = spr::RectRect;
	constexpr sprId s9 = s9::Test;
	constexpr u8 font = fnt::Text;
	commandbuffer
		.sprite(spr, 16, 32)
		.sprite(spr, 16, sh)
		.sprite(spr, sw - 10, sh)
		.sprite(spr, sw - 10, 32)
		.scale9(s9, 250, 250, 294, 300)
		.scale9(s9, 306, 250, 500, 300)
		.textblock(100, 100).text("Ciao ")
		.color(iColor::Red).font(font).text("Mondo!!!");
}

int	app::main(int argc, char** argv) 
{
	kinc_random_init(0);
	using namespace win;
	framework::init("Hello World");
	/*
#ifndef KORE_HTML5
	auto D = Kore::Display::primary();
	w = D->width()/2;
	h = D->height()/2;
	scale = 1.f;
	sw = (w / scale);
	sh = (h / scale);
#endif*/
	framework::setResolution(w, h, scale, 1);
	//g2 = new Kore::Graphics2::Graphics2(w, h);
	setupCommandBuffer();
	entities::init(registry, 4000);
	return 0;
}

void app::resize(int x, int y, void* data)
{
	using namespace win;
	w = x;
	h = y;
	scale = 1.0f;
	sw = (x / scale);
	sh = (y / scale);
	gfx2d::resize(w, h, scale);
	commandbuffer.reset();
	setupCommandBuffer();
}

void app::update(float dt) 
{
	const aabb bounds(14, 32, win::sw - 10, win::sh);
	ecs::move::step_in_aabb(registry, dt, bounds);
	ecs::system::UpdateFlipbooks(registry, dt);
}



void app::render(float dt) {
	kinc_g4_scissor(0, 0, win::w, win::h);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF808080, 0, 0);
	//kinc_g4_clear(KINC_G4_CLEAR_COLOR, iColor((u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255)), 0, 0);
	
	auto quads = gfx2d::quad::batcher;
	quads->set_atlas(gfx2d::quad::atlases::gui);
	quads->begin();
	entities::render(dt, registry, quads);
	//----------------------------------------
	quads->scale9(id::s9::Test, aabb(8.f, 8.f, 200.f, 200.f), iColor::White);
	//----------------------------------------------------------------
	commandbuffer.execute();
	//----------------------------------------------------------------------
	char buffer[20];
	sprintf(buffer, "%d", (int)(1.f/dt));
	auto tc = textCtx(10, 10);
	quads->text(tc, buffer, iColor::Black);
	quads->end();
}

#ifdef KISS_IMGUI	
void app::imgui(float dt) {

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::End();

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
	//ImGui::ShowDemoWindow();
}
#endif

void app::shutdown() {

}