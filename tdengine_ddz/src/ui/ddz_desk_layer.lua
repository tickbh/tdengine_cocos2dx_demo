DDZ_DESK_LAYER_CLASS = class("DDZ_DESK_LAYER_CLASS", function()
    return display.newScene("DDZ_DESK_LAYER_CLASS")
end)


function DDZ_DESK_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
    self.poker_list_spirte = {}
    self.count_downs = {}
    self.ready_tip = {}
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

    self:add_ready_btn()
    self:add_choose_lord()
    self:add_cancel_lord()
    self:add_cancel_round()
    self:add_ok_round()
    self:add_tip_tbn()

    self:recover_first_status()

    self:add_count_downs()
    self:add_ready_tip()

    self:add_listener()

    for i,poker in ipairs({0x1, 0x2, 0x11, 0x12, 0x19}) do
        local poker = POKER_SPRITE_CLASS:create({id=poker})
        poker:setPosition(cc.p(200 + 30 * i,200))
        self:addChild(poker)
        table.insert(self.poker_list_spirte, poker)
    end
end

function DDZ_DESK_LAYER_CLASS:add_listener()
    -- handing touch events
    self.touchBeginPoint = nil
    local function onTouchBegan(touch, event)
        local location = touch:getLocation()
        trace("onTouchBegan: %o, %o", location.x, location.y)
        self.touchBeginPoint = {x = location.x, y = location.y}
        -- CCTOUCHBEGAN event must return true
        return true
    end

    local function onTouchMoved(touch, event)
        local location = touch:getLocation()
        trace("onTouchMoved: %o, %o", location.x, location.y)
    end

    local function onTouchEnded(touch, event)
        if not self.touchBeginPoint then
            return
        end
        local location = touch:getLocation()
        trace("onTouchEnded: %o, %o", location.x, location.y)
        trace("self.touchBeginPoint is %o", self.touchBeginPoint)
        local touchRect = cc.rect(math.min(self.touchBeginPoint.x, location.x), math.min(self.touchBeginPoint.y, location.y),
            math.abs(self.touchBeginPoint.x - location.x), math.abs(self.touchBeginPoint.y - location.y))
        local is_move = touchRect.width > 30

        trace("touchRect is %o", touchRect)
        local pre_inter_rect = nil
        for i=#self.poker_list_spirte,1,-1 do
            local poker = self.poker_list_spirte[i]
            local rect = cc.rectIntersection(touchRect, poker:getRect())
            trace("inter rect is %o", rect)
            if rect.width >= 0 and rect.height >= 0 then
                if not pre_inter_rect or not is_rect_contains_rect(pre_inter_rect, rect) then
                    poker:reverse_select()
                end
                if not is_move then
                    break
                end
                pre_inter_rect = rect
            end
        end
        self.touchBeginPoint = nil
    end

    local listener = cc.EventListenerTouchOneByOne:create()
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function DDZ_DESK_LAYER_CLASS:add_ready_tip()
    local ready_pre = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(141, 346))
        :addTo(self)
    self.ready_tip["pre"] = ready_pre

    local ready_my = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(487, 187))
        :addTo(self)
    self.ready_tip["my"] = ready_my

    local ready_after = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(744, 377))
        :addTo(self)
    self.ready_tip["after"] = ready_after

    self:hide_all_ready_tip()
end

function DDZ_DESK_LAYER_CLASS:add_count_downs()
    local count_down_pre = COUNT_DOWN_CLASS:create({left_time = 100})
    count_down_pre:setPosition(cc.p(141, 346))
    self:addChild(count_down_pre)
    self.count_downs["pre"] = count_down_pre

    local count_down_my = COUNT_DOWN_CLASS:create({left_time = 100})
    count_down_my:setPosition(cc.p(487, 187))
    self:addChild(count_down_my)
    self.count_downs["my"] = count_down_my 

    local count_down_after = COUNT_DOWN_CLASS:create({left_time = 100})
    count_down_after:setPosition(cc.p(744, 377))
    self:addChild(count_down_after)
    self.count_downs["after"] = count_down_after 

    self:hide_all_count_down()
end

function DDZ_DESK_LAYER_CLASS:hide_all_ready_tip()
    for _,node in pairs(self.ready_tip) do
        node:hide()
    end
