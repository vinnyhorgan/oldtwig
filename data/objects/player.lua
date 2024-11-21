local Tile = require("objects.tile")

local Player = Tile:extend()

function Player:new(spritesheet, sheet_x, sheet_y)
    Player.super.new(self, spritesheet, sheet_x, sheet_y)
    self.x = core.player_spawn.x
    self.y = core.player_spawn.y
    self.area = core.player_spawn.area
end

function Player:draw()
    local quad = self.quad
    graphics.blit_alpha(self.spritesheet, (self.x - 1) * core.TILE_SIZE, (self.y - 1) * core.TILE_SIZE, quad[1], quad[2], quad[3], quad[4])
end

function Player:keypressed(key)
    if key == "w" then
        if self.y > 1 and core.areas[self.area].collision[self.x][self.y - 1] == false then
            if core.areas[self.area].events[self.x][self.y - 1] then
                if core.areas[self.area].events[self.x][self.y - 1].collidable == false then
                    self.y = self.y - 1
                end
            else
                self.y = self.y - 1
            end
        end
    elseif key == "s" then
        if self.y < core.GRID_HEIGHT and core.areas[self.area].collision[self.x][self.y + 1] == false then
            if core.areas[self.area].events[self.x][self.y + 1] then
                if core.areas[self.area].events[self.x][self.y + 1].collidable == false then
                    self.y = self.y + 1
                end
            else
                self.y = self.y + 1
            end
        end
    elseif key == "a" then
        if self.x > 1 and core.areas[self.area].collision[self.x - 1][self.y] == false then
            if core.areas[self.area].events[self.x - 1][self.y] then
                if core.areas[self.area].events[self.x - 1][self.y].collidable == false then
                    self.x = self.x - 1
                end
            else
                self.x = self.x - 1
            end
        end
    elseif key == "d" then
        if self.x < core.GRID_WIDTH and core.areas[self.area].collision[self.x + 1][self.y] == false then
            if core.areas[self.area].events[self.x + 1][self.y] then
                if core.areas[self.area].events[self.x + 1][self.y].collidable == false then
                    self.x = self.x + 1
                end
            else
                self.x = self.x + 1
            end
        end
    end
end

return Player
