local Scene = require("core.scene")
local ui = require("core.ui")
local core = require("core")
local Area = require("objects.area")
local scenemanager = require("core.scenemanager")

local Edit = Scene:extend()

function Edit:update()
    core.areas[core.current_area]:draw()

    graphics.fill_rect(-1, -1, core.WIDTH + 2, core.HEIGHT + 2, { 0, 0, 0, 200 })

    if ui.button(20, 20, 20, 20, "<", 1) then
        scenemanager.switch("areas")
    end

    if ui.button(core.WIDTH - 40, 20, 20, 20, " ") then
        scenemanager.switch("spritesheet")
    end

    graphics.fill_rect(core.WIDTH - 40, 60, 20, 20, core.areas[core.current_area].background)

    if ui.button(core.WIDTH - 40, 60, 20, 20, " ") then
        scenemanager.switch("background")
    end

    local quad = core.current_tile.quad
    graphics.blit_alpha(core.current_tile.spritesheet, core.WIDTH - 40 + 2, 20 + 2, quad[1], quad[2], quad[3], quad[4])

    if ui.button(core.WIDTH / 2 - 35, core.HEIGHT - 40, 70, 20, "Walls") then
        scenemanager.switch("collisions")
    end

    if ui.button(core.WIDTH - 90, core.HEIGHT - 40, 70, 20, "B.ground") then
        scenemanager.switch("paint")
    end

    if ui.button(20, core.HEIGHT - 40, 70, 20, "Events") then
        scenemanager.switch("paintentities")
    end
end

function Edit:keypressed(key)
    if key == "escape" then
        scenemanager.switch("areas")
    end
end

return Edit
