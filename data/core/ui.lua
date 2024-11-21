local ui = {
    pressed = false
}

local core = require("core")

function ui.button(x, y, w, h, text, text_ox, text_oy)
    text_ox = text_ox or 0
    text_oy = text_oy or 0

    local color = { 30, 30, 30, 100 }

    if core.mouse_x > x and core.mouse_x < x + w and core.mouse_y > y and core.mouse_y < y + h then
        color = { 0, 0, 0, 100 }
    end

    -- background
    graphics.fill_rect(x, y, w, h, color)

    -- corners
    graphics.plot(x + 1, y + 1, { 0, 0, 0 })
    graphics.plot(x + w - 2, y + 1, { 0, 0, 0 })
    graphics.plot(x + 1, y + h - 2, { 0, 0, 0 })
    graphics.plot(x + w - 2, y + h - 2, { 0, 0, 0 })

    -- lines
    graphics.line(x + 2, y, x + w - 2, y, { 0, 0, 0 })
    graphics.line(x + 2, y + h - 1, x + w - 2, y + h - 1, { 0, 0, 0 })
    graphics.line(x, y + 2, x, y + h - 2, { 0, 0, 0 })
    graphics.line(x + w - 1, y + 2, x + w - 1, y + h - 2, { 0, 0, 0 })

    graphics.print(core.font, (x + w / 2 - core.font:text_width(text) / 2) + text_ox, (y + h / 2 - core.font:text_height(text) / 2) + text_oy, text, { 255, 255, 255 })
    return core.mouse_x > x and core.mouse_x < x + w and core.mouse_y > y and core.mouse_y < y + h and ui.pressed
end

return ui
