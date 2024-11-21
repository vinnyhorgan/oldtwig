local Scene = require("core.scene")
local core = require("core")
local scenemanager = require("core.scenemanager")
local Tile = require("objects.tile")

local Spritesheet = Scene:extend()

function Spritesheet:update()
    graphics.blit_alpha(core.test_spritesheet, core.spritesheet_ox, core.spritesheet_oy)

    local grid_x = math.floor(core.mouse_x / core.TILE_SIZE)
    local grid_y = math.floor(core.mouse_y / core.TILE_SIZE)
    graphics.rect(grid_x * core.TILE_SIZE, grid_y * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE, { 255, 0, 0, 150 })
end

function Spritesheet:keypressed(key)
    if key == "escape" then
        scenemanager.switch("edit")
    end

    if key == "w" then
        if core.spritesheet_oy < 0 then
            core.spritesheet_oy = core.spritesheet_oy + core.TILE_SIZE
        end
    elseif key == "s" then
        if core.spritesheet_oy > -core.test_spritesheet:get_height() + core.HEIGHT then
            core.spritesheet_oy = core.spritesheet_oy - core.TILE_SIZE
        end
    elseif key == "a" then
        if core.spritesheet_ox < 0 then
            core.spritesheet_ox = core.spritesheet_ox + core.TILE_SIZE
        end
    elseif key == "d" then
        if core.spritesheet_ox > -core.test_spritesheet:get_width() + core.WIDTH then
            core.spritesheet_ox = core.spritesheet_ox - core.TILE_SIZE
        end
    end
end

function Spritesheet:mousepressed(button, x, y)
    local sheet_x = math.floor((core.mouse_x - core.spritesheet_ox) / core.TILE_SIZE)
    local sheet_y = math.floor((core.mouse_y - core.spritesheet_oy) / core.TILE_SIZE)
    core.current_tile = Tile(core.test_spritesheet, sheet_x, sheet_y)
    scenemanager.switch("edit")
end

return Spritesheet
