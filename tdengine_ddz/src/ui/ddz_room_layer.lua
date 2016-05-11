DDZ_ROOM_LAYER_CLASS = class("DDZ_ROOM_LAYER_CLASS", function()
    return display.newScene("DDZ_ROOM_LAYER_CLASS")
end)


function DDZ_ROOM_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
    self:onInit()
end

function DDZ_ROOM_LAYER_CLASS:onInit()
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Ddz Room Layer", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "enter_desk", {})
        end
    end

    local textButton = ccui.Button:create()
    textButton:setTouchEnabled(true)
    textButton:setScale9Enabled(true)
    textButton:loadTextures("button.png", "buttonHighlighted.png", "")
    textButton:setContentSize(cc.size(180, textButton:getVirtualRendererSize().height * 1.5))
    textButton:setTitleText("开始游戏")
    textButton:setPosition(cc.p(500,100))
    textButton:addTouchEventListener(touchEvent)
    self:addChild(textButton)
end

function DDZ_ROOM_LAYER_CLASS:enter_desk()
    trace("DDZ_ROOM_LAYER_CLASS:enter_desk")
end

function DDZ_ROOM_LAYER_CLASS:onCleanup_()

end

function DDZ_ROOM_LAYER_CLASS:onEnter_()
    register_as_audience(self.uid, {EVENT_ENTER_DESK={func = self.enter_desk, args = {self} }})
end

function DDZ_ROOM_LAYER_CLASS:onExit_()
    remove_audience_from_raiser(self.uid)
end