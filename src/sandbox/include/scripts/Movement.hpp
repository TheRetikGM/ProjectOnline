#pragma once
#include <Ren/ECS/NativeScript.h>
#include "sandbox.h"

class MovementScript : public Ren::NativeScript
{
public:
    void OnInit() override
    {
        SAND_STATUS("Native movement script initialized.");
    }

    void OnDestroy() override
    {
        SAND_STATUS("Native movement script destroyed.");
    }

    void OnUpdate(float dt) override
    {
        auto& transform = GetComponent<Ren::TransformComponent>();

		float move_speed = 5.0f;
		if (KeyHeld(SDLK_w))
			transform.position += Ren::UpDir() * move_speed * dt;
		if (KeyHeld(SDLK_s))
			transform.position -= Ren::UpDir() * move_speed * dt;
		if (KeyHeld(SDLK_a))
			transform.position -= Ren::RightDir() * move_speed * dt;
		if (KeyHeld(SDLK_d))
			transform.position += Ren::RightDir() * move_speed * dt;
    }
};