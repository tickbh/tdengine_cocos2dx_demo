DDZ_DESK_LAYER_CLASS = class("DDZ_DESK_LAYER_CLASS", function()
    return display.newScene("DDZ_DESK_LAYER_CLASS")
end)


function DDZ_DESK_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
    self:onInit()
end

function DDZ_DESK_LAYER_CLASS:onInit()
    print("DDZ_DESK_LAYER_CLASS init")
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Ddz Desk Layer", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    local function touchEvent(sender,eventType)

        if eventType == ccui.TouchEventType.began then

            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then



            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "ready"})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end
    local poker = POKER_SPRITE_CLASS:create({id=0x12})
    poker:setPosition(cc.p(500,200))
    self:addChild(poker)
    local ready_btn = ccui.Button:create()
    ready_btn:setTouchEnabled(true)
    ready_btn:setScale9Enabled(true)
    ready_btn:loadTextures("button.png", "buttonHighlighted.png", "")
    ready_btn:setContentSize(cc.size(180, ready_btn:getVirtualRendererSize().height * 1.5))
    ready_btn:setTitleText("准备")
    ready_btn:setPosition(cc.p(500,100))
    ready_btn:addTouchEventListener(touchEvent)
    self:addChild(ready_btn)
    self.ready_btn = ready_btn
end

function DDZ_DESK_LAYER_CLASS:enter_desk()
    trace("DDZ_DESK_LAYER_CLASS:enter_desk")
end

function DDZ_DESK_LAYER_CLASS:room_msg_receive(user, oper, info)
    trace("DDZ_DESK_LAYER_CLASS:room_msg_receive %o %o", oper, info)
    if oper == "poker_init" then
        for i,poker in ipairs(info.poker_list or {}) do
            local poker = POKER_SPRITE_CLASS:create({id=poker})
            poker:setPosition(cc.p(200 + 30 * i,200))
            self:addChild(poker)
        end
    end
end

function DDZ_DESK_LAYER_CLASS:onCleanup_()

end

function DDZ_DESK_LAYER_CLASS:onEnter_()
    register_as_audience(self.uid, {EVENT_ENTER_DESK={func = self.enter_desk, args = {self} }})
    register_as_audience(self.uid, {EVENT_ROOM_MSG_RECEIVE={func = self.room_msg_receive, args = {self} }})
end

function DDZ_DESK_LAYER_CLASS:onExit_()
    remove_audience_from_raiser(self.uid)
end