function Test:OnInit()
    self.time = 0
    self.orig_ppu = Sprite.ppu
end

function Test:OnUpdate(dt)
    local dir = Vec2:new(0, 0)

    if (KeyHeld(KEY_W)) then dir.y = dir.y + 1 end
    if (KeyHeld(KEY_S)) then dir.y = dir.y - 1 end
    if (KeyHeld(KEY_A)) then dir.x = dir.x - 1 end
    if (KeyHeld(KEY_D)) then dir.x = dir.x + 1 end

    if (dir ~= {0, 0}) then
        local norm_dir = dir / #dir
        Transform.position = Transform.position + norm_dir * dt * 10
    end

    local new_rot = Transform.rotation - 180 * dt
    if (new_rot > 360) then new_rot = new_rot - 360 end
    Transform.rotation = new_rot

    self.time = self.time + dt
    Sprite.ppu = self.orig_ppu + self.orig_ppu * math.abs(math.sin(self.time))
end

