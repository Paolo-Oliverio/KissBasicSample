#include <pch.h>
#include <Kiss/app.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/display.h>
#include <kinc/math/random.h>
#include <string>//for string appending.
#include <assets/Data.h>
#include "tests/gfxcommandstest.h"

#ifndef KISS_BOX2D
#define WithBox2D(x)
#else
#define WithBox2D(x) x
#include "tests/box2dtest.h"
#endif

#ifndef KISS_ENTT
#define WithEntt(x)
#else
#define WithEntt(x) x
#include "tests/ecstest.h"
#endif

using namespace kiss;

namespace win 
{
	u32 w = 1280;
	u32 h = 720;
	f32 sw = (f32)w;
	f32 sh = (f32)h;
}

int	app::on_launch(int argc, char** argv)
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

void app::on_shutdown() {}

void app::on_resize(int x, int y)
{
	using namespace win;
	w = x;
	h = y;
	sw = (x / gfx2d::Pipe2d.scaling);
	sh = (y / gfx2d::Pipe2d.scaling);
	gfxCmdBuffer::setup(sw,sh);
}

void app::on_update(float dt) 
{
	WithEntt(ecstest::update(win::sw,win::sh,dt));
	WithBox2D(box2dtest::update(dt));
}

void app::on_render(float dt) 
{
	//kinc_g4_viewport(0, 0, win::w, win::h);
	kinc_g4_scissor(0, 0, win::w, win::h);
	//kinc_g4_viewport(0, 0, 1280, 720);
	//kinc_g4_scissor(0, 0, 1280,720);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF808080, 0, 0);
	//kinc_g4_clear(KINC_G4_CLEAR_COLOR, iColor((u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255)), 0, 0);
	//----------------------------------------------------------------
	auto quads = gfx2d::quad::batcher;
	quads->set_atlas(gfx2d::quad::atlases::gui);
	quads->begin();
	WithEntt(ecstest::render());
	//----------------------------------------------------------------
	gfxCmdBuffer::render();
	//----------------------------------------------------------------
	quads->setVData(iColor::White, iColor::White, iColor::White, 0xFF404040);
	quads->scale9(id::s9::Test, aabb(10, 10, 210, 210));
	quads->setVData(iColor::White, iColor::White, 0xFFA0A0A0, 0xFFA0A0A0);
	std::string str = std::to_string((int)(1.f / dt));
	auto tc = textCtx(10, 10);
	quads->text(tc, str.c_str());	

	WithBox2D(box2dtest::render());
	//----------------------------------------------------------------
	quads->end();
}