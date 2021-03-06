#include <entt/entt.hpp>
#include "pch.h"
#include <Kiss/systems/spriteMove.h>
#include <Kiss/systems/SpriteSheet.h>
#include "assets/data.h"

using namespace kiss;

ecs::flipbookData Anims[] = {
	{ 1.f, id::spr::RectAnim1, 3}
};

namespace ecstest 
{
	entt::registry world;

	void init(f32 w, f32 h, int numEntities)
	{
		using namespace ecs;
		auto group = world.group<pos2d, vel2d>();
		//auto spr = sprite{ id::spr::RectAnim1, 2 ,0 };
		auto bounds = aabb(14, 32, w - 10, h);
		flipbook::properties options{ 1,0,0,4,0 };
		for (auto i = 0; i < numEntities; ++i)
		{
			auto entity = world.create();
			auto pos = pos2d::rand(bounds);
			auto vel = vel2d::rand(-90, 90, (float)kinc_random_get_in(150, 500));
			auto s = vel.len() * 0.01f;
			world.emplace<pos2d>(entity, pos);
			world.emplace<vel2d>(entity, vel);
			flipbook::play(world, entity, &Anims[0], options, kinc_random_get_in(0, 20) * 0.1f, s);
		}
	}

	void update(f32 w, f32 h, float dt)
	{
		const aabb bounds(14, 32, w - 10, h);
		ecs::move::step_in_aabb(world, dt, bounds);
		ecs::system::UpdateFlipbooks(world, dt);
	}

	void render()
	{
		using namespace ecs;
		auto view = world.group<pos2d, vel2d>(entt::get<flipbook>);
		const iColor cMultiplier = 0x80203040;
		const iColor colors[] = { 0xFF00A5FF, 0xFF800080, 0xFFCBC0FF };
		const iColor dark[] = { mul(colors[0], cMultiplier),   mul(colors[1], cMultiplier),  mul(colors[2], cMultiplier) };
		auto& b = gfx2d::batcher;
		for (auto entity : view)
		{
			const auto& [position, animation] = view.get<pos2d, flipbook>(entity);
			b.vertexdata(colors[animation.frameOffset], colors[animation.frameOffset], dark[animation.frameOffset], dark[animation.frameOffset]);
			b.sprite(animation.getFrame(), position.x, position.y);
		}
	}
}