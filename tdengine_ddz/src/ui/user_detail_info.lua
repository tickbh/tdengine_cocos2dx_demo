USER_DETEAIL_INFO_CLASS = class("USER_DETEAIL_INFO_CLASS", function()
    return display.newNode()
end)

function USER_DETEAIL_INFO_CLASS:ctor(data)
    self:setContentSize(cc.size(400, 300))
    self:init_ui()
    self:set_data(data)
end

function USER_DETEAIL_INFO_CLASS:init_ui()
    local size = self:getContentSize()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:removeFromParent()
        end
    end

    local bg = cc.Scale9Sprite:create("CyanSquare.png")
    bg:setAnchorPoint(cc.p(0, 0))
    bg:setContentSize(size)
    bg:addTo(self)

    local close_btn = ccui.Button:create()
    close_btn:setTouchEnabled(true)
    close_btn:loadTextures("close.png", "", "")
    close_btn:setContentSize(cc.size(close_btn:getVirtualRendererSize().width, close_btn:getVirtualRendererSize().height * 1.5))
    close_btn:setPosition(cc.p(size.width, size.height))
    close_btn:addTouchEventListener(touchEvent)
    self:addChild(close_btn)

    self.account = cc.Label:createWithSystemFont("账号:", "Arial", 20):move(100, 50):addTo(self)
    self.account:setAnchorPoint(cc.p(0.5, 0.5))


    self.account_detail = cc.Label:createWithSystemFont("", "Arial", 20):move(200, 50):addTo(self)
    self.account_detail:setAnchorPoint(cc.p(0, 0.5))
end

function USER_DETEAIL_INFO_CLASS:set_data(data)
    self.account_detail:setString(data.name or "xxxx")
end




