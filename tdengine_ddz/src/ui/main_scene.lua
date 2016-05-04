
MAIN_SCENE_CLASS = class("MAIN_SCENE_CLASS", function()
    return display.newScene("MAIN_SCENE_CLASS")
end)

function MAIN_SCENE_CLASS:ctor()
    self:enableNodeEvents()
    self.gameAllSize = 0
    self.gameCurSize = 0
    self:onInit()
end

function MAIN_SCENE_CLASS:onInit()
    local layer = LOGIN_LAYER_CLASS:create()
    self:addChild(layer)
end

function MAIN_SCENE_CLASS:openMainLayer()
    self:removeAllChildren()
    local layer = MAIN_LAYER_CLASS:create()
    self:addChild(layer)
end

function MAIN_SCENE_CLASS:onCleanup_()

end