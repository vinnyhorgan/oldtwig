local Object = require("core.object")

local Scene = Object:extend()

function Scene:new()
end

function Scene:enter()
end

function Scene:exit()
end

function Scene:update()
end

function Scene:keypressed(key)
end

function Scene:textinput(text)
end

function Scene:mousepressed(button, x, y)
end

function Scene:mousereleased(button, x, y)
end

return Scene
