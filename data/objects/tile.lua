local Object = require("core.object")
local core = require("core")

local Tile = Object:extend()

function Tile:new(spritesheet, sheet_x, sheet_y)
    self.spritesheet = spritesheet
    self.quad = { sheet_x * core.TILE_SIZE, sheet_y * core.TILE_SIZE, core.TILE_SIZE, core.TILE_SIZE }
end

return Tile
