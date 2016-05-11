
MAIN_SCENE_CLASS = class("MAIN_SCENE_CLASS", function()
    return display.newScene("MAIN_SCENE_CLASS")
end)

function MAIN_SCENE_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
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

function MAIN_SCENE_CLASS:openDdzRoomLayer()
    self:removeAllChildren()
    local layer = DDZ_ROOM_LAYER_CLASS:create()
    self:addChild(layer)
end

function MAIN_SCENE_CLASS:openDdzDeskLayer()
    self:removeAllChildren()
    local layer = DDZ_DESK_LAYER_CLASS:create()
    self:addChild(layer)
end

function MAIN_SCENE_CLASS:onCleanup_()

end

function MAIN_SCENE_CLASS:onEnter_()
    register_as_audience(self.uid, {EVENT_ENTER_GAME={func = self.openMainLayer, args = {self} }})
    register_as_audience(self.uid, {EVENT_ENTER_ROOM={func = self.openDdzRoomLayer, args = {self} }})
    register_as_audience(self.uid, {EVENT_ENTER_DESK={func = self.openDdzDeskLayer, args = {self} }})
end

function MAIN_SCENE_CLASS:onExit_()
    remove_audience_from_raiser(self.uid)
end