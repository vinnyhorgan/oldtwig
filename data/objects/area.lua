local Object = require("core.object")

local Area = Object:extend()

function Area:new(name)
    self.name = name
    self.background = { math.random(150, 255), math.random(150, 255), math.random(150, 255) }

    self.grid = {}

    for x = 1, core.GRID_WIDTH do
        self.grid[x] = {}
        for y = 1, core.GRID_HEIGHT do
            self.grid[x][y] = nil
        end
    end

    self.events = {}

    for x = 1, core.GRID_WIDTH do
        self.events[x] = {}
        for y = 1, core.GRID_HEIGHT do
            self.events[x][y] = nil
        end
    end

    self.collision = {}

    for x = 1, core.GRID_WIDTH do
        self.collision[x] = {}
        for y = 1, core.GRID_HEIGHT do
            self.collision[x][y] = false
        end
    end
end

function Area:set_tile(x, y, tile)
    if x >= 1 and x <= core.GRID_WIDTH and y >= 1 and y <= core.GRID_HEIGHT then
        self.grid[x][y] = tile
    end
end

function Area:set_event(x, y, event)
    if x >= 1 and x <= core.GRID_WIDTH and y >= 1 and y <= core.GRID_HEIGHT then
        self.events[x][y] = event
    end
end

function Area:set_collision(x, y, collision)
    if x >= 1 and x <= core.GRID_WIDTH and y >= 1 and y <= core.GRID_HEIGHT then
        self.collision[x][y] = collision
    end
end

function Area:draw()
    graphics.clear(self.background)

    for x = 1, core.GRID_WIDTH do
        for y = 1, core.GRID_HEIGHT do
            if self.grid[x][y] then
                local quad = self.grid[x][y].quad
                graphics.blit_alpha(self.grid[x][y].spritesheet, (x - 1) * core.TILE_SIZE, (y - 1) * core.TILE_SIZE, quad[1], quad[2], quad[3], quad[4])
            end
        end
    end

    for x = 1, core.GRID_WIDTH do
        for y = 1, core.GRID_HEIGHT do
            if self.events[x][y] then
                --local quad = self.entities[x][y].quad
                --graphics.blit_alpha(self.entities[x][y].spritesheet, (x - 1) * core.TILE_SIZE, (y - 1) * core.TILE_SIZE, quad[1], quad[2], quad[3], quad[4])
            end
        end
    end
end

return Area
