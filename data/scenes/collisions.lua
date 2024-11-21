local Scene = require("core.scene")
local core = require("core")
local Area = require("objects.area")
local scenemanager = require("core.scenemanager")

local Collisions = Scene:extend()

function Collisions:enter()
    self.setting = false
    self.deleting = false
end

function Collisions:update()
    if self.setting then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.areas[core.current_area]:set_collision(grid_x, grid_y, true)
    elseif self.deleting then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.areas[core.current_area]:set_collision(grid_x, grid_y, false)
    end

    core.areas[core.current_area]:draw()

    for x = 1, core.GRID_WIDTH do
        for y = 1, core.GRID_HEIGHT do
            if core.areas[core.current_area].collision[x][y] then
                graphics.fill_rect((x - 1) * core.TILE_SIZE, (y - 1) * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 0, 0, 255, 150 })
            end
        end
    end

    local grid_x = math.floor(core.mouse_x / core.TILE_SIZE)
    local grid_y = math.floor(core.mouse_y / core.TILE_SIZE)
    local quad = core.current_tile.quad

    local color = { 0, 0, 255, 150 }

    if self.deleting then
        color = { 255, 0, 0, 150 }
    end

    graphics.fill_rect(grid_x * core.TILE_SIZE, grid_y * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, color)
end

function Collisions:keypressed(key)
    if key == "escape" then
        scenemanager.switch("edit")
    end
end

function Collisions:mousepressed(button, x, y)
    if button == "left" then
        self.setting = true
    elseif button == "right" then
        self.deleting = true
    end
end

function Collisions:mousereleased(button, x, y)
    if button == "left" then
        self.setting = false
    elseif button == "right" then
        self.deleting = false
    end
end

return Collisions
