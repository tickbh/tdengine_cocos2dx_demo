COUNT_DOWN_CLASS = class("COUNT_DOWN_CLASS", function()
    return display.newNode()
end)

function COUNT_DOWN_CLASS:ctor(data)
    self:enableNodeEvents()
    self.left_time = data.left_time or 0
    local bg = display.newSprite("button/btn_kfu.png")
        :addTo(self)
    bg:setAnchorPoint(cc.p(0, 0))
    self:setContentSize(bg:getContentSize())
    self.text = cc.Label:createWithSystemFont(tostring(self.left_time), "Arial", 20):addTo(self)
    self.text:setPosition(bg:getContentSize().width / 2, bg:getContentSize().height / 2)
end

function COUNT_DOWN_CLASS:set_left_time(left_time)
    self.left_time = left_time
end

function COUNT_DOWN_CLASS:onEnter_()
    local function time_update()
        self.left_time = math.max(self.left_time - 1, 0)
        self.text:setString(tostring(self.left_time))
    end
    self.timer_id = cc.Director:getInstance():getScheduler():scheduleScriptFunc(time_update, 1, false)
end

function COUNT_DOWN_CLASS:onExit_()
    if self.timer_id then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.timer_id)
        self.timer_id = nil
    end
end