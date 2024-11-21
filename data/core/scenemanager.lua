local scenemanager = {
    scenes = {},
    current = nil
}

function scenemanager.register(name, scene)
    scenemanager.scenes[name] = scene
end

function scenemanager.switch(name)
    if scenemanager.scenes[name] then
        if scenemanager.current then
            scenemanager.current:exit()
        end

        scenemanager.current = scenemanager.scenes[name]
        scenemanager.current:enter()
    end
end

function scenemanager.update()
    if scenemanager.current then
        scenemanager.current:update()
    end
end

function scenemanager.keypressed(key)
    if scenemanager.current then
        scenemanager.current:keypressed(key)
    end
end

function scenemanager.textinput(text)
    if scenemanager.current then
        scenemanager.current:textinput(text)
    end
end

function scenemanager.mousepressed(button, x, y)
    if scenemanager.current then
        scenemanager.current:mousepressed(button, x, y)
    end
end

function scenemanager.mousereleased(button, x, y)
    if scenemanager.current then
        scenemanager.current:mousereleased(button, x, y)
    end
end

return scenemanager
