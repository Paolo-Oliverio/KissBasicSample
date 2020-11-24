#include <pch.h>
#include <Kiss/app.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/display.h>
#include <kinc/math/random.h>
#include <kinc/input/keyboard.h>
#include <kinc/input/surface.h>
#include <kinc/input/mouse.h>

#include <string>//for string appending.
#include <assets/data.h>
#include "tests/gfxcommandstest.h"

#ifdef KISS_BOX2D
	#include "tests/box2dtest.h"
#endif
#ifdef KISS_ENTT 
	#include "tests/ecstest.h"
#endif
#ifdef KISS_SOLOUD
	#include "tests/soloudtest.h"
#endif
#ifdef KISS_IMGUI
	#include "tests/imguitest.h"
#endif

using namespace kiss;

namespace win 
{
	u32 w = 1280;
	u32 h = 720;
	f32 sw = (f32)w;
	f32 sh = (f32)h;
}

int	app::start(int argc, char** argv)
{
	kinc_random_init(0);
	using namespace win;
	framework::init("Hello World");
/*
#ifndef KORE_HTML5
	auto d = kinc_display_current_mode(0);
	w = d.width;
	h = d.height;
#endif
*/
	framework::setResolution(w, h);
	gfxCmdBuffer::setup(sw, sh);
	WithEntt(ecstest::init(sw, sh, 4000));
	WithBox2D(box2dtest::init());
	return 0;
}

void app::release() {

}

void app::resize(int x, int y)
{
	using namespace win;
	w = x; sw = (x / gfx2d::scaling);
	h = y; sh = (y / gfx2d::scaling);
	gfxCmdBuffer::setup(sw,sh);
}

void app::input(float dt) {

}

void app::prePhysics(float dt) 
{
	WithEntt(ecstest::update(win::sw,win::sh,dt));
	WithBox2D(box2dtest::update(dt));
}

void app::postPhysics(float dt)
{
	WithSoloud(soundtest::update(dt));
}

#ifdef KISS_IMGUI
void app::gui(float dt) 
{
	WithImgui(imguitest::exec(dt));
}
#endif

void app::render(float dt) 
{
	//kinc_g4_viewport(0, 0, win::w, win::h);
	kinc_g4_scissor(0, 0, win::w, win::h);
	//kinc_g4_viewport(0, 0, 1280, 720);
	//kinc_g4_scissor(0, 0, 1280,720);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF808080, 0, 0);
	//kinc_g4_clear(KINC_G4_CLEAR_COLOR, iColor((u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255)), 0, 0);
	//----------------------------------------------------------------
	auto& b = gfx2d::batcher;
	b.begin(&gfx2d::base::pipeline, gfx2d::atlas0);
	WithEntt(ecstest::render());
	//----------------------------------------------------------------
	gfxCmdBuffer::render();
	//----------------------------------------------------------------
	b.vertexdata(iColor::White, iColor::White, iColor::White, 0xFF404040);
	b.scale9(id::s9::Test, aabb(10, 10, 210, 210));
	b.vertexdata(iColor::White, iColor::White, 0xFFA0A0A0, 0xFFA0A0A0);
	std::string str("fps : ");
	str.append(std::to_string((int)(1.f / dt)));
	auto tc = textCtx(10, 10);
	b.text(tc, str.c_str());
	//----------------------------------------------------------------
	WithBox2D(box2dtest::render());
	//----------------------------------------------------------------
	b.end();
}