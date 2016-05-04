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

    local function touchEvent(sender,eventType)

        if eventType == ccui.TouchEventType.began then

            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ENTER_ROOM, {room_name = "ddz1"})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local textButton = ccui.Button:create()
    textButton:setTouchEnabled(true)
    textButton:setScale9Enabled(true)
    textButton:loadTextures("button.png", "buttonHighlighted.png", "")
    textButton:setContentSize(cc.size(180, textButton:getVirtualRendererSize().height * 1.5))
    textButton:setTitleText("进入房间")
    textButton:setPosition(cc.p(500,100))
    textButton:addTouchEventListener(touchEvent)
    self:addChild(textButton)
end

function MAIN_LAYER_CLASS:onCleanup_()

end