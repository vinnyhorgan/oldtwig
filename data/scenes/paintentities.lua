local Scene = require("core.scene")
local core = require("core")
local Event = require("objects.event")
local scenemanager = require("core.scenemanager")

local PaintEntities = Scene:extend()

function PaintEntities:enter()

end

function PaintEntities:update()
    core.areas[core.current_area]:draw()

    for x = 1, core.GRID_WIDTH do
        for y = 1, core.GRID_HEIGHT do
            if core.areas[core.current_area].events[x][y] then
                graphics.rect((x - 1) * core.TILE_SIZE, (y - 1) * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 0, 255, 0, 150 })
                graphics.fill_rect((x - 1) * core.TILE_SIZE, (y - 1) * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 0, 255, 0, 100 })
            end
        end
    end

    if core.current_area == core.player_spawn.area then
        graphics.rect((core.player_spawn.x - 1) * core.TILE_SIZE, (core.player_spawn.y - 1) * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 0, 0, 255, 150 })
        graphics.fill_rect((core.player_spawn.x - 1) * core.TILE_SIZE, (core.player_spawn.y - 1) * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 0, 0, 255, 100 })
    end
end

function PaintEntities:keypressed(key)
    if key == "escape" then
        scenemanager.switch("edit")
    end
end

function PaintEntities:mousepressed(button, x, y)
    if button == "left" then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.areas[core.current_area]:set_event(grid_x, grid_y, Event())
    elseif button == "middle" then
        local grid_x = math.floor(core.mouse_x / core.TILE_SIZE) + 1
        local grid_y = math.floor(core.mouse_y / core.TILE_SIZE) + 1
        core.player_spawn.x = grid_x
        core.player_spawn.y = grid_y
        core.player_spawn.area = core.current_area
    end
end

return PaintEntities