end

function DDZ_DESK_LAYER_CLASS:hide_all_count_down()
    for _,node in pairs(self.count_downs) do
        node:hide()
    end
end

function DDZ_DESK_LAYER_CLASS:add_ready_btn()
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

function DDZ_DESK_LAYER_CLASS:add_choose_lord()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "choose", is_choose = 1})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local choose_lord = ccui.Button:create()
    choose_lord:setTouchEnabled(true)
    choose_lord:setScale9Enabled(true)
    choose_lord:loadTextures("button.png", "buttonHighlighted.png", "")
    choose_lord:setContentSize(cc.size(80, choose_lord:getVirtualRendererSize().height * 1.5))
    choose_lord:setTitleText("叫地主")
    choose_lord:setPosition(cc.p(200,400))
    choose_lord:addTouchEventListener(touchEvent)
    self:addChild(choose_lord)
    self.choose_lord = choose_lord
end

function DDZ_DESK_LAYER_CLASS:add_cancel_lord()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "choose", is_choose = 0})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local cancel_lord = ccui.Button:create()
    cancel_lord:setTouchEnabled(true)
    cancel_lord:setScale9Enabled(true)
    cancel_lord:loadTextures("button.png", "buttonHighlighted.png", "")
    cancel_lord:setContentSize(cc.size(80, cancel_lord:getVirtualRendererSize().height * 1.5))
    cancel_lord:setTitleText("不叫")
    cancel_lord:setPosition(cc.p(300,400))
    cancel_lord:addTouchEventListener(touchEvent)
    self:addChild(cancel_lord)
    self.cancel_lord = cancel_lord
end

function DDZ_DESK_LAYER_CLASS:add_cancel_round()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "round", is_choose = 0})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local cancel_round = ccui.Button:create()
    cancel_round:setTouchEnabled(true)
    cancel_round:setScale9Enabled(true)
    cancel_round:loadTextures("button.png", "buttonHighlighted.png", "")
    cancel_round:setContentSize(cc.size(80, cancel_round:getVirtualRendererSize().height * 1.5))
    cancel_round:setTitleText("不出")
    cancel_round:setPosition(cc.p(400,400))
    cancel_round:addTouchEventListener(touchEvent)
    self:addChild(cancel_round)
    self.cancel_round = cancel_round
end

function DDZ_DESK_LAYER_CLASS:get_poker_select()
    local select_ids = {}
    for _,poker in ipairs(self.poker_list_spirte) do
        if poker:get_select() then
            table.insert(select_ids, poker:get_data_id())
        end
    end
    return select_ids
end

function DDZ_DESK_LAYER_CLASS:add_ok_round()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            local select_list = self:get_poker_select()
            if #select_list == 0 then
                trace("请选择您要出的牌")
                return
            end
            local poker_type = DDZ_D.get_card_type(select_list)
            if poker_type == DDZ_D.TYPE_ERROR then
                trace("您所出的牌不符合规则")
                return 
            end
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "deal_poker", poker_list = select_list})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local ok_round = ccui.Button:create()
    ok_round:setTouchEnabled(true)
    ok_round:setScale9Enabled(true)
    ok_round:loadTextures("button.png", "buttonHighlighted.png", "")
    ok_round:setContentSize(cc.size(80, ok_round:getVirtualRendererSize().height * 1.5))
    ok_round:setTitleText("出牌")
    ok_round:setPosition(cc.p(500,400))
    ok_round:addTouchEventListener(touchEvent)
    self:addChild(ok_round)
    self.ok_round = ok_round
end

function DDZ_DESK_LAYER_CLASS:add_tip_tbn()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "round", is_choose = 0})
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local tip_tbn = ccui.Button:create()
    tip_tbn:setTouchEnabled(true)
    tip_tbn:setScale9Enabled(true)
    tip_tbn:loadTextures("button.png", "buttonHighlighted.png", "")
    tip_tbn:setContentSize(cc.size(80, tip_tbn:getVirtualRendererSize().height * 1.5))
    tip_tbn:setTitleText("提示")
    tip_tbn:setPosition(cc.p(600,400))
    tip_tbn:addTouchEventListener(touchEvent)
    self:addChild(tip_tbn)
    self.tip_tbn = tip_tbn
end

