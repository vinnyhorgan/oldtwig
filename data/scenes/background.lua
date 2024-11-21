local Scene = require("core.scene")
local scenemanager = require("core.scenemanager")
local core = require("core")

local Background = Scene:extend()

function Background:enter()
    local background = core.areas[core.current_area].background
    self.sliders = {
        r = {value = background[1], x = core.WIDTH / 2 - 50, y = core.HEIGHT / 2 - 30, width = 100, color = { 255, 0, 0 }},
        g = {value = background[2], x = core.WIDTH / 2 - 50, y = core.HEIGHT / 2, width = 100, color = { 0, 255, 0 }},
        b = {value = background[3], x = core.WIDTH / 2 - 50, y = core.HEIGHT / 2 + 30, width = 100, color = { 0, 0, 255 }}
    }
    self.dragging = nil
end

function Background:update()
    local background = core.areas[core.current_area].background
    background[1] = self.sliders.r.value
    background[2] = self.sliders.g.value
    background[3] = self.sliders.b.value

    if self.dragging then
        local slider = self.sliders[self.dragging]
        local new_value = math.max(0, math.min(255, ((core.mouse_x - slider.x) / slider.width) * 255))
        slider.value = new_value
    end

    core.areas[core.current_area]:draw()

    graphics.fill_rect(core.WIDTH / 2 - 60, core.HEIGHT / 2 - 60, 120, 120, { 0, 0, 0, 200 })

    for _, slider in pairs(self.sliders) do
        graphics.fill_rect(slider.x, slider.y, slider.width, 10, { 100, 100, 100, 200 })

        local handle_x = slider.x + (slider.value / 255) * slider.width
        graphics.fill_rect(handle_x - 5, slider.y - 5, 10, 20, slider.color)
    end
end

function Background:mousepressed(button, x, y)
    if button == "left" then
        for name, slider in pairs(self.sliders) do
            local handle_x = slider.x + (slider.value / 255) * slider.width
            if x >= handle_x - 5 and x <= handle_x + 5 and y >= slider.y - 5 and y <= slider.y + 10 + 5 then
                self.dragging = name
            end
        end
    end
end

function Background:mousereleased(button, x, y)
    if button == "left" then
        self.dragging = nil
    end
end

function Background:keypressed(key)
    if key == "escape" then
        scenemanager.switch("edit")
    end
end

return Background
