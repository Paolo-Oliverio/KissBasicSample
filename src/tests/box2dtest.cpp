#ifdef KISS_BOX2D
#include "pch.h"
#include "box2dtest.h"
#include <box2d/box2d.h>
#include <assets/Data.h>
#include <kinc/math/random.h>


using namespace kiss;

namespace {
	b2World world = b2World({ 0.f, 720.f });
	float	elapsed = 0;
	b2Body* groundBody;
	b2Body* body;
	b2Body* body2;
}

namespace box2dtest {
	void init() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(640.f, 500.0f);
		groundBodyDef.angle = deg2rad(20);
		groundBody = world.CreateBody(&groundBodyDef);
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
		body = world.CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		bodyDef.position.Set(90.0f, 132.0f);
		body2 = world.CreateBody(&bodyDef);
		body2->CreateFixture(&fixtureDef);
	}

	void update(float dt)
	{
		elapsed += dt;

		if (elapsed > 10) {
			body->SetTransform(b2Vec2(100 + (float)kinc_random_get_in(-10, 10), 100), 0);
			body->SetAngularVelocity(0);
			body->SetLinearVelocity(b2Vec2(0, 0));
			body2->SetTransform(b2Vec2(100 + (float)kinc_random_get_in(-20, 20), 132), 0);
			body2->SetAngularVelocity(0);
			body2->SetLinearVelocity(b2Vec2(0, 0));
			elapsed = 0;
		}
		world.Step(dt, 10, 10);
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