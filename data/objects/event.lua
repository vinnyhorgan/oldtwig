local Object = require("core.object")

local Event = Object:extend()

function Event:new(script)
    self.script = script
    self.collidable = false
end

return Event
