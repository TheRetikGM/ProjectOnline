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
        b2Body* rig = GetComponent<Ren::RigidBodyComponent>().p_body;

        float force = 100.0f;
        float jump_imp = 50.0f;
        if (KeyPressed(Ren::Key::W))
            rig->ApplyLinearImpulse({ 0.0f, jump_imp }, rig->GetWorldCenter(), true);
        if (KeyHeld(Ren::Key::S))
            rig->ApplyForce({ 0.0f, -force }, rig->GetWorldCenter(), true);
        if (KeyHeld(Ren::Key::A))
            rig->ApplyForce({ -force, 0.0f }, rig->GetWorldCenter(), true);
        if (KeyHeld(Ren::Key::D))
            rig->ApplyForce({ force, 0.0f }, rig->GetWorldCenter(), true);
    }
};