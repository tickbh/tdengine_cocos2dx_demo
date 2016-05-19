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

    -- self.account = cc.Label:createWithSystemFont("账号:", "Arial", 20):move(100, 50):addTo(self)
    -- self.account:setAnchorPoint(cc.p(0.5, 0.5))


    -- self.account_detail = cc.Label:createWithSystemFont("", "Arial", 20):move(200, 50):addTo(self)
    -- self.account_detail:setAnchorPoint(cc.p(0, 0.5))

    self.all_detail = cc.Label:createWithSystemFont("", "Arial", 20):move(200, 50):addTo(self)
    self.all_detail:setContentSize(self:getContentSize())
    self.all_detail:setAnchorPoint(cc.p(0, 1))
    self.all_detail:setPosition(cc.p(0, self:getContentSize().height))
end

function USER_DETEAIL_INFO_CLASS:get_all_desc()
    local data = self.data
    local ddz_info = data.ddz_info
    trace("data is %o", data)
    return "名称:" .. (data.name or "") .. 
           "\n胜场:" .. tostring(ddz_info.win_amount or 0) .. 
           "\n负场:" .. tostring(ddz_info.lose_amount or 0) .. 
           "\n豆豆:" .. tostring(ddz_info.pea_amount or 0)
end

function USER_DETEAIL_INFO_CLASS:set_data(data)
    self.data = data
    self.all_detail:setString(self:get_all_desc())
end




