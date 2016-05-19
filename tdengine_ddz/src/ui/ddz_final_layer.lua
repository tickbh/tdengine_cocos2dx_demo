DDZ_FINAL_LAYER_CLASS = class("DDZ_FINAL_LAYER_CLASS", function()
    return display.newNode()
end)

function DDZ_FINAL_LAYER_CLASS:ctor(is_win)
    self:setContentSize(cc.size(400, 300))
    self:init_ui()
    self:set_data(is_win)
end


function DDZ_FINAL_LAYER_CLASS:init_ui()
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

function DDZ_FINAL_LAYER_CLASS:get_all_desc()
    return self.is_win and "    胜利    " or "    失败    "

end

function DDZ_FINAL_LAYER_CLASS:set_data(is_win)
    self.is_win = is_win
    self.all_detail:setString(self:get_all_desc())
end
