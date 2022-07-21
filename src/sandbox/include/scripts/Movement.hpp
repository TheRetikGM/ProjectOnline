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
		if (KeyHeld(Ren::Key::W))
			transform.position += Ren::UpDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::S))
			transform.position -= Ren::UpDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::A))
			transform.position -= Ren::RightDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::D))
			transform.position += Ren::RightDir() * move_speed * dt;
    }
};