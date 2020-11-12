#include <pch.h>
#include <Kiss/app.h>
#include <entt/entt.hpp>
#include <Kiss/components/sprite.h>
#include <Kiss/render/commands.h>
#include <Kiss/systems/spriteMove.h>
#include <Kiss/systems/SpriteSheet.h>
#include <kinc/graphics4/graphics.h>
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

	float elapsed = 0;
}

#ifdef KISS_BOX2D
namespace box2d 
{
	b2Vec2 gravity(0.0f, 9800.0f);
	b2World* world;
	b2Body* groundBody;
	b2Body* body;
	b2Body* body2;

	void init() {
		world = new b2World(gravity);
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(640.f, 500.0f);
		groundBodyDef.angle = deg2rad(0);
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
		fixtureDef.friction = 0.75f;
		fixtureDef.restitution = 1.f;
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		
		bodyDef.position.Set(100.0f, 100.0f);
		body = world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		bodyDef.position.Set(100.0f, 132.0f);
		body2 = world->CreateBody(&bodyDef);
		//body2->CreateFixture(&fixtureDef);
	}

	void step(float dt) {
		world->Step(dt, 6, 2);
	}

	void render() {
		auto a = body->GetTransform();
		auto quads = gfx2d::quad::batcher;
		quads->sprite(id::spr::RectAnim1, a.p.x, a.p.y, rot(a.q.c, a.q.s), iColor::White);
		a = body2->GetTransform();
		quads->sprite(id::spr::RectAnim1, a.p.x, a.p.y, rot(a.q.c, a.q.s), iColor::White);
	}
}
#endif



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
	setupCommandBuffer();
	entities::init(registry, 4000);
	WithBox2D(box2d::init());
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
	gfx2d::resize((float)w, (float)h, scale);
	commandbuffer.reset();
	setupCommandBuffer();
}

void app::update(float dt) 
{
	const aabb bounds(14, 32, win::sw - 10, win::sh);
	ecs::move::step_in_aabb(registry, dt, bounds);
	ecs::system::UpdateFlipbooks(registry, dt);
	WithBox2D(box2d::step(dt);)
}

void app::render(float dt) {
	kinc_g4_viewport(0, 0, win::w, win::h);
	kinc_g4_scissor(0, 0, win::w, win::h);
	//kinc_g4_scissor(0, 0, 1280, 720);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF808080, 0, 0);
	//kinc_g4_clear(KINC_G4_CLEAR_COLOR, iColor((u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255), (u8)kinc_random_get_in(40, 255)), 0, 0);
	
	auto quads = gfx2d::quad::batcher;
	quads->set_atlas(gfx2d::quad::atlases::gui);
	quads->begin();
	//entities::render(dt, registry, quads);
	//----------------------------------------
	quads->scale9(id::s9::Test, aabb(0.f, 0.f, 200.f, 200.f), iColor::White);
	//----------------------------------------------------------------
	commandbuffer.execute();
	//----------------------------------------------------------------------
	elapsed += dt*360;
	quads->sprite(id::spr::RectAnim1, 100, 100, rot(deg2rad(elapsed)), iColor::White);
	WithBox2D(box2d::render());
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
	WithBox2D(delete box2d::world);
}