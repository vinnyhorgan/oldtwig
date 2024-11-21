local Scene = require("core.scene")
local ui = require("core.ui")
local scenemanager = require("core.scenemanager")
local core = require("core")
local Area = require("objects.area")

local Areas = Scene:extend()

function Areas:enter()
    self.editing = false
    self.buffer = ""
end

function Areas:update()
    core.areas[core.current_area]:draw()

    graphics.fill_rect(-1, -1, core.WIDTH + 2, core.HEIGHT + 2, { 0, 0, 0, 200 })

    if self.editing then
        if ui.button(core.WIDTH / 2 - 50, 20, 100, 30, self.buffer) then
        end
    else
        if ui.button(core.WIDTH / 2 - 50, 20, 100, 30, core.areas[core.current_area].name) then
            self.editing = true
        end
    end

    if ui.button(core.WIDTH / 2 - 30, core.HEIGHT - 40, 60, 20, "Edit") then
        scenemanager.switch("edit")
    end

    if ui.button(20, core.HEIGHT - 40, 20, 20, "+", 1, 1) then
        table.insert(core.areas, Area("New Area " .. #core.areas + 1))
        core.current_area = #core.areas
        self.editing = false
        self.buffer = ""
    end

    if ui.button(core.WIDTH - 70, core.HEIGHT - 40, 20, 20, "") then
        scenemanager.switch("play")
    end

    graphics.blit_alpha(core.play_icon, core.WIDTH - 70 + 6, core.HEIGHT - 40 + 6)

    if ui.button(core.WIDTH - 40, core.HEIGHT - 40, 20, 20, "") then
        scenemanager.switch("play")
    end

    graphics.blit_alpha(core.settings_icon, core.WIDTH - 40 + 6, core.HEIGHT - 40 + 6)

    if #core.areas > 1 then
        if ui.button(20, core.HEIGHT / 2 - 10, 20, 20, "<", 1) then
            if core.current_area == 1 then
                core.current_area = #core.areas
            else
                core.current_area = core.current_area - 1
            end

            self.editing = false
            self.buffer = ""
        end

        if ui.button(core.WIDTH - 40, core.HEIGHT / 2 - 10, 20, 20, ">", 1) then
            if core.current_area == #core.areas then
                core.current_area = 1
            else
                core.current_area = core.current_area + 1
            end

            self.editing = false
            self.buffer = ""
        end
    end
end

function Areas:keypressed(key)
    if self.editing then
        if key == "return" then
            core.areas[core.current_area].name = self.buffer
            self.editing = false
            self.buffer = ""
        elseif key == "backspace" then
            local byteoffset = utf8.offset(self.buffer, -1)
            if byteoffset then
                self.buffer = string.sub(self.buffer, 1, byteoffset - 1)
            end
        end
    end
end

function Areas:textinput(text)
    if self.editing and core.font:text_width(self.buffer .. text) < 90 then
        self.buffer = self.buffer .. text
    end
end

return Areas
