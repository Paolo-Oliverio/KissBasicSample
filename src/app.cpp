#include <pch.h>
#include <Kiss/app.h>
#include <entt/entt.hpp>
#include <Kiss/components/sprite.h>
#include <Kiss/render/commands.h>
#include <Kiss/systems/spriteMove.h>
#include <Kiss/systems/SpriteSheet.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/display.h>
#include <thread>

#ifdef KISS_IMGUI
	#include <imgui.h>
#endif

#ifdef KISS_BOX2D
	#define WithBox2D(x) x
	#include <box2d/box2d.h>
#else
	#define WithBox2D(x)
#endif


using namespace kiss;

namespace 
{
	#ifdef KISS_IMGUI //imgui state
		bool show_another_window = true;
	#endif

	namespace win 
	{
		u32 w = 1280;
		u32 h = 720;
		f32 sw = w;
		f32 sh = h;
	}
	ecs::flipbookData Anims[1] = 
	{
		1.f,//u16 duration; //second = 1000
		id::spr::RectAnim1,
		3//u8	 numFrames;
	};

	float elapsed = 0;
}

#ifdef KISS_BOX2D
namespace box2d 
{
	b2Vec2 gravity(0.0f, 720.0f);
	b2World* world;
	b2Body* groundBody;
	b2Body* body;
	b2Body* body2;

	void init() 
	{
		world = new b2World(gravity);
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(640.f, 500.0f);
		groundBodyDef.angle = deg2rad(20);
		groundBody = world->CreateBody(&groundBodyDef);
		b2PolygonShape groundBox;
		groundBox.SetAsBox(1000.0f, 5.0f);
		groundBody->CreateFixture(&groundBox, 0.0f);
		
		b2PolygonShape dynamicBox;
		
		b2Vec2 vertices[4];
		vertices[0].Set(-16.f, 0.f);
		vertices[1].Set(10.f, 0.f);
		vertices[2].Set(10.f, -32.f);
		vertices[3].Set(-16.f, -32.f);

		dynamicBox.Set(vertices, 4);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;
		fixtureDef.restitution = 0.8f;
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		
		bodyDef.position.Set(100.0f, 100.0f);
		body = world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		bodyDef.position.Set(90.0f, 132.0f);
		body2 = world->CreateBody(&bodyDef);
		body2->CreateFixture(&fixtureDef);
	}

	void update(float dt) 
	{
		elapsed += dt;

		if (elapsed > 10) {
			body->SetTransform(b2Vec2(100 + (float)kinc_random_get_in(-10, 10), 100), 0);
			body->SetAngularVelocity(0);
			body->SetLinearVelocity(b2Vec2(0,0));
			body2->SetTransform(b2Vec2(100 + (float)kinc_random_get_in(-20, 20), 132), 0);
			body2->SetAngularVelocity(0);
			body2->SetLinearVelocity(b2Vec2(0, 0));
			elapsed = 0;
		}
		world->Step(dt, 10, 10);
	}

	void render() 
	{
		auto a = body->GetTransform();
		auto quads = gfx2d::quad::batcher;
		quads->sprite(id::spr::RectAnim1, 100, 100, rot(deg2rad(elapsed * 360)), iColor::White);
		quads->sprite(id::spr::RectAnim1, a.p.x, a.p.y, rot(a.q.c, a.q.s), iColor::White);
		a = body2->GetTransform();
		quads->sprite(id::spr::RectAnim1, a.p.x, a.p.y, rot(a.q.c, a.q.s), iColor::White);
	}
}
#endif

namespace entities
{
	entt::registry world;

	void init(int entities_n) 
	{
		using namespace ecs;
		auto group = world.group<pos2d, vel2d>();
		auto spr = sprite{ id::spr::RectAnim1, 2 ,0 };
		auto bounds = aabb(14, 32, win::w - 10, win::h);
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

	void update(float dt) {
		const aabb bounds(14, 32, win::sw - 10, win::sh);
		ecs::move::step_in_aabb(world, dt, bounds);
		ecs::system::UpdateFlipbooks(world, dt);
	}

	void render() 
	{
		using namespace ecs;
		auto view = world.group<pos2d, vel2d>(entt::get<flipbook>);
		auto quads = gfx2d::quad::batcher;
		const u32 colors[] = { 0x8000A5FF, 0x80800080, 0x80CBC0FF };
		for (auto entity : view) 
		{
			const auto& [position,animation] = view.get<pos2d, flipbook>(entity);
			quads->sprite(animation.getFrame(), position.x, position.y, colors[animation.frameOffset]);
		}
	}
}

void setupCommandBuffer(float sh, float sw) 
{

}

namespace bufferedCommands {

	constexpr int buffersize = 512;
	u8 bufferData[buffersize];
	gfx2d::command::buffer commandbuffer(bufferData, buffersize);

	void setup(float x, float y) {
		commandbuffer.reset();
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

	void render() {
		commandbuffer.execute();
	}
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
	bufferedCommands::setup(w, h);
	entities::init(4000);
	WithBox2D(box2d::init());
	return 0;
}

void app::on_resize(int x, int y)
{
	using namespace win;
	w = x;
	h = y;
	sw = (x / gfx2d::Pipe2d.scaling);
	sh = (y / gfx2d::Pipe2d.scaling);
	setupCommandBuffer(sw,sh);
}

void app::on_update(float dt)
{
	entities::update(dt);
	WithBox2D(box2d::update(dt);)
}

void app::on_render(float dt) {
	//kinc_g4_viewport(0, 0, win::w, win::h);
	//kinc_g4_scissor(0, 0, win::w, win::h);
	//kinc_g4_viewport(0, 0, 1280, 720);
	//kinc_g4_scissor(0, 0, 1280,720);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF808080, 0, 0);
	//kinc_g4_clear(KINC_G4_CLEAR_COLOR, iColor((u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255)), 0, 0);
	
	auto quads = gfx2d::quad::batcher;
	quads->set_atlas(gfx2d::quad::atlases::gui);
	quads->begin();

	entities::render();
	//----------------------------------------------------------------
	bufferedCommands::render();
	//----------------------------------------------------------------------
	WithBox2D(box2d::render());
	//----------------------------------------------------------------------
	char buffer[20];
	sprintf(buffer, "%d", (int)(1.f/dt));
	auto tc = textCtx(10, 10);
	quads->text(tc, buffer, iColor::Black);
	quads->end();
}

#ifdef KISS_IMGUI	
void app::on_imgui(float dt) {

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::End();

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}
#endif

void app::on_shutdown() {
	WithBox2D(delete box2d::world);
}