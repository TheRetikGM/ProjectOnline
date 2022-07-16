#pragma once
#include <Ren/ECS/Scene.hpp>
#include "sandbox.h"

class MovementScript
{
public:
	MovementScript(Ren::Entity ent) : m_ent(ent) {}

	void OnAttach()
	{
		SAND_STATUS("MovementScript attached.");
	}
	void OnDetach()
	{
		SAND_STATUS("MovementScript detached.");
	}
	void OnUpdate(Ren::KeyInterface* input, float dt)
	{
		auto& transform = m_ent.Get<Ren::TransformComponent>();

		float move_speed = 500.0f;
		if (input->KeyHeld(SDLK_w))
			transform.position.y -= move_speed * dt;
		if (input->KeyHeld(SDLK_s))
			transform.position.y += move_speed * dt;
		if (input->KeyHeld(SDLK_a))
			transform.position.x -= move_speed * dt;
		if (input->KeyHeld(SDLK_d))
			transform.position.x += move_speed * dt;
	}
	void OnFixedUpdate(Ren::KeyInterface* input, float dt)
	{
		SAND_STATUS("MovementScript fixed updated.");
	}
private:
	Ren::Entity m_ent;
};