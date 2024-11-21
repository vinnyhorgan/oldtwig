local scenemanager
local ui
local Areas
local Edit
local Paint
local Play
local Area
local Tile
local Collisions
local Event
local PaintEntities
local Spritesheet
local Background

local core = {}

-- dialogue globals (awful solution)
responses = nil
ans = 1
routine = nil
script_active = false

up_pressed = false
down_pressed = false

function script_run(func)
    routine = coroutine.create(function()
        script_active = true
        func()
        script_active = false
    end)
    coroutine.resume(routine)
end

function script_update()
    coroutine.resume(routine)
end

function reveal_text(str)
    box:send(str, core.WIDTH - 20)

    repeat
        if up_pressed then
            box:send(str, core.WIDTH - 20, true)
        end

        coroutine.yield()
    until box:is_finished()
end

function say(str)
    reveal_text(str)
    repeat
        coroutine.yield()
    until ui.pressed
    text = nil
end

function ask(str, ...)
    reveal_text(str)
    responses = {...}
    ans = 1
    repeat
        coroutine.yield()
        if up_pressed and ans > 1 then
            ans = ans -1
        elseif down_pressed and ans < #responses then
            ans = ans + 1
        end
    until ui.pressed
    text = nil
    responses = nil
end

function core.init()
    scenemanager = require("core.scenemanager")
    ui = require("core.ui")
    Areas = require("scenes.areas")
    Edit = require("scenes.edit")
    Paint = require("scenes.paint")
    Area = require("objects.area")
    Tile = require("objects.tile")
    Play = require("scenes.play")
    Collisions = require("scenes.collisions")
    Event = require("objects.event")
    PaintEntities = require("scenes.paintentities")
    Spritesheet = require("scenes.spritesheet")
    Background = require("scenes.background")

    core.cursor = graphics.image.load("assets/cursor.png")
    core.font = graphics.font.load("assets/ibm.png")
    core.test_spritesheet = graphics.image.load("assets/roguelike.png")
    core.play_icon = graphics.image.load("assets/play.png")
    core.settings_icon = graphics.image.load("assets/settings.png")

    core.WIDTH = graphics.get_width()
    core.HEIGHT = graphics.get_height()
    core.TILE_SIZE = 16
    core.GRID_WIDTH = core.WIDTH / core.TILE_SIZE
    core.GRID_HEIGHT = core.HEIGHT / core.TILE_SIZE

    core.spritesheet_ox = 0
    core.spritesheet_oy = 0

    core.mouse_x = 0
    core.mouse_y = 0
    system.show_cursor(false)

    core.areas = {}
    core.areas[1] = Area("Test Area")
    core.current_area = 1
    core.current_tile = Tile(core.test_spritesheet, 4, 4)
    core.player_spawn = { area = 1, x = 1, y = 1 }

    core.areas[1]:set_event(3, 3, Event(function()
        say "[bounce]Hellooooooo![/bounce]"
        ask("Are you ok?", "yes", "no")
        if ans == 1 then
            say "That's [rainbow]GREAT[/rainbow!"
        else
            say "I'm sorry to hear that..."
            say "Is there anything I can do to help?"

            ask("Can you help me?", "yes", "no")
            if ans == 1 then
                say "I'm glad to hear that!"
            else
                say "I'm sorry to hear that..."
            end
        end
    end))

    core.areas[1]:set_event(10, 3, Event(function()
        say "[shake]GO AWAY![/shake]"
    end))

    core.areas[1].events[10][3].collidable = true

    scenemanager.register("areas", Areas())
    scenemanager.register("edit", Edit())
    scenemanager.register("paint", Paint())
    scenemanager.register("play", Play())
    scenemanager.register("collisions", Collisions())
    scenemanager.register("paintentities", PaintEntities())
    scenemanager.register("spritesheet", Spritesheet())
    scenemanager.register("background", Background())
    scenemanager.switch("areas")

    math.randomseed(os.time())

    -- small compression test

    local src = io.open("./data/assets/roguelike.bmp", "rb")
    if not src then
        error("Could not open roguelike.bmp")
    end

    local d = src:read("*all")
    src:close()

    local comp = data.compress(d)
    if not comp then
        error("Could not compress roguelike.bmp")
    end

    local dst = io.open("roguelike.bin", "wb")
    if not dst then
        error("Could not open roguelike.bin")
    end

    dst:write(comp)
    dst:close()

    print("Compressed roguelike.bmp to roguelike.bin")

    local src = io.open("roguelike.bin", "rb")
    if not src then
        error("Could not open roguelike.bin")
    end

    local d = src:read("*all")
    src:close()

    local uncomp = data.uncompress(d)
    if not uncomp then
        error("Could not uncompress roguelike.bin")
    end

    local dst = io.open("uncomp_roguelike.bmp", "wb")
    if not dst then
        error("Could not open uncomp_roguelike.bmp")
    end

    dst:write(uncomp)
    dst:close()

    print("Uncompressed roguelike.bin to uncomp_roguelike.bmp")
end

function core.run()
    while true do
        local frame_start = system.get_time()

        ui.pressed = false
        up_pressed = false
        down_pressed = false

        for type, a,b,c,d in system.poll_event do
            if type == "quit" then
                return
            elseif type == "keypressed" then
                scenemanager.keypressed(a)

                if a == "w" then
                    up_pressed = true
                elseif a == "s" then
                    down_pressed = true
                end
            elseif type == "mousepressed" then
                if a == "left" then
                    ui.pressed = true
                end
                scenemanager.mousepressed(a, b, c)
            elseif type == "mousereleased" then
                scenemanager.mousereleased(a, b, c)
            elseif type == "mousemoved" then
                core.mouse_x = a
                core.mouse_y = b
            elseif type == "textinput" then
                scenemanager.textinput(a)
            end
        end

        graphics.clear({ 255 / 2, 255 / 2, 255 / 2 })

        scenemanager.update()

        if (core.mouse_x > 0 and core.mouse_x < graphics.get_width() - 1 and core.mouse_y > 0 and core.mouse_y < graphics.get_height() - 1) then
            graphics.blit_alpha(core.cursor, core.mouse_x, core.mouse_y)
        end

        graphics.update()

        local elapsed = system.get_time() - frame_start
        system.sleep(math.max(0, 1/60 - elapsed))
    end
end

return core
