MAIN_LAYER_CLASS = class("MAIN_LAYER_CLASS", function()
    return display.newScene("MAIN_LAYER_CLASS")
end)


function MAIN_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
    self:onInit()
end

function MAIN_LAYER_CLASS:onInit()
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Enter Main Layer", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ENTER_ROOM, {room_name = "ddz1"})
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

function MAIN_LAYER_CLASS:room_detail(detail)
    detail.map_list = detail.map_list or {}
    trace("收到房间具体信息一共有%d房间", #detail.map_list)
    for i,v in ipairs(detail.map_list) do
        trace("房间:%d，名字:%s，类型为:%s，一共有%d人在此房间", i, v.room_name, v.game_type, v.amount)
    end
end

function MAIN_LAYER_CLASS:onEnter_()
    register_as_audience(self.uid, {EVENT_ROOM_DETAIL={func = self.room_detail, args = {self} }})
    ME_D.request_message(CMD_ROOM_OPER, "detail_room", {})
end

function MAIN_LAYER_CLASS:onExit_()
    remove_audience_from_raiser(self.uid)
end