local Scene = require("core.scene")
local core = require("core")
local Area = require("objects.area")
local scenemanager = require("core.scenemanager")

local Paint = Scene:extend()

function Paint:enter()
    self.setting = false
    self.deleting = false
end

function Paint:update()
    if self.setting then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.areas[core.current_area]:set_tile(grid_x, grid_y, core.current_tile)
    elseif self.deleting then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.areas[core.current_area]:set_tile(grid_x, grid_y, nil)
    end

    core.areas[core.current_area]:draw()

    local grid_x = math.floor(core.mouse_x / core.TILE_SIZE)
    local grid_y = math.floor(core.mouse_y / core.TILE_SIZE)
    local quad = core.current_tile.quad

    local color = { 255, 255, 255, 150 }

    if self.deleting then
        color = { 255, 0, 0, 150 }
    end

    graphics.blit_tint(core.current_tile.spritesheet, grid_x * core.TILE_SIZE, grid_y * core.TILE_SIZE, quad[1], quad[2], quad[3], quad[4], color)
end

function Paint:keypressed(key)
    if key == "escape" then
        scenemanager.switch("edit")
    end
end

function Paint:mousepressed(button, x, y)
    if button == "left" then
        self.setting = true
    elseif button == "right" then
        self.deleting = true
    end
end

function Paint:mousereleased(button, x, y)
    if button == "left" then
        self.setting = false
    elseif button == "right" then
        self.deleting = false
    end
end

return Paint
