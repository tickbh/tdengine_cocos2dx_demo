MAIN_LAYER_CLASS = class("MAIN_LAYER_CLASS", function()
    return display.newScene("MAIN_LAYER_CLASS")
end)


function MAIN_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.gameAllSize = 0
    self.gameCurSize = 0
    self:onInit()
end

function MAIN_LAYER_CLASS:onInit()
    print("MAIN_LAYER_CLASS init")
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Enter Main Layer", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

end

function MAIN_LAYER_CLASS:onCleanup_()

end