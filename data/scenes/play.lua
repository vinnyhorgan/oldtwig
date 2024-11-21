local Scene = require("core.scene")
local core = require("core")
local Area = require("objects.area")
local scenemanager = require("core.scenemanager")
local Player = require("objects.player")

local Text = require("text")

local Play = Scene:extend()

function Play:enter()
    self.player = Player(core.test_spritesheet, 4, 28)
    box = Text.new("left", { color = { 1, 1, 1, 1 }})
    script_active = false
end

function Play:update()
    box:update(0.016)

    core.areas[self.player.area]:draw()

    if script_active then
        script_update()
    end

    self.player:draw()

    if script_active then
        graphics.fill_rect(-1, core.HEIGHT - 100 - 1, core.WIDTH + 2, 100 + 2, { 0, 0, 0, 200 })
        box:draw(10, core.HEIGHT - 90)
    end

    if responses then
        local top = core.HEIGHT - 10 - 10 * #responses
        for i=1, #responses do
            local color = { 0, 0, 0 }
            if i == ans then
                color = { 255, 255, 255, 200 }
            end
            graphics.print(core.font, 10, top + 10 * (i - 1), "> " .. responses[i], color)
        end
    end
end

function Play:keypressed(key)
    if key == "escape" then
        scenemanager.switch("areas")
    end

    if self.options then
        if key == "w" and self.current_option > 1 then
            self.current_option = self.current_option - 1
        elseif key == "s" and self.current_option < #self.options then
            self.current_option = self.current_option + 1
        end
    end

    if not script_active then
        self.player:keypressed(key)

        if core.areas[self.player.area].events[self.player.x][self.player.y] then
            core.interacting_entity = core.areas[self.player.area].events[self.player.x][self.player.y]
            script_run(core.interacting_entity.script)
        end

        if key == "e" then
            if self.player.x > 1 and core.areas[self.player.area].events[self.player.x - 1][self.player.y] and core.areas[self.player.area].events[self.player.x - 1][self.player.y].collidable then
                core.interacting_entity = core.areas[self.player.area].events[self.player.x - 1][self.player.y]
                script_run(core.interacting_entity.script)
            elseif self.player.x < core.GRID_WIDTH and core.areas[self.player.area].events[self.player.x + 1][self.player.y] and core.areas[self.player.area].events[self.player.x + 1][self.player.y].collidable then
                core.interacting_entity = core.areas[self.player.area].events[self.player.x + 1][self.player.y]
                script_run(core.interacting_entity.script)
            elseif self.player.y > 1 and core.areas[self.player.area].events[self.player.x][self.player.y - 1] and core.areas[self.player.area].events[self.player.x][self.player.y - 1].collidable then
                core.interacting_entity = core.areas[self.player.area].events[self.player.x][self.player.y - 1]
                script_run(core.interacting_entity.script)
            elseif self.player.y < core.GRID_HEIGHT and core.areas[self.player.area].events[self.player.x][self.player.y + 1] and core.areas[self.player.area].events[self.player.x][self.player.y + 1].collidable then
                core.interacting_entity = core.areas[self.player.area].events[self.player.x][self.player.y + 1]
                script_run(core.interacting_entity.script)
            end
        end
    end
end

return Play