function DDZ_DESK_LAYER_CLASS:enter_desk()
    trace("DDZ_DESK_LAYER_CLASS:enter_desk")
end

function DDZ_DESK_LAYER_CLASS:remove_all_poker()
    for _,poker in ipairs(self.poker_list_spirte) do
        poker:removeFromParent()
    end
    self.poker_list_spirte = {}
end

function DDZ_DESK_LAYER_CLASS:get_my_idx()
    self.desk_info = self.desk_info or {}
    trace("desk_info is %o", self.desk_info)
    for idx,info in pairs(self.desk_info.wheels or {}) do
        if info.rid == ME_D.get_rid() then
            return idx
        end
    end
    return -1
end

function DDZ_DESK_LAYER_CLASS:hide_all_btn()
    self.ready_btn:setVisible(false)
    self.choose_lord:setVisible(false)
    self.cancel_lord:setVisible(false)
    self.cancel_round:setVisible(false)
    self.ok_round:setVisible(false)
    self.tip_tbn:setVisible(false)
end

function DDZ_DESK_LAYER_CLASS:recover_first_status() 
    self:remove_all_poker()
    self:hide_all_btn()
    self:hide_all_count_down()
    self:hide_all_ready_tip()
    self.ready_btn:setVisible(true)
end

function DDZ_DESK_LAYER_CLASS:calc_idx_tag(idx)
    local my_idx = self:get_my_idx()
    if my_idx == idx then
        return "my"
    elseif (my_idx + 1) % 3 == idx % 3 then
        return "after"
    elseif (my_idx - 1 + 3) % 3 == idx % 3 then
        return "pre"
    else
        return "unknow"
    end
end

function DDZ_DESK_LAYER_CLASS:show_count_down_tag(idx, left_time)
    self:hide_all_count_down()
    local tag = self:calc_idx_tag(idx)
    trace("DDZ_DESK_LAYER_CLASS:show_count_down_tag tag = %o idx = %o, count_downs = %o", tag, idx, self.count_downs)
    local count_down = self.count_downs[tag]
    if count_down then
        count_down:set_left_time(left_time)
        count_down:show()
    end
end

function DDZ_DESK_LAYER_CLASS:show_ready_status(idx)
    local tag = self:calc_idx_tag(idx)
    trace("DDZ_DESK_LAYER_CLASS:show_ready_status tag = %o", tag)
    local ready_tip = self.ready_tip[tag]
    if ready_tip then
        ready_tip:show()
    end
end


function DDZ_DESK_LAYER_CLASS:turn_index(idx)
    self:hide_all_btn()
    if self.cur_step == DDZ_STEP_LORD then
        if idx == self:get_my_idx() then
            self.choose_lord:setVisible(true)
            self.cancel_lord:setVisible(true)
        end
        self:show_count_down_tag(idx, 10)
    elseif self.cur_step == DDZ_STEP_PLAY then
        if idx == self:get_my_idx() then
            self.cancel_round:setVisible(true)
            self.ok_round:setVisible(true)
            self.tip_tbn:setVisible(true)
        end
        self:show_count_down_tag(idx, 30)
    end
end

function DDZ_DESK_LAYER_CLASS:room_msg_receive(user, oper, info)
    trace("DDZ_DESK_LAYER_CLASS:room_msg_receive %o %o", oper, info)
    if oper == "poker_init" then
        self:remove_all_poker()
        for i,poker in ipairs(info.poker_list or {}) do
            local poker = POKER_SPRITE_CLASS:create({id=poker})
            poker:setPosition(cc.p(200 + 30 * i,200))
            self:addChild(poker)
            table.insert(self.poker_list_spirte, poker)
        end
    elseif oper == "success_user_ready" then
        if info.rid == ME_D.get_rid() then
            self.ready_btn:setVisible(false)
        end
        self:show_ready_status(info.idx)
    elseif oper == "step_change" then
        self.cur_step = info.cur_step
        if info.cur_step == DDZ_STEP_NONE then
            self:recover_first_status()
        elseif self.cur_step == DDZ_STEP_LORD then
            self:hide_all_ready_tip()
        end
    elseif oper == "desk_info" then
        self.desk_info = info
    elseif oper == "op_idx" then
        self:turn_index(info.cur_op_idx)
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